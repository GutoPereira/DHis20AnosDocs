#include "globals.h"
#include "ofMain.h"

int globals::fadeTime = 2000;
bool globals::fullscreen = true;
std::string globals::serialPort = "COM3";
int globals::baudrate = 115200;
std::vector<std::string> globals::videoPaths;

void globals::loadConfig() {
	globals::videoPaths.clear();

	ofXml xml;

	if (!xml.load("config.xml")) {
		ofLogError() << "Erro ao carregar config.xml";
		return;
	}

	ofXml app = xml;

	if (app.getName() != "App") {
		app = xml.getChild("App");
	}

	if (app.getName() != "App") {
		ofLogError() << "Elemento App nao encontrado";
		return;
	}

	auto getString = [&](const std::string & name,
						 const std::string & fallback) {
		std::string value = app.getAttribute(name).getValue();

		return value.empty() ? fallback : value;
	};

	auto getInt = [&](const std::string & name, int fallback) {
		std::string value = app.getAttribute(name).getValue();

		if (value.empty()) return fallback;

		try {
			return std::stoi(value);
		} catch (...) {
			return fallback;
		}
	};

	// Arduino
	globals::serialPort = getString("arduinoPort", globals::serialPort);
	globals::baudrate = getInt("baudrate", globals::baudrate);

	globals::fadeTime = std::max(0, getInt("fadeTime", globals::fadeTime));

	globals::fullscreen = getString("fullscreen", globals::fullscreen ? "true" : "false") == "true";

	// A ordem dos elementos define os botoes 1 a 8.
	// Preserva entradas vazias para nao deslocar os numeros seguintes.
	for (auto videoXml : app.getChildren("video")) {
		std::string path = videoXml.getAttribute("file").getValue();

		videoPaths.push_back(path);
	}

	ofLogNotice()
		<< "Posicoes de video no XML: " << videoPaths.size();
}
