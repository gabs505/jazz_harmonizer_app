
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include <map>
#include "Melody.h"
#include "Chord.h"


class OtherAlgorithms {
public:
	//checking if a chord isn't continued for too long
	void checkForChordContinuity(Melody* &melody,int currentChordIndex) {
		if (currentChordIndex % 2 == 0 && currentChordIndex>0) {
			//iterating through vector of chord choices for current note
			for (auto it = melody->chordProgressionMatchesMap[currentChordIndex].begin(); it != melody->chordProgressionMatchesMap[currentChordIndex].end(); ++it) {
				if (melody->chordsInProgression[currentChordIndex-1]->name == (*it)->name) {
					(*it)->overallScore -= 100;
				}
			}
		}
	}

	void checkForChordSequenceContinuity() {
		
	}

	void avoidChoosingDominantOnWeakBeat(Melody*& melody, int currentChordIndex) {
		if (currentChordIndex % 2 != 0 && currentChordIndex > 0) {
			for (auto it = melody->chordProgressionMatchesMap[currentChordIndex].begin(); it != melody->chordProgressionMatchesMap[currentChordIndex].end(); ++it) {
				if((*it)->mode=="7") {
					(*it)->overallScore = 0;
				}
			}
		}
	}

	void continueTonicFrom251Progression(Melody*& melody, int currentChordIndex) {
		if (currentChordIndex > 0) {
			Chord* previousChord = melody->chordsInProgression[currentChordIndex - 1];
			if (std::count(previousChord->belongsToProgession.begin(), previousChord->belongsToProgession.end(), "major251") != 0) {
				for (auto it = melody->chordProgressionMatchesMap[currentChordIndex].begin(); it != melody->chordProgressionMatchesMap[currentChordIndex].end(); ++it) {
					if ((*it)->name == melody->chordsInProgression[currentChordIndex - 1]->name) {
						(*it)->overallScore += 100;
					}
				}
			}
		}
	
	}
};