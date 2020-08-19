#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Chord.h"
#include "otherVariables.h"
#include <stdlib.h>   
#include "PresetsData.h"

class JazzAlgorithms {
public:
	void searchForHarmonicStructures(std::map<int, std::vector<Chord*>>& chordsMap,std::string chosenPreset) {
		setWeigths(chosenPreset);

		searchForMajor251(chordsMap);
		//searchForMajor251Long(chordsMap);
		searchForMinor251(chordsMap);
		searchForFifthDownMovement(chordsMap);
		searchForFourthDownMovement(chordsMap);

	}
	void searchForMajor251(std::map<int, std::vector<Chord*>>& chordsMap) {

		int i = 0;
		int j = 0;
		int k = 0;
		int l = 0;
		int noteNumber = 0; int noteNumber2 = 0;
		

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k < chordsMap.size() - 3) {
				i = 0;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
					j = 0;
					if ((*it2)->mode == "m7") {
						for (auto it3 = chordsMap[k + 1].begin(); it3 != chordsMap[k + 1].end(); ++it3) {//iterating through k+1 vector
							noteNumber = 60 + ((*it2)->primeMidiNumber - 7) % 12;
							if ((*it3)->primeMidiNumber == noteNumber && (*it3)->mode == "7") {
								l = 0;
								for (auto it4 = chordsMap[k + 2].begin(); it4 != chordsMap[k + 2].end(); ++it4) {//iterating through k+2 vector
									noteNumber2 = 60 + ((*it2)->primeMidiNumber - 2) % 12;
									if ((*it4)->primeMidiNumber == noteNumber2) {
										int increment;//score to add based on priority
										int sumOfPriorities = (*it2)->priority + (*it3)->priority + (*it4)->priority;
										if (sumOfPriorities == 300) {
											increment = 11+weightsFor251[0];
										}
										else if (sumOfPriorities > 200) {
											increment = 10 + weightsFor251[1];
										}
										else if (sumOfPriorities >= 120) {
											increment = 9 + weightsFor251[2];
										}
										else if (sumOfPriorities >= 100) {
											increment = 6 + weightsFor251[3];
										}
										else {
											increment = 5 + weightsFor251[4];
										}

										DBG(increment);
										(*it2)->score->scoreForMajor251 += increment;//incrementing II chord
										(*it3)->score->scoreForMajor251 += increment;
										(*it4)->score->scoreForMajor251 += increment;

										//adding pointers to next chords from II-V-I progression
										(*it2)->pointersToNextChordsFromProgression.push_back(*it3);
										(*it2)->pointersToNextChordsFromProgression.push_back(*it4);

										//info to which progression certain chord belongs

										(*it2)->belongsToProgession.push_back("major251");
										(*it3)->belongsToProgession.push_back("major251");
										(*it4)->belongsToProgession.push_back("major251");
									}
									l++;
								}
							}
							j++;
						}
					}
					i++;
				}
			}
			k++;
		}

	}

	/*void searchForMajor251Long(std::map<int, std::vector<Chord*>>& chordsMap) {

		int i = 0;
		int j = 0;
		int k = 0;
		int l = 0;
		int m = 0;
		int n = 0;
		int noteNumber = 0; int noteNumber2 = 0;


		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k < chordsMap.size() - 3) {
				i = 0;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector (II)
					j = 0;
					if ((*it2)->mode == "m7"&&k%2==0) {
						for (auto it3 = chordsMap[k + 1].begin(); it3 != chordsMap[k + 1].end(); ++it3) {//iterating through k+1 vector (II)
							l = 0;
							if ((*it3)->primeMidiNumber == (*it2)->primeMidiNumber) {
								for (auto it4 = chordsMap[k + 2].begin(); it4 != chordsMap[k + 2].end(); ++it4){//iterating through k+2 vector (V)
									noteNumber = 60 + ((*it2)->primeMidiNumber - 7) % 12;
									if ((*it3)->primeMidiNumber == noteNumber && (*it3)->mode == "7") {
										m = 0;
										for (auto it5 = chordsMap[k + 3].begin(); it5 != chordsMap[k + 3].end(); ++it5) {//iterating through k+3 vector (V)
											n = 0;
											if ((*it5)->primeMidiNumber == (*it4)->primeMidiNumber) {
												for (auto it6 = chordsMap[k + 4].begin(); it6 != chordsMap[k + 4].end(); ++it6) {//iterating through k+3 vector (V)
													noteNumber2 = 60 + ((*it2)->primeMidiNumber - 2) % 12;
													if ((*it5)->primeMidiNumber == noteNumber2) {
														int increment;//score to add based on priority
														int sumOfPriorities = (*it2)->priority + (*it3)->priority + (*it4)->priority + (*it5)->priority + (*it6)->priority;
														if (sumOfPriorities == 500) {
															increment = 12;
														}
														else if (sumOfPriorities > 400) {
															increment = 11;
														}
														else if (sumOfPriorities >= 320) {
															increment = 10;
														}
														else if (sumOfPriorities >= 200) {
															increment = 6;
														}
														else {
															increment = 2;
														}

														(*it2)->score->scoreForMajor251Long += increment;//incrementing II chord
														(*it3)->score->scoreForMajor251Long += increment;
														(*it4)->score->scoreForMajor251Long += increment;
														(*it5)->score->scoreForMajor251Long += increment;
														(*it6)->score->scoreForMajor251Long += increment;

														//adding pointers to next chords from II-V-I progression
														(*it2)->pointersToNextChordsFromProgression.push_back(*it3);
														(*it2)->pointersToNextChordsFromProgression.push_back(*it4);
														(*it2)->pointersToNextChordsFromProgression.push_back(*it5);
														(*it2)->pointersToNextChordsFromProgression.push_back(*it6);

														//info to which progression certain chord belongs
														(*it2)->belongsToProgession = "major251Long";
														(*it3)->belongsToProgession = "major251Long";
														(*it4)->belongsToProgession = "major251Long";
														(*it5)->belongsToProgession = "major251Long";
														(*it6)->belongsToProgession = "major251Long";
													}
													n++;
												}
											}
											m++;
										}		
									}
									l++;
								}
							
							}
							j++;
						}
					}
					i++;
				}
			}
			k++;
		}

	}*/

	void searchForMinor251(std::map<int, std::vector<Chord*>>& chordsMap) {

		int i = 0;
		int j = 0;
		int k = 0;
		int l = 0;
		int noteNumber = 0; int noteNumber2 = 0;

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k < chordsMap.size() - 3) {
				i = 0;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
					j = 0;
					if ((*it2)->mode == "m7b5") {
						for (auto it3 = chordsMap[k + 1].begin(); it3 != chordsMap[k + 1].end(); ++it3) {//iterating through k+1 vector
							noteNumber = 60 + ((*it2)->primeMidiNumber - 7) % 12;
							if ((*it3)->primeMidiNumber == noteNumber && (*it3)->mode == "7" && (*it3)->mode == "m7") {
								l = 0;
								for (auto it4 = chordsMap[k + 2].begin(); it4 != chordsMap[k + 2].end(); ++it4) {
									noteNumber2 = 60 + ((*it2)->primeMidiNumber - 2) % 12;
									if ((*it4)->primeMidiNumber == noteNumber2) {
										int increment;//score to add based on priority
										int sumOfPriorities = (*it2)->priority + (*it3)->priority + (*it4)->priority;
										if (sumOfPriorities == 300) {
											increment = 11 + weightsFor251[0];;
										}
										else if (sumOfPriorities > 200) {
											increment = 10 + weightsFor251[1];;
										}
										else if (sumOfPriorities >= 120) {
											increment = 6 + weightsFor251[2];;
										}
										else if (sumOfPriorities >= 100) {
											increment = 5 + weightsFor251[3];;
										}
										else {
											increment = 5 + weightsFor251[4];;
										}

										(*it2)->score->scoreForMinor251 += increment;//incrementing II chord
										(*it3)->score->scoreForMinor251 += increment;
										(*it4)->score->scoreForMinor251 += increment;

										//adding pointers to next chords from II-V-I progression
										(*it2)->pointersToNextChordsFromProgression.push_back(*it3);
										(*it2)->pointersToNextChordsFromProgression.push_back(*it4);

										//info to which progression certain chord belongs
										(*it2)->belongsToProgession.push_back("minor251");
										(*it3)->belongsToProgession.push_back("minor251");
										(*it4)->belongsToProgession.push_back("minor251");
									}
									l++;
								}
							}
							j++;
						}
					}
					i++;
				}
			}
			k++;
		}

	}

	void searchForFifthDownMovement(std::map<int, std::vector<Chord*>>& chordsMap) {
		int k = 0;//note of melody index
		int i = 0;//k chord index
		int j = 0;//k-1 chord index

		int noteNumber;

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k > 0) {
					i = 0;
					for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
						noteNumber = 60 + ((*it2)->primeMidiNumber + 7) % 12;
						j = 0;
						for (auto it3 = chordsMap[k - 1].begin(); it3 != chordsMap[k - 1].end(); it3++) {//iterating through k-1 chord vector
							if ((*it3)->primeMidiNumber == noteNumber){
								int increment;//score to add based on priority
								int sumOfPriorities = (*it2)->priority + (*it3)->priority;
								if (sumOfPriorities == 200) {
									increment = 9+weightsForFifthDown[0];
								}
								else if (sumOfPriorities >= 110) {
									increment = 6 + weightsForFifthDown[1];
								}
								else{
									increment = 3 + weightsForFifthDown[2];
								}
								
								(*it2)->score->scoreForFifthDown += increment;
								(*it3)->score->scoreForFifthDown += increment;

								//adding pointers to next chords from V-I progression
								(*it2)->pointersToNextChordsFromProgression.push_back(*it3);

								//info to which progression certain chord belongs
								(*it2)->belongsToProgession.push_back("fifthDown");
								(*it3)->belongsToProgession.push_back("fifthDown");
								
								
							}
							j++;
						}

						i++;
					}
				}

			}
			k++;
	}


	void searchForFourthDownMovement(std::map<int, std::vector<Chord*>>& chordsMap) {
		int k = 0;//note of melody index
		int i = 0;//k chord index
		int j = 0;//k-1 chord index

		int noteNumber;

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k > 0) {
				
				i = 0;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
						noteNumber = 60 + ((*it2)->primeMidiNumber + 5) % 12;
						j = 0;
						for (auto it3 = chordsMap[k - 1].begin(); it3 != chordsMap[k - 1].end(); it3++) {//iterating through k-1 chord vector
							if ((*it3)->primeMidiNumber == noteNumber) {
								int increment;//score to add based on priority
								int sumOfPriorities = (*it2)->priority + (*it3)->priority;
								if (sumOfPriorities == 200) {
									increment = 7+ weightsForFourthDown[0];
								}
								else if (sumOfPriorities >= 110) {
									increment = 2 + weightsForFourthDown[1];
								}
								else {
									increment = 1 + weightsForFourthDown[2];
								}

								(*it2)->score->scoreForFourthDown += increment;
								(*it3)->score->scoreForFourthDown += increment;

								//adding pointers to next chords from IV-I progression
								(*it2)->pointersToNextChordsFromProgression.push_back(*it3);

								//info to which progression certain chord belongs
								(*it2)->belongsToProgession.push_back("fourthDown");
								(*it3)->belongsToProgession.push_back("fourthDown");
							}
							j++;
						}

						i++;
				}
			}
			k++;
		}
	}

	void replaceChordsWithSecondaryDominants(Melody* &melody) {
		int i = 1;
		for (auto it = melody->chordsInProgression.begin()+1; it != melody->chordsInProgression.end()-1; ++it) {
			Chord* secondaryDominant;
			bool isIn251=false;
			if(std::count((*it)->belongsToProgession.begin(), (*it)->belongsToProgession.end(),"major251")!=0){
				isIn251 = true;
			}
			if ((*it)->name == melody->chordsInProgression[i - 1]->name && i%2!=0&&isIn251==false) {
				Chord* nextChord = melody->chordsInProgression[i + 1];
				auto it_ncPrime = std::find(sharps.begin(), sharps.end(), nextChord->primeNote);
				int ncPrimeIdx= std::distance(sharps.begin(), it_ncPrime);
				std::string sdPrimeName;
				int newIdx = ncPrimeIdx + 7;
				if (newIdx > sharps.size() - 1) {
					sdPrimeName = sharps[newIdx % sharps.size()];
				}
				else {
					sdPrimeName = sharps[newIdx];
				}

				secondaryDominant = new Chord(sdPrimeName,nextChord->primeMidiNumber+7,"7");

				bool isCollapsing = false; //is chord note a semitone away from melody note
				bool hasMelodyNote = false;
				for (auto it2 = secondaryDominant->chordNotesMidiNumbers.begin(); it2 != secondaryDominant->chordNotesMidiNumbers.end(); ++it2) {
					if (abs(60 + *it2 % 12 - 60 + melody->melodyNotesToProcess[i] % 12)==1) {
						isCollapsing = true;
					}
					if (60 + *it2 % 12 == 60 + melody->melodyNotesToProcess[i] % 12) {
						hasMelodyNote = true;
					}
				}
				if (!isCollapsing && hasMelodyNote) {
					melody->chordsInProgression[i] = secondaryDominant;
					melody->chordProgressionMatchesMap[i].push_back(secondaryDominant);
					melody->chordsInProgressionIds[i] = melody->chordProgressionMatchesMap[i].size() - 1;
				}
				
			}
			i++;
		}
	}

	void replaceDominantWithTritoneSubstitute(Melody*& melody) {
		//V chord indexes
		std::vector<int>dominantChordIndexes;
		int i = 0;
		for (auto it = melody->chordsInProgression.begin(); it != melody->chordsInProgression.end(); ++it) {
			//std::count((*it)->belongsToProgession.begin(), (*it)->belongsToProgession.end(), "major251") != 0
			if ((*it)->mode == "7" && (*it)->overallScore>=1000) {
				
				dominantChordIndexes.push_back(i);
			}
			i++;
		}

		int numOfReplacements = ceil(percentForTSReplacement * (float)dominantChordIndexes.size());
		std::vector<int>drawnDominantIndexes;
		//randomly choosing indexes
		for (int j = 0; j < numOfReplacements; j++) {
			int randIdx = rand() % dominantChordIndexes.size();
			while (std::count(drawnDominantIndexes.begin(),drawnDominantIndexes.end(), dominantChordIndexes[randIdx])!=0) {
				randIdx = rand() % dominantChordIndexes.size();
			}
			drawnDominantIndexes.push_back(dominantChordIndexes[randIdx]);
		}

		for (auto it = drawnDominantIndexes.begin(); it != drawnDominantIndexes.end(); ++it) {
			Chord* tritoneSubstitute;
			Chord* currentChord = melody->chordsInProgression[*it];
			auto it_ccPrime = std::find(sharps.begin(), sharps.end(), currentChord->primeNote);
			int ncPrimeIdx = std::distance(sharps.begin(), it_ccPrime);
			std::string tsPrimeName;
			int newIdx = ncPrimeIdx + 6;
			if (newIdx > sharps.size() - 1) {
				tsPrimeName = sharps[newIdx % sharps.size()];
			}
			else {
				tsPrimeName = sharps[newIdx];
			}

			tritoneSubstitute = new Chord(tsPrimeName, currentChord->primeMidiNumber + 6, "7");

			bool hasMelodyNote = false;
			for (auto it2 = tritoneSubstitute->chordNotesMidiNumbers.begin(); it2 != tritoneSubstitute->chordNotesMidiNumbers.end(); ++it2) {
				if (60 + *it2 % 12 == 60 + melody->melodyNotesToProcess[*it] % 12) {
					hasMelodyNote = true;
				}
			}
			
			if (hasMelodyNote) {
				melody->chordsInProgression[*it] = tritoneSubstitute;
				melody->chordProgressionMatchesMap[*it].push_back(tritoneSubstitute);
				melody->chordsInProgressionIds[*it] = melody->chordProgressionMatchesMap[*it].size() - 1;
			}
			
			
		}
	}
};