#include "ofApp.h"
#include "ofMain.h"
#include "globals.h"

int main() {

	globals::loadConfig();

	ofSetupOpenGL(1280, 720, globals::fullscreen?OF_FULLSCREEN:OF_WINDOW);
	ofRunApp(std::make_shared<ofApp>());

	ofLogToConsole();
}
