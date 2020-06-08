

#pragma once
#include <JuceHeader.h>
#include "BasicAlgorithms.h"
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
		matchedScales.clear();
		int mostFrequentMelodyNote = basicAlgorithms.findMostFrequentMelodyNote(melody->melodyNotesVector);
		int idx = 0;
		int i = 0;
		for (auto it = melody->melodyNotesVectorToScaleDetection.begin(); it != melody->melodyNotesVectorToScaleDetection.end(); ++it) {
			if (*it == -1) {
				std::vector<int>notesRange=basicAlgorithms.slice(melody->melodyNotesVectorToScaleDetection,idx,i-1); //getting subvector from vector
				idx = i + 1;

				std::set<int>notesRangeSet(notesRange.begin(), notesRange.end());

				if (matchedScales.size() != 0) {
						std::string matchedScale = basicAlgorithms.detectScale(notesRange);

						//checking if scale continuation is possible
						int numOfCommonNotes = 0;//number of common notes with scale in previous bar
						for (auto it2 = notesRangeSet.begin(); it2 != notesRangeSet.end(); ++it2) {
							if (std::count(matchedScales.back()->notesMidiNumbers.begin(), matchedScales.back()->notesMidiNumbers.end(), *it2)) {
								numOfCommonNotes++;
							}
						}
						if (numOfCommonNotes==notesRangeSet.size()) {
							matchedScales.push_back(matchedScales.back());
						}
						else {
							for (auto it2 = majorScalesVector.begin(); it2 != majorScalesVector.end(); ++it2) {
								if ((*it2)->scaleName == matchedScale) {
									matchedScales.push_back(*it2);
								}
							}
						}
						

				}
				else {
					for (auto it2 = majorScalesVector.begin(); it2 != majorScalesVector.end(); ++it2) {//matching first and main scale based on last note of the melody
						if ((*it2)->notesMidiNumbers[0] == melody->melodyNotesVector.back()) {
							matchedScales.push_back(*it2);
						}
					}
				}	
			}

			i++;
		}
		
		
		for(auto it = matchedScales.begin(); it != matchedScales.end(); ++it) {
			DBG((*it)->scaleName);
		}
	}

	std::vector<std::string> scaleMatchingByCommonNotes(std::set<int>melodyNotesSet) {//matches scale to melody
		std::vector<int>matchCountsVector = countMatches(melodyNotesSet);//vector with number of matches for each scale
		std::vector<int>maxCounts;//vector with indexes of scales that have max number of counts
		int maxIndex = std::max_element(matchCountsVector.begin(), matchCountsVector.end()) - matchCountsVector.begin();
		int counter = 0;
		for (auto it = matchCountsVector.begin(); it != matchCountsVector.end(); it++) {
			if (*it == matchCountsVector[maxIndex])
				maxCounts.push_back(counter);
			counter++;
		}
		std::vector<std::string>matchedScales;
		for (auto it = maxCounts.begin(); it != maxCounts.end(); ++it) {
			matchedScales.push_back(majorScalesVector[*it]->scaleName);
		}
		//Scale* matchedScale = majorScalesVector[maxCounts[0]];//scale fit to melody
		return matchedScales;
		

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

	BasicAlgorithms basicAlgorithms;
};