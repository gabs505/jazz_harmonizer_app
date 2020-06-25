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
#include "Synth.h"

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
		synthAudioSource.setMidiFile();
		};

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
		container.setBounds(0, 0, synthAudioSource.notesToProcessVector.size() * 120, 400);
		setChordComboBoxes();
		setPlayButtons();
		setPlaySingleChordButtons();
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
	
	}

	void setChordComboBoxes() {
		comboBoxVec.clear();
		if (synthAudioSource.notesToProcessVector.size() != 0) {
			int i = 0;
			for (auto it = synthAudioSource.notesToProcessVector.begin(); it != synthAudioSource.notesToProcessVector.end(); ++it) {
				std::string id = "menu" + std::to_string(i);
				auto menu = new ComboBox(String(id));
				menu->setComponentID(String(i));

				
				int j = 0;
				for (auto it2 = synthAudioSource.possibleChordsToEachNoteMap[i].begin(); it2 != synthAudioSource.possibleChordsToEachNoteMap[i].end(); ++it2) {
					menu->addItem((*it2)->name, j + 1);
					j++;
				}

				menu->setSelectedId(synthAudioSource.chordsInProgressionIds[i]+1);

				/*menu->onChange = [this, menu]
				{synthAudioSource.changeChordProgressionFromGUI(menu->getComponentID(), menu->getSelectedId() - 1); };*/

				comboBoxVec.push_back(menu);
				menu->setBounds(10 + i * 110, 140, 100, 30);

				container.addAndMakeVisible(menu);
				i++;
			}
		}

	}

	void setPlayButtons() {
		if (synthAudioSource.notesToProcessVector.size() != 0) {
			int i = 0;
			for (auto it = synthAudioSource.notesToProcessVector.begin(); it != synthAudioSource.notesToProcessVector.end(); ++it) {
				std::string id =std::to_string(i);
				auto button = new TextButton();
				button->setComponentID(String(i));

				button->onClick = [this, button] { synthAudioSource.playFromChosenChord(button->getComponentID()); };

				//buttonsVec.push_back(button);
				button->setBounds(10 + i * 110, 180, 30, 20);
				//button->setButtonText("Play");
				button->setTooltip("Play chord progression from this point");
				container.addAndMakeVisible(button);
				i++;
			}
		}
	}

	void setPlaySingleChordButtons() {
		if (synthAudioSource.notesToProcessVector.size() != 0) {
			int i = 0;
			for (auto it = synthAudioSource.notesToProcessVector.begin(); it != synthAudioSource.notesToProcessVector.end(); ++it) {
				std::string id = std::to_string(i);
				auto button = new TextButton();
				button->setComponentID(String(i));

				button->onClick = [this, button] { synthAudioSource.playSingleChosenChord(button->getComponentID()); };

				button->setBounds(10 + i * 110, 210, 30, 20);
				button->setTooltip("Play single chord");

				container.addAndMakeVisible(button);
				i++;
			}
		}
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

	std::vector<TextButton*>buttonsVec;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};