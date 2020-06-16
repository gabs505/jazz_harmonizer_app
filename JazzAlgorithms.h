#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Chord.h"

class JazzAlgorithms {
public:
	void searchForMajor251(std::map<int, std::vector<Chord*>>& chordsMap, std::vector<Chord*>& chordsInProgression, std::vector<int>& chordsInProgressionIds) {

		int i = 0;
		int j = 0;
		int k = 0;
		int l = 0;
		int noteNumber = 0; int noteNumber2 = 0;
		std::vector<int>chosenChordIndexes(chordsMap.size(), 100);//100 - akord na tym miejscu nie zosta³ jeszcze wybrany

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k < chordsMap.size() - 3) {
				i = 0;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
					j = 0;
					if ((*it2)->mode == "m7" && chosenChordIndexes[k] == 100) {
						for (auto it3 = chordsMap[k + 1].begin(); it3 != chordsMap[k + 1].end(); ++it3) {//iterating through k+1 vector
							noteNumber = 60 + ((*it2)->primeMidiNumber - 7) % 12;
							if ((*it3)->primeMidiNumber == noteNumber && (*it3)->mode == "7") {
								l = 0;
								for (auto it4 = chordsMap[k + 2].begin(); it4 != chordsMap[k + 2].end(); ++it4) {//iterating through k+2 vector
									noteNumber2 = 60 + ((*it2)->primeMidiNumber - 2) % 12;
									if ((*it4)->primeMidiNumber == noteNumber2) {

										chordsInProgression[k] = chordsMap[k][i];
										chordsInProgressionIds[k] = i;
										chosenChordIndexes[k] = i;

										chordsInProgression[k + 1] = chordsMap[k + 1][j];
										chordsInProgressionIds[k + 1] = j;
										chosenChordIndexes[k + 1] = j;

										chordsInProgression[k + 2] = chordsMap[k + 2][l];
										chordsInProgressionIds[k + 2] = l;
										chosenChordIndexes[k + 2] = l;
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

	void searchForMinor251(std::map<int, std::vector<Chord*>>& chordsMap, std::vector<Chord*>& chordsInProgression, std::vector<int>& chordsInProgressionIds) {

		int i = 0;
		int j = 0;
		int k = 0;
		int l = 0;
		int noteNumber = 0; int noteNumber2 = 0;
		std::vector<int>chosenChordIndexes(chordsMap.size(), 100);//100 - akord na tym miejscu nie zosta³ jeszcze wybrany

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k < chordsMap.size() - 3) {
				i = 0;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
					j = 0;
					if ((*it2)->mode == "m7b5" && chosenChordIndexes[k] == 100) {
						if (k == 7) {
							DBG("X");
						}
						for (auto it3 = chordsMap[k + 1].begin(); it3 != chordsMap[k + 1].end(); ++it3) {//iterating through k+1 vector
							noteNumber = 60 + ((*it2)->primeMidiNumber - 7) % 12;
							if ((*it3)->primeMidiNumber == noteNumber && (*it3)->mode == "7" && (*it3)->mode == "m7") {
								l = 0;
								for (auto it4 = chordsMap[k + 2].begin(); it4 != chordsMap[k + 2].end(); ++it4) {
									noteNumber2 = 60 + ((*it2)->primeMidiNumber - 2) % 12;
									if ((*it4)->primeMidiNumber == noteNumber2) {

										chordsInProgression[k] = chordsMap[k][i];
										chordsInProgressionIds[k] = i;
										chosenChordIndexes[k] = i;

										//changing V minor chord to dominant
										if (chordsMap[k + 1][j]->mode == "m7") {
											chordsMap[k + 1][j] = new Chord(chordsMap[k + 1][j]->primeNote, chordsMap[k + 1][j]->primeMidiNumber, "7", chordsMap[k + 1][j]->step);
										}
										chordsInProgression[k + 1] = chordsMap[k + 1][j];
										chordsInProgressionIds[k + 1] = j;
										chosenChordIndexes[k + 1] = j;

										chordsInProgression[k + 2] = chordsMap[k + 2][l];
										chordsInProgressionIds[k + 2] = l;
										chosenChordIndexes[k + 2] = l;
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

	void searchForFifthDownMovement(std::map<int, std::vector<Chord*>>& chordsMap, std::vector<Chord*>& chordsInProgression, std::vector<int>& chordsInProgressionIds) {
		int k = 0;//note of melody index
		int i = 0;//k chord index
		int j = 0;//k-1 chord index

		int noteNumber;

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k > 0) {
				//k chord is empty and k-1 chord is already chosen
				if (chordsInProgressionIds[k] == -1 && chordsInProgressionIds[k - 1] != -1) {
					i = 0;
					for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
						noteNumber = 60 + ((*it2)->primeMidiNumber + 7) % 12;

						if (chordsInProgression[k - 1]->primeMidiNumber == noteNumber) {

							chordsInProgression[k] = chordsMap[k][i];
							chordsInProgressionIds[k] = i;

						}
						i++;
					}
				}
				//both k and k-1 chords aren't chosen yet
				else if (chordsInProgressionIds[k] == -1 && chordsInProgressionIds[k - 1] == -1) {
					i = 0;
					for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
						noteNumber = 60 + ((*it2)->primeMidiNumber + 7) % 12;
						j = 0;
						for (auto it3 = chordsMap[k - 1].begin(); it3 != chordsMap[k - 1].end(); it3++) {//iterating through k-1 chord vector
							if ((*it3)->primeMidiNumber == noteNumber) {
								chordsInProgression[k] = chordsMap[k][i];
								chordsInProgressionIds[k] = i;

								chordsInProgression[k - 1] = chordsMap[k - 1][j];
								chordsInProgressionIds[k - 1] = j;
							}
							j++;
						}

						i++;
					}
				}

			}

			k++;
		}
	}


	void searchForFourthDownMovement(std::map<int, std::vector<Chord*>>& chordsMap, std::vector<Chord*>& chordsInProgression, std::vector<int>& chordsInProgressionIds) {
		int k = 0;//note of melody index
		int i = 0;//k chord index
		int j = 0;//k-1 chord index

		int noteNumber;

		for (auto it = chordsMap.begin(); it != chordsMap.end(); ++it) {
			if (k > 0) {
				//k chord is empty and k-1 chord is already chosen
				if (chordsInProgressionIds[k] == -1 && chordsInProgressionIds[k - 1] != -1) {
					i = 0;
					for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
						noteNumber = 60 + ((*it2)->primeMidiNumber + 5) % 12;

						if (chordsInProgression[k - 1]->primeMidiNumber == noteNumber) {

							chordsInProgression[k] = chordsMap[k][i];
							chordsInProgressionIds[k] = i;

						}
						i++;
					}
				}
				//both k and k-1 chords aren't chosen yet
				else if (chordsInProgressionIds[k] == -1 && chordsInProgressionIds[k - 1] == -1) {
					i = 0;
					for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {//iterating through k chord vector
						noteNumber = 60 + ((*it2)->primeMidiNumber + 5) % 12;
						j = 0;
						for (auto it3 = chordsMap[k - 1].begin(); it3 != chordsMap[k - 1].end(); it3++) {//iterating through k-1 chord vector
							if ((*it3)->primeMidiNumber == noteNumber) {
								chordsInProgression[k] = chordsMap[k][i];
								chordsInProgressionIds[k] = i;

								chordsInProgression[k - 1] = chordsMap[k - 1][j];
								chordsInProgressionIds[k - 1] = j;
							}
							j++;
						}

						i++;
					}
				}

			}

			k++;
		}
	}
};