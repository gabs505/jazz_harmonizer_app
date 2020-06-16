#pragma once


#include<vector>
#include<map>
#include<string>

using namespace std;

vector<string>sharps = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
vector<string>flats = { "C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B" };

map<string, int>noteNameToMidiNumberMap = { {"C",60},{"C#",61},{"Db",61},{"D",62} ,{"D#",63} ,{"Eb",63} ,{"E",64} ,{"E#",65} ,{"Fb",64} ,{"F",65},
{"F#",66} ,{"Gb",66} ,{"G",67} ,{"G#",68} ,{"Ab",68} ,{"A",69} ,{"A#",70} ,{"Bb",70},{"B",71},{"Cb",71} };