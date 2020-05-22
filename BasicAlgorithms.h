
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Chord.h"

class BasicAlgorithms {
public:
	void checkForHarmonicTriad(Chord* currentChord,std::vector<Chord*>chordsInProgression) {
		Chord* lastChord = chordsInProgression.back();
		if (lastChord->specialFunction == "D" && currentChord->specialFunction == "T")
			currentChord->priority = 1000;
		else if (lastChord->specialFunction == "T" && (currentChord->specialFunction == "S" || currentChord->specialFunction == "D"))
			currentChord->priority = 1000;
		else if(lastChord->specialFunction=="S"&&(currentChord->specialFunction=="D"||currentChord->specialFunction=="T"))
			currentChord->priority = 1000;
	}
};
