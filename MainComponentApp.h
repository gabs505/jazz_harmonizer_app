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
			for (MidiBuffer::Iterator i(*midiBuffer); i.getNextEvent(m, time);) {
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

	void loadMidi(File fileMIDI) {
		theMidiFile.clear();

		FileInputStream theStream(fileMIDI);
		theMidiFile.readFrom(theStream);
		theMidiFile.convertTimestampTicksToSeconds();
	}

	void playChords() {
		midiBufferChords->clear();
		MidiMessage m;
		int time;
		for (MidiBuffer::Iterator i(*midiBuffer); i.getNextEvent(m, time);) {
			
		}
	}

	void setMidiFile() {
		midiBuffer->clear(); //clearing midi buffer if any midi file was already loaded

		double sampleRate = synth.getSampleRate();// getting sample rate
		for (int t = 0; t < theMidiFile.getNumTracks(); t++) {//iterating through all tracks (in case of this app i need only one)
			const MidiMessageSequence* track = theMidiFile.getTrack(t);//pointer to selected track of MidiMessageSequence type
			for (int i = 0; i < track->getNumEvents(); i++) {
				MidiMessage& m = track->getEventPointer(i)->message;//accessing single MidiMessage
				//m.setNoteNumber(m.getNoteNumber() + 30);
				int sampleOffset = (int)(sampleRate * m.getTimeStamp());
				midiBuffer->addEvent(m, sampleOffset);
				
			}
		}
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

	void pauseResumePlayback(){

		/*sendAllNotesOff(*midiBuffer);*/

		if (midiIsPaused) {
			midiIsPlaying = true;
			midiIsPaused = false;
		}
			
		else {
			midiIsPlaying = false;
			midiIsPaused = true;
		}
		
			

		
		

	}

	ScopedPointer<MidiBuffer> midiBuffer = new MidiBuffer();
	ScopedPointer<MidiBuffer> midiBufferChords = new MidiBuffer();
	int samplesPlayed;
	bool midiIsPlaying = false;
	bool midiIsPaused = false;
	int bufferLength;
	int currentSample;
private:
	MidiKeyboardState& keyboardState;
	Synthesiser synth;
	MidiFile theMidiFile;
	
};

//==============================================================================
class MainContentComponent : public AudioAppComponent,
	private Timer
{
public:
	MainContentComponent()
		: synthAudioSource(keyboardState),
		keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard)
	{
		
		setAudioChannels(0, 2);

		setSize(600, 160);
		startTimer(400);

		addAndMakeVisible(button);
		button.setButtonText("Load MIDI");
		
		button.onClick = [this] {FileChooser theFileChooser("Find a MIDI file", File(), "*.mid*");
		theFileChooser.browseForFileToOpen();
		synthAudioSource.loadMidi(theFileChooser.getResult()); };

		addAndMakeVisible(playMidi);
		playMidi.setButtonText("Play MIDI");
		playMidi.onClick = [this] {synthAudioSource.setMidiFile(); };

		addAndMakeVisible(pauseResume);
		pauseResume.setButtonText("Play/Pause");
		pauseResume.onClick = [this] {synthAudioSource.pauseResumePlayback(); };

		addAndMakeVisible(playChords);
		playChords.onClick = [this] {synthAudioSource.playChords(); };
		playChords.setButtonText("Play Chords");


	}

	~MainContentComponent() override
	{
		shutdownAudio();
	}

	void resized() override
	{
		
		button.setBounds(10, 10, 100, 30);
		playMidi.setBounds(10, 50, 100, 30);
		pauseResume.setBounds(10, 90, 100, 30);
		playChords.setBounds(10, 120, 100, 30);
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



private:
	void timerCallback() override
	{
		//keyboardComponent.grabKeyboardFocus();
		stopTimer();
	}

	//==========================================================================
	SynthAudioSource synthAudioSource;
	MidiKeyboardState keyboardState;
	MidiKeyboardComponent keyboardComponent;
	TextButton button;
	TextButton playMidi;
	TextButton pauseResume;
	TextButton playChords;
	
	
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
