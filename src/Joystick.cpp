#include "Joystick.h"

void Joystick::ReadEvents()
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

Joystick::Joystick() {}

bool Joystick::Load(string path)
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

uint8_t Joystick::GetAxisCount()
{
	return axisCount;
}

uint8_t Joystick::GetButtonCount()
{
	return buttonCount;
}

bool Joystick::GetButtonDown(__u8 id)
{
	ReadEvents();

	if (id >= buttonCount || !IsLoaded())
		return 0;
	return buttons[id];
}

float Joystick::GetAxisValue(__u8 id)
{
	ReadEvents();

	if (id >= axisCount || !IsLoaded())
		return 0;
	return axes[id];
	// blub
}

bool Joystick::IsLoaded()
{
	return js != -1;
}
