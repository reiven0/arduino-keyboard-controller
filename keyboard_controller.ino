#include <TinyUSB_Mouse_and_Keyboard.h>

// Pin configurations
const uint8_t SWITCH_PINS[] = { A9, A10 };
const uint8_t PIN_COUNT = sizeof(SWITCH_PINS) / sizeof(SWITCH_PINS[0]);

// Timing constants (milliseconds)
const unsigned long CHATTERING_DELAY = 30;
const unsigned long KEY_PRESS_DURATION = 100;

// State variables
struct KeyState {
    bool isSessionActive;
    bool isKeyPressed;
    unsigned long lastPressTime;
    unsigned long lastReleaseTime;
} keyState;

// Key combination for mute
const uint8_t MUTE_KEYS[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, 'M'};

void setupPins() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    
    for (uint8_t i = 0; i < PIN_COUNT; i++) {
        pinMode(SWITCH_PINS[i], INPUT_PULLUP);
    }
}

void sendMuteCommand() {
    Keyboard.press(MUTE_KEYS[0]);  // CTRL
    Keyboard.press(MUTE_KEYS[1]);  // SHIFT
    Keyboard.write(MUTE_KEYS[2]);  // M
    digitalWrite(LED_BUILTIN, LOW);
}

void releaseKeys() {
    Keyboard.releaseAll();
    digitalWrite(LED_BUILTIN, HIGH);
}

void handleKeyPress() {
    if (!keyState.isSessionActive) {
        sendMuteCommand();
        keyState.isKeyPressed = true;
        keyState.isSessionActive = true;
        keyState.lastPressTime = millis();
    }
    keyState.lastReleaseTime = millis();
}

void updateKeyState() {
    unsigned long currentTime = millis();
    
    if (keyState.isKeyPressed && 
        (currentTime - keyState.lastPressTime >= KEY_PRESS_DURATION)) {
        releaseKeys();
        keyState.isKeyPressed = false;
    }
    
    if (keyState.isSessionActive && 
        (currentTime - keyState.lastReleaseTime >= CHATTERING_DELAY)) {
        keyState.isSessionActive = false;
    }
}

void setup() {
    Keyboard.begin();
    setupPins();
    keyState = {false, false, 0, 0};
}

void loop() {
    if (digitalRead(SWITCH_PINS[1]) == LOW) {
        handleKeyPress();
    }
    updateKeyState();
}
