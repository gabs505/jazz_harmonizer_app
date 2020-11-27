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
		for (auto i = 0; i < 4; ++i)                
			synth.addVoice(new SineWaveVoice());

		synth.addSound(new SineWaveSound());       
	}

	void setUsingSineWaveSound()
	{
		synth.clearSounds();
	}

	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
	{
		synth.setCurrentPlaybackSampleRate(sampleRate); 
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
		
		else if(!midiIsPlaying) {
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

		melodyMidiFile = fileMIDI;

	}

	void saveMidi(File& outputProgressionFile) {
		
		FileOutputStream fos(outputProgressionFile);
		//fos.write(midiBufferChords,6000);
		theMidiFile.writeTo(fos);
		
		
	}

	void setMidiFile() {
		midiBufferMelody->clear(); //clearing midi buffer if any midi file was already loaded
		melodyBufferToProcess->clear();
		midiEventsTimes.clear();
		chosenScalesNames.clear();
		//initializeChosenScalesNamesVector();

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

		fillMelodyBufferToProcessWithMissingHalfnotes();
		calculateMelodicDensity();
		calculateRhythmicDensity();
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

	
	void fillMelodyBufferToProcessWithMissingHalfnotes() {
		MidiMessage m; int time;
		MidiMessage lastEvent; int lastEventTime;
		MidiBuffer newBuffer;

		float halfNoteLengthInSamples = (float)quarterNoteLengthInSamples * 2.0;
		for (MidiBuffer::Iterator i(*melodyBufferToProcess); i.getNextEvent(m, time);) {
			float division = (float)time / (float)halfNoteLengthInSamples;
			if (m.isNoteOn()) {
				if (time > 0 && lastEventTime != (round(division) - 1) * (int)halfNoteLengthInSamples) {
					newBuffer.addEvent(lastEvent, lastEventTime + (int)halfNoteLengthInSamples);
				}
				newBuffer.addEvent(m, round(division)*(int)halfNoteLengthInSamples);
				lastEvent = m;
				lastEventTime = round(division) * (int)halfNoteLengthInSamples;
			}
			else {
				newBuffer.addEvent(m, time);
			}
		}
		melodyBufferToProcess->swapWith(newBuffer);
		
	}

	void calculateMelodicDensity() {
		MidiMessage m; int time; 
		int j = 0; int previousNoteNumber;
		int numOfBigJumps = 0;
		int numOfMediumJumps = 0;
		int numOfSmallJumps = 0;
		for (MidiBuffer::Iterator i(*melodyBufferToProcess); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				if (j > 0) {
					int distance = abs(m.getNoteNumber() - previousNoteNumber);
					if (distance >= 10) {
						numOfBigJumps++;
					}
					else if (distance >= 9 && distance>=5) {
						numOfMediumJumps++;
					}
					else if (distance >= 4 && distance >= 3) {
						numOfSmallJumps++;
					}
				}
				j++;
				previousNoteNumber = m.getNoteNumber();
			}
			
		}

		melodicDensity = (double)(numOfBigJumps+numOfMediumJumps*0.8+numOfSmallJumps*0.5) / (double)(j-1);
	}

	void calculateRhythmicDensity() {
		MidiMessage m; int time;
		int j = 0; int previousNoteTime;
		int numOfQuarters = 0;
		int numOfEights = 0;
		int numOfSixteens = 0;
		int distance = 0;
		for (MidiBuffer::Iterator i(*midiBufferMelody); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				if (j > 0) {
					distance=time-previousNoteTime;
					if (distance <=(quarterNoteLengthInSamples+10)/4.0) {
						numOfSixteens++;
					}
					else if (distance <= (quarterNoteLengthInSamples+10) / 2.0) {
						numOfEights++;
					}
					else if (distance <= quarterNoteLengthInSamples+10) {
						numOfQuarters++;
					}
				}
				j++;
				previousNoteTime = time;
			}

		}

		rhythmicDensity = (double)(numOfSixteens + numOfEights*0.8 + numOfQuarters* 0.4) / (double)j;
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

	void createChordsAndMelodyBuffer() {
		MidiMessage m; int time;
		midiBufferChordsAndMelody->clear();

		for (MidiBuffer::Iterator i(*midiBufferChords); i.getNextEvent(m, time);) {
			midiBufferChordsAndMelody->addEvent(m, time);
		}

		for (MidiBuffer::Iterator i(*midiBufferMelody); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {

				midiBufferChordsAndMelody->addEvent(m, time);
			}
			else {
				midiBufferChordsAndMelody->addEvent(m, time);
			}

		}
	}
	void playChordsAndMelody() {
		
		*currentMidiBuffer = *midiBufferChordsAndMelody;
		samplesPlayed = 0;
		midiIsPlaying = true;
		

		

	}

	void addChordProgression() {
		MidiMessage m; int time;
		midiBufferChords->clear();
		

		chordCreator->createChordProgressionOutput(midiBufferMelody, melodyBufferToProcess, midiBufferChords,quarterNoteLengthInSamples,
			notesToProcessVector, possibleChordsToEachNoteMap, chordsInProgressionIds, chordsInProgression,chosenPreset,chosenScales,chosenScalesNames, mainKey);
		createChordsAndMelodyBuffer();
	}

	void redoChordProgression() {
		midiBufferChords->clear();
		chordCreator->createChordProgressionOutput(midiBufferMelody, melodyBufferToProcess, midiBufferChords, quarterNoteLengthInSamples,
			notesToProcessVector, possibleChordsToEachNoteMap, chordsInProgressionIds, chordsInProgression, chosenPreset, chosenScales, chosenScalesNames, mainKey);
		createChordsAndMelodyBuffer();
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
		
		int currentTime;
		currentTime = quarterNoteLengthInSamples*2*idx;
		/*
			if (midiEventsTimes[idx] != 0)
				currentTime = midiEventsTimes[idx] - (int)halfNoteLengthInSamples;
			else
				currentTime = midiEventsTimes[idx];*/
		/*MidiBuffer* newMidiBuffer = new MidiBuffer();
		newMidiBuffer->addEvents(*midiBufferChords, currentTime, midiBufferChords->getLastEventTime(), -currentTime);
		*currentMidiBuffer = *newMidiBuffer;*/

		//*currentMidiBuffer = *midiBufferChords;
		*currentMidiBuffer = *midiBufferChordsAndMelody;
		samplesPlayed = currentTime;
		midiIsPlaying = true;
	}

	void playSingleChosenChord(String id) {
		int idx = id.getIntValue();
		int currentTime = midiEventsTimes[idx];
		Chord* chordToPlay = chordsInProgression[idx];
		int add = 0;
		MidiBuffer* newMidiBuffer = new MidiBuffer();
		int minusValue;
		if (chordToPlay->chordNotesMidiNumbers.size() > 3) {
			minusValue = 2;
		}
		else {
			minusValue = 0;
		}
		
		for (auto it = chordToPlay->chordNotesMidiNumbers.begin(); it != chordToPlay->chordNotesMidiNumbers.end() - minusValue; it++) {
			if (*it > 78) {
				*it -= 12;
			}
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

	void pauseResumePlayback(TextButton& button) {
		*currentMidiBuffer = *midiBufferMelody;
		if (!midiIsPlaying && !midiIsPaused) {
			samplesPlayed = 0;
			midiIsPaused = true;
		}
			
		if (midiIsPaused) {
			midiIsPlaying = true;
			midiIsPaused = false;
			button.setButtonText("Stop");
		}

		else {
			midiIsPlaying = false;
			midiIsPaused = true;
			button.setButtonText("Play Melody");
		}

	}

	void stopPlayback() {
		midiIsPlaying = false;
		midiIsPaused = false;
		samplesPlayed = 0;
	}

	void changeChordProgressionFromGUI(String menuId, int chordId) {
		if (changeComboBoxCounter>0) {
			midiBufferChords->clear();
			int menuIdInt = menuId.getIntValue();
			chordsInProgression[menuIdInt] = possibleChordsToEachNoteMap[menuIdInt][chordId];
			MidiMessage m; int time; int idx = 0;
			MidiBuffer* newBuffer = new MidiBuffer();
			for (MidiBuffer::Iterator i(*melodyBufferToProcess); i.getNextEvent(m, time);) {
				if (idx < chordsInProgression.size()) {
					Chord* matchedChord = chordsInProgression[idx];
					if (m.isNoteOn()) {
						idx++;
					}
					chordCreator->addChordToMidiBuffer(m, time, matchedChord, newBuffer);
				}
			}

			midiBufferChords->swapWith(*newBuffer);

		}	
	}

	void changeChosenScalesVectorFromGUI(std::string scaleName,int idx) {
		int size = chosenScalesNames.size();
		chosenScalesNames[idx] = scaleName;
	}

	void initializeChosenScalesNamesVector() {
		std::vector<std::string> vec(100, "X");
		chosenScalesNames = vec;
	}

	void applySettings(String optionValue, String mainKeyName,int metreIdx) {
		if (optionValue == "Yes") {
			mainKey = mainKeyName.toStdString();
			DBG(mainKeyName);
		}
	}



	MidiBuffer* currentMidiBuffer = new MidiBuffer();
	MidiBuffer* midiBufferMelody = new MidiBuffer();
	MidiBuffer* melodyBufferToProcess = new MidiBuffer();
	MidiBuffer* midiBufferChords = new MidiBuffer();
	MidiBuffer* midiBufferChordsAndMelody = new MidiBuffer();
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
	std::vector<Scale*>chosenScales;
	std::vector<std::string>chosenScalesNames;

	int lastNoteOnTime;
	int adjacentNoteOns=0;

	int changeComboBoxCounter = -1;

	std::string chosenPreset;

	double melodicDensity=0.0;
	double rhythmicDensity = 0.0;

	MidiFile theMidiFile;
	File melodyMidiFile;

	std::string mainKey = "X";
private:

	MidiKeyboardState& keyboardState;
	Synthesiser synth;

	int counter = 0;
	int quarterNoteLengthInSamples;

	ChordCreator* chordCreator = new ChordCreator();

};