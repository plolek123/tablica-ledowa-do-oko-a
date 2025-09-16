#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

// ======================= WYŚWIETLACZ =======================
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES   9
#define CS_PIN        10

MD_Parola   display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX  mx      = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// ======================= PRZEKAŹNIKI =======================
const int relayPinsA[3] = {2, 3, 4};   // Taśma A – bity 0–2
const int relayPinsB[3] = {6, 7, 8};   // Taśma B – bity 0–2
const int sirenPin      = 9;           // Przekaźnik 8 (syrena)
const bool RELAY_ACTIVE_LOW = true;    // aktywne LOW

inline void setRelay(int pin, bool on) {
  digitalWrite(pin,
    on
      ? (RELAY_ACTIVE_LOW ? LOW  : HIGH)
      : (RELAY_ACTIVE_LOW ? HIGH : LOW)
  );
}

// ======================= PARAMETRY =======================
// stały czas zmiany kolorów (ms)
const unsigned long COLOR_INTERVAL = 250;
unsigned long lastColorToggle = 0;

// syrena: ON 1.5 s, OFF losowo 5…10 s
unsigned long lastSirenToggle = 0;
unsigned long sirenInterval   = 0;
bool         sirenOn          = false;

// ======================= BITMAPA =======================
const uint8_t smiley[8] = {
  B00111100, B01000010, B10100101, B10000001,
  B10100101, B10011001, B01000010, B00111100
};

// ======================= WYŚWIETLACZ ASYNC =======================
enum DispState {
  TXT_TECHNIKUM, TXT_ENERGETYCZNE, BMP_SMILEY1,
  TXT_ZAWODY, TXT_ELEKTRONIK, TXT_ELEKTRYK,
  TXT_ENERGETYK, TXT_AUTOMATYK, BMP_SMILEY2,
  DISP_END
};
DispState dispState = TXT_TECHNIKUM;
bool     waiting   = false;

struct Item {
  const char*    txt;
  textPosition_t align;
  uint16_t       speed;
  uint16_t       pause;
  textEffect_t   inE;
  textEffect_t   outE;
} items[] = {
  { "TECHNIKUM",          PA_CENTER, 100, 1000, PA_SCROLL_DOWN, PA_SCROLL_UP   },
  { "ENERGETYCZNE",       PA_LEFT,    80,    0,  PA_SCROLL_LEFT, PA_SCROLL_LEFT },
  { nullptr,              PA_CENTER,   0,    0,  PA_PRINT,       PA_PRINT       }, // bitmap
  { "ZAWODY:",            PA_CENTER,   80, 1500, PA_SCROLL_DOWN, PA_SCROLL_UP   },
  { "TECHNIK ELEKTRONIK", PA_LEFT,    80, 1500, PA_SCROLL_LEFT, PA_SCROLL_LEFT },
  { "TECHNIK ELEKTRYK",   PA_LEFT,    80, 1500, PA_SCROLL_LEFT, PA_SCROLL_LEFT },
  { "TECHNIK ENERGETYK",  PA_LEFT,    80, 1500, PA_SCROLL_LEFT, PA_SCROLL_LEFT },
  { "TECHNIK AUTOMATYK",  PA_LEFT,    80, 1500, PA_SCROLL_LEFT, PA_SCROLL_LEFT },
  { nullptr,              PA_CENTER,   0,    0,  PA_PRINT,       PA_PRINT       }
};

void startNextDisplay() {
  if (dispState >= DISP_END) dispState = TXT_TECHNIKUM;
  Item &it = items[(int)dispState];

  if (it.txt) {
    display.displayText(it.txt,
                        it.align,
                        it.speed,
                        it.pause,
                        it.inE,
                        it.outE);
  } else {
    // wyświetl bitmapę
    display.displayClear();
    for (uint8_t c = 0; c < 8; c++) {
      for (uint8_t r = 0; r < 8; r++) {
        bool px = bitRead(smiley[r], 7 - c);
        for (uint8_t dev = 0; dev < MAX_DEVICES; dev++) {
          mx.setPoint(r, dev * 8 + c, px);
        }
      }
    }
    delay(1500);
  }
  waiting = true;
}

// ======================= SETUP =======================
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // wyświetlacz
  display.begin();
  mx.begin();
  display.setIntensity(5);
  display.displayClear();
  display.setTextAlignment(PA_CENTER);

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

  // pierwszy interwał syreny
  sirenInterval   = random(5000, 10000);
  lastSirenToggle = millis();
}

// ======================= LOOP =======================
void loop() {
  unsigned long now = millis();

  // 1) kolor taśm co stały interwał
  if (now - lastColorToggle >= COLOR_INTERVAL) {
    lastColorToggle = now;

    // generuj maskę 1..7 → zawsze przynajmniej jeden kolor
    uint8_t colorA = random(1, 8);
    uint8_t colorB = random(1, 8);

    Serial.print("Taśma A = "); Serial.println(colorA, BIN);
    Serial.print("Taśma B = "); Serial.println(colorB, BIN);

    for (uint8_t bit = 0; bit < 3; bit++) {
      setRelay(relayPinsA[bit], colorA & (1 << bit));
      setRelay(relayPinsB[bit], colorB & (1 << bit));
    }
  }

  // 2) syrena: ON 1.5 s, OFF 5–10 s
  if (!sirenOn && now - lastSirenToggle >= sirenInterval) {
    setRelay(sirenPin, true);
    sirenOn         = true;
    lastSirenToggle = now;
    Serial.println("Syrena ON");
  }
  else if (sirenOn && now - lastSirenToggle >= 1500) {
    setRelay(sirenPin, false);
    sirenOn         = false;
    lastSirenToggle = now;
    sirenInterval   = random(5000, 10000);
    Serial.print("Syrena OFF, next in ");
    Serial.print(sirenInterval);
    Serial.println(" ms");
  }

  // 3) wyświetlacz asynchronicznie
  if (!waiting)       startNextDisplay();
  if (display.displayAnimate()) {
    waiting   = false;
    dispState = DispState(dispState + 1);
  }

  delay(1);
}
