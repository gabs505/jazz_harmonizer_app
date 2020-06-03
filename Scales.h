

#pragma once
#include <JuceHeader.h>
#include "Melody.h"
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



		majorScalesVector.push_back(new MajorScale("C", "none", 60));
		majorScalesVector.push_back(new MajorScale("G", "sharp", 67));
		majorScalesVector.push_back(new MajorScale("D", "sharp", 62));
		majorScalesVector.push_back(new MajorScale("A", "sharp", 69));
		majorScalesVector.push_back(new MajorScale("E", "sharp", 64));
		majorScalesVector.push_back(new MajorScale("B", "sharp",71));
		majorScalesVector.push_back(new MajorScale("F#", "sharp", 66));

		majorScalesVector.push_back(new MajorScale("F", "flat", 65));
		majorScalesVector.push_back(new MajorScale("Bb", "flat", 70));
		majorScalesVector.push_back(new MajorScale("Eb", "flat", 63));
		majorScalesVector.push_back(new MajorScale("Ab", "flat", 68));
		majorScalesVector.push_back(new MajorScale("Db", "flat", 61));
		
		

	}


	std::vector<int> countMatches(std::set<int>melodyNotes) {//returns how many common notes has melody with each scale
		std::vector<int>matchCounts(majorScalesVector.size(), 0);

		for (auto it = melodyNotes.begin(); it != melodyNotes.end(); it++) {
			int index = 0;
			for (auto it2 = majorScalesVector.begin(); it2 != majorScalesVector.end(); ++it2) {
				std::vector<int>::iterator it3 = std::find((*it2)->notesMidiNumbers.begin(), (*it2)->notesMidiNumbers.end(), *it);
				if (it3 != (*it2)->notesMidiNumbers.end()) {
					matchCounts[index] ++;
				}
				index++;
			}
		}

		return matchCounts;



	}

	void findScaleMatches(Melody* &melody) {
		int idx = 0;
		int i = 0;
		for (auto it = melody->melodyNotesVectorToScaleDetection.begin(); it != melody->melodyNotesVectorToScaleDetection.end(); ++it) {
			if (*it == -1) {
				std::vector<int>notesRange(melody->melodyNotesVectorToScaleDetection[idx], melody->melodyNotesVectorToScaleDetection[i-1]);
				idx = i + 1;

				std::set<int>notesRangeSet(notesRange.begin(), notesRange.end());

				matchedScales.push_back(findScaleMatch(notesRangeSet));
				
			}

			i++;
		}

	}

	Scale* findScaleMatch(std::set<int>melodyNotesSet) {//matches scale to melody
		std::vector<int>matchCountsVector = countMatches(melodyNotesSet);//vector with number of matches for each scale
		std::vector<int>maxCounts;//vector with indexes of scales that have max number of counts
		int maxIndex = std::max_element(matchCountsVector.begin(), matchCountsVector.end()) - matchCountsVector.begin();
		int counter = 0;
		for (auto it = matchCountsVector.begin(); it != matchCountsVector.end(); it++) {
			if (*it == matchCountsVector[maxIndex])
				maxCounts.push_back(counter);
			counter++;
		}
		Scale* matchedScale = majorScalesVector[maxCounts[0]];//scale fit to melody
		return matchedScale;
		

	}

	//========================
	std::vector<Scale*>majorScalesVector;

	std::vector<Scale*>matchedScales;
	//========================
	std::map<int, std::vector < int >> scalesMap;
	std::map<int, std::string> scalesIndexToName;
	std::map<std::string, int> scalesMatches; //iloœæ dŸwiêków wspólnych miêdzy melodi¹ a kolejnymi skalami
	std::string matchedScaleName;
	int matchedScaleIndex;
	std::map<int, std::string> noteNumberToNoteNameMap;
};