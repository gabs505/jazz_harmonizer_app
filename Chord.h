
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>

class Chord {
public:
	Chord(std::string prime, int pn, std::string m, std::string specialF = "none") {
		primeNote = prime;
		primeMidiNumber = pn;
		mode = m;
		name = primeNote + mode;
		specialFunction = specialF;
		setChordNotes();
	}
	std::string primeNote;
	int primeMidiNumber;
	std::string mode;//major/minor/diminished
	std::string name;
	std::string specialFunction;//tonic,subdominant or dominant
	std::vector<int>chordNotesMidiNumbers;

	void setChordNotes() {
		std::vector<int>intervals;
		chordNotesMidiNumbers.push_back(primeMidiNumber);
		if (mode == "major") {
			intervals = { 4,7 };

		}
		else if (mode == "minor") {
			intervals = { 3,7 };
		}
		else if (mode == "diminished") {
			intervals = { 3,6 };
		}

		for (auto it = intervals.begin(); it != intervals.end(); ++it)
			chordNotesMidiNumbers.push_back(primeMidiNumber + *it);

	}
};