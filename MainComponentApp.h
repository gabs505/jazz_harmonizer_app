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
		int x = 350;
		
		setAudioChannels(0, 2);

		setSize(700, 600);

	
		container.setBounds(0, 0, 110, 200);

		//labels
		mainAppLabel.setBounds(100, 10, 500, 40);
		melodySectionLabel.setBounds(x + 5, 55, 200, 30);
		mainSectionLabel.setBounds(10, 55, 200, 30);
		chordSectionLabel.setBounds(10, 265, 200, 30);

		choosePresetLabel.setBounds(20, 300, 100, 40);

	


		addAndMakeVisible(mainAppLabel);
		mainAppLabel.setColour(Label::textColourId, Colour(221, 221, 222));
		mainAppLabel.setFont(juce::Font(40.0f, juce::Font::italic));
		mainAppLabel.setJustificationType(juce::Justification::centred);
		mainAppLabel.setText("Jazz Harmonizer", juce::dontSendNotification);

		addAndMakeVisible(mainSectionLabel);
		adjustMainLabel(mainSectionLabel);
		mainSectionLabel.setText("Main Menu", juce::dontSendNotification);
		

		addAndMakeVisible(melodySectionLabel);
		adjustMainLabel(melodySectionLabel);
		melodySectionLabel.setText("Melody Section", juce::dontSendNotification);
		

		addAndMakeVisible(chordSectionLabel);
		adjustMainLabel(chordSectionLabel);
		chordSectionLabel.setText("Chord Section", juce::dontSendNotification);
		
		addAndMakeVisible(choosePresetLabel);
		choosePresetLabel.setText("Preset: ", juce::dontSendNotification);


		addAndMakeVisible(viewport);
		viewport.setTopLeftPosition(10, 300);
		viewport.setSize(680, 300);
		viewport.setViewedComponent(&container, false);
		

		//buttons
		button.setBounds(20, 90, 100, 30);
		playMidi.setBounds(20, 125, 100, 30);
		//playChords.setBounds(10, 120, 100, 30);
		addChordsButton.setBounds(190, 305, 100, 30);
		playChordsAndMelodyButton.setBounds(125, 125, 100, 30);
		//saveToMidiFile.setBounds(300, 305, 100, 30);
		stopPlayback.setBounds(300, 305, 100, 30);


		addAndMakeVisible(button);
		setButtonColours(button);
		button.setButtonText("Load Melody");


		button.onClick = [this] {FileChooser theFileChooser("Find a MIDI file", File(), "*.mid*");
		theFileChooser.browseForFileToOpen();
		synthAudioSource.loadMidi(theFileChooser.getResult());
		synthAudioSource.setMidiFile();
		};

		addAndMakeVisible(playMidi);
		playMidi.setButtonText("Play Melody");
		setButtonColours(playMidi);
		playMidi.onClick = [this] {//synthAudioSource.playMelody();
			synthAudioSource.pauseResumePlayback(playMidi); };



		addAndMakeVisible(addChordsButton);
		addChordsButton.onClick = [this] {synthAudioSource.addChordProgression();
		makeComponentRepaint(); };
		setButtonColours(addChordsButton);
		addChordsButton.setButtonText("Add Chords");


		addAndMakeVisible(playChordsAndMelodyButton);
		setButtonColours(playChordsAndMelodyButton);
		playChordsAndMelodyButton.onClick = [this] {synthAudioSource.playChordsAndMelody(); };
		playChordsAndMelodyButton.setButtonText("Play all");

		/*addAndMakeVisible(saveToMidiFile);
		setButtonColours(saveToMidiFile);
		saveToMidiFile.onClick = [this] {
			
			FileChooser theFileChooser("Save file",File(),"*.mid");
			theFileChooser.browseForFileToSave(false);
			ScopedPointer<FileOutputStream> fos = theFileChooser.getResult().createOutputStream();
			synthAudioSource.theMidiFile.writeTo(*fos);
			 };
		saveToMidiFile.setButtonText("Save");*/

		addAndMakeVisible(stopPlayback);
		setButtonColours(stopPlayback);
		stopPlayback.onClick = [this] {synthAudioSource.stopPlayback(); };
		stopPlayback.setButtonText("STOP");
		//preset menu
		presetMenu.setBounds(80, 305, 100, 30);

		addAndMakeVisible(presetMenu);
		presetMenu.addItem("Classic", 1);
		presetMenu.addItem("Modern", 2);
		presetMenu.addItem("Random", 3);
		presetMenu.setSelectedId(1);
		presetMenu.onChange = [this]
		{ synthAudioSource.chosenPreset = (presetMenu.getText()).toStdString(); };
		presetMenu.setColour(ComboBox::backgroundColourId, Colour::fromRGB(49,64,67));
		presetMenu.setColour(ComboBox::textColourId, Colours::white);

		
			
		melodicDensityMeter = new ProgressBar(synthAudioSource.melodicDensity);
		addAndMakeVisible(melodicDensityMeter);
		melodicDensityMeter->setPercentageDisplay(true);
		melodicDensityMeter->setColour(ProgressBar::backgroundColourId, Colour::fromRGB(49, 64, 67));

		
		rhythmicDensityMeter = new ProgressBar(synthAudioSource.rhythmicDensity);
		addAndMakeVisible(rhythmicDensityMeter);
		rhythmicDensityMeter->setPercentageDisplay(true);

		////meters

		melodicDensityMeter->setBounds(x + 5+10, 100, x-5-40, 20);
		rhythmicDensityMeter->setBounds(x+ 5+10, 130, x - 5 - 40, 20);
		
		
	}

	~MainContentComponent() override
	{
		shutdownAudio();
	}

	void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colour(72, 94, 121));
		g.setColour(juce::Colour(91, 119, 153));
		g.setFont(15.0f);
		//g.fillRoundedRectangle(10, 70, 335, 200,20);
		g.fillRoundedRectangle(10, 70, getWidth()/2-15, 200, 20);
		//g.fillRoundedRectangle(355, 70, 335, 200, 20);
		g.fillRoundedRectangle(getWidth() / 2+5, 70, getWidth() / 2 - 15, 200, 20);
		g.fillRoundedRectangle(10, 280, getWidth()-20, getHeight()-290, 20);
		//g.drawFittedText("Super duper harmonizer", 0, 0, 500, 90, Justification::horizontallyCentred,1);
		//g.drawLine(10, 200, getWidth() - 10, 200, 10.0);
	}

	void setButtonColours(TextButton& button) {
		button.setColour(TextButton::buttonColourId, Colour::fromRGB(49, 64, 67));
		button.setColour(TextButton::textColourOffId, Colours::white);
	}

	void adjustMainLabel(Label& label) {
		
		label.setColour(Label::textColourId, Colour(221, 221, 222));
		label.setFont(juce::Font(20.0f, juce::Font::bold));
	}

	void makeComponentRepaint() {
		container.removeAllChildren();
		container.setBounds(0, 0, synthAudioSource.notesToProcessVector.size() * 112, 300);
		setChordComboBoxes();
		setPlayButtons();
		setPlaySingleChordButtons();

		playProgressionLabel.setBounds(10, 165, 200, 30);
		playProgressionLabel.setText("Play progression: ", juce::dontSendNotification);
		container.addAndMakeVisible(playProgressionLabel);
		playSingleChordLabel.setBounds(10, 205, 200, 30);
		playSingleChordLabel.setText("Play chord: ", juce::dontSendNotification);
		container.addAndMakeVisible(playSingleChordLabel);


		repaint();
	}

	void resized() override
	{
		
		viewport.setSize(getWidth()-20, 400);

		
		
	
	}

	void setChordComboBoxes() {
		comboBoxVec.clear();
		synthAudioSource.changeComboBoxCounter = -(synthAudioSource.notesToProcessVector.size());
		if (synthAudioSource.notesToProcessVector.size() != 0) {
			int i = 0;
			for (auto it = synthAudioSource.notesToProcessVector.begin(); it != synthAudioSource.notesToProcessVector.end(); ++it) {
				std::string id = "menu" + std::to_string(i);
				auto menu = new ComboBox(String(id));
				menu->setComponentID(String(i));

				menu->setColour(ComboBox::backgroundColourId, Colour(49, 64, 67));

				
				int j = 0;
				for (auto it2 = synthAudioSource.possibleChordsToEachNoteMap[i].begin(); it2 != synthAudioSource.possibleChordsToEachNoteMap[i].end(); ++it2) {
					menu->addItem((*it2)->name, j + 1);
					j++;
				}

				menu->setSelectedId(synthAudioSource.chordsInProgressionIds[i]+1);

				menu->onChange = [this, menu]
				{ synthAudioSource.changeComboBoxCounter++; synthAudioSource.changeChordProgressionFromGUI(menu->getComponentID(), menu->getSelectedId() - 1); };

				comboBoxVec.push_back(menu);
				menu->setBounds(40 + i * 110, 140, 100, 30);

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

				button->setColour(TextButton::buttonColourId, Colour(133,85,133));
				//buttonsVec.push_back(button);
				button->setBounds(40 + i * 110, 190, 30, 20);
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

				button->setColour(TextButton::buttonColourId, Colour(169,106,169));

				button->setBounds(40 + i * 110, 230, 30, 20);
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
	TextButton saveToMidiFile;
	TextButton stopPlayback;
	
	Viewport viewport;
	Component container;


	Label mainAppLabel;
	Label mainSectionLabel;
	Label melodySectionLabel;
	Label chordSectionLabel;
	Label choosePresetLabel;
	Label playProgressionLabel;
	Label playSingleChordLabel;
	

	ComboBox presetMenu;
	ProgressBar* melodicDensityMeter;
	ProgressBar* rhythmicDensityMeter;

	
	std::vector<ComboBox*> comboBoxVec;
	std::vector<int>intVector;
	String selectedComboBoxIdx;

	std::vector<TextButton*>buttonsVec;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};