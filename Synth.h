#pragma once
#include <math.h>
#include "ChordCreator.h"

//==============================================================================
struct SineWaveSound : public SynthesiserSound
{
	SineWaveSound() {}

	bool appliesToNote(int) override { return true; }
	bool appliesToChannel(int) override { return true; }
};

//==============================================================================
struct SineWaveVoice : public SynthesiserVoice
{
	SineWaveVoice() {}

	bool canPlaySound(SynthesiserSound* sound) override
	{
		return dynamic_cast<SineWaveSound*> (sound) != nullptr;
	}

	void startNote(int midiNoteNumber, float velocity,
		SynthesiserSound*, int /*currentPitchWheelPosition*/) override
	{
		currentAngle = 0.0;
		level = velocity * 0.15;
		tailOff = 0.0;

		auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		auto cyclesPerSample = cyclesPerSecond / getSampleRate();

		angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;
	}

	void stopNote(float /*velocity*/, bool allowTailOff) override
	{
		if (allowTailOff)
		{
			if (tailOff == 0.0)
				tailOff = 1.0;
		}
		else
		{
			clearCurrentNote();
			angleDelta = 0.0;
		}
	}

	void pitchWheelMoved(int) override {}
	void controllerMoved(int, int) override {}

	void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
	{
		if (angleDelta != 0.0)
		{
			if (tailOff > 0.0) // [7]
			{
				while (--numSamples >= 0)
				{
					auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);

					for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);

					currentAngle += angleDelta;
					++startSample;

					tailOff *= 0.99; // [8]

					if (tailOff <= 0.005)
					{
						clearCurrentNote(); // [9]

						angleDelta = 0.0;
						break;
					}
				}
			}
			else
			{
				while (--numSamples >= 0) // [6]
				{
					auto currentSample = (float)(std::sin(currentAngle) * level);

					for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);

					currentAngle += angleDelta;
					++startSample;
				}
			}
		}
	}

private:
	double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};

//==============================================================================
class SynthAudioSource : public AudioSource
{
public:
	SynthAudioSource(MidiKeyboardState& keyState)
		: keyboardState(keyState)
	{
		for (auto i = 0; i < 4; ++i)                // [1]
			synth.addVoice(new SineWaveVoice());

		synth.addSound(new SineWaveSound());       // [2]
	}

	void setUsingSineWaveSound()
	{
		synth.clearSounds();
	}

	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
	{
		synth.setCurrentPlaybackSampleRate(sampleRate); // [3]
	}

	void releaseResources() override {}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		bufferToFill.clearActiveBufferRegion();

		MidiBuffer incomingMidi;
		MidiMessage m;
		int time;

		if (midiIsPlaying) {
			int sampleDeltaToAdd = -samplesPlayed;

			for (MidiBuffer::Iterator i(*currentMidiBuffer); i.getNextEvent(m, time);) {
				if (time >= samplesPlayed && time <= samplesPlayed + bufferToFill.numSamples) {
					incomingMidi.addEvent(m, time);
					currentSample = time;
				}


			}
			//incomingMidi.addEvents(*midiBuffer, samplesPlayed, bufferToFill.numSamples, sampleDeltaToAdd); //adding all messages at once
			samplesPlayed += bufferToFill.numSamples;
		}
		else {
			sendAllNotesOff(incomingMidi);
		}
		bufferLength = bufferToFill.numSamples;
		keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample,
			bufferToFill.numSamples, true);
		synth.renderNextBlock(*bufferToFill.buffer, incomingMidi,
			bufferToFill.startSample, bufferToFill.numSamples);


	}

	void loadMidi(File& fileMIDI) {
		theMidiFile.clear();
		midiBufferMelody->clear();

		FileInputStream theStream(fileMIDI);
		theMidiFile.readFrom(theStream);
		theMidiFile.convertTimestampTicksToSeconds();

	}

	void setMidiFile() {
		midiBufferMelody->clear(); //clearing midi buffer if any midi file was already loaded
		melodyBufferToProcess->clear();
		midiEventsTimes.clear();

		double sampleRate = synth.getSampleRate();// getting sample rate
		for (int t = 0; t < theMidiFile.getNumTracks(); t++) {//iterating through all tracks (in case of this app i need only one)
			const MidiMessageSequence* track = theMidiFile.getTrack(t);//pointer to selected track of MidiMessageSequence type
			int lastTime= (int)(sampleRate * track->getEndTime());
			for (int i = 0; i < track->getNumEvents(); i++) {
				MidiMessage& m = track->getEventPointer(i)->message;//accessing single MidiMessage
				if (m.isTempoMetaEvent()) {
					quarterNoteLengthInSamples = round(m.getTempoSecondsPerQuarterNote() * synth.getSampleRate());
				}

				int sampleOffset = (int)(sampleRate * m.getTimeStamp());
				int newSampleOffset = alignNoteToGrid(m, sampleOffset);
				//int newSampleOffset = sampleOffset;
				if (newSampleOffset < lastTime) {
					midiBufferMelody->addEvent(m, newSampleOffset);
					createMelodyBufferToProcess(m, newSampleOffset);//memory leak-zapis poza zakres
				}
				else {
					for (auto i = 1; i <= 16; i++)
					{
						newSampleOffset = (int)(((float)newSampleOffset / (float)quarterNoteLengthInSamples) * (float)quarterNoteLengthInSamples);
						midiBufferMelody->addEvent(MidiMessage::allNotesOff(i), newSampleOffset);
						midiBufferMelody->addEvent(MidiMessage::allSoundOff(i), newSampleOffset);
						midiBufferMelody->addEvent(MidiMessage::allControllersOff(i), newSampleOffset);

						melodyBufferToProcess->addEvent(MidiMessage::allNotesOff(i), newSampleOffset);
						melodyBufferToProcess->addEvent(MidiMessage::allSoundOff(i), newSampleOffset);
						melodyBufferToProcess->addEvent(MidiMessage::allControllersOff(i), newSampleOffset);
					}
				}
				

			}
		}
	}


	void createMelodyBufferToProcess(MidiMessage m, int sampleOffset) {
		
		float halfNoteLengthInSamples = (float)quarterNoteLengthInSamples * 2.0;
		float division = (float)sampleOffset / (float)halfNoteLengthInSamples;
		int test;

		if (m.isNoteOn()) {
			if (ceil(division) - division < 0.1 || abs(floor(division) - division) < 0.1 || division == 0) {
				//sprawdza czy nuta znajduje siê blisko wartoœci pó³nuty
				melodyBufferToProcess->addEvent(m, round(division) * (int)halfNoteLengthInSamples);
				midiEventsTimes.push_back(round(division) * (int)halfNoteLengthInSamples);

			}
			else if(midiEventsTimes.size()!=0) {
				//sprawdzamy czy na ostatni¹ pó³nutê przypad³a nuta z melodii,jeœli nie dodajemy najbli¿szy dŸwiêk na prawo
				test = (int)(floor(division) * (int)halfNoteLengthInSamples);
				if (std::count(midiEventsTimes.begin(), midiEventsTimes.end(), test) == 0) {
					melodyBufferToProcess->addEvent(m, (int)(floor(division) *(int)halfNoteLengthInSamples));
					midiEventsTimes.push_back((int)(floor(division) * (int)halfNoteLengthInSamples));
				}

			}

		}
		
		else if (m.isNoteOff())
			melodyBufferToProcess->addEvent(m, (int)ceil(division)*(int)halfNoteLengthInSamples);
	}

	void playMelody() {
		*currentMidiBuffer = *midiBufferMelody;
		samplesPlayed = 0;
		midiIsPlaying = true;
	}

	void playChords() {
		*currentMidiBuffer = *midiBufferChords;
		samplesPlayed = 0;
		midiIsPlaying = true;
	}

	void playChordsAndMelody() {
		MidiMessage m; int time;
		currentMidiBuffer->clear();
	
		for (MidiBuffer::Iterator i(*midiBufferChords); i.getNextEvent(m, time);) {
			currentMidiBuffer->addEvent(m, time);
		}

		for (MidiBuffer::Iterator i(*midiBufferMelody); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				MidiMessage transposedMidiMessage = MidiMessage::noteOn(m.getChannel(), m.getNoteNumber() + 12, m.getVelocity());
				currentMidiBuffer->addEvent(transposedMidiMessage, time);
			}
			else {
				currentMidiBuffer->addEvent(m, time);
			}

		}

		samplesPlayed = 0;
		midiIsPlaying = true;
	}

	void addChordProgression() {
		MidiMessage m; int time;
		midiBufferChords->clear();
		chordCreator->createChordProgressionOutput(midiBufferMelody, melodyBufferToProcess, midiBufferChords,quarterNoteLengthInSamples,
			notesToProcessVector, possibleChordsToEachNoteMap, chordsInProgressionIds, chordsInProgression);
	}

	int alignNoteToGrid(MidiMessage m, int sampleOffset) {
		sixteenthNoteLengthInSamples = round((float)quarterNoteLengthInSamples / 4.0);
			
		if (m.isNoteOnOrOff()) {
			float division = (float)sampleOffset / (float)sixteenthNoteLengthInSamples;
			sampleOffset = round(division) * sixteenthNoteLengthInSamples;
			lastNoteOnTime = sampleOffset;
			return sampleOffset;

		}
	}

	void playFromChosenChord(String id) {
		int idx = id.getIntValue();
		int currentTime = midiEventsTimes[idx];
		/*MidiBuffer* newMidiBuffer = new MidiBuffer();
		newMidiBuffer->addEvents(*midiBufferChords, currentTime, midiBufferChords->getLastEventTime(), -currentTime);
		*currentMidiBuffer = *newMidiBuffer;*/

		*currentMidiBuffer = *midiBufferChords;
		samplesPlayed = currentTime;
		midiIsPlaying = true;
	}

	void playSingleChosenChord(String id) {
		int idx = id.getIntValue();
		int currentTime = midiEventsTimes[idx];
		Chord* chordToPlay = chordsInProgression[idx];
		int add = 0;
		MidiBuffer* newMidiBuffer = new MidiBuffer();
		for (auto it = chordToPlay->chordNotesMidiNumbers.begin(); it != chordToPlay->chordNotesMidiNumbers.end(); ++it) {
			MidiMessage m = MidiMessage::noteOn(1, *it, (uint8)100);
			newMidiBuffer->addEvent(m, 0+add);
			add++;
		}

		MidiMessage m = MidiMessage::allNotesOff(1);
		
		newMidiBuffer->addEvent(m, (int)synth.getSampleRate() / 3.0);
		
		*currentMidiBuffer = *newMidiBuffer;
		samplesPlayed = 0;
		midiIsPlaying = true;
	}


	void sendAllNotesOff(MidiBuffer& midiMessages)
	{
		for (auto i = 1; i <= 16; i++)
		{
			midiMessages.addEvent(MidiMessage::allNotesOff(i), 0);
			midiMessages.addEvent(MidiMessage::allSoundOff(i), 0);
			midiMessages.addEvent(MidiMessage::allControllersOff(i), 0);
		}

		midiIsPlaying = false;
	}

	void pauseResumePlayback() {

		if (midiIsPaused) {
			midiIsPlaying = true;
			midiIsPaused = false;
		}

		else {
			midiIsPlaying = false;
			midiIsPaused = true;
		}

	}

	void changeChordProgressionFromGUI(String menuId, int chordId) {
		midiBufferChords->clear();
		int menuIdInt = menuId.getIntValue();
		chordsInProgression[menuIdInt] = possibleChordsToEachNoteMap[menuIdInt][chordId];
		MidiMessage m; int time; int idx = 0;

		for (MidiBuffer::Iterator i(*melodyBufferToProcess); i.getNextEvent(m, time);) {
			Chord* matchedChord = chordsInProgression[idx];
			chordCreator->addChordToMidiBuffer(m, time, matchedChord, midiBufferChords);
			if (m.isNoteOn()) {
				idx++;
			}

		}
	}



	MidiBuffer* currentMidiBuffer = new MidiBuffer();
	MidiBuffer* midiBufferMelody = new MidiBuffer();
	MidiBuffer* melodyBufferToProcess = new MidiBuffer();
	MidiBuffer* midiBufferChords = new MidiBuffer();
	int samplesPlayed;
	bool midiIsPlaying = false;
	bool midiIsPaused = false;
	int bufferLength;
	int currentSample;
	int sixteenthNoteLengthInSamples;
	std::vector<std::pair<int,int>>notesToProcessVector;//vector of pairs (bar index,note number)
	std::map<int, std::vector<Chord*>>possibleChordsToEachNoteMap; //melody notes indexes with fitting chords vector e.g. key:60 ,value:Chord*
	std::vector<int>chordsInProgressionIds;
	std::vector<Chord*>chordsInProgression;
	std::vector<int>midiEventsTimes;//times for notesToProcess vector

	int lastNoteOnTime;
	int adjacentNoteOns=0;
private:

	MidiKeyboardState& keyboardState;
	Synthesiser synth;
	MidiFile theMidiFile;
	int counter = 0;
	int quarterNoteLengthInSamples;

	ChordCreator* chordCreator = new ChordCreator();

};