#include "ofApp.h"

#include "globals.h"

void ofApp::setup() {
	ofSetFrameRate(60);
	ofBackground(0);

	loadConfig();

	serial.listDevices();

	if (serial.setup(globals::serialPort, globals::baudrate)) {
		ofLogNotice()
			<< "Arduino conectado: "
			<< globals::serialPort
			<< " @ " << globals::baudrate;
	} else {
		ofLogError()
			<< "Erro ao abrir porta serial: "
			<< globals::serialPort;
	}

	toggleCursor();
}

void ofApp::loadConfig() {

	for (size_t i = 0; i < globals::videoPaths.size(); i++) {
		if (videos.size() >= N_BUTTONS) break;

		std::string path = "videos/" + globals::videoPaths[i];

		if (path.empty()) {
			ofLogWarning() << "Video " << i + 1 << " sem arquivo";
		}

		auto video = std::make_unique<Video>();
		video->setFadeTime(globals::fadeTime);

		if (!path.empty() && video->setup(path)) {
			videos.push_back(std::move(video));
			validVideos++;
		} else {
			// Mantem a correspondencia entre
			// a posicao no XML e o numero do botao.
			videos.push_back(nullptr);
		}
	}

	ofLogNotice()
		<< "Posicoes de video: " << videos.size();
}

void ofApp::update() {
	// Drena o serial antes de concluir os fades para descartar inputs
	// recebidos durante a transicao, inclusive no seu ultimo frame.
	updateSerial();

	// Mesmo pausados, os players precisam processar os eventos de stop/seek
	// do Media Foundation para poderem ser acionados novamente.
	for (auto & video : videos) {
		if (video) video->update();
	}

	if (activeVideo >= 0 && videos[activeVideo]->isIdle()) {
		activeVideo = pendingVideo;
		pendingVideo = -1;

		if (activeVideo >= 0) {
			videos[activeVideo]->activate();
		}
	}
}

void ofApp::updateSerial() {
	if (!serial.isInitialized()) return;

	// Cada byte representa o estado completo dos oito botoes.
	// Processa todos para nao perder um pressionar/soltar entre frames.
	while (serial.available() > 0) {
		int received = serial.readByte();

		if (received == OF_SERIAL_ERROR) {
			ofLogError() << "Erro de leitura serial";
			break;
		}

		if (received == OF_SERIAL_NO_DATA) {
			break;
		}

		if (received >= 0 && received <= 255) {
			processButtons(static_cast<uint8_t>(received));
		}
	}
}

void ofApp::processButtons(uint8_t buttons) {
	// Bit 0 = botao 1, ... bit 7 = botao 8. Aciona apenas na borda 0 -> 1.
	uint8_t pressed = buttons & ~lastButtons;
	lastButtons = buttons;

	// Atualiza lastButtons mesmo durante o bloqueio: nao ha fila de inputs.
	if (isTransitioning()) return;

	for (int i = 0;
		i < N_BUTTONS && i < (int)videos.size();
		i++) {

		if (pressed & (1u << i)) {
			if (videos[i]) {
				selectVideo(i);
				// Pressionamentos simultaneos: vence o menor numero valido.
				break;
			}
		}
	}
}

void ofApp::draw() {
	ofBackground(0);

	if (activeVideo >= 0) {
		videos[activeVideo]->draw();
	}

	if (debug) displayDebug();
}

void ofApp::keyPressed(int key) {
	int index = key - '1';
	if (index < 0 || index >= N_BUTTONS) return;

	// Impede que a repeticao automatica de uma tecla mantida pressionada
	// transforme um input ignorado em um acionamento depois do fade.
	if (keysDown[index]) return;
	keysDown[index] = true;
	selectVideo(index);
}

void ofApp::keyReleased(int key) {
	int index = key - '1';
	if (index >= 0 && index < N_BUTTONS) {
		keysDown[index] = false;
	} else {
		switch (key) {
			case 'f':
			case 'F':
				ofToggleFullscreen();
				break;
			case 'c':
			case 'C':
				toggleCursor();
				break;
			case 'd':
			case 'D':
				toggleDebug();
				break;
		}
	}
}

bool ofApp::isTransitioning() const {
	return pendingVideo >= 0
		|| (activeVideo >= 0 && videos[activeVideo]->isTransitioning());
}

void ofApp::selectVideo(int index) {
	if (isTransitioning()) return;
	if (index < 0 || index >= static_cast<int>(videos.size()) || !videos[index]) return;
	if (index == activeVideo) {
		// Repetir o numero atual desativa e volta ao preto, sem proximo video.
		pendingVideo = -1;
		videos[activeVideo]->deactivate();
		ofLogNotice() << "Desativado video " << index + 1;
		return;
	}

	ofLogNotice() << "Selecionado video " << index + 1;

	if (activeVideo < 0) {
		activeVideo = index;
		videos[activeVideo]->activate();
	} else {
		// O proximo player so comeca quando o atual parar, em preto e mudo.
		pendingVideo = index;
		videos[activeVideo]->deactivate();
	}
}

void ofApp::toggleCursor() {
	if (showCursor) ofHideCursor();
	if (!showCursor) ofShowCursor();
	showCursor = !showCursor;
}

void ofApp::displayDebug() {
	std::string debugTxt = "FPS: " + ofToString(ofGetFrameRate()) + "\n";
	debugTxt += "Videos Validos: " + ofToString(validVideos) + "\n";
	debugTxt += "Posicoes de video: " + ofToString(videos.size());
	ofSetColor(255);
	ofDrawBitmapStringHighlight(debugTxt, 20, 20,0,255);
}

void ofApp::toggleDebug() {
	debug = !debug;
}
