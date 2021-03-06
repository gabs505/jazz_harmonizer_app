

#pragma once
#include <JuceHeader.h>
#include "BasicAlgorithms.h"
#include "Melody.h"
#include "otherVariables.h"
#include <vector>
#include <set>




class Scale {
public:
	Scale(std::string scaleN, std::string t) {
		scaleName = scaleN;
		type = t;
	}
	Scale() {

	}

	std::string scaleName;
	std::vector<int>notesMidiNumbers;
	std::vector<std::string>notesNames;
	std::string mode; //minor or major
	std::string type;//sharp or flat or none
	std::vector<std::string>scaleChords;//vector containing chord types for each step of scale


	void createScaleNotes(int primeNumber) {

	}
};

class MajorScale :public Scale {
public:
	MajorScale(std::string name, std::string t, int primeNumber) :Scale(name, t) {
		createScaleNotes(primeNumber);
		scaleChords = { "maj7","m7","m7","maj7","7","m7","m7b5" };

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

	void findScaleMatches(Melody* &melody) {//returns vector of scales for each bar
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
							if (std::count(matchedScales.back()->notesMidiNumbers.begin(), matchedScales.back()->notesMidiNumbers.end(), 60+(*it2)%12)) {
								numOfCommonNotes++;
							}
						}
						
						int numOfCommonNotes2 = 0;
						if (matchedScales.size() > 2) {
							for (auto it2 = notesRangeSet.begin(); it2 != notesRangeSet.end(); ++it2) {
								if (std::count(matchedScales[matchedScales.size()-2]->notesMidiNumbers.begin(), matchedScales[matchedScales.size() - 2]->notesMidiNumbers.end(), 60 + (*it2) % 12)) {
									numOfCommonNotes2++;
								}
							}
						}


						if (numOfCommonNotes==notesRangeSet.size()) {//if all notes from current bar exist in previous bar scale, continue scale in current bar
							matchedScales.push_back(matchedScales.back());
						}
						else if (numOfCommonNotes2 == notesRangeSet.size()) {
							matchedScales.push_back(matchedScales[matchedScales.size()-2]);
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
					//matching first and main scale based on last note of the melody
					for (auto it2 = majorScalesVector.begin(); it2 != majorScalesVector.end(); ++it2) {
						if ((*it2)->notesMidiNumbers[0] == melody->melodyNotesVector.back()) {
							matchedScales.push_back(*it2);
						}
					}
				}	
			}

			i++;
		}
		
		recheckScaleMatches();
		DBG("chosen scales:");
		for(auto it = matchedScales.begin(); it != matchedScales.end(); ++it) {
			DBG((*it)->scaleName);
		}

		detectScaleBreakpoints();
	}

	void setMatchedScalesFromGUI(std::vector<std::string>matchedScalesNames) {
		int i = 0;
		for (auto it = matchedScalesNames.begin(); it != matchedScalesNames.end(); ++it) {
			for (auto it2 = majorScalesVector.begin(); it2 != majorScalesVector.end(); ++it2) {
				if ((*it2)->scaleName == *it) {
					matchedScales[i]=*it2;
				}
			}
			i++;
		}
		
	}

	

	//saves indexes of point on which scale changes to vector
	void detectScaleBreakpoints() {
		std::string lastScaleName = matchedScales[0]->scaleName;
		int i = 0;
		for (auto it = matchedScales.begin(); it != matchedScales.end(); ++it) {
			if ((*it)->scaleName != lastScaleName) {
				lastScaleName = (*it)->scaleName;
				scaleBreakpointsIndexes.push_back(4*i-1);
			}
			i++;
		}
	}

	//checking if scale doesnt change beacause of passing notes
	void recheckScaleMatches() {
		int i = 0;
		if (matchedScales.size() > 2) {
			for (auto it = matchedScales.begin() + 1; it != matchedScales.end() - 1; ++it) {

				int firstPrime = matchedScales[i]->notesMidiNumbers[0];
				int secondPrime = (*it)->notesMidiNumbers[0];
				int thirdPrime = matchedScales[i + 2]->notesMidiNumbers[0];

				if ((*it)->scaleName != matchedScales[i]->scaleName &&
					(matchedScales[i + 2]->scaleName == matchedScales[i]->scaleName ||
						firstPrime == 60 + (thirdPrime - 7) % 12 ||
						firstPrime == 60 + (thirdPrime + 7) % 12)) {

					if (firstPrime % 12 != (secondPrime + 7) % 12 && firstPrime % 12 != (secondPrime - 7) % 12) {
						*it = matchedScales[i];
					}
				}
				i++;


			}

		}
		
	}

	std::vector<std::string> scaleMatchingByCommonNotes(std::set<int>melodyNotesSet) {//matches scale to melody based on number of common notes
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
		return matchedScales;
		

	}

	//========================
	std::vector<Scale*>majorScalesVector;//vector of majorScale objects
	std::vector<Scale*>matchedScales;//scales matched for each two bars
	std::vector<int>scaleBreakpointsIndexes;//indexes of scale changes
	
	BasicAlgorithms basicAlgorithms;
};