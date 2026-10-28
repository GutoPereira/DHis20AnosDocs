#pragma once

#include "Video.h"
#include "ofMain.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class ofApp : public ofBaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;

	void keyPressed(int key) override;
	void keyReleased(int key) override;

	void toggleCursor();
	void displayDebug();
	void toggleDebug();

private:
	static constexpr int N_BUTTONS = 8;

	std::vector<std::unique_ptr<Video>> videos;
	int activeVideo = -1;
	int pendingVideo = -1;
	bool keysDown[N_BUTTONS] = {};

	bool showCursor = true;
	bool debug = false;

	// Arduino
	ofSerial serial;

	uint8_t lastButtons = 0;

	void loadConfig();
	void updateSerial();
	void processButtons(uint8_t buttons);
	void selectVideo(int index);
	bool isTransitioning() const;
	int validVideos = 0;
};
