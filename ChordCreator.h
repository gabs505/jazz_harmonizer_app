
#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>

class Scales {
public:
	Scales() {
		//scalesMap["C"] = std::vector<int>{ 24,26,28,29,31,33,35 }; //C major/a minor
		//scalesMap["G"] = std::vector<int>{ 24,26,28,30,31,33,35 };//G major
		//scalesMap["D"] = std::vector<int>{ 25,26,28,30,31,33,35 };//D major
		//scalesMap["A"] = std::vector<int>{ 25,26,28,30,32,33,35 };//A major
		//scalesMap["E"] = std::vector<int>{ 25,27,28,30,32,33,35 };//E major
		//scalesMap["B"] = std::vector<int>{ 25,27,28,30,32,34,35 };//B major

		scalesMap[0] = std::vector<int>{ 24,26,28,29,31,33,35 }; //C major/a minor
		scalesMap[1] = std::vector<int>{ 24,26,28,30,31,33,35 };//G major
		scalesMap[2] = std::vector<int>{ 25,26,28,30,31,33,35 };//D major
		scalesMap[3] = std::vector<int>{ 25,26,28,30,32,33,35 };//A major
		scalesMap[4] = std::vector<int>{ 25,27,28,30,32,33,35 };//E major
		scalesMap[5] = std::vector<int>{ 25,27,28,30,32,34,35 };//B major


		scalesIndexToName[0] = "C";
		scalesIndexToName[1] = "G";
		scalesIndexToName[2] = "D";
		scalesIndexToName[3] = "A";
		scalesIndexToName[4] = "E";
		scalesIndexToName[5] = "B";
	}

	std::vector<int> countMatches(std::set<int>melodyNotes) {
		std::vector<int>matchCounts(scalesMap.size(), 0);
		
		for (auto it = melodyNotes.begin(); it != melodyNotes.end(); it++) {
			int index = 0;
			for (auto it2 = scalesMap.begin(); it2 != scalesMap.end(); ++it2) {
				// iter->first = klucz
				// iter->second = wartosc
				std::vector<int>::iterator it3 = std::find(it2->second.begin(), it2->second.end(),24+ (*it)%12);
				/*DBG(String(it2->first));*/
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

	void findScaleMatch(std::set<int>melodyNotes) {
		std::vector<int>matchCountsVector = countMatches(melodyNotes);
		int maxCountIndex = std::max_element(matchCountsVector.begin(), matchCountsVector.end()) - matchCountsVector.begin();
		matchedScaleName = scalesIndexToName[maxCountIndex];
		DBG(String(matchedScaleName));
		
	}

	//std::map<std::string, std::vector < int >> scalesMap;
	std::map<int, std::vector < int >> scalesMap;
	std::map<int, std::string> scalesIndexToName;
	std::map<std::string, int> scalesMatches; //iloœæ dŸwiêków wspólnych miêdzy melodi¹ a kolejnymi skalami
	std::string matchedScaleName;
};

class ChordCreator {
public:
	ChordCreator() {};

	void setMelodyNotes(ScopedPointer<MidiBuffer> melody) {
		MidiMessage m; int time;
		for (MidiBuffer::Iterator i(*melody); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				melodyNotes.insert(m.getNoteNumber());
			}
		}
		/*for (auto it = melodyNotes.begin(); it != melodyNotes.end(); it++)
			DBG(String(*it));*/
	}

	void checkScaleMatches() {
		scales.findScaleMatch(melodyNotes);
	}


	//=======================
	//PROPERTIES
	std::set<int>melodyNotes;
	Scales scales;
	
};

