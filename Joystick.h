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

	void ReadEvents()
	{
		struct js_event event;

		fd_set set;
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 1;

		FD_ZERO(&set);
		FD_SET(js, &set);

		// TODO: Implement behaviour

		while (select(js + 1, &set, NULL, NULL, &timeout) > 0)
		{
			read(js, &event, sizeof(event));
			// cout << int(event.type) << "\n";
			switch (event.type)
			{
			case JS_EVENT_BUTTON:
				buttons[event.number] = event.value;
				break;
			case JS_EVENT_AXIS:
				axes[event.number] = float(event.value) / 32768.f;
				break;
			}
		}
	}

public:
	Joystick() {}

	bool Load(string path)
	{
		js = open(path.c_str(), O_RDONLY);

		if (js == -1)
		{
			cerr << "Could not open joystick at " << path << endl;
			return 0;
		}

		ioctl(js, JSIOCGAXES, &axisCount);
		ioctl(js, JSIOCGBUTTONS, &buttonCount);

		axes.resize(axisCount);
		buttons.resize(buttonCount);

		return 1;
	}

	uint8_t GetAxisCount()
	{
		return axisCount;
	}

	uint8_t GetButtonCount()
	{
		return buttonCount;
	}

	bool GetButtonDown(__u8 id)
	{
		ReadEvents();

		if (id >= buttonCount)
			return 0;
		return buttons[id];
	}

	float GetAxisValue(__u8 id)
	{
		ReadEvents();

		if (id >= axisCount)
			return 0;
		return axes[id];
	}

	bool IsLoaded()
	{
		return js != -1;
	}
};