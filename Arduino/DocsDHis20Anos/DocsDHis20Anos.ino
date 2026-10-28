#include <Arduino.h>

const uint8_t N_BUTTONS = 8;

const uint8_t buttonPins[N_BUTTONS] = {
  4,  // Botao 1
  5,  // Botao 2
  6,  // Botao 3
  7,  // Botao 4
  8,  // Botao 5
  9,  // Botao 6
  10, // Botao 7
  11  // Botao 8
};

const unsigned long DEBOUNCE_TIME = 30;
const unsigned long STARTUP_DELAY = 1500;

uint8_t rawState = 0;
uint8_t stableState = 0;
uint8_t lastSentState = 0;

unsigned long lastChangeTime[N_BUTTONS] = {0};

bool initialSent = false;

void setup() {
  Serial.begin(115200);

  for (uint8_t i = 0; i < N_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);

    bool pressed = digitalRead(buttonPins[i]) == LOW;

    bitWrite(rawState, i, pressed);
    bitWrite(stableState, i, pressed);

    lastChangeTime[i] = millis();
  }

  lastSentState = stableState;
}

void loop() {
  unsigned long now = millis();

  for (uint8_t i = 0; i < N_BUTTONS; i++) {
    bool pressed = digitalRead(buttonPins[i]) == LOW;

    bool lastRaw = bitRead(rawState, i);
    bool stable = bitRead(stableState, i);

    // Detecta alteracao eletrica
    if (pressed != lastRaw) {
      bitWrite(rawState, i, pressed);
      lastChangeTime[i] = now;
    }

    // Confirma estado apos debounce
    if ((now - lastChangeTime[i] >= DEBOUNCE_TIME) &&
        (pressed != stable)) {

      bitWrite(stableState, i, pressed);
    }
  }

  // Primeiro envio apos a inicializacao da serial
  if (!initialSent) {
    if (now >= STARTUP_DELAY) {
      Serial.write(stableState);

      lastSentState = stableState;
      initialSent = true;
    }

    return;
  }

  // Envia somente quando o byte mudar
  if (stableState != lastSentState) {
    Serial.write(stableState);
    lastSentState = stableState;
  }
}