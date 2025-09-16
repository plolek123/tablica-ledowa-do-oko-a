// ======================= KONFIGURACJA =======================
const int relayPinsA[3] = {2, 3, 4};   // Taśma A – bity 0–2
const int relayPinsB[3] = {6, 7, 8};   // Taśma B – bity 0–2
const int sirenPin      = 9;           // Przekaźnik 8 (syrena)

const bool RELAY_ACTIVE_LOW = true;    // aktywne LOW

// ======================= HELPERY =======================
inline void setRelay(int pin, bool on) {
  digitalWrite(pin,
    on
      ? (RELAY_ACTIVE_LOW ? LOW  : HIGH)
      : (RELAY_ACTIVE_LOW ? HIGH : LOW)
  );
}

// Generuje 3-bitową maskę z co najmniej 2 bitami ON
uint8_t randomMask2to3() {
  uint8_t mask = 0;
  // losujemy ile bitów ma być włączonych (2 lub 3)
  uint8_t count = random(2, 4);
  while (count) {
    uint8_t b = random(0, 3);
    if (!(mask & (1 << b))) {
      mask |= (1 << b);
      count--;
    }
  }
  return mask;
}

// ======================= TIMERY =======================
// kolorowe taśmy
unsigned long lastColorToggle = 0;
unsigned long colorInterval   = 0;
// syrena
unsigned long lastSirenToggle = 0;
unsigned long sirenInterval   = 0;
bool         sirenOn          = false;

// ======================= SETUP =======================
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // przekaźniki taśm
  for (int i = 0; i < 3; i++) {
    pinMode(relayPinsA[i], OUTPUT);
    pinMode(relayPinsB[i], OUTPUT);
    setRelay(relayPinsA[i], false);
    setRelay(relayPinsB[i], false);
  }

  // przekaźnik syreny
  pinMode(sirenPin, OUTPUT);
  setRelay(sirenPin, false);

  // pierwszy interwał kolorów i syreny
  colorInterval   = random(200, 1000);
  lastColorToggle = millis();
  sirenInterval   = random(1000, 10000);
  lastSirenToggle = millis();
}

// ======================= LOOP =======================
void loop() {
  unsigned long now = millis();

  // --- 1) Losowanie kolorów taśm co losowy interwał ---
  if (now - lastColorToggle >= colorInterval) {
    lastColorToggle = now;
    colorInterval   = random(200, 1000);

    uint8_t colorA = randomMask2to3();
    uint8_t colorB = randomMask2to3();

    Serial.print("Nowe kolory – A: ");
    Serial.print(colorA, BIN);
    Serial.print("   B: ");
    Serial.println(colorB, BIN);

    for (int bit = 0; bit < 3; bit++) {
      setRelay(relayPinsA[bit], colorA & (1 << bit));
      setRelay(relayPinsB[bit], colorB & (1 << bit));
    }
  }

  // --- 2) Syrena włącza się losowo, pali 1.5 s ---
  if (!sirenOn && now - lastSirenToggle >= sirenInterval) {
    setRelay(sirenPin, true);
    sirenOn         = true;
    lastSirenToggle = now;
    Serial.println("Syrena ON (1.5 s)");
  }
  else if (sirenOn && now - lastSirenToggle >= 1500) {
    setRelay(sirenPin, false);
    sirenOn         = false;
    lastSirenToggle = now;
    sirenInterval   = random(5000, 10000);
    Serial.print("Syrena OFF – następne włączenie za ");
    Serial.print(sirenInterval);
    Serial.println(" ms");
  }
}
