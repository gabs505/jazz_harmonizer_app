
#pragma once
#include <JuceHeader.h>
#include <vector>
#include <set>



class Scales {
public:
	Scales() {
		createScales();
		setNoteNumberToNoteNameMap();
	}

	void createScales() {
		//scalesMap["C"] = std::vector<int>{ 24,26,28,29,31,33,35 }; //C major/a minor
		//scalesMap["G"] = std::vector<int>{ 24,26,28,30,31,33,35 };//G major
		//scalesMap["D"] = std::vector<int>{ 25,26,28,30,31,33,35 };//D major
		//scalesMap["A"] = std::vector<int>{ 25,26,28,30,32,33,35 };//A major
		//scalesMap["E"] = std::vector<int>{ 25,27,28,30,32,33,35 };//E major
		//scalesMap["B"] = std::vector<int>{ 25,27,28,30,32,34,35 };//B major

		scalesMap[0] = std::vector<int>{ 60,62,64,65,67,69,71 }; //C major/a minor
		scalesMap[1] = std::vector<int>{ 60,62,64,66,67,69,71 };//G major
		scalesMap[2] = std::vector<int>{ 61,62,64,66,67,69,71 };//D major
		scalesMap[3] = std::vector<int>{ 61,62,64,66,68,69,71 };//A major
		scalesMap[4] = std::vector<int>{ 61,63,64,66,68,69,71 };//E major
		scalesMap[5] = std::vector<int>{ 61,63,64,66,68,70,71 };//B major


		scalesIndexToName[0] = "C";
		scalesIndexToName[1] = "G";
		scalesIndexToName[2] = "D";
		scalesIndexToName[3] = "A";
		scalesIndexToName[4] = "E";
		scalesIndexToName[5] = "B";
	}

	void setNoteNumberToNoteNameMap() {
		noteNumberToNoteNameMap[60] = "C";
		noteNumberToNoteNameMap[61] = "C#";
		noteNumberToNoteNameMap[62] = "D";
		noteNumberToNoteNameMap[63] = "D#";
		noteNumberToNoteNameMap[64] = "E";
		noteNumberToNoteNameMap[65] = "F";
		noteNumberToNoteNameMap[66] = "F#";
		noteNumberToNoteNameMap[67] = "G";
		noteNumberToNoteNameMap[68] = "G#";
		noteNumberToNoteNameMap[69] = "A";
		noteNumberToNoteNameMap[70] = "A#";
		noteNumberToNoteNameMap[71] = "B";
	}

	std::vector<int> countMatches(std::set<int>melodyNotes) {//returns how many common notes has melody with each scale
		std::vector<int>matchCounts(scalesMap.size(), 0);
		
		for (auto it = melodyNotes.begin(); it != melodyNotes.end(); it++) {
			int index = 0;
			for (auto it2 = scalesMap.begin(); it2 != scalesMap.end(); ++it2) {
				// iter->first = klucz
				// iter->second = wartosc
				std::vector<int>::iterator it3 = std::find(it2->second.begin(), it2->second.end(),*it);
				if (it3 != it2->second.end()) {
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
				maxCounts.push_back(matchCountsVector[counter]);
			counter++;
		}
		DBG("matchCounts vector size:");
		DBG(maxCounts.size());
		if (maxCounts.size() == 1) {
			matchedScaleIndex = maxCounts[0];
			matchedScaleName = scalesIndexToName[matchedScaleIndex];
		}
		else {
			int idx;
			for (auto it = maxCounts.begin(); it != maxCounts.end(); it++) {
				DBG("scalesMap[*it][0]");
				DBG(String(scalesMap[*it][0]));
				DBG("first note in melody:");
				DBG(melodyNotesVector[0]);
				if (scalesMap[*it][0]==melodyNotesVector[0]) {
					matchedScaleIndex = *it;
					matchedScaleName = scalesIndexToName[matchedScaleIndex];

				}
			}
		}
		/*matchedScaleIndex = maxCountIndex;
		matchedScaleName = scalesIndexToName[maxCountIndex];*/
		
		
	}

	
	std::map<int, std::vector < int >> scalesMap;
	std::map<int, std::string> scalesIndexToName;
	std::map<std::string, int> scalesMatches; //iloœæ dŸwiêków wspólnych miêdzy melodi¹ a kolejnymi skalami
	std::string matchedScaleName;
	int matchedScaleIndex;
	std::map<int, std::string> noteNumberToNoteNameMap;
};

class ChordCreator {
public:
	ChordCreator() {
		//middle C -> 60
		majorScaleChordComponents[1] = { 4,7 };
		majorScaleChordComponents[2] = { 3,7 };
		majorScaleChordComponents[3] = { 3,7 };
		majorScaleChordComponents[4] = { 4,7 };
		majorScaleChordComponents[5] = { 4,7 };
		majorScaleChordComponents[6] = { 3,7 };
		majorScaleChordComponents[7] = { 3,6 };

	};

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

	
	void setScaleChords() {//sets currentScaleChords
		std::vector<int> matchedScaleNotes=scales.scalesMap[scales.matchedScaleIndex];
		int index = 0;
		for (auto it = matchedScaleNotes.begin(); it != matchedScaleNotes.end(); ++it) {
			currentScaleChords[index] = {*it,*it+majorScaleChordComponents[index+1][0],*it + majorScaleChordComponents[index + 1][1] };
			
			index++;
		}
		DBG(String(scales.matchedScaleName));
	}

	/*void transposeMelodyToProcessNotes() {//transpose melody notes to lower octave (C4 - middle C)
		for (auto it = melodyNotesToProcessVector.begin(); it != melodyNotesToProcessVector.end(); it++) {
			*it=60+*it%12;//60-middle C note number
		}
	}*/
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

	std::vector<int> matchChord(int midiNoteNumber) {//matches chord to melody note

		std::vector<int>chordMatchesCounts(currentScaleChords.size(), 0);

		for (auto it = currentScaleChords.begin(); it != currentScaleChords.end(); it++) {//iteration through current scale chords map
			
			std::vector<int>::iterator it2 = std::find(it->second.begin(), it->second.end(), midiNoteNumber);
			if (it2 != it->second.end()) {
				chordMatchesCounts[it->first] ++;
			}
			}
		int chordMapKey= std::max_element(chordMatchesCounts.begin(), chordMatchesCounts.end()) - chordMatchesCounts.begin()+1;
		return currentScaleChords[chordMapKey];
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
				/*MidiMessage message = MidiMessage::noteOff(m.getChannel(), *it, m.getVelocity());

				midiBufferChords->addEvent(message, time+add);
				add++;*/
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
			std::vector<int>notesVec= matchChord(melodyNotesToProcessVector[vectorIndex]);
			addChordToMidiBuffer(m, time, notesVec, midiBufferChords);
			//*midiBufferChords = *newMidiBufferChords;
			if(m.isNoteOn()&&vectorIndex< melodyNotesToProcessVector.size()-1)
				vectorIndex++;

		}
	}

	void createChordProgressionOutput(MidiBuffer* midiBufferMelody, MidiBuffer* melodyBufferToProcess, MidiBuffer* midiBufferChords) {
		matchScale(midiBufferMelody);
		prepareMelodyToProcess(melodyBufferToProcess);
		setScaleChords();//create map containing chords corresponding to each scale step
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
	std::map<int, std::vector<int>> currentScaleChords;//chords for matched scale
	
};

