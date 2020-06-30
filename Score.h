#pragma once

#include <vector>

class Score {
public:
	Score() {
		scoreForMajor251 = 0;
		scoreForMinor251=0;
		scoreForFifthDown = 0;
		scoreForFourthDown = 0;
		otherScore = 0;
		scoreForMajor251Long = 0;
		scoreForMajorScheme = 0;
	}

	int addScore() {
		return scoreForMajor251 + scoreForMinor251 + scoreForFifthDown + scoreForFourthDown + scoreForMajorScheme + otherScore;// +scoreForMajor251Long;
	}

	int scoreForMajor251;
	int scoreForMajor251Long;
	int scoreForMinor251;
	int scoreForFifthDown;
	int scoreForFourthDown;
	int otherScore;
	int scoreForMajorScheme;
	int overallScore;
};