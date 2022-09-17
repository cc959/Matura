#pragma once

#include <bits/stdc++.h>
#include <linux/joystick.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <poll.h>

using namespace std;

class Joystick
{
private:
	int js = -1;

	uint8_t axisCount = -1, buttonCount = -1;

	vector<bool> buttons;
	vector<float> axes;

	struct js_event
	{
		__u32 time; // event timestamp in ms
		__s16 value;
		__u8 type;
		__u8 number;
	};

	void ReadEvents();

public:
	Joystick();

	bool Load(string path);

	uint8_t GetAxisCount();

	uint8_t GetButtonCount();

	bool GetButtonDown(__u8 id);

	float GetAxisValue(__u8 id);

	bool IsLoaded();
};