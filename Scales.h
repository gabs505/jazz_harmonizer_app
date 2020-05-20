

#pragma once
#include <JuceHeader.h>
#include "Chord.h"
#include <vector>
#include <set>

std::vector<std::string>sharps = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
std::vector<std::string>flats = { "C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B" };

class Scale {
public:
	Scale(std::string scaleN, std::string t) {
		scaleName = scaleN;
		type = t;
	}

	std::string scaleName;
	std::vector<int>notesMidiNumbers;
	std::vector<std::string>notesNames;
	std::string mode; //minor or major
	std::string type;//sharp or flat or none
	std::vector<std::string>scaleChords;//vector containing chord for each step of scale


	void createScaleNotes(int primeNumber) {

	}
};

class MajorScale :public Scale {
public:
	MajorScale(std::string name, std::string t, int primeNumber) :Scale(name, t) {
		createScaleNotes(primeNumber);
		scaleChords = { "major","minor","minor","major","major","minor","diminished" };

	}

	std::string mode = "major";
	std::vector<int>scaleIntervals = { 0,2,4,5,7,9,11 };



	void createScaleNotes(int primeNumber) {

		int idx;
		std::vector<std::string>notesVector;
		if (type == "sharp" || type == "none") {
			notesVector = sharps;
			std::vector<std::string>::iterator it = std::find(sharps.begin(), sharps.end(), scaleName);
			if (it != sharps.end()) {
				idx = std::distance(sharps.begin(), it);
			}
		}
		else if (type == "flat") {
			notesVector = flats;
			std::vector<std::string>::iterator it = std::find(flats.begin(), flats.end(), scaleName);
			if (it != flats.end()) {
				idx = std::distance(flats.begin(), it);
			}
		}


		for (int i = 0; i < 7; i++) {
			if (primeNumber + scaleIntervals[i] > 71) {
				notesMidiNumbers.push_back(60 + (primeNumber + scaleIntervals[i]) % 12);
			}
			else {
				notesMidiNumbers.push_back(primeNumber + scaleIntervals[i]);
			}

			if (idx + scaleIntervals[i] > notesVector.size() - 1) {
				notesNames.push_back(notesVector[(idx + scaleIntervals[i]) % notesVector.size()]);
			}
			else
				notesNames.push_back(notesVector[idx + scaleIntervals[i]]);
		}
	}



};

class MinorScale :public Scale {
public:
	MinorScale(std::string name, std::string m, std::string t, int primeNumber) :Scale(name, t) {
		createScaleNotes(primeNumber);
	}

	std::string mode = "minor";
	std::vector<int>scaleIntervals = { 0,2,4,5,7,9,11 };
	std::vector<std::string>scaleChords = { "minor","diminished", "major","minor","minor","major","major" };


	void createScaleNotes(int primeNumber) {

	}


};


class Scales {
public:
	Scales() {
		createScales();
	}

	void createScales() {



		scalesVector.push_back(new MajorScale("C", "none", 60));
		scalesVector.push_back(new MajorScale("G", "none", 67));
		scalesVector.push_back(new MajorScale("D", "none", 62));
		scalesVector.push_back(new MajorScale("A", "none", 69));
		scalesVector.push_back(new MajorScale("E", "none", 64));

	}


	std::vector<int> countMatches(std::set<int>melodyNotes) {//returns how many common notes has melody with each scale
		std::vector<int>matchCounts(scalesVector.size(), 0);

		for (auto it = melodyNotes.begin(); it != melodyNotes.end(); it++) {
			int index = 0;
			for (auto it2 = scalesVector.begin(); it2 != scalesVector.end(); ++it2) {
				std::vector<int>::iterator it3 = std::find((*it2)->notesMidiNumbers.begin(), (*it2)->notesMidiNumbers.end(), *it);
				if (it3 != (*it2)->notesMidiNumbers.end()) {
					matchCounts[index] ++;
				}
				index++;
			}
		}

		return matchCounts;



	}

	void findScaleMatch(std::set<int>melodyNotesSet, std::vector<int>melodyNotesVector, int mostFrequentMelodyNote) {//matches scale to melody
		std::vector<int>matchCountsVector = countMatches(melodyNotesSet);//vector with number of matches for each scale
		std::vector<int>maxCounts;//vector with indexes of scales that have max number of counts
		int maxIndex = std::max_element(matchCountsVector.begin(), matchCountsVector.end()) - matchCountsVector.begin();
		int counter = 0;
		for (auto it = matchCountsVector.begin(); it != matchCountsVector.end(); it++) {
			if (*it == matchCountsVector[maxIndex])
				maxCounts.push_back(counter);
			counter++;
		}
		if (maxCounts.size() == 1) {
			matchedScale = scalesVector[maxCounts[0]];//scale fit to melody
		}
		else {
			for (auto it = maxCounts.begin(); it != maxCounts.end(); ++it) {
				if (scalesVector[*it]->notesMidiNumbers[0] == mostFrequentMelodyNote) {
					matchedScale = scalesVector[*it];
				}
				else if (scalesVector[*it]->notesMidiNumbers[0] == melodyNotesVector[0]) {
					matchedScale = scalesVector[*it];
				}
			}
		}



		DBG("matched scale name:");
		DBG(matchedScale->scaleName);



	}
	//========================
	std::vector<Scale*>scalesVector;
	Scale* matchedScale;
	//========================
	std::map<int, std::vector < int >> scalesMap;
	std::map<int, std::string> scalesIndexToName;
	std::map<std::string, int> scalesMatches; //iloœæ dŸwiêków wspólnych miêdzy melodi¹ a kolejnymi skalami
	std::string matchedScaleName;
	int matchedScaleIndex;
	std::map<int, std::string> noteNumberToNoteNameMap;
};