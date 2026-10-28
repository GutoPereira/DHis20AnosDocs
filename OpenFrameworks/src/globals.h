#pragma once
#include <algorithm>
#include <ofLog.h>
#include <ofXml.h>
#include <string>
#include <vector>

class globals {
public:
	static int fadeTime;
	static bool fullscreen;

	static std::string serialPort;

	static int baudrate;

	static std::vector<std::string> videoPaths;

	static void loadConfig();
};
