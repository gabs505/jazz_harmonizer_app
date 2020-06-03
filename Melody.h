
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
		DBG("In destructor");
		//melodyNotesSet.clear();
		melodyNotesSet = std::set<int>();
		for (auto itr = possibleChordsToEachNoteMap.begin(); itr != possibleChordsToEachNoteMap.end(); itr++)
		{
			for (auto it2 = itr->second.begin(); it2 != itr->second.end(); ++it2) {
				*it2 = nullptr;
			}
		}

	}

	void setMelodyNotesVectorToScaleDetection(MidiBuffer* melodyNotes, int quarterNoteLengthInSamples) {
		float barLength = (float)quarterNoteLengthInSamples * 4.0;
		MidiMessage m; int time;
		int modulo;
		for (MidiBuffer::Iterator i(*melodyNotes); i.getNextEvent(m, time);)
			if (m.isNoteOn()) {
				if (time != 0) {
					if (time < barLength) {
						modulo = (int)barLength % time;
					}
					else {
						if (time % (int)barLength < modulo) {
							melodyNotesVectorToScaleDetection.push_back(-1);
						}
						modulo = time % (int)barLength;
					}
				}
				melodyNotesVectorToScaleDetection.push_back(m.getNoteNumber());

			}
		melodyNotesVectorToScaleDetection.push_back(-1);

		/*DBG("Vector for matching scale:");
		for (auto it = melodyNotesVectorToScaleDetection.begin(); it != melodyNotesVectorToScaleDetection.end(); ++it) {
			DBG(*it);
		}*/
	}
	std::vector<int>melodyNotesVector; //all melody notes vector
	std::vector<int>melodyNotesVectorToScaleDetection;
	std::set<int>melodyNotesSet;//unique melody notes set
	std::vector<std::pair<int,int>>melodyNotesToProcessVector; //melody notes to which chord should be added

	std::vector<Chord*>currentScaleChords;//chords for matched scale
	std::map<std::string, std::vector<Chord*>>scalesChordsMap;
	std::vector<Chord*>chordsInProgression;//chords which occured earlier in progression
	std::vector<int>chordsInProgressionIds;

	std::map<int, std::vector<Chord*>>possibleChordsToEachNoteMap; // map containing chords for each note which fit
	std::map<int, std::vector<Chord*>>chordProgressionMatchesMap; //map with chord choices to each melody note

};