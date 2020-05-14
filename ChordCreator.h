
#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>

class Scales {
public:
	Scales() {
		createScales();
	}

	void createScales() {
		//scalesMap["C"] = std::vector<int>{ 24,26,28,29,31,33,35 }; //C major/a minor
		//scalesMap["G"] = std::vector<int>{ 24,26,28,30,31,33,35 };//G major
		//scalesMap["D"] = std::vector<int>{ 25,26,28,30,31,33,35 };//D major
		//scalesMap["A"] = std::vector<int>{ 25,26,28,30,32,33,35 };//A major
		//scalesMap["E"] = std::vector<int>{ 25,27,28,30,32,33,35 };//E major
		//scalesMap["B"] = std::vector<int>{ 25,27,28,30,32,34,35 };//B major

		scalesMap[0] = std::vector<int>{ 60,62,64,65,67,69,71 }; //C major/a minor
		scalesMap[1] = std::vector<int>{ 60,62,64,66,67,69,71 };//G major
		scalesMap[2] = std::vector<int>{ 61,62,64,66,67,69,71 };//D major
		scalesMap[3] = std::vector<int>{ 61,62,64,66,68,69,71 };//A major
		scalesMap[4] = std::vector<int>{ 61,63,64,66,68,69,71 };//E major
		scalesMap[5] = std::vector<int>{ 61,63,64,66,68,70,71 };//B major


		scalesIndexToName[0] = "C";
		scalesIndexToName[1] = "G";
		scalesIndexToName[2] = "D";
		scalesIndexToName[3] = "A";
		scalesIndexToName[4] = "E";
		scalesIndexToName[5] = "B";
	}

	std::vector<int> countMatches(std::set<int>melodyNotes) {//returns how many common notes has melody with each scale
		std::vector<int>matchCounts(scalesMap.size(), 0);
		
		for (auto it = melodyNotes.begin(); it != melodyNotes.end(); it++) {
			int index = 0;
			for (auto it2 = scalesMap.begin(); it2 != scalesMap.end(); ++it2) {
				// iter->first = klucz
				// iter->second = wartosc
				std::vector<int>::iterator it3 = std::find(it2->second.begin(), it2->second.end(),60+ (*it)%12);
				if (it3 != it2->second.end()) {
					matchCounts[index] ++;
				}
				index++;
			}
		}

		return matchCounts;
		/*for (auto it = matchCounts.begin(); it != matchCounts.end(); it++)
			DBG(String(*it));*/


	}

	void findScaleMatch(std::set<int>melodyNotes) {//matches scale to melody
		std::vector<int>matchCountsVector = countMatches(melodyNotes);
		int maxCountIndex = std::max_element(matchCountsVector.begin(), matchCountsVector.end()) - matchCountsVector.begin();
		matchedScaleIndex = maxCountIndex;
		matchedScaleName = scalesIndexToName[maxCountIndex];
		//DBG(String(matchedScaleName));
		
	}

	//std::map<std::string, std::vector < int >> scalesMap;
	std::map<int, std::vector < int >> scalesMap;
	std::map<int, std::string> scalesIndexToName;
	std::map<std::string, int> scalesMatches; //iloœæ dŸwiêków wspólnych miêdzy melodi¹ a kolejnymi skalami
	std::string matchedScaleName;
	int matchedScaleIndex;
};

class ChordCreator {
public:
	ChordCreator() {
		//middle C -> 60
		majorScaleChordComponents[1] = { 4,7 };
		majorScaleChordComponents[2] = { 3,7 };
		majorScaleChordComponents[3] = { 3,7 };
		majorScaleChordComponents[4] = { 4,7 };
		majorScaleChordComponents[5] = { 4,7 };
		majorScaleChordComponents[6] = { 3,7 };
		majorScaleChordComponents[7] = { 3,6 };

	};

	void setMelodyNotes(ScopedPointer<MidiBuffer> melody) { //creates melodyNotesSet and melodyNotesVector from MidiBuffer
		MidiMessage m; int time;
		for (MidiBuffer::Iterator i(*melody); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				melodyNotesSet.insert(m.getNoteNumber());
				melodyNotesVector.push_back(m.getNoteNumber());
			}
		}
		/*for (auto it = melodyNotes.begin(); it != melodyNotes.end(); it++)
			DBG(String(*it));*/
	}

	
	void setScaleChords() {//sets currentScaleChords
		std::vector<int> matchedScaleNotes=scales.scalesMap[scales.matchedScaleIndex];
		int index = 0;
		for (auto it = matchedScaleNotes.begin(); it != matchedScaleNotes.end(); ++it) {
			currentScaleChords[index] = {*it,*it+majorScaleChordComponents[index+1][0],*it + majorScaleChordComponents[index + 1][1] };
			DBG(String(index));
			/*for (auto it2 = currentScaleChords[index].begin(); it2 != currentScaleChords[index].end(); ++it2)
				DBG(String(*it2));*/
			index++;
		}
	}

	void transposeMelodyNotes() {//transpose melody notes to lower octave (C4 - middle C)
		for (auto it = melodyNotesVector.begin(); it != melodyNotesVector.end(); it++) {
			*it=60+*it%12;//60-middle C note number
		}
	}

	void createChords(ScopedPointer<MidiBuffer> melody) {
		setMelodyNotes(melody); //creating a set of melody notes
		scales.findScaleMatch(melodyNotesSet);//matching scale to melody
		transposeMelodyNotes();
		setScaleChords();//create map containing chords corresponding to each scale step

	}

	//=======================
	//PROPERTIES
	std::vector<int>melodyNotesVector; //all melody notes vector
	std::set<int>melodyNotesSet;//unique melody notes set
	std::map<std::string, std::vector<int>> chords; //output chord progression
	
	Scales scales;// scales object
	std::map<int, std::vector<int>> majorScaleChordComponents;//thirds and fifths for each major scale chord
	std::map<int, std::vector<int>> currentScaleChords;//chords for matched scale
	
};

