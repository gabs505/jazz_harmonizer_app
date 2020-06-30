#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>
#include <math.h>
#include "Melody.h"
#include "Scales.h"
#include "BasicAlgorithms.h"
#include "JazzAlgorithms.h"
#include "ScoringAlgorithms.h"
#include "OtherAlgorithms.h"





class ChordCreator {
public:

	void setChordsToScaleMap(Melody*& melody) {//creating map "scale-[7]chords" e.g. key:"C", value:vector<Chord*>
for (auto it = scales.matchedScales.begin(); it != scales.matchedScales.end(); ++it) {
	setCurrentScaleChords(*it, melody);
}
	}

	void setCurrentScaleChords(Scale* matchedScale, Melody*& melody) {//creates a vector of chord objects for each scale step
		std::vector<int> matchedScaleNotesNumbers = matchedScale->notesMidiNumbers;//notes of current matched scale
		std::vector<std::string> matchedScaleNotesNames = matchedScale->notesNames;//notes names of current matched scale
		std::vector<std::string> matchedScaleChordsModes = matchedScale->scaleChords;//chord modes of current matched scale

		int index = 0;
		std::vector<Chord*>currentScaleChords;

		for (auto it = matchedScaleNotesNumbers.begin(); it != matchedScaleNotesNumbers.end(); ++it) {
			currentScaleChords.push_back(new Chord(matchedScaleNotesNames[index], *it, matchedScaleChordsModes[index], index + 1));
			index++;
		}
		melody->scalesChordsMap.insert(std::pair<std::string, std::vector<Chord*>>(matchedScale->scaleName, currentScaleChords));
	}




	void prepareMelodyToProcess(MidiBuffer* melodyBufferToProcess, Melody*& melody, int quarterNoteLengthInSamples) {
		melody->setMelodyNotesVectorToProcess(melodyBufferToProcess, quarterNoteLengthInSamples);

		melody->transposeMelodyNotesToProcess();
	}

	void matchScales(MidiBuffer* melodyNotes, Melody*& melody, int quarterNoteLengthInSamples) {
		melody->setMelodyNotes(melodyNotes); //creating a set of melody notes
		melody->setMelodyNotesVectorToScaleDetection(melodyNotes, quarterNoteLengthInSamples);
		melody->transposeMelodyNotes();
		scales.findScaleMatches(melody);
		

	}

	//map with chords vector matched to each melody to process note
	void createChordProgressionMatchesMap(Melody*& melody) {
		int melodyNoteNumber = 0;
		int barIdx = 0;
		int chordToneNumber;
		for (auto it = melody->melodyNotesToProcessVector.begin(); it != melody->melodyNotesToProcessVector.end(); ++it) {

			barIdx = it->first;
			std::vector<Chord*>matchedChords;//creating vector for matching chords
			int idx = 0;
			std::vector<Chord*>currentBarScaleChords = melody->scalesChordsMap[scales.matchedScales[barIdx]->scaleName];
			for (auto it2 = currentBarScaleChords.begin(); it2 != currentBarScaleChords.end(); it2++) {//iteration through current scale chords vector
				chordToneNumber = 1;
				for (auto it3 = (*it2)->chordNotesMidiNumbers.begin(); it3 != (*it2)->chordNotesMidiNumbers.end(); ++it3) {//iterating through chord's notes
					if (*it3 % 12 == (it->second) % 12) {
						Chord* chord = new Chord(melody->scalesChordsMap[scales.matchedScales[barIdx]->scaleName][idx]);
						if (chordToneNumber <= 5) {
							chord->priority == 100;
						}
						else if (chordToneNumber <= 9) {
							chord->priority == 10;
						}
						else {
							chord->priority = 1;
						}
						matchedChords.push_back(chord);
					}
					chordToneNumber += 2;
				}
				idx++;
			}
			//if notes out of scale occur in melody
			if (matchedChords.size() == 0) {
				//b9
				int newPrimeNote = 60 + (it->second - 1) % 12;
				for (auto it2 = currentBarScaleChords.begin(); it2 != currentBarScaleChords.end(); ++it2) {
					if ((*it2)->primeMidiNumber == newPrimeNote && ((*it2)->mode == "7" || (*it2)->mode == "m7")) {
						Chord* newChord = new Chord((*it2)->primeNote, newPrimeNote, (*it2)->mode, (*it2)->step);
						newChord->chordNotesMidiNumbers.push_back(newPrimeNote + 13);
						newChord->name += "b9";
						matchedChords.push_back(newChord);
					}
				}
				//+9
				newPrimeNote = 60 + (it->second - 2) % 12;
				for (auto it2 = currentBarScaleChords.begin(); it2 != currentBarScaleChords.end(); ++it2) {
					if ((*it2)->primeMidiNumber == newPrimeNote && ((*it2)->mode == "7" || (*it2)->mode == "m7")) {
						Chord* newChord = new Chord((*it2)->primeNote, newPrimeNote, (*it2)->mode, (*it2)->step);
						newChord->chordNotesMidiNumbers.push_back(newPrimeNote + 14);
						newChord->name += "+9";
						matchedChords.push_back(newChord);
					}
				}
			}
			melody->chordProgressionMatchesMap.insert(std::pair<int, std::vector<Chord*>>(melodyNoteNumber, matchedChords));
			melodyNoteNumber++;
			//initializing vectors of zeros
			int vectorSize = melody->melodyNotesToProcessVector.size();
			melody->chordsInProgression = std::vector<Chord*>(vectorSize, new Chord());
			melody->chordsInProgressionIds = std::vector<int>(vectorSize, -1);
		}
	}

	

	void matchChordInProgression(Melody*& melody) {
		
		int i = 0; 
		
		for (auto it = melody->chordProgressionMatchesMap.begin(); it != melody->chordProgressionMatchesMap.end(); ++it) {
			Chord* matchedChord = new Chord();
			int chordId=0;
			
			otherAlgorithms.checkForChordContinuity(melody, i);
			otherAlgorithms.avoidChoosingDominantOnWeakBeat(melody, i);
			otherAlgorithms.continueTonicFrom251Progression(melody, i);

			std::pair<Chord*, int> chosenChordData = scoringAlgorithms.matchChordBasedOnScore(it->second);
			matchedChord = chosenChordData.first;
			chordId = chosenChordData.second;
			
			if (matchedChord->mode=="7") {
				int j = 0;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
					if ((*it2)->priority == 100) {
						matchedChord = *it2;
						chordId = j;
					}
					j++;
				}
			}
			
			scoringAlgorithms.recountScoreForNextChords(matchedChord,it->second);
			scoringAlgorithms.addScoreForChosenProgression(matchedChord);

			melody->chordsInProgression[i] = new Chord(matchedChord);
			melody->chordsInProgressionIds[i] = chordId;
			i++;
			
		}
	}

	//match final chord progression based on algorithms
	void matchChordProgression(Melody*& melody) {
		
		jazzAlgorithms.searchForHarmonicStructures(melody->chordProgressionMatchesMap);
		basicAlgorithms.applyMajorScheme(melody->chordProgressionMatchesMap, scales.scaleBreakpointsIndexes);
		scoringAlgorithms.countScoreForEachPossibleChord(melody);
		matchChordInProgression(melody);
		
	}



	void addChordToMidiBuffer(MidiMessage m, int time, Chord* matchedChord, MidiBuffer*& midiBufferChords) {
		int add = 0;
		
		int minusValue;
		if (matchedChord->chordNotesMidiNumbers.size() > 3) {
			minusValue = 2;
		}
		else {
			minusValue = 0;
		}
		if (matchedChord->name != "") {
			for (auto it = matchedChord->chordNotesMidiNumbers.begin(); it != matchedChord->chordNotesMidiNumbers.end() - minusValue; it++) {
				//playing higher notes in lower octave
				if (m.isNoteOn()) {
					if (*it > 80) {
						*it -= 12;
					}
				//-------------------------------------
					MidiMessage message = MidiMessage::noteOn(m.getChannel(), *it, (uint8)70);
					midiBufferChords->addEvent(message, time);


				}
				else if (m.isNoteOff()) {
					for (auto i = 1; i <= 16; i++)
					{
						midiBufferChords->addEvent(MidiMessage::allNotesOff(i), time + add - 10);
						midiBufferChords->addEvent(MidiMessage::allSoundOff(i), time + add - 10);
						midiBufferChords->addEvent(MidiMessage::allControllersOff(i), time + add - 10);
					}
					add++;

				}
			}
		}
	
	}


	//creates MidiBuffer with resulting chord progression
	void createChordProgression(MidiBuffer*& melodyBufferToProcess, MidiBuffer*& midiBufferChords, Melody*& melody) {
		MidiMessage m; int time; int index = 0;
		midiBufferChords->clear();
		for (MidiBuffer::Iterator i(*melodyBufferToProcess); i.getNextEvent(m, time);) {

			Chord* matchedChord = melody->chordsInProgression[index];
			addChordToMidiBuffer(m, time, matchedChord, midiBufferChords);
			if (m.isNoteOn() && index < melody->melodyNotesToProcessVector.size() - 1) {
				index++;
			}
		}
	}



	void createChordProgressionOutput(MidiBuffer*& midiBufferMelody, MidiBuffer*& melodyBufferToProcess, MidiBuffer*& midiBufferChords, int quarterNoteLengthInSamples,
		std::vector<std::pair<int, int>>& notesToProcess, std::map<int, std::vector<Chord*>>& possibleChordsMap, std::vector<int>& chordsIds, std::vector<Chord*>& chordsInProgression) {
		Melody* melody = new Melody();
		melody->melodyNotesSet.clear();
		MidiBuffer* newBuffer = new MidiBuffer();

		prepareMelodyToProcess(melodyBufferToProcess, melody, quarterNoteLengthInSamples);
		matchScales(midiBufferMelody, melody, quarterNoteLengthInSamples);

		setChordsToScaleMap(melody);//create map containing chords corresponding to each scale step
		createChordProgressionMatchesMap(melody);
		matchChordProgression(melody);

		createChordProgression(melodyBufferToProcess, midiBufferChords, melody);

		notesToProcess = melody->melodyNotesToProcessVector;//giving Synth Audio Source class access to melodyNotesToProcessVector (GUI use)
		possibleChordsMap = melody->chordProgressionMatchesMap;//giving Synth Audio Source class access to possibleChordsToEachNoteMap (GUI use)
		chordsIds = melody->chordsInProgressionIds;
		chordsInProgression = melody->chordsInProgression;
		delete melody;



	}

	//==========================================================

	Scales scales;// scales object
	BasicAlgorithms basicAlgorithms;
	JazzAlgorithms jazzAlgorithms;
	ScoringAlgorithms scoringAlgorithms;
	OtherAlgorithms otherAlgorithms;

};