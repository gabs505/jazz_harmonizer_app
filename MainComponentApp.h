/*
  ==============================================================================
   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.
   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.
   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.
  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.
 BEGIN_JUCE_PIP_METADATA
 name:             SynthUsingMidiInputTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Synthesiser with midi input.
 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
				   juce_audio_processors, juce_audio_utils, juce_core,
				   juce_data_structures, juce_events, juce_graphics,
				   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2017, linux_make
 type:             Component
 mainClass:        MainContentComponent
 useLocalCopy:     1
 END_JUCE_PIP_METADATA
*******************************************************************************/


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

		double sampleRate = synth.getSampleRate();// getting sample rate
		for (int t = 0; t < theMidiFile.getNumTracks(); t++) {//iterating through all tracks (in case of this app i need only one)
			const MidiMessageSequence* track = theMidiFile.getTrack(t);//pointer to selected track of MidiMessageSequence type
			for (int i = 0; i < track->getNumEvents(); i++) {
				MidiMessage& m = track->getEventPointer(i)->message;//accessing single MidiMessage
				if (m.isTempoMetaEvent()) {
					quarterNoteLengthInSamples = round(m.getTempoSecondsPerQuarterNote() * synth.getSampleRate());
				}

				int sampleOffset = (int)(sampleRate * m.getTimeStamp());
				//int newSampleOffset = alignNoteToGrid(m, sampleOffset);
				int newSampleOffset = sampleOffset;
				midiBufferMelody->addEvent(m, newSampleOffset);
				createMelodyBufferToProcess(m, newSampleOffset);//memory leak-zapis poza zakres

			}
		}

	}

	void createMelodyBufferToProcess(MidiMessage m, int sampleOffset) {//taking to buffer only notes with more then quarter note pauses between

		if (m.isNoteOn()) {
			float division = (float)sampleOffset / (float)quarterNoteLengthInSamples;
			if (ceil(division) - division < 0.1 || abs(floor(division) - division) < 0.1 || division == 0) {
				melodyBufferToProcess->addEvent(m, sampleOffset);
			}

		}
		else if (m.isNoteOff())
			melodyBufferToProcess->addEvent(m, sampleOffset);
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
		for (MidiBuffer::Iterator i(*midiBufferMelody); i.getNextEvent(m, time);) {
			currentMidiBuffer->addEvent(m, time);
		}
		for (MidiBuffer::Iterator i(*midiBufferChords); i.getNextEvent(m, time);) {
			currentMidiBuffer->addEvent(m, time);
		}
		samplesPlayed = 0;
		midiIsPlaying = true;
	}

	void addChordProgression() {
		MidiMessage m; int time;
		midiBufferChords->clear();
		chordCreator->createChordProgressionOutput(midiBufferMelody, melodyBufferToProcess,midiBufferChords, 
			notesToProcessVector, possibleChordsToEachNoteMap, chordsInProgressionIds,chordsInProgression);
	}

	int alignNoteToGrid(MidiMessage m, int sampleOffset) {
		sixteenthNoteLengthInSamples = round((float)quarterNoteLengthInSamples / 4.0);
		if (m.isNoteOff())
			return sampleOffset;
		else if (m.isNoteOn()) {
			float division = (float)sampleOffset / (float)sixteenthNoteLengthInSamples;
			return sampleOffset = round(division) * sixteenthNoteLengthInSamples;

		}
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
		chordsInProgression[menuIdInt] = possibleChordsToEachNoteMap[notesToProcessVector[menuIdInt]][chordId];
		MidiMessage m; int time; int idx = 0;
		
		for (MidiBuffer::Iterator i(*melodyBufferToProcess); i.getNextEvent(m, time);) {
			Chord* matchedChord = chordsInProgression[idx];
			chordCreator->addChordToMidiBuffer(m, time,matchedChord ,midiBufferChords);
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
	std::vector<int>notesToProcessVector;
	std::map<int, std::vector<Chord*>>possibleChordsToEachNoteMap;
	std::vector<int>chordsInProgressionIds;
	std::vector<Chord*>chordsInProgression;
private:
	MidiKeyboardState& keyboardState;
	Synthesiser synth;
	MidiFile theMidiFile;
	int counter = 0;

	int quarterNoteLengthInSamples;

	ChordCreator* chordCreator = new ChordCreator();

};

//==============================================================================
class MainContentComponent :public AudioAppComponent
{
public:
	MainContentComponent()
		: synthAudioSource(keyboardState),
		keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard)
	{

		setAudioChannels(0, 2);

		setSize(800, 700);

	
		container.setBounds(0, 0, 110, 200);

		addAndMakeVisible(viewport);
		viewport.setTopLeftPosition(10, 160);
		viewport.setSize(900, 400);
		viewport.setViewedComponent(&container, false);
		

		addAndMakeVisible(button);
		button.setButtonText("Load MIDI");


		button.onClick = [this] {FileChooser theFileChooser("Find a MIDI file", File(), "*.mid*");
		theFileChooser.browseForFileToOpen();
		synthAudioSource.loadMidi(theFileChooser.getResult());
		synthAudioSource.setMidiFile(); };

		addAndMakeVisible(playMidi);
		playMidi.setButtonText("Play MIDI");
		playMidi.onClick = [this] {synthAudioSource.playMelody(); };

		addAndMakeVisible(pauseResume);
		pauseResume.setButtonText("Play/Pause");
		pauseResume.onClick = [this] {synthAudioSource.pauseResumePlayback(); };

		addAndMakeVisible(playChords);
		playChords.onClick = [this] {synthAudioSource.playChords(); };
		playChords.setButtonText("Play Chords");

		addAndMakeVisible(addChordsButton);
		addChordsButton.onClick = [this] {synthAudioSource.addChordProgression();
		makeComponentRepaint(); };
		addChordsButton.setButtonText("Add Chords");


		addAndMakeVisible(playChordsAndMelodyButton);
		playChordsAndMelodyButton.onClick = [this] {synthAudioSource.playChordsAndMelody(); };
		playChordsAndMelodyButton.setButtonText("Play all");





	}

	~MainContentComponent() override
	{
		shutdownAudio();
	}

	void makeComponentRepaint() {
		container.deleteAllChildren();
		container.setBounds(0, 0, synthAudioSource.notesToProcessVector.size() * 120, 200);
		setChordComboBoxes();
		repaint();
	}

	void resized() override
	{

		button.setBounds(10, 10, 100, 30);
		playMidi.setBounds(10, 50, 100, 30);
		pauseResume.setBounds(10, 90, 100, 30);
		playChords.setBounds(10, 120, 100, 30);
		addChordsButton.setBounds(150, 10, 100, 30);
		playChordsAndMelodyButton.setBounds(150, 50, 100, 30);

		viewport.setSize(getWidth(), 400);
		

		/*int idx = 0;
		for (auto it = comboBoxVec.begin(); it != comboBoxVec.end(); ++it) {
			ComboBox* box = *it;
			box->setBounds(110 + idx * 110, 140, 100, 30);
			idx++;
		}*/



	}

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{
		synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		synthAudioSource.getNextAudioBlock(bufferToFill);
	}

	void releaseResources() override
	{
		synthAudioSource.releaseResources();
	}

	void setChordComboBoxes() {
		comboBoxVec.clear();
		if (synthAudioSource.notesToProcessVector.size() != 0) {
			int i = 0;
			for (auto it = synthAudioSource.notesToProcessVector.begin(); it != synthAudioSource.notesToProcessVector.end(); ++it) {
				std::string id = "menu" + std::to_string(i);
				auto menu = new ComboBox(String(id));
				menu->setComponentID(String(i));

				std::vector<Chord*>chords = synthAudioSource.possibleChordsToEachNoteMap[*it];
				int j = 0;
				for (auto it2 = chords.begin(); it2 != chords.end(); ++it2) {
					menu->addItem((*it2)->name, j + 1);
					j++;
				}

				menu->setSelectedId(synthAudioSource.chordsInProgressionIds[i]);

				menu->onChange = [this, menu]
				{synthAudioSource.changeChordProgressionFromGUI(menu->getComponentID(), menu->getSelectedId() - 1); };
				
				comboBoxVec.push_back(menu);
				menu->setBounds(10 + i * 110, 140, 100, 30);
				
				container.addAndMakeVisible(menu);
				i++;
			}
		}

	}




private:


	//==========================================================================
	SynthAudioSource synthAudioSource;
	MidiKeyboardState keyboardState;
	MidiKeyboardComponent keyboardComponent;
	TextButton button;
	TextButton playMidi;
	TextButton pauseResume;
	TextButton playChords;
	TextButton addChordsButton;
	TextButton playChordsAndMelodyButton;
	
	Viewport viewport;
	Component container;

	
	std::vector<ComboBox*> comboBoxVec;
	std::vector<int>intVector;
	String selectedComboBoxIdx;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};