
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Chord.h"

class BasicAlgorithms {
public:
	/*void checkForHarmonicTriad(Chord* currentChord,std::vector<Chord*>chordsInProgression) {
		Chord* lastChord = chordsInProgression.back();
		if (lastChord->specialFunction == "D" && currentChord->specialFunction == "T")
			currentChord->priority = 1000;
		else if (lastChord->specialFunction == "T" && (currentChord->specialFunction == "S" || currentChord->specialFunction == "D"))
			currentChord->priority = 1000;
		else if(lastChord->specialFunction=="S"&&(currentChord->specialFunction=="D"||currentChord->specialFunction=="T"))
			currentChord->priority = 1000;
	}*/

	void checkForHarmonicTriad(std::map<int, std::vector<Chord*>>chordsMap, std::vector<Chord*>chordsInProgression,std::vector<int>chordsInProgressionIds) {
		std::vector<std::string>functionNames = std::vector<std::string>(chordsMap.size(),"none");
		std::vector<int>chosenChordIndexes= std::vector<int>(chordsMap.size(), -1);
		int idx = 0;
		int chosenChordIdx = 0;
		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
				if ((*it2)->specialFunction == "T" || (*it2)->specialFunction == "S" || (*it2)->specialFunction == "D") {
					functionNames[idx]=(*it2)->specialFunction;
					chosenChordIndexes[idx] = chosenChordIdx;
				}
				chosenChordIdx++;
			}
			idx++;
		}

		idx = 0;
		for (auto it = functionNames.begin(); it!=functionNames.end(); ++it) {
			if (idx < functionNames.size() - 3) {
				if (*it == "T") {
					if (functionNames[idx + 1] == "S" && functionNames[idx + 2] == "D") {
						chordsInProgression[idx] = chordsMap[idx][chosenChordIndexes[idx]];
						chordsInProgressionIds[idx] = chosenChordIndexes[idx];

						chordsInProgression[idx+1] = chordsMap[idx+1][chosenChordIndexes[idx+1]];
						chordsInProgressionIds[idx+1] = chosenChordIndexes[idx+1];

						chordsInProgression[idx+2] = chordsMap[idx+2][chosenChordIndexes[idx+2]];
						chordsInProgressionIds[idx+2] = chosenChordIndexes[idx+2];
					}
				}
			}
			
			idx++;
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
};
