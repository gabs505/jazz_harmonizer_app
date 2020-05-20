
#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>

std::vector<std::string>sharps = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
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

class majorScale :public Scale {
public:
	majorScale(std::string name, std::string t, int primeNumber) :Scale(name,t) {
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
				notesMidiNumbers.push_back(60+(primeNumber + scaleIntervals[i])%12);
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

class minorScale :public Scale {
public:
	minorScale(std::string name, std::string m, std::string t, int primeNumber) :Scale(name, t) {
		createScaleNotes(primeNumber);
	}

	std::string mode = "minor";
	std::vector<int>scaleIntervals = { 0,2,4,5,7,9,11 };
	std::vector<std::string>scaleChords = {"minor","diminished", "major","minor","minor","major","major" };


	void createScaleNotes(int primeNumber) {

	}


};


class Scales {
public:
	Scales() {
		createScales();
	}

	void createScales() {

		

		scalesVector.push_back(new majorScale("C", "none", 60));
		
		scalesVector.push_back(new majorScale("G", "none", 67));
		
		/*scalesVector.push_back(new Scale("D", "major", "none", 62));
		scalesVector.push_back(new Scale("A", "major", "none", 69));
		scalesVector.push_back(new Scale("E", "major", "none", 64));*/
		
	}


	std::vector<int> countMatches(std::set<int>melodyNotes) {//returns how many common notes has melody with each scale
		std::vector<int>matchCounts(scalesVector.size(), 0);
		
		for (auto it = melodyNotes.begin(); it != melodyNotes.end(); it++) {
			int index = 0;
			for (auto it2 = scalesVector.begin(); it2 != scalesVector.end(); ++it2) {
				std::vector<int>::iterator it3 = std::find((*it2)->notesMidiNumbers.begin(), (*it2)->notesMidiNumbers.end(),*it);
				if (it3 != (*it2)->notesMidiNumbers.end()) {
					matchCounts[index] ++;
				}
				index++;
			}
		}

		return matchCounts;
		


	}

	void findScaleMatch(std::set<int>melodyNotesSet, std::vector<int>melodyNotesVector) {//matches scale to melody
		std::vector<int>matchCountsVector = countMatches(melodyNotesSet);//vector with number of matches for each scale
		std::vector<int>maxCounts;//vector with indexes of scales that have max number of counts
		int maxIndex = std::max_element(matchCountsVector.begin(), matchCountsVector.end()) - matchCountsVector.begin();
		int counter = 0;
		for (auto it = matchCountsVector.begin(); it != matchCountsVector.end(); it++) {
			if (*it == matchCountsVector[maxIndex])
				maxCounts.push_back(counter);
			counter++;
		}
		//najczêœciej wystepuj¹cy dŸwiêk- tonika
		
		if (maxCounts.size() == 1) {
			matchedScale = scalesVector[maxCounts[0]];//scale fit to melody
		}
		else {
			for (auto it = maxCounts.begin(); it != maxCounts.end(); ++it) {
				if (scalesVector[*it]->notesMidiNumbers[0] == melodyNotesVector[0]) {
					matchedScale = scalesVector[*it];
				}
			}
		}

		
		
		DBG("matched scale name:");
		DBG(matchedScale->scaleName);
		
		
		
	}
	//========================
	std::vector<Scale*>scalesVector;
	Scale* matchedScale;
	//========================
	std::map<int, std::vector < int >> scalesMap;
	std::map<int, std::string> scalesIndexToName;
	std::map<std::string, int> scalesMatches; //iloœæ dŸwiêków wspólnych miêdzy melodi¹ a kolejnymi skalami
	std::string matchedScaleName;
	int matchedScaleIndex;
	std::map<int, std::string> noteNumberToNoteNameMap;
};

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



class ChordCreator {
public:
	ChordCreator() {
		

	};

	void setCurrentScaleChords() {//creates a vector of chord objects for each scale step
		std::vector<int> matchedScaleNotesNumbers = scales.matchedScale->notesMidiNumbers;//notes of current matched scale
		std::vector<std::string> matchedScaleNotesNames = scales.matchedScale->notesNames;//notes names of current matched scale
		std::vector<std::string> matchedScaleChordsModes = scales.matchedScale->scaleChords;//chord modes of current matched scale

		int index = 0;
		
		for (auto it = matchedScaleNotesNumbers.begin(); it != matchedScaleNotesNumbers.end(); ++it) {
			currentScaleChords.push_back(new Chord(matchedScaleNotesNames[index], *it, matchedScaleChordsModes[index]));
			index++;
		}
	}


	void setMelodyNotes(MidiBuffer* melody) { //creates melodyNotesSet and melodyNotesVector from MidiBuffer
		MidiMessage m; int time;
		for (MidiBuffer::Iterator i(*melody); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				melodyNotesSet.insert(m.getNoteNumber());
				melodyNotesVector.push_back(m.getNoteNumber());
			}
		}
		
	}

	void setMelodyNotesVectorToProcess(MidiBuffer* melody) {
		MidiMessage m; int time;
		for (MidiBuffer::Iterator i(*melody); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				melodyNotesToProcessVector.push_back(m.getNoteNumber());
			}
		}
	}
	//notes to which chords should be added

	

	void transposeMelodyNotes(std::vector<int>notes) {
		for (auto it = notes.begin(); it != notes.end(); it++) {
			*it = 60 + *it % 12;//60-middle C note number
		}
	}

	void matchScale(MidiBuffer* melody) {
		setMelodyNotes(melody); //creating a set of melody notes
		transposeMelodyNotes(melodyNotesVector);
		scales.findScaleMatch(melodyNotesSet,melodyNotesVector);//matching scale to melody
	}

	Chord* matchChord(int midiNoteNumber) {//matches chord to melody note

		std::vector<int>chordMatchesCounts(currentScaleChords.size(), 0);//creating vector of zeros of currentScaleChords size
		int idx = 0;
		for (auto it = currentScaleChords.begin(); it != currentScaleChords.end(); it++) {//iteration through current scale chords vector
			
			for (auto it2 = (*it)->chordNotesMidiNumbers.begin(); it2 != (*it)->chordNotesMidiNumbers.end(); ++it2) {//iterating through chord's notes
				if (*it2 == midiNoteNumber)
					chordMatchesCounts[idx]++;

			}
			
			idx++;
		}
			
		//finding max in chordMatchesCounts vector, which will be index of matched chord
		//tu mamy b³¹d w szukaniu max indexu: (zwraca 1 zamiast 0, a w konsoli wyœwietla siê ¿e pierwszy akord pasuje)
		int matchedChordIndex= std::max_element(chordMatchesCounts.begin(), chordMatchesCounts.end()) - chordMatchesCounts.begin();
		return currentScaleChords[matchedChordIndex];
	}

	void addChordToMidiBuffer(MidiMessage m,int time,std::vector<int>notesVec,MidiBuffer* midiBufferChords) {
		int add = 0;
		for (auto it = notesVec.begin(); it != notesVec.end(); it++) {
			if (m.isNoteOn()) {
				MidiMessage message = MidiMessage::noteOn(m.getChannel(), *it, m.getVelocity());
				midiBufferChords->addEvent(message,time+add);
				add++;

			}
			else if (m.isNoteOff()) {
				for (auto i = 1; i <= 16; i++)
				{
					midiBufferChords->addEvent(MidiMessage::allNotesOff(i), time + add);
					midiBufferChords->addEvent(MidiMessage::allSoundOff(i), time + add);
					midiBufferChords->addEvent(MidiMessage::allControllersOff(i), time + add);
				}
				add++;

			}
		}
	}
		
	void prepareMelodyToProcess(MidiBuffer* melodyBufferToProcess) {
		setMelodyNotesVectorToProcess(melodyBufferToProcess);
		transposeMelodyNotes(melodyNotesToProcessVector);
	}

	void createChords(MidiBuffer* melodyBufferToProcess, MidiBuffer* midiBufferChords) {
		midiBufferChords->clear();
		MidiMessage m; int time; int vectorIndex=0;
		MidiBuffer*newMidiBufferChords;
		
		for (MidiBuffer::Iterator i(*melodyBufferToProcess); i.getNextEvent(m, time);) {
			
			midiBufferChords->addEvent(m, time);
			Chord* matchedChord = matchChord(melodyNotesToProcessVector[vectorIndex]);
			std::vector<int>notesVec= matchedChord->chordNotesMidiNumbers;
			
			addChordToMidiBuffer(m, time, notesVec, midiBufferChords);
			if (m.isNoteOn() && vectorIndex < melodyNotesToProcessVector.size() - 1) {
				vectorIndex++;
				//DBG(matchedChord->name);
			}
				

		}
	}

	void createChordProgressionOutput(MidiBuffer* midiBufferMelody, MidiBuffer* melodyBufferToProcess, MidiBuffer* midiBufferChords) {
		matchScale(midiBufferMelody);
		prepareMelodyToProcess(melodyBufferToProcess);
		setCurrentScaleChords();//create map containing chords corresponding to each scale step
		createChords(melodyBufferToProcess, midiBufferChords);
	}

	//=======================
	//PROPERTIES
	std::vector<int>melodyNotesVector; //all melody notes vector
	std::set<int>melodyNotesSet;//unique melody notes set
	std::map<std::string, std::vector<int>> chords; //output chord progression
	std::vector<int>melodyNotesToProcessVector; //melody notes to which chord should be added
	
	Scales scales;// scales object
	std::map<int, std::vector<int>> majorScaleChordComponents;//thirds and fifths for each major scale chord
	//std::map<int, std::vector<int>> currentScaleChords;//chords for matched scale
	std::vector<Chord*>currentScaleChords;//chords for matched scale
	
};

