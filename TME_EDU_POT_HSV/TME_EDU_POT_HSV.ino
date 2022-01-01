#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(60, 12, NEO_GRB + NEO_KHZ800);

int peak = 1;
byte cnt;

void setup() {
  ledStrip.begin();
}

void loop() {
  int mini = 250;
  int maxi = 400;
  int mic = analogRead(A0);
  mic = constrain(mic, mini, maxi);
  mic = map(mic, mini, maxi, 0, 60);

  if (mic > peak) {
    peak = mic;
  }


  cnt++;
  if (cnt > 40) {
    uint32_t rgbcolor = ledStrip.ColorHSV(analogRead(A1) * 64 , 255, peak);
    for (int i = 0; i < peak ; i++) {
      ledStrip.setPixelColor(i, rgbcolor);
    }
    ledStrip.show();
    for (int i = 0; i < 60 ; i++) {
      ledStrip.setPixelColor(i, 0);
    }
    if (peak > 0) peak --;
    cnt = 0;
  }
}
