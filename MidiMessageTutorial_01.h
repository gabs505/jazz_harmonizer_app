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

 name:             MidiMessageTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Produces outgoing midi messages.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
				   juce_audio_processors, juce_audio_utils, juce_core,
				   juce_data_structures, juce_events, juce_graphics,
				   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2017, linux_make, xcode_iphone, androidstudio

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

//==============================================================================
class MainContentComponent : public Component
{
public:
	//==============================================================================
	MainContentComponent()
		: startTime(Time::getMillisecondCounterHiRes() * 0.001)
	{
		addAndMakeVisible(bassDrumButton);
		bassDrumButton.setButtonText("Bass Drum (36)");
		bassDrumButton.onClick = [this] { setNoteNumber(36); };

		addAndMakeVisible(snareDrumButton);
		snareDrumButton.setButtonText("Snare Drum (38)");
		snareDrumButton.onClick = [this] { setNoteNumber(38); };

		addAndMakeVisible(closedHiHatButton);
		closedHiHatButton.setButtonText("Closed HH (42)");
		closedHiHatButton.onClick = [this] { setNoteNumber(42); };

		addAndMakeVisible(openHiHatButton);
		openHiHatButton.setButtonText("Open HH (46)");
		openHiHatButton.onClick = [this] { setNoteNumber(46); };

		addAndMakeVisible(volumeLabel);
		volumeLabel.setText("Volume (CC7)", dontSendNotification);

		addAndMakeVisible(volumeSlider);
		volumeSlider.setRange(0, 127, 1);
		volumeSlider.onValueChange = [this]
		{
			auto message = MidiMessage::controllerEvent(midiChannel, 7, (int)volumeSlider.getValue());
			message.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001 - startTime);
			addMessageToList(message);
		};

		addAndMakeVisible(midiMessagesBox);
		midiMessagesBox.setMultiLine(true);
		midiMessagesBox.setReturnKeyStartsNewLine(true);
		midiMessagesBox.setReadOnly(true);
		midiMessagesBox.setScrollbarsShown(true);
		midiMessagesBox.setCaretVisible(false);
		midiMessagesBox.setPopupMenuEnabled(true);
		midiMessagesBox.setColour(TextEditor::backgroundColourId, Colour(0x32ffffff));
		midiMessagesBox.setColour(TextEditor::outlineColourId, Colour(0x1c000000));
		midiMessagesBox.setColour(TextEditor::shadowColourId, Colour(0x16000000));

		setSize(800, 300);
	}

	void paint(Graphics&) override {}

	void resized() override
	{
		auto halfWidth = getWidth() / 2;

		auto buttonsBounds = getLocalBounds().withWidth(halfWidth).reduced(10);

		bassDrumButton.setBounds(buttonsBounds.getX(), 10, buttonsBounds.getWidth(), 20);
		snareDrumButton.setBounds(buttonsBounds.getX(), 40, buttonsBounds.getWidth(), 20);
		closedHiHatButton.setBounds(buttonsBounds.getX(), 70, buttonsBounds.getWidth(), 20);
		openHiHatButton.setBounds(buttonsBounds.getX(), 100, buttonsBounds.getWidth(), 20);
		volumeLabel.setBounds(buttonsBounds.getX(), 190, buttonsBounds.getWidth(), 20);
		volumeSlider.setBounds(buttonsBounds.getX(), 220, buttonsBounds.getWidth(), 20);

		midiMessagesBox.setBounds(getLocalBounds().withWidth(halfWidth).withX(halfWidth).reduced(10));
	}

private:
	static String getMidiMessageDescription(const MidiMessage& m)
	{
		if (m.isNoteOn())           return "Note on " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
		if (m.isNoteOff())          return "Note off " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
		if (m.isProgramChange())    return "Program change " + String(m.getProgramChangeNumber());
		if (m.isPitchWheel())       return "Pitch wheel " + String(m.getPitchWheelValue());
		if (m.isAftertouch())       return "After touch " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + String(m.getAfterTouchValue());
		if (m.isChannelPressure())  return "Channel pressure " + String(m.getChannelPressureValue());
		if (m.isAllNotesOff())      return "All notes off";
		if (m.isAllSoundOff())      return "All sound off";
		if (m.isMetaEvent())        return "Meta event";

		if (m.isController())
		{
			String name(MidiMessage::getControllerName(m.getControllerNumber()));

			if (name.isEmpty())
				name = "[" + String(m.getControllerNumber()) + "]";

			return "Controller " + name + ": " + String(m.getControllerValue());
		}

		return String::toHexString(m.getRawData(), m.getRawDataSize());
	}

	void setNoteNumber(int noteNumber)
	{
		auto message = MidiMessage::noteOn(midiChannel, noteNumber, (uint8)100);
		message.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001 - startTime);
		addMessageToList(message);

		auto messageOff = MidiMessage::noteOff(message.getChannel(), message.getNoteNumber());
		messageOff.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001 + 0.5 - startTime);
		addMessageToList(messageOff);
	}

	void logMessage(const String& m)
	{
		midiMessagesBox.moveCaretToEnd();
		midiMessagesBox.insertTextAtCaret(m + newLine);
	}

	void addMessageToList(const MidiMessage& message)
	{
		auto time = message.getTimeStamp();

		auto hours = ((int)(time / 3600.0)) % 24;
		auto minutes = ((int)(time / 60.0)) % 60;
		auto seconds = ((int)time) % 60;
		auto millis = ((int)(time * 1000.0)) % 1000;

		auto timecode = String::formatted("%02d:%02d:%02d.%03d",
			hours,
			minutes,
			seconds,
			millis);

		logMessage(timecode + "  -  " + getMidiMessageDescription(message));
	}

	//==============================================================================
	TextButton bassDrumButton;
	TextButton snareDrumButton;
	TextButton closedHiHatButton;
	TextButton openHiHatButton;

	Label volumeLabel;
	Slider volumeSlider;

	TextEditor midiMessagesBox;

	int midiChannel = 10;
	double startTime;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
