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
		//80, 305, 100, 30
		stopPlayback.setBounds(20, 160, 205, 30);
		changeScales.setBounds(410, 305, 100, 30);
		saveToMidiFile.setBounds(520, 305, 100, 30);

		addAndMakeVisible(button);
		setButtonColours(button);
		button.setButtonText("Load Melody");


		button.onClick = [this] {FileChooser theFileChooser("Find a MIDI file", File(), "*.mid*");
		theFileChooser.browseForFileToOpen();
		synthAudioSource.loadMidi(theFileChooser.getResult());
		synthAudioSource.setMidiFile();
		setMelodyMenu();
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

		addAndMakeVisible(saveToMidiFile);
		setButtonColours(saveToMidiFile);
		saveToMidiFile.onClick = [this] {
			File f("D:\\Studia\\Magisterka\\zapis_tekstowy_progresji\\"+synthAudioSource.melodyMidiFile.getFileNameWithoutExtension()+"_"+presetMenu.getText()+".txt" );
			f.replaceWithText(" ");
			int i = 1;
			for (auto it = synthAudioSource.chordsInProgression.begin(); it != synthAudioSource.chordsInProgression.end(); ++it) {
				f.appendText("  " + (*it)->name);
				
					if (i % 2 == 0) {
						f.appendText(" |");
					}
					if (i % 8 == 0) {
						f.appendText(" \n");
					}
				
				
				i++;
			}
		};
		saveToMidiFile.setButtonText("Save");
		/*saveToMidiFile.onClick = [this] {
			
			FileChooser theFileChooser("Save file",File(),"*.mid");
			theFileChooser.browseForFileToSave(false);
			ScopedPointer<FileOutputStream> fos = theFileChooser.getResult().createOutputStream();
			synthAudioSource.theMidiFile.writeTo(*fos);
			 };
		saveToMidiFile.setButtonText("Save");*/

		addAndMakeVisible(stopPlayback);
		setButtonColours(stopPlayback);
		stopPlayback.onClick = [this] {synthAudioSource.stopPlayback(); };
		stopPlayback.setButtonText("Stop playback");

		addAndMakeVisible(changeScales);
		setButtonColours(changeScales);
		changeScales.onClick = [this] {synthAudioSource.redoChordProgression();
		makeComponentRepaint(); };
		changeScales.setButtonText("Change");


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
		melodicDensityMeter->setColour(ProgressBar::backgroundColourId, Colour::fromRGB(49, 64, 67));

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
		createBarLabels();
		createScaleLabels();

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
				

				container.addAndMakeVisible(button);
				i++;
			}
		}
	}

	void createBarLabels() {
		if (synthAudioSource.notesToProcessVector.size() != 0) {
			int i = 1;
			for (auto it = synthAudioSource.notesToProcessVector.begin(); it != synthAudioSource.notesToProcessVector.end(); ++it) {
				if (i == 1 || i % 4 == 0) {
					std::string barNumber;
					auto label = new Label();
					if (i == 1) {
						barNumber = std::to_string(i);
						label->setBounds(40, 120, 30, 20);
					}
					else {
						barNumber = std::to_string((int)(i + 2) / (int)2);
						label->setBounds(40 + (i) * 110, 120, 30, 20);
					}
						
					label->setText(barNumber, juce::dontSendNotification);
					container.addAndMakeVisible(label);
				}
				
				i++;
			}
		}

	}

	void createScaleLabels() {
		if (synthAudioSource.chosenScales.size() != 0) {
			int i = 0;
			for (auto it = synthAudioSource.chosenScales.begin(); it != synthAudioSource.chosenScales.end(); ++it) {
				Label* scaleLabel = new Label();
				scaleLabel->setComponentID(String(std::to_string(i)));
				std::string scaleName = (*it)->scaleName;
				scaleLabel->setBounds(60 + (i*4) * 110, 120, 30, 20);
				scaleLabel->setEditable(true);
				scaleLabel->setText(scaleName, juce::dontSendNotification);
				scaleLabel->onTextChange = [this, scaleLabel] {
					std::string scaleName = scaleLabel->getText().toStdString();
					synthAudioSource.changeChosenScalesVectorFromGUI(scaleName, scaleLabel->getComponentID().getIntValue());
					 };
				
				container.addAndMakeVisible(scaleLabel);


				i++;
			}
		}
		
	}
	
	void setMelodyMenu() {
		DialogWindow* melodyMenu = new DialogWindow("Menu", Colour(169, 106, 169),true,true);
		//ResizableWindow* melodyMenu = new ResizableWindow("Menu", false);
		melodyMenu->setBounds(getWidth()/2-150, getHeight()/2-150, 500, 500);

		Component* menuContent = new Component();
		menuContent->setBounds(0, 100, 200, 200);
		menuContent->centreWithSize(400, 400);

		TextButton acceptButton;
		acceptButton.setBounds(230,360, 150, 30);
		acceptButton.setButtonText("Accept settings");
		 
		//acceptButton.setColour(TextButton::buttonColourId, Colour(133, 85, 133));

		Label chooseKeyOptionLabel;
		chooseKeyOptionLabel.setText("Is melody in one key? ", juce::dontSendNotification);
		chooseKeyOptionLabel.setColour(Label::textColourId, juce::Colours::black);
		chooseKeyOptionLabel.setBounds(50, 50, 150, 30);

		Label enterKeyLabel;
		enterKeyLabel.setText("Enter main major key: ", juce::dontSendNotification);
		enterKeyLabel.setColour(Label::textColourId, juce::Colours::black);
		enterKeyLabel.setBounds(50, 100, 150, 30);

		Label enterKey;
		enterKey.setText("C", juce::dontSendNotification);
		enterKey.setColour(Label::textColourId, juce::Colours::white);
		enterKey.setColour(Label::backgroundColourId, Colour(91, 119, 153));
		enterKey.setEditable(true);
		enterKey.setBounds(200, 100, 60, 30);
		
		enterKey.onTextChange = [this, &enterKey] {
			mainKey = enterKey.getText();
		};

		auto chooseKeyOptionMenu = new ComboBox("100");
		chooseKeyOptionMenu->setColour(ComboBox::backgroundColourId, Colour(91, 119, 153));
		chooseKeyOptionMenu->addItem("Yes", 1);
		chooseKeyOptionMenu->addItem("No", 2);
		chooseKeyOptionMenu->setSelectedId(1);
		chooseKeyOptionMenu->setBounds(200, 50, 60, 30);
		chooseKeyOptionMenu->onChange = [this, chooseKeyOptionMenu] {selectedOption = chooseKeyOptionMenu->getText(); };


		Label chooseMetreLabel;
		chooseMetreLabel.setText("Choose time signature: ", juce::dontSendNotification);
		chooseMetreLabel.setColour(Label::textColourId, juce::Colours::black);
		chooseMetreLabel.setBounds(50, 150, 150, 30);

		auto chooseMetreMenu = new ComboBox("100");
		chooseMetreMenu->setColour(ComboBox::backgroundColourId, Colour(91, 119, 153));
		chooseMetreMenu->addItem("4/4", 1);
		chooseMetreMenu->addItem("3/4", 2);
		chooseMetreMenu->setSelectedId(1);
		chooseMetreMenu->setBounds(200, 150, 60, 30);
		chooseMetreMenu->onChange = [this, chooseMetreMenu] {
			metreId = chooseMetreMenu->getSelectedItemIndex();
		};


		

		
		menuContent->addAndMakeVisible(&chooseKeyOptionLabel);
		menuContent->addAndMakeVisible(chooseKeyOptionMenu);
		menuContent->addAndMakeVisible(&acceptButton);
		menuContent->addAndMakeVisible(&enterKeyLabel);
		menuContent->addAndMakeVisible(&enterKey);
		menuContent->addAndMakeVisible(&chooseMetreLabel);
		menuContent->addAndMakeVisible(chooseMetreMenu);
		
		
		
		
		acceptButton.onClick = [this,melodyMenu, menuContent, chooseKeyOptionMenu, &enterKey] {
			if (chooseKeyOptionMenu->getText() == "Yes") {
				mainKey = enterKey.getText();
			}
			synthAudioSource.applySettings(selectedOption,mainKey,metreId);
		DialogWindow* dw = menuContent->findParentComponentOfClass<DialogWindow>();
		dw->exitModalState(0); };
		
		
		
		melodyMenu->showModalDialog("Menu", menuContent, this, Colour(191,191,191), true);
	

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
	TextButton changeScales;
	
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

	String selectedOption;
	String mainKey;
	int metreId;

	std::vector<TextButton*>buttonsVec;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};