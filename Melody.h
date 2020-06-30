
#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Chord.h"

class Melody {
public:
	Melody() {

	}
	~Melody() {
		melodyNotesSet = std::set<int>();
		for (auto itr = possibleChordsToEachNoteMap.begin(); itr != possibleChordsToEachNoteMap.end(); itr++)
		{
			for (auto it2 = itr->second.begin(); it2 != itr->second.end(); ++it2) {
				*it2 = nullptr;
			}
		}

	}

	void setMelodyNotes(MidiBuffer* melodyNotes) { //creates melodyNotesSet and melodyNotesVector from MidiBuffer
		MidiMessage m; int time;
		for (MidiBuffer::Iterator i(*melodyNotes); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				melodyNotesSet.insert(m.getNoteNumber());
				melodyNotesVector.push_back(m.getNoteNumber());
			}
		}

	}

	//notes to which chords should be added
	void setMelodyNotesVectorToProcess(MidiBuffer* melodyNotes, int quarterNoteLengthInSamples) {

		MidiMessage m; int time;
		float barLength = (float)quarterNoteLengthInSamples * 8.0;
		float actualBar = barLength;
		int barIdx = 0;
		int modulo;

		for (MidiBuffer::Iterator i(*melodyNotes); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				if (time >= (int)actualBar) {
					actualBar += barLength;
					barIdx++;
				}
				melodyNotesToProcessVector.push_back(std::pair<int, int>(barIdx, m.getNoteNumber()));
			}

		}

		//adding missing halfnotes to vector
		/*barIdx = 0;
		int counter = 0;
		int i = 0;
		std::vector<std::pair<int, int>>newVector;
		for (auto it = melodyNotesToProcessVector.begin(); it != melodyNotesToProcessVector.end(); ++it) {

			if (it->first > barIdx) {
				barIdx = it->first;
				if (counter < 4) {

					newVector.push_back(melodyNotesToProcessVector[i-1]);
				}

				counter = 0;
			}
			newVector.push_back(*it);
			counter++;
			i++;
		}
		melodyNotesToProcessVector = newVector;*/
		
	}

	void setMelodyNotesVectorToScaleDetection(MidiBuffer* melodyNotes, int quarterNoteLengthInSamples) {
		float barLength = (float)quarterNoteLengthInSamples * 8.0;
		MidiMessage m; int time;
		float actualBar = barLength;
		int modulo;
		int j = 0;
		for (MidiBuffer::Iterator i(*melodyNotes); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {

				DBG("something");
				if (time >= (int)actualBar) {
					melodyNotesVectorToScaleDetection.push_back(-1);
					actualBar += barLength;
				}
				melodyNotesVectorToScaleDetection.push_back(m.getNoteNumber());
				j++;
			}
		}
		melodyNotesVectorToScaleDetection.push_back(-1);//adding -1 to the end

	}

	void transposeMelodyNotes() {
		for (auto it = melodyNotesVector.begin(); it != melodyNotesVector.end(); it++) {
			*it = 60 + *it % 12;//60-middle C note number
		}
	}

	void transposeMelodyNotesToProcess() {
		for (auto it = melodyNotesToProcessVector.begin(); it != melodyNotesToProcessVector.end(); it++) {
			(it->second) = 60 + (it->second) % 12;//60-middle C note number
		}
	}

	std::vector<int>melodyNotesVector; //all melody notes vector
	std::vector<int>melodyNotesVectorToScaleDetection;//notes divided on bars by (-1) sign
	std::set<int>melodyNotesSet;//unique melody notes set
	std::vector<std::pair<int, int>>melodyNotesToProcessVector; //melody notes to which chord should be added

	std::vector<Chord*>currentScaleChords;//chords for matched scale
	std::map<std::string, std::vector<Chord*>>scalesChordsMap;
	std::vector<Chord*>chordsInProgression;//chords which occured earlier in progression
	std::vector<int>chordsInProgressionIds;

	std::map<int, std::vector<Chord*>>possibleChordsToEachNoteMap; // map containing chords for each note which fit
	std::map<int, std::vector<Chord*>>chordProgressionMatchesMap; //map with chord choices to each melody note

};