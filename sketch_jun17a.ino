#define RELAY_COUNT 8
const int relayPins[RELAY_COUNT] = {2, 3, 4, 5, 6, 7, 8, 9};

#define BUTTON_PIN A0
const bool RELAY_ACTIVE_LOW = true;

bool isRunning = false;
bool lastButtonState = HIGH;
unsigned long lastChangeTime = 0;
const unsigned long interval = 300; // czas między losowaniami

void setup() {
  randomSeed(analogRead(0));
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], relayOff());
  }
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Obsługa przycisku (kliknięcie)
  bool currentState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && currentState == LOW) {
    isRunning = !isRunning;

    if (!isRunning) {
      // Jeśli wyłączamy: gasimy wszystko
      for (int i = 0; i < RELAY_COUNT; i++) {
        digitalWrite(relayPins[i], relayOff());
      }
    }
  }
  lastButtonState = currentState;

  // W trybie włączonym: losujemy co 1 sekundę
  if (isRunning && millis() - lastChangeTime >= interval) {
    lastChangeTime = millis();
    for (int i = 0; i < RELAY_COUNT; i++) {
      digitalWrite(relayPins[i], random(0, 2) ? relayOn() : relayOff());
    }
  }

  delay(20); // lekki debounce
}

// Pomocnicze funkcje logiczne
int relayOn() {
  return RELAY_ACTIVE_LOW ? LOW : HIGH;
}

int relayOff() {
  return RELAY_ACTIVE_LOW ? HIGH : LOW;
}
