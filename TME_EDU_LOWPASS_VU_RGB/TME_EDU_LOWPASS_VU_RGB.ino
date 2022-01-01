#include <filters.h>
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(60, 12, NEO_GRB + NEO_KHZ800);

byte peak = 1;
byte cnt;

const float cutoff_freq   = 20.0;  //Cutoff frequency in Hz
const float sampling_time = 0.005; //Sampling time in seconds.
IIR::ORDER  order  = IIR::ORDER::OD3; // Order (OD1 to OD4)

// Low-pass filter
Filter f(cutoff_freq, sampling_time, order);

void setup() {
  pinMode(A0, INPUT);
  ledStrip.begin();
}

void loop() {
  int value = analogRead(A0);
  int filteredval = f.filterIn(value) / 7;

  if (filteredval > peak) {
    peak = filteredval;
  }

  cnt++;
  if (cnt > 148 - (analogRead(A1) / 8)) {
    uint32_t rgbcolor;
    //uint32_t rgbcolor = ledStrip.ColorHSV(analogRead(A1) * (64 - peak) , 255, 255-peak);
    for (int i = 0; i < peak ; i++) {
      rgbcolor = ledStrip.ColorHSV(59000 - (i * 1000) , 255, 255);
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
