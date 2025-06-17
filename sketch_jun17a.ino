/*
 * Losowe sterowanie 4 przekaźnikami (D2–D5)
 * Każdy przekaźnik losowo ON/OFF i utrzymuje stan przez jakiś czas
 */

#define RELAY_COUNT 8
const int relayPins[RELAY_COUNT] = {2, 3, 4, 5, 6, 7, 8, 9};

void setup() {
  randomSeed(analogRead(0)); // zainicjowanie generatora losowego

  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }
}

void loop() {
  // Ustaw losowy stan każdego przekaźnika
  for (int i = 0; i < RELAY_COUNT; i++) {
    int state = random(0, 2); // 0 = LOW, 1 = HIGH
    digitalWrite(relayPins[i], state);
  }

  delay(1000); // zmiana koloru co 1 sekundę
}
