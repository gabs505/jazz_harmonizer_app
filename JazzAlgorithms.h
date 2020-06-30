#pragma once

#include <JuceHeader.h>
#include <vector>
#include <set>
#include "Chord.h"

class JazzAlgorithms {
public:
	void searchForHarmonicStructures(std::map<int, std::vector<Chord*>>& chordsMap) {
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

										//adding pointers to next chords from II-V-I progression
										(*it2)->pointersToNextChordsFromProgression.push_back(*it3);
										(*it2)->pointersToNextChordsFromProgression.push_back(*it4);

										//info to which progression certain chord belongs
										(*it2)->belongsToProgession = "major251";
										(*it3)->belongsToProgession = "major251";
										(*it4)->belongsToProgession = "major251";
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

										//adding pointers to next chords from II-V-I progression
										(*it2)->pointersToNextChordsFromProgression.push_back(*it3);
										(*it2)->pointersToNextChordsFromProgression.push_back(*it4);

										//info to which progression certain chord belongs
										(*it2)->belongsToProgession = "minor251";
										(*it3)->belongsToProgession = "minor251";
										(*it4)->belongsToProgession = "minor251";
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

								//adding pointers to next chords from V-I progression
								(*it2)->pointersToNextChordsFromProgression.push_back(*it3);

								//info to which progression certain chord belongs
								(*it2)->belongsToProgession = "fifthDown";
								(*it3)->belongsToProgession = "fifthDown";
								
								
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

								//adding pointers to next chords from IV-I progression
								(*it2)->pointersToNextChordsFromProgression.push_back(*it3);

								//info to which progression certain chord belongs
								(*it2)->belongsToProgession = "fourthDown";
								(*it3)->belongsToProgession = "fourthDown";
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