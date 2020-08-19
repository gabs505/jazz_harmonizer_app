
#pragma once

#include<vector>
#include<string>
#include <stdlib.h>
#include <ctime>

std::vector<int>weightsFor251;
std::vector<int>weightsForFifthDown;
std::vector<int>weightsForFourthDown;
int scoreForMajorScheme;
float percentForTSReplacement;
bool replaceWithSD;

std::vector<int> draw(int num) {
	srand((unsigned)time(0));
	std::vector<int> drawnWeights;
	for (int i = 0; i < num; i++) {
		int drawnWeight = (rand() % 24 - 12) * 10;
		drawnWeights.push_back(drawnWeight);
	}
	return drawnWeights;

}

void setWeigths(std::string presetName) {
	if (presetName == "Classic") {
		weightsFor251 = { 0, 0, 0, 0, 0 };
		weightsForFifthDown = { 0,0,0 };
		weightsForFourthDown = { 0,0,0 };
		percentForTSReplacement = 0.5;
		scoreForMajorScheme = 0;
		replaceWithSD = false;

	}
	else if (presetName == "Modern") {
		weightsFor251 = { -40, -20, -16, -12, -10 };
		weightsForFifthDown = { -18,0,20 };
		weightsForFourthDown = { 10,20,30 };
		percentForTSReplacement = 1.0;
		scoreForMajorScheme = 10;
		replaceWithSD = true;
	}
	else if (presetName == "Random") {
		weightsFor251 = draw(5);
		std::vector<int>vec = weightsFor251;
		weightsForFifthDown = draw(3);
		weightsForFourthDown = draw(3);
		scoreForMajorScheme = draw(1)[0];
		replaceWithSD = true;

	}
}

