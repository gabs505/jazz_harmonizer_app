
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Chord.h"


std::vector<int>signsSharps = { 66,61,68,63,70,65};//F#-E#
std::vector<int>signsFlats = {70,63,68,61,66};//Bb-Gb
std::vector<std::string>scaleNamesSharps = {"G","D","A","E","B","F#"};
std::vector<std::string>scaleNamesFlats= { "F","Bb","Eb","Ab","Db","Gb" };
//int shIdx = 0; int scaleIdx = 0;
class BasicAlgorithms {
public:
	BasicAlgorithms() {
		
	}
	
	void applyMajorScheme(std::map<int, std::vector<Chord*>>& chordsMap,std::vector<int>scaleBreakpoints) {
		int i = 0;
		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
				int step = (*it2)->step;
				if (step != 1&&i<chordsMap.size()-1&&std::count(scaleBreakpoints.begin(),scaleBreakpoints.end(),i)==0&&(*it2)->belongsToProgession.size()==0) {
					std::vector<int>nextPossibleSteps = getMajorSchemeVector(step);
					for (auto it3 = chordsMap[i + 1].begin(); it3 != chordsMap[i + 1].end(); ++it3) {
						if (std::count(nextPossibleSteps.begin(), nextPossibleSteps.end(), (*it3)->step) != 0) {
							(*it3)->score->scoreForMajorScheme = 3;
							
						}
					}
				}

			}
			i++;
		}
	}

	std::vector<int> getMajorSchemeVector(int step) {
		if (step == 3)
			return std::vector<int>{ 6 };
		else if(step==6)
			return std::vector<int>{2,4};
		else if (step == 4)
			return std::vector<int>{2,7};
		else if (step == 2)
			return std::vector<int>{5,7};
		else if (step == 7)
			return std::vector<int>{5, 1};
		else if (step == 5)
			return std::vector<int>{1};
	}

	std::string detectScale(std::vector<int>notes,int shIdx=0) {
		std::vector<int>detectedSharpsIndexes;
		std::vector<int>detectedFlatsIndexes;
		std::string matchedScaleName;


		for (auto it = notes.begin(); it != notes.end(); ++it) {
			auto findingItS = std::find(signsSharps.begin(), signsSharps.end(), *it);//iterator for finding sharps in melody notes
			if (findingItS != signsSharps.end())
			{
				detectedSharpsIndexes.push_back(distance(signsSharps.begin(), findingItS));//if sharp is found in melody, it's index is pushed to vector detectedSharpsIndexes
			}
			auto findingItF= std::find(signsFlats.begin(), signsFlats.end(), *it);//iterator for finding flats in melody notes
			if (findingItF != signsFlats.end())
			{
				detectedFlatsIndexes.push_back(distance(signsFlats.begin(), findingItF));
			}
		}
		std::sort(detectedSharpsIndexes.begin(), detectedSharpsIndexes.end());
		std::sort(detectedFlatsIndexes.begin(), detectedFlatsIndexes.end());

		//deleting enharmonic notes, which are not additional sharps or flats:
		if (detectedSharpsIndexes.size() != 0&&detectedSharpsIndexes.back() == 5 && detectedSharpsIndexes.size() < 4) {
			detectedSharpsIndexes.pop_back();
		}
		if (detectedFlatsIndexes.size() != 0&& detectedFlatsIndexes.back() == 5 && detectedFlatsIndexes.size() < 4 ) {
			detectedFlatsIndexes.pop_back();
		}

		std::string detectedSigns;
		//checking if scale contains sharps or flats:
		if (detectedSharpsIndexes.size() != 0 && detectedFlatsIndexes.size() != 0) {
			if (detectedSharpsIndexes.back() <= detectedFlatsIndexes.back() &&
				detectedSharpsIndexes.size() >= detectedFlatsIndexes.size()) {
				detectedSigns = "sharps";
			}
			else {
				detectedSigns = "flats";
			}

		}
		
		if (detectedSharpsIndexes.size() == 0 && detectedFlatsIndexes.size() == 0) {//if no sharps or flats found matched scale is C-major
			matchedScaleName="C";
		}
		else {
			if (detectedSigns == "sharps") {
				//searching for note without corresponding cross in melody (e.g. F#,F)
				if (std::count(notes.begin(), notes.end(), detectedSharpsIndexes.back() - 1) == 0) {//if e.g only cross is in melody
					matchedScaleName=scaleNamesSharps[detectedSharpsIndexes.back()];
				}
				else if (detectedSharpsIndexes.size() != 1) {//if e.g. cross and note without cross are present in melody
					matchedScaleName=scaleNamesSharps[detectedSharpsIndexes[detectedSharpsIndexes.size()-2]];//taking previous cross in vector (one before the last)
				}
				else {
					matchedScaleName="C";
				}
			}
			else if (detectedSigns == "flats") {
				//searching for note without corresponding flat in melody
				if (std::count(notes.begin(), notes.end(), detectedFlatsIndexes.back() + 1) == 0) {
					matchedScaleName=scaleNamesFlats[detectedFlatsIndexes.back()];
				}
				else if (detectedFlatsIndexes.size() != 1) {//if e.g. cross and note without cross are present in melody
					matchedScaleName=scaleNamesFlats[detectedFlatsIndexes[detectedFlatsIndexes.size() - 2]];//taking previous cross in vector (one before the last)
				}
				else {
					matchedScaleName="C";
				}
			}
			
		}
		return matchedScaleName;
	
	}

	

	void checkForHarmonicTriad(std::map<int, std::vector<Chord*>>&chordsMap, std::vector<Chord*>&chordsInProgression,std::vector<int>&chordsInProgressionIds) {
		int i = 0;
		int j = 0;
		int k = 0;
		int l = 0;

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k < chordsMap.size() - 3) {
				i = 0;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
					j = 0;
					if ((*it2)->specialFunction=="T"&& chordsInProgressionIds[i]==-1) {
						for (auto it3 = chordsMap[k + 1].begin(); it3 != chordsMap[k + 1].end(); ++it3) {//iterating through k+1 vector
							if ((*it3)->specialFunction=="S" && chordsInProgressionIds[j] == -1) {
								l = 0;
								for (auto it4 = chordsMap[k + 2].begin(); it4 != chordsMap[k + 2].end(); ++it4) {//iterating through k+2 vector
									
									if ((*it4)->specialFunction=="D" && chordsInProgressionIds[l] == -1) {

										chordsInProgression[k] = chordsMap[k][i];
										chordsInProgressionIds[k] = i;
										

										chordsInProgression[k + 1] = chordsMap[k + 1][j];
										chordsInProgressionIds[k + 1] = j;
										

										chordsInProgression[k + 2] = chordsMap[k + 2][l];
										chordsInProgressionIds[k + 2] = l;
										
									}
									l++;
								}
							}
							j++;

						}
					}

					i++;

				}

			}
			k++;
		}
	}

	std::map<int,Chord*> chooseChordBasedOnPriority(std::vector<Chord*>chords) {
		int prior = 0;
		int idx = 0; int matchedChordIdx = 0;
		Chord* highestPriorityChord = new Chord();
		for (auto it = chords.begin(); it != chords.end(); ++it) {
			if ((*it)->priority > prior) {
				prior = (*it)->priority;
				matchedChordIdx = idx;
				highestPriorityChord = *it;

			}
			++idx;
		}
		std::map<int, Chord*>chordDataVector = { {matchedChordIdx, highestPriorityChord} };
		return chordDataVector;
	}

	template<typename T>
	std::vector<T>slice(std::vector<T>const& v, int m, int n) {//m-first index,n-last index
		auto first = v.cbegin() + m;
		auto last = v.cbegin() + n + 1;
		std::vector<T>vec(first, last);
		return vec;
	}

	int findMostFrequentMelodyNote(std::vector<int>melodyNotesVector) {
		std::map<int, int>noteOccurrences;//number of occurences in melody for each note

		for (auto it = melodyNotesVector.begin(); it != melodyNotesVector.end(); ++it) {
			//checking if map already contains this note
			if (noteOccurrences.count(*it) == 0) {
				noteOccurrences.insert(std::make_pair(*it, 1));
			}
			else {
				noteOccurrences[*it]++;
			}
		}

		int mostFrequentMidiNote;
		int max = 0;
		for (std::map<int, int>::iterator it2 = noteOccurrences.begin(); it2 != noteOccurrences.end(); ++it2) {//finding note which occurs most frequently (only one note)
			if (it2->second > max) {
				max = it2->second;
				mostFrequentMidiNote = it2->first;
			}
		}
		return mostFrequentMidiNote;
	}

	std::string chosenScale;
};
