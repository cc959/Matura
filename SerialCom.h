#pragma once

#include <bits/stdc++.h>

using namespace std;

class SerialCom
{

public:
	SerialCom(string path);

	void writeBytes(vector<unsigned char> data);
};
