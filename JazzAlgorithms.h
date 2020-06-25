#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Chord.h"

class JazzAlgorithms {
public:
	void searchForHarmonicStructures(std::map<int, std::vector<Chord*>>& chordsMap) {
		searchForMajor251(chordsMap);
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
											increment = 11;
										}
										else if (sumOfPriorities > 200) {
											increment = 10;
										}
										else if (sumOfPriorities >= 120) {
											increment = 9;
										}
										else if (sumOfPriorities >= 100) {
											increment = 6;
										}
										else {
											increment = 3;
										}

										(*it2)->score->scoreForMajor251 += increment;//incrementing II chord
										(*it3)->score->scoreForMajor251 += increment;
										(*it4)->score->scoreForMajor251 += increment;
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
											increment = 11;
										}
										else if (sumOfPriorities > 200) {
											increment = 10;
										}
										else if (sumOfPriorities >= 120) {
											increment = 6;
										}
										else if (sumOfPriorities >= 100) {
											increment = 5;
										}
										else {
											increment = 3;
										}

										(*it2)->score->scoreForMinor251 += increment;//incrementing II chord
										(*it3)->score->scoreForMinor251 += increment;
										(*it4)->score->scoreForMinor251 += increment;
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
									increment = 9;
								}
								else if (sumOfPriorities >= 110) {
									increment = 6;
								}
								else{
									increment = 2;
								}
								
								(*it2)->score->scoreForFifthDown += increment;
								(*it3)->score->scoreForFifthDown += increment;
								
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
									increment = 7;
								}
								else if (sumOfPriorities >= 110) {
									increment = 3;
								}
								else {
									increment = 1;
								}

								(*it2)->score->scoreForFourthDown += increment;
								(*it3)->score->scoreForFourthDown += increment;
							}
							j++;
						}

						i++;
				}
			}
			k++;
		}
	}
};