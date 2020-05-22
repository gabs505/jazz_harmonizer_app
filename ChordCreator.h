
#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>

#include "Scales.h"
#include "BasicAlgorithms.h"



class ChordCreator {
public:

	void setCurrentScaleChords() {//creates a vector of chord objects for each scale step
		std::vector<int> matchedScaleNotesNumbers = scales.matchedScale->notesMidiNumbers;//notes of current matched scale
		std::vector<std::string> matchedScaleNotesNames = scales.matchedScale->notesNames;//notes names of current matched scale
		std::vector<std::string> matchedScaleChordsModes = scales.matchedScale->scaleChords;//chord modes of current matched scale

		int index = 0;
		
		for (auto it = matchedScaleNotesNumbers.begin(); it != matchedScaleNotesNumbers.end(); ++it) {
			currentScaleChords.push_back(new Chord(matchedScaleNotesNames[index], *it, matchedScaleChordsModes[index], index + 1));
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

	//notes to which chords should be added
	void setMelodyNotesVectorToProcess(MidiBuffer* melody) {
		MidiMessage m; int time;
		for (MidiBuffer::Iterator i(*melody); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				melodyNotesToProcessVector.push_back(m.getNoteNumber());
			}
		}
	}
	

	int findMostFrequentMelodyNote() {
		std::map<int, int>noteOccurrences;//number of occurences in melody for each note

		for (auto it = melodyNotesVector.begin(); it != melodyNotesVector.end(); ++it) {
			//checking if map already contains this note
			if (noteOccurrences.count(*it)==0) {
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
				mostFrequentMidiNote=it2->first;
			}
		}
		return mostFrequentMidiNote;
	}
	
	void transposeMelodyNotes(std::vector<int>notes) {
		for (auto it = notes.begin(); it != notes.end(); it++) {
			*it = 60 + *it % 12;//60-middle C note number
		}
	}

	void matchScale(MidiBuffer* melody) {
		setMelodyNotes(melody); //creating a set of melody notes
		transposeMelodyNotes(melodyNotesVector);
		int mostFrequentMelodyNote = findMostFrequentMelodyNote();
		scales.findScaleMatch(melodyNotesSet,melodyNotesVector,mostFrequentMelodyNote);//matching scale to melody
	}

	Chord* matchChord(int midiNoteNumber,bool isNoteOn) {//matches chord to melody note

		std::vector<int>chordMatchesCounts(currentScaleChords.size(), 0);//creating vector of zeros of currentScaleChords size
		int idx = 0;
		for (auto it = currentScaleChords.begin(); it != currentScaleChords.end(); it++) {//iteration through current scale chords vector
			
			for (auto it2 = (*it)->chordNotesMidiNumbers.begin(); it2 != (*it)->chordNotesMidiNumbers.end(); ++it2) {//iterating through chord's notes
				if (*it2 == midiNoteNumber)
					chordMatchesCounts[idx]++;

			}
			
			idx++;
		}
			
		//finding max in chordMatchesCounts vector
		std::vector<int>matchingChordsIndexesVector;
		int maxIndex= std::max_element(chordMatchesCounts.begin(), chordMatchesCounts.end()) - chordMatchesCounts.begin();
		idx = 0;
		for (auto it = chordMatchesCounts.begin(); it != chordMatchesCounts.end(); ++it) {//checking which chords fit the most to the melody note
			if (*it == chordMatchesCounts[maxIndex])
				matchingChordsIndexesVector.push_back(idx);
			idx++;
		}

		int matchedChordIndex;
		int prior = 0;
		if (chordsInProgression.size() > 0) {
			for (auto it = matchingChordsIndexesVector.begin(); it != matchingChordsIndexesVector.end(); ++it) {
				basicAlgorithms.checkForHarmonicTriad(currentScaleChords[*it], chordsInProgression);
			}
			
		}
		
		for (auto it = matchingChordsIndexesVector.begin(); it != matchingChordsIndexesVector.end(); ++it) {//checking for highest chord priority
			if (currentScaleChords[*it]->priority > prior) {
					prior = currentScaleChords[*it]->priority;
					matchedChordIndex = *it;
			}
		}
		
		
		
		//DBG(currentScaleChords[matchedChordIndex]->chordNotesMidiNumbers.size());
		if(isNoteOn)
			chordsInProgression.push_back(currentScaleChords[matchedChordIndex]);
		return currentScaleChords[matchedChordIndex];
	}

	void addChordToMidiBuffer(MidiMessage m,int time,std::vector<int>notesVec,MidiBuffer* midiBufferChords) {
		int add = 0;
		for (auto it = notesVec.begin(); it != notesVec.end(); it++) {
			if (m.isNoteOn()) {
				MidiMessage message = MidiMessage::noteOn(m.getChannel(), *it, (uint8) 70);
				midiBufferChords->addEvent(message,time+add);
				add++;

			}
			else if (m.isNoteOff()) {
				for (auto i = 1; i <= 16; i++)
				{
					midiBufferChords->addEvent(MidiMessage::allNotesOff(i), time + add-10);
					midiBufferChords->addEvent(MidiMessage::allSoundOff(i), time + add-10);
					midiBufferChords->addEvent(MidiMessage::allControllersOff(i), time + add-10);
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
			Chord* matchedChord = matchChord(melodyNotesToProcessVector[vectorIndex],m.isNoteOn());
			std::vector<int>notesVec= matchedChord->chordNotesMidiNumbers;
			//DBG(notesVec.size());
			
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
	std::vector<Chord*>currentScaleChords;//chords for matched scale
	std::vector<Chord*>chordsInProgression;//chords which occured earlier in progression
	
	BasicAlgorithms basicAlgorithms;

	std::map<int, Chord*>possibleChordsToEachNoteMap; // map containing chords for each note which fit
};

