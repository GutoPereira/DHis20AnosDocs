#pragma once

#include "ofMain.h"
#include <cstdint>
#include <string>

class Video {
public:
	enum class State {
		IDLE,
		FADING_IN,
		PLAYING,
		FADING_OUT
	};

	bool setup(const std::string & path);

	void update();
	void draw();

	void activate();
	void deactivate();
	void stop();
	bool isIdle() const;
	bool isTransitioning() const;

	void setFadeTime(int milliseconds);

private:
	ofVideoPlayer movie;

	State state = State::IDLE;

	uint64_t fadeStartTime = 0;

	int fadeTime = 2000;
	int fadeDuration = 2000;

	float alpha = 0.0f;
	float fadeStartAlpha = 0.0f;
};
