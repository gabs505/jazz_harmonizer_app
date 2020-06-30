
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Melody.h"
#include "Chord.h"

class ScoringAlgorithms {
public:

	void countScoreForEachPossibleChord(Melody*& melody) {
		for (auto it = melody->chordProgressionMatchesMap.begin(); it != melody->chordProgressionMatchesMap.end(); ++it) {
			for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
				(*it2)->countOverallScore();
			}
		}
	}

	void recountScoreForNextChords(Chord* chosenChord, std::vector<Chord*>optionalChords) {
		for (auto it = optionalChords.begin(); it != optionalChords.end(); ++it) {
			Chord* currentChord = *it;
			if (currentChord->name != chosenChord->name) {//not including already chosen chord
				if (currentChord->pointersToNextChordsFromProgression.size() != 0) {
					for (auto it2 = currentChord->pointersToNextChordsFromProgression.begin();
						it2 != currentChord->pointersToNextChordsFromProgression.end(); ++it2) {
						if (currentChord->belongsToProgession == "major251") {
							(*it2)->score->overallScore -= (*it2)->score->scoreForMajor251;//substracting points if certain progression wasn't chosen
						}
						else if (currentChord->belongsToProgession == "major251Long") {
							(*it2)->score->overallScore -= (*it2)->score->scoreForMajor251Long;
						}
						else if (currentChord->belongsToProgession == "minor251") {
							(*it2)->score->overallScore -= (*it2)->score->scoreForMinor251;
						}
						else if (currentChord->belongsToProgession == "fifthDown") {
							(*it2)->score->overallScore -= (*it2)->score->scoreForFifthDown;
						}
						else if (currentChord->belongsToProgession == "fourthDown") {
							(*it2)->score->overallScore -= (*it2)->score->scoreForFourthDown;
						}

					}
				}
			}
		}

	}

	void addScoreForChosenProgression(Chord* chord) {
		if (chord->pointersToNextChordsFromProgression.size() != 0) {
			for (auto it = chord->pointersToNextChordsFromProgression.begin(); it != chord->pointersToNextChordsFromProgression.end(); ++it) {
				(*it)->overallScore += 1000;
			}
		}

	}

	std::pair<Chord*, int> matchChordBasedOnScore(std::vector<Chord*>chords) {
		int maxScore = 0;
		int chordId;
		Chord* matchedChord = new Chord();
		int j = 0;
		for (auto it2 = chords.begin(); it2 != chords.end(); ++it2) {
			if ((*it2)->overallScore >= maxScore) {
				maxScore = (*it2)->overallScore;
				matchedChord = *it2;
				chordId = j;
			}
			j++;
		}
		return std::make_pair(matchedChord, chordId);
	}
};