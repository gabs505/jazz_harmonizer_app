#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>
#include <math.h>
#include "Melody.h"
#include "Scales.h"
#include "BasicAlgorithms.h"





class ChordCreator {
public:

	void setCurrentScaleChords(Scale* matchedScale,Melody*& melody) {//creates a vector of chord objects for each scale step
		std::vector<int> matchedScaleNotesNumbers = matchedScale->notesMidiNumbers;//notes of current matched scale
		std::vector<std::string> matchedScaleNotesNames = matchedScale->notesNames;//notes names of current matched scale
		std::vector<std::string> matchedScaleChordsModes = matchedScale->scaleChords;//chord modes of current matched scale

		int index = 0;
		std::vector<Chord*>currentScaleChords;

		for (auto it = matchedScaleNotesNumbers.begin(); it != matchedScaleNotesNumbers.end(); ++it) {
			currentScaleChords.push_back(new Chord(matchedScaleNotesNames[index], *it, matchedScaleChordsModes[index], index + 1));
			index++;
		}
		melody->scalesChordsMap.insert(std::pair<std::string,std::vector<Chord*>>(matchedScale->scaleName, currentScaleChords));
	}


	void setMelodyNotes(MidiBuffer* melodyNotes, Melody* &melody) { //creates melodyNotesSet and melodyNotesVector from MidiBuffer
		MidiMessage m; int time;
		for (MidiBuffer::Iterator i(*melodyNotes); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				melody->melodyNotesSet.insert(m.getNoteNumber());
				melody->melodyNotesVector.push_back(m.getNoteNumber());
			}
		}

	}

	//notes to which chords should be added
	void setMelodyNotesVectorToProcess(MidiBuffer* melodyNotes, Melody*& melody,int quarterNoteLengthInSamples) {
		
		MidiMessage m; int time;
		float barLength = (float)quarterNoteLengthInSamples * 8.0;
		float actualBar = barLength;
		int barIdx = 0;
		int modulo;
		
		for (MidiBuffer::Iterator i(*melodyNotes); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				if (time >= (int)actualBar) {
					actualBar += barLength;
					barIdx++;
				}
				melody->melodyNotesToProcessVector.push_back(std::pair<int, int>(barIdx, m.getNoteNumber()));
			}
			
		}	
	}


	

	void transposeMelodyNotes(std::vector<int>notes) {
		for (auto it = notes.begin(); it != notes.end(); it++) {
			*it = 60 + *it % 12;//60-middle C note number
		}
	}

	void transposeMelodyNotesToProcess(std::vector<std::pair<int,int>>notes) {
		for (auto it = notes.begin(); it != notes.end(); it++) {
			(it->second) = 60 + (it->second) % 12;//60-middle C note number
		}
	}

	void prepareMelodyToProcess(MidiBuffer* melodyBufferToProcess, Melody*& melody,int quarterNoteLengthInSamples) {
		setMelodyNotesVectorToProcess(melodyBufferToProcess, melody,quarterNoteLengthInSamples);

		transposeMelodyNotesToProcess(melody->melodyNotesToProcessVector);
	}

	void matchScales(MidiBuffer* melodyNotes, Melody* &melody,int quarterNoteLengthInSamples) {
		setMelodyNotes(melodyNotes,melody); //creating a set of melody notes
		melody->setMelodyNotesVectorToScaleDetection(melodyNotes, quarterNoteLengthInSamples);
		transposeMelodyNotes(melody->melodyNotesVector);
		scales.findScaleMatches(melody);
		
		
	}


	void createChordProgressionMatchesMap(Melody* &melody) {
		int melodyNoteNumber = 0;
		int barIdx=0;
		for (auto it = melody->melodyNotesToProcessVector.begin(); it != melody->melodyNotesToProcessVector.end(); ++it) {
			barIdx = it->first;
			std::vector<Chord*>matchedChords;//creating vector for matching chords
			int idx = 0;
			for (auto it2 = melody->scalesChordsMap[scales.matchedScales[barIdx]->scaleName].begin(); it2 != melody->scalesChordsMap[scales.matchedScales[barIdx]->scaleName].end(); it2++) {//iteration through current scale chords vector

				for (auto it3 = (*it2)->chordNotesMidiNumbers.begin(); it3 != (*it2)->chordNotesMidiNumbers.end(); ++it3) {//iterating through chord's notes
					if (*it3 % 12 == (it->second) % 12)
						matchedChords.push_back(melody->scalesChordsMap[scales.matchedScales[barIdx]->scaleName][idx]);
				}
				idx++;
			}
			melody->chordProgressionMatchesMap.insert(std::pair<int, std::vector<Chord*>>(melodyNoteNumber, matchedChords));
			melodyNoteNumber++;
			//initializing vectors of zeros
			int vectorSize = melody->melodyNotesToProcessVector.size();
			melody->chordsInProgression = std::vector<Chord*>(vectorSize,new Chord());
			melody->chordsInProgressionIds = std::vector<int>(vectorSize,-1 );
		}

	}
	void matchChordProgression(Melody* &melody) {
		basicAlgorithms.checkForHarmonicTriad(melody->chordProgressionMatchesMap, melody->chordsInProgression, melody->chordsInProgressionIds);
		int idx = 0;

		std::map<int, Chord*>matchedChordDataVector;
		std::map<int, Chord*>::iterator it2;
		for (auto it = melody->chordProgressionMatchesMap.begin(); it != melody->chordProgressionMatchesMap.end(); ++it) {
			if (melody->chordsInProgressionIds[idx] == -1) {
				matchedChordDataVector = basicAlgorithms.chooseChordBasedOnPriority(it->second);
				it2 = matchedChordDataVector.begin();
				melody->chordsInProgressionIds[idx] = it2->first;
				melody->chordsInProgression[idx] = it2->second;
			}
			
			idx++;
			
		}

	}
	

	
	void addChordToMidiBuffer(MidiMessage m, int time, Chord* matchedChord, MidiBuffer* &midiBufferChords) {
		int add = 0;
		for (auto it =matchedChord->chordNotesMidiNumbers.begin(); it != matchedChord->chordNotesMidiNumbers.end(); it++) {
			if (m.isNoteOn()) {
				MidiMessage message = MidiMessage::noteOn(m.getChannel(), *it, (uint8)70);
				midiBufferChords->addEvent(message, time);
				add++;

			}
			else if (m.isNoteOff()) {
				for (auto i = 1; i <= 16; i++)
				{
					midiBufferChords->addEvent(MidiMessage::allNotesOff(i), time+add-10);
					midiBufferChords->addEvent(MidiMessage::allSoundOff(i), time+add-10);
					midiBufferChords->addEvent(MidiMessage::allControllersOff(i), time+add-10);
				}
				add++;

			}
		}
	}

	

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

	void setChordsToScaleMap(Melody*&melody) {//creating map "scale-[7]chords" e.g. key:"C", value:vector<Chord*>
		for (auto it = scales.matchedScales.begin(); it != scales.matchedScales.end(); ++it) {
			setCurrentScaleChords(*it,melody);
		}
	}

	void createChordProgressionOutput(MidiBuffer*& midiBufferMelody, MidiBuffer*& melodyBufferToProcess, MidiBuffer*& midiBufferChords,int quarterNoteLengthInSamples,
		std::vector<std::pair<int,int>>& notesToProcess, std::map<int, std::vector<Chord*>>& possibleChordsMap, std::vector<int>& chordsIds, std::vector<Chord*>&chordsInProgression) {
		Melody* melody = new Melody();
		melody->melodyNotesSet.clear();
		MidiBuffer* newBuffer = new MidiBuffer();

		prepareMelodyToProcess(melodyBufferToProcess, melody, quarterNoteLengthInSamples);
		matchScales(midiBufferMelody,melody,quarterNoteLengthInSamples);
		
		
		notesToProcess = melody->melodyNotesToProcessVector;//giving Synth Audio Source class access to melodyNotesToProcessVector (GUI use)
		setChordsToScaleMap(melody);//create map containing chords corresponding to each scale step

		createChordProgressionMatchesMap(melody);


		possibleChordsMap = melody->chordProgressionMatchesMap;//giving Synth Audio Source class access to possibleChordsToEachNoteMap (GUI use)

		matchChordProgression(melody);

		createChordProgression(melodyBufferToProcess, midiBufferChords, melody);
		chordsIds = melody->chordsInProgressionIds;
		chordsInProgression = melody->chordsInProgression;
		delete melody;



	}

	//==========================================================

	Scales scales;// scales object
	BasicAlgorithms basicAlgorithms;

};
