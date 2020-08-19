#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Score.h"

class Chord {
public:
	Chord() {

	}
	~Chord() {
	}
	Chord(Chord* chord) {
		primeNote = chord->primeNote;
		primeMidiNumber = chord->primeMidiNumber;
		mode = chord->mode;
		name = primeNote + mode;
		step = chord->step;
		overallScore = chord->overallScore;
		belongsToProgession = chord->belongsToProgession;
		updateSpecialFunction();
		updatePriority();
		setChordNotes();
	}
	Chord(std::string prime, int pn, std::string m, int st) {
		primeNote = prime;
		primeMidiNumber = pn;
		mode = m;
		name = primeNote + mode;
		step = st;
		updateSpecialFunction();
		updatePriority();
		setChordNotes();
	}

	Chord(std::string prime, int pn, std::string m) {
		primeNote = prime;
		primeMidiNumber = pn;
		mode = m;
		name = primeNote + mode;
		setChordNotes();
		priority = 0;
		step = 0;
		overallScore = 0;
	}
	
	std::string primeNote;
	int primeMidiNumber;
	std::string mode;//major/minor/diminished
	std::string name;
	std::string specialFunction;//tonic,subdominant or dominant
	std::vector<int>chordNotesMidiNumbers;
	int priority;//priority of choice: 1, 10 or 100
	int step;//step of scale
	Score* score=new Score();
	int overallScore;

	std::vector<Chord*> pointersToNextChordsFromProgression;
	std::vector<std::string> belongsToProgession;//name of progression to which chord belongs

	void setChordNotes() {
		std::vector<int>intervals;
		chordNotesMidiNumbers.push_back(primeMidiNumber);
		if (mode == "maj7") {
			intervals = { 4,7,11,14,21};//9,13

		}
		else if (mode == "m7") {
			intervals = { 3,7,10,17};//11
		}
		else if (mode == "7") {
			intervals = { 4,7,10 };
		}
		else if (mode == "m7b5") {
			intervals = { 3,6,10 };
		}

		for (auto it = intervals.begin(); it != intervals.end(); ++it)
			chordNotesMidiNumbers.push_back(primeMidiNumber + *it);

	}

	void updatePriority() {
		/*if (step == 1 || step == 4 || step == 5)
			priority = 100;
		else if (step == 2 || step == 6)
			priority = 10;
		else if (step == 3 || step == 7)
			priority = 1;
		*/
		priority = 0;
	}

	void updateSpecialFunction() {
		if (step == 1)
			specialFunction = "T";
		else if (step == 4)
			specialFunction = "S";
		else if (step == 5)
			specialFunction = "D";
		else
			specialFunction = "none";
	}

	void countOverallScore() {
		overallScore = score->addScore();
	}
};