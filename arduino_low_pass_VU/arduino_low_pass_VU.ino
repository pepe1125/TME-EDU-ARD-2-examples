#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(60, 12, NEO_GRB + NEO_KHZ800);

// keeps values for each analog pin, assuming a basic arduino that muxes 6 pins.
//   you can save a handful of bytes of RAM by putting your sensors on lower-numbered pins and lowering...
#define LOWPASS_ANALOG_PIN_AMT 6

float   lowpass_prev_out[LOWPASS_ANALOG_PIN_AMT],
        lowpass_cur_out[LOWPASS_ANALOG_PIN_AMT];
int        lowpass_input[LOWPASS_ANALOG_PIN_AMT];

uint32_t rgbcolor = 50;

int adcsample_and_lowpass(int pin, int sample_rate, int samples, float alpha, char use_previous) {
  // pin:            arduino analog pin number to sample on   (should be < LOWPASS_ANALOG_PIN_AMT)
  // sample_rate:    approximate rate to sample at (less than ~9000 for default ADC settings)
  // samples:        how many samples to take in this call  (>1 if you want smoother results)
  // alpha:          lowpass alpha
  // use_previous:   If true,  we continue adjusting from the most recent output value.
  //                 If false, we do one extra analogRead here to prime the value.
  //   On noisy signals this non-priming value can be misleading,
  //     and with few samples per call it may not quite adjust to a realistic value.
  //   If you want to continue with the value we saw last -- which is most valid when the
  //     value is not expected to change significantly between calls, you can use true.
  //   You may still want one initial sampling, possibly in setup(), to start from something real.

  float one_minus_alpha = 1.0 - alpha;
  int micro_delay = max(100, (1000000 / sample_rate) - 160); // 160 being our estimate of how long a loop takes
  //(~110us for analogRead at the default ~9ksample/sec,  +50 grasped from thin air (TODO: test)
  if (!use_previous) {
    //prime with a real value (instead of letting it adjust from the value in the arrays)
    lowpass_input[pin] = analogRead(pin);
    lowpass_prev_out[pin] = lowpass_input[pin];
  }

  //Do the amount of samples, and lowpass along the way
  int i;
  for (i = samples; i > 0; i--) {
    delayMicroseconds(micro_delay);
    lowpass_input[pin]    = analogRead(pin);
    lowpass_cur_out[pin]  = alpha * lowpass_input[pin] + one_minus_alpha * lowpass_prev_out[pin];
    lowpass_prev_out[pin] = lowpass_cur_out[pin];
  }
  return lowpass_cur_out[pin];
}


void setup() {
  pinMode(8, INPUT);
  ledStrip.begin();
  //get an initial gauge on the pin. Assume it may do some initial weirdness so take some time
  //  Takes  approx 300ms (300 samples at approx 1000 samples/sec)
  adcsample_and_lowpass(0, 1000, 300, 0.015, false); // see below
}

void loop() {
  // updates can often be shorter (consider maximum adaptation speed to large changes, though)
  int resulting_value = adcsample_and_lowpass(0,     // sample A0 pin
                        5000,  // aim for 1000Hz sampling
                        50,    // follow 50 new samples
                        0.015, // alpha
                        true); // adapt from stored value for this pin (settled in setup())

  if (digitalRead(8)) rgbcolor = ledStrip.ColorHSV(analogRead(A1) * 64);

  for (int i = 0; i < resulting_value / 4 ; i++) {
    ledStrip.setPixelColor(i, rgbcolor);
  }
  ledStrip.show();
  for (int i = 0; i < 60 ; i++) {
    ledStrip.setPixelColor(i, 0);
  }

}
