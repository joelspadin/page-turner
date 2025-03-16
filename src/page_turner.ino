// Copyright (c) 2025 Joel Spadin
// SPDX-License-Identifier: MIT

#include <Keyboard.h>

// Target scan rate
const unsigned long MicrosPerLoop = 1000;

// Key must be pressed/released for this number of milliseconds to register.
const int DebouncePressMillis = 1;
const int DebounceReleaseMillis = 5;

// Number of scan cycles for debounce press/release
const int DebouncePressCount = DebouncePressMillis * 1000 / MicrosPerLoop;
const int DebounceReleaseCount = DebounceReleaseMillis * 1000 / MicrosPerLoop;

const uint16_t DebounceCounterMax = 0b0111111111111111;

struct KeyState {
  bool pressed : 1;
  uint16_t counter : 15;
};

struct Key {
  uint32_t pin;
  int activeValue;
  uint8_t key;
  struct KeyState state;
};

// Keymap
// Change LOW to HIGH if your pedals are normally closed swithces.
struct Key keys[] = {
  {PA7, LOW, KEY_LEFT_ARROW},
  {PA4, LOW, KEY_RIGHT_ARROW},
};

// Get the number of consecutive scan cycles needed to change states
uint16_t getDebounceThreshold(const struct KeyState& state) {
  return state.pressed ? DebounceReleaseCount : DebouncePressCount;
}

void decrementCounter(struct KeyState& state) {
  if (state.counter > 0) {
    state.counter--;
  }
}

void incrementCounter(struct KeyState& state) {
  if (state.counter < DebounceCounterMax) {
    state.counter++;
  }
}

// Debounce using a variant of the algorithm described at
// https://www.kennethkuhn.com/electronics/debounce.c
// Returns true if the state changed.
bool debounce(struct KeyState& state, bool pressed) {
  if (pressed == state.pressed) {
    decrementCounter(state);
    return false;
  }

  const auto flipThreshold = getDebounceThreshold(state);

  if (state.counter < flipThreshold) {
    incrementCounter(state);
    return false;
  }


  state.pressed = !state.pressed;
  state.counter = 0;

  return true;
}

unsigned long nextLoopTime;

void setup() {
  for (const auto& key : keys) {
    pinMode(key.pin, INPUT_PULLUP);
  }

  Keyboard.begin();

  nextLoopTime = micros() + MicrosPerLoop;
}

void loop() {
  for (auto& key : keys) {
    const auto pressed = digitalRead(key.pin) == key.activeValue;
    const auto changed = debounce(key.state, pressed);

    if (changed) {
      if (key.state.pressed) {
        Keyboard.press(key.key);
      } else {
        Keyboard.release(key.key);
      }
    }
  }

  nextLoopTime += MicrosPerLoop;
  const auto now = micros();
  if (now < nextLoopTime) {
    delayMicroseconds(nextLoopTime - now);
  }
}
