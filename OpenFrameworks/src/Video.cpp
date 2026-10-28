#include "Video.h"
#include <algorithm>

bool Video::setup(const std::string & path) {
	ofLogNotice() << "Carregando: " << path;

	if (!movie.load(path)) {
		ofLogError() << "Erro ao carregar: " << path;
		return false;
	}

	movie.setLoopState(OF_LOOP_NONE);
	movie.setVolume(0.0f);

	return true;
}

void Video::setFadeTime(int milliseconds) {
	fadeTime = std::max(0, milliseconds);
}

void Video::update() {
	// Drena tambem os eventos pendentes dos players parados.
	movie.update();
	if (state == State::IDLE) return;

	float duration = movie.getDuration();
	float position = movie.getPosition();
	float remaining = duration * (1.0f - position);

	// Fade-out automatico proximo ao final do video
	if (state != State::FADING_OUT && duration > 0) {
		if (remaining <= fadeTime / 1000.0f) {
			deactivate();
		}
	}

	// Atualizacao dos fades
	if (state == State::FADING_IN || state == State::FADING_OUT) {

		float progress = 1.0f;

		if (fadeDuration > 0) {
			progress = ofClamp(
				(ofGetElapsedTimeMillis() - fadeStartTime) / (float)fadeDuration,
				0.0f, 1.0f);
		}

		float target = state == State::FADING_IN ? 1.0f : 0.0f;

		alpha = ofLerp(fadeStartAlpha, target, progress);

		if (progress >= 1.0f) {
			if (state == State::FADING_IN) {
				state = State::PLAYING;
				alpha = 1.0f;
			} else {
				stop();
				return;
			}
		}
	}

	if (state == State::PLAYING) {
		alpha = 1.0f;
	}

	// Media Foundation tambem sinaliza getIsMovieDone() ao concluir um seek
	// para o inicio. So aceita esse sinal perto do final real do arquivo.
	bool atEnd = duration > 0 && remaining <= 0.1f;
	bool finished = movie.getIsMovieDone() && (duration <= 0 || atEnd);
	if (finished || (duration > 0 && position >= 1.0f)) {
		stop();
		return;
	}

	alpha = ofClamp(alpha, 0.0f, 1.0f);

	// Fade de audio acompanha o alpha
	movie.setVolume(alpha);
}

void Video::draw() {
	if (state == State::IDLE || alpha <= 0.0f) return;

	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, alpha * 255.0f);
	movie.draw(0,0,ofGetWidth(), ofGetHeight());
	ofPopStyle();
}

void Video::activate() {
	if (state != State::IDLE) return;

	state = State::FADING_IN;

	alpha = 0.0f;
	fadeStartAlpha = 0.0f;

	fadeStartTime = ofGetElapsedTimeMillis();
	fadeDuration = fadeTime;

	movie.setPosition(0.0f);
	movie.setVolume(0.0f);
	movie.play();
}

void Video::deactivate() {
	if (state != State::PLAYING && state != State::FADING_IN) return;

	state = State::FADING_OUT;

	fadeStartAlpha = alpha;
	fadeStartTime = ofGetElapsedTimeMillis();
	fadeDuration = fadeTime;

	float duration = movie.getDuration();

	if (duration > 0) {
		float remaining = duration * (1.0f - movie.getPosition());

		int remainingMs = std::max(
			0, static_cast<int>(remaining * 1000.0f));

		fadeDuration = std::min(fadeTime, remainingMs);
	}
}

void Video::stop() {
	movie.setVolume(0.0f);
	movie.stop();

	state = State::IDLE;

	alpha = 0.0f;
	fadeStartAlpha = 0.0f;
}

bool Video::isIdle() const {
	return state == State::IDLE;
}

bool Video::isTransitioning() const {
	return state == State::FADING_IN || state == State::FADING_OUT;
}
