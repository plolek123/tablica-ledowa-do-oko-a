#define RELAY_COUNT 8
const int relayPins[RELAY_COUNT] = {2, 3, 4, 5, 6, 7, 8, 9};

#define BUTTON_OFF A0
#define BUTTON_ON  A1

bool lastStateOff = HIGH;
bool lastStateOn  = HIGH;

// Jeśli przekaźniki są aktywne stanem NISKIM (LOW), ustaw na true:
const bool RELAY_ACTIVE_LOW = true;

void setup() {
  randomSeed(analogRead(0));

  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], relayOff());
  }

  pinMode(BUTTON_OFF, INPUT_PULLUP);
  pinMode(BUTTON_ON, INPUT_PULLUP);
}

void loop() {
  bool currentStateOff = digitalRead(BUTTON_OFF);
  bool currentStateOn  = digitalRead(BUTTON_ON);

  // Wyłączenie LED-ów
  if (lastStateOff == HIGH && currentStateOff == LOW) {
    for (int i = 0; i < RELAY_COUNT; i++) {
      digitalWrite(relayPins[i], relayOff());
    }
  }

  // Wylosowanie nowego koloru
  if (lastStateOn == HIGH && currentStateOn == LOW) {
    for (int i = 0; i < RELAY_COUNT; i++) {
      int r = random(0, 2); // 0 lub 1
      digitalWrite(relayPins[i], r ? relayOn() : relayOff());
    }
  }

  lastStateOff = currentStateOff;
  lastStateOn  = currentStateOn;

  delay(50); // mały debounce
}

int relayOn() {
  return RELAY_ACTIVE_LOW ? LOW : HIGH;
}

int relayOff() {
  return RELAY_ACTIVE_LOW ? HIGH : LOW;
}
