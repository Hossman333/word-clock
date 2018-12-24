#include <Time.h>
#include <TimeLib.h>

/**
   HSL -> RGB helper functions
   https://github.com/Mazaryk/neopixel-hsl/blob/master/NeoPixel-HSL.ino
   Map HSL color space to RGB

   Totally borrowed from:
   http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/

   Probably not the most efficient solution, but
   it get's the job done.
*/
uint32_t hsl(uint16_t ih, uint8_t is, uint8_t il) {

  float h, s, l, t1, t2, tr, tg, tb;
  uint8_t r, g, b;

  h = (ih % 360) / 360.0;
  s = constrain(is, 0, 100) / 100.0;
  l = constrain(il, 0, 100) / 100.0;

  if ( s == 0 ) {
    r = g = b = 255 * l;
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
  }

  if ( l < 0.5 ) t1 = l * (1.0 + s);
  else t1 = l + s - l * s;

  t2 = 2 * l - t1;
  tr = h + 1 / 3.0;
  tg = h;
  tb = h - 1 / 3.0;

  r = hsl_convert(tr, t1, t2);
  g = hsl_convert(tg, t1, t2);
  b = hsl_convert(tb, t1, t2);

  // NeoPixel packed RGB color
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}
/**
   HSL Convert
   Helper function
*/
uint8_t hsl_convert(float c, float t1, float t2) {

  if ( c < 0 ) c += 1;
  else if ( c > 1 ) c -= 1;

  if ( 6 * c < 1 ) c = t2 + ( t1 - t2 ) * 6 * c;
  else if ( 2 * c < 1 ) c = t1;
  else if ( 3 * c < 2 ) c = t2 + ( t1 - t2 ) * ( 2 / 3.0 - c ) * 6;
  else c = t2;

  return (uint8_t)(c * 255);
}
/*

  Arduino + Neopixel Word Clock Code
  by: Alex - Super Make Something
  date: August 16, 2015
  license: Public domain.  Please use, reuse, and modify this sketch!
  additional: Modified from "simple.ino" NeoPixel example sketch by Shae Erisson of Adafruit Industries.
  additional: This was modified by Josh R. - @hossman333 in the months of November-December 2018.

  NOTE: REQUIRES NEOPIXEL & TIME LIBRARIES TO BE INSTALLED UNDER ...\Arduino\libraries
  NEOPIXEL LIBRARY AVAILABLE AT: https://github.com/adafruit/Adafruit_NeoPixel
  TIME LIBRARY AVAILABLE AT: https://github.com/PaulStoffregen/Time

  Explanation: This code lights up Neopixels corresponding to the current time.
  Time is kept using the time library.
  Neopixels are lit using the Adafruit Neopixel library.

  Depending on the current time, flags to light corresponding Neopixels are saved in an array
  After parsing the time, Neopixels are turned on/off according to the flags using a for loop

*/

#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include "Time.h"


#define PIN           3
#define NUMPIXELS     130

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define MINUSFIVEMINS_BUTTON         5
#define PLUSFIVEMINS_BUTTON          6

#define RANDOMCOLOR_BUTTON           7
#define TOGGLEQUOTES_BUTTON          8
#define RANDOMQUOTECOLOR_BUTTON      9

#define POT_ONE                      A0
#define POT_TWO                      A1

int minusPrevState = HIGH;
int minusCurrState = HIGH;
int plusPrevState = HIGH;
int plusCurrState = HIGH;

int randomColorPrevState = HIGH;
int randomColorCurrState = HIGH;
int randomQuoteColorPrevState = HIGH;
int randomQuoteColorCurrState = HIGH;
int randomQuotePrevState = HIGH;
int randomQuoteCurrState = HIGH;

int h;
int m;
int s;


int potOneValue = 0;
int potTwoValue = 0;
int saturation = 100;
int clockLEDBrightness = 25;
int quoteBrightness = 25;
int clockHue = 0;
uint32_t mainColor = hsl(clockHue, saturation, clockLEDBrightness);
int quoteHue = 120;
uint32_t quoteColor = hsl(quoteHue, saturation, quoteBrightness);

int currQuoteSet = 1;
/*
   0: Be Kind
   1: ; Not One Faileth
   2: XII XXV MMXVIII
   3: Utes
*/

void setup()
{
  pinMode(MINUSFIVEMINS_BUTTON, INPUT_PULLUP);
  pinMode(PLUSFIVEMINS_BUTTON, INPUT_PULLUP);
  pinMode(RANDOMCOLOR_BUTTON, INPUT_PULLUP);
  pinMode(TOGGLEQUOTES_BUTTON, INPUT_PULLUP);
  pinMode(RANDOMQUOTECOLOR_BUTTON, INPUT_PULLUP);

  pinMode(POT_ONE, INPUT);
  pinMode(POT_TWO, INPUT);

  setTime(12, 0, 0, 18, 12, 2018); //Initialize current time as Midnight/noon 12/18/2018
  pixels.begin();

}

void loop()
{
  //  B E X ; I N O T I O N E X
  //  I T X I S V T E N H A L F
  //  Q U A R T E R T W E N T Y
  //  F I V E M M I N U T E S M
  //  P A S T T O X O N E T W O
  //  T H R E E F O U R F I V E
  //  S I X S E V E N E I G H T
  //  N I N E T E N E L E V E N
  //  T W E L V E V O C L O C K
  //  F A I L E T H I I K I N D
  int individualPixels[NUMPIXELS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                    };

  randomColorCurrState = digitalRead(RANDOMCOLOR_BUTTON);
  randomQuoteCurrState = digitalRead(TOGGLEQUOTES_BUTTON);
  randomQuoteColorCurrState = digitalRead(RANDOMQUOTECOLOR_BUTTON);
  potOneValue = analogRead(POT_ONE);
  potTwoValue = analogRead(POT_TWO);
  // Pot sends 0volts to 5volts. It reads 1023, so map will convert it to a legit HSL num.
  clockLEDBrightness = map(potOneValue, 0, 1023, 0, 50);
  quoteBrightness = map(potTwoValue, 0, 1023, 0, 50);
  mainColor = hsl(clockHue, saturation, clockLEDBrightness);
  quoteColor = hsl(quoteHue, saturation, quoteBrightness);

  if ((randomColorCurrState != randomColorPrevState) && (randomColorCurrState == LOW)) {
    clockHue = random(0, 360);
    mainColor = hsl(clockHue, saturation, clockLEDBrightness);
    randomColorPrevState = randomColorCurrState;
  } else {
    randomColorPrevState = randomColorCurrState;
  };

  if ((randomQuoteColorCurrState != randomQuoteColorPrevState) && (randomQuoteColorCurrState == LOW)) {
    quoteHue = random(0, 360);
    quoteColor = hsl(quoteHue, saturation, quoteBrightness);
    randomQuoteColorPrevState = randomQuoteColorCurrState;
  } else {
    randomQuoteColorPrevState = randomQuoteColorCurrState;
  };

  if ((randomQuoteCurrState != randomQuotePrevState) && (randomQuoteCurrState == LOW)) {
    if (currQuoteSet == 3) {
      currQuoteSet = 0;
    } else {
      currQuoteSet = currQuoteSet + 1;
    }
    randomQuotePrevState = randomQuoteCurrState;
  } else {
    randomQuotePrevState = randomQuoteCurrState;
  };

  minusCurrState = digitalRead(MINUSFIVEMINS_BUTTON);

  if ((minusCurrState != minusPrevState) && (minusCurrState == LOW)) {
    adjustTime(-5 * 60);
    minusPrevState = minusCurrState;
  } else {
    minusPrevState = minusCurrState;
  };

  plusCurrState = digitalRead(PLUSFIVEMINS_BUTTON);

  if ((plusCurrState != plusPrevState) && (plusCurrState == LOW)) {
    adjustTime(5 * 60); //Shift time five minutes forwards
    plusPrevState = plusCurrState;
  } else {
    plusPrevState = plusCurrState;
  };

  h = hourFormat12();
  m = minute();
  s = second();

  /* Parse time values to light corresponding pixels */
  //  LIGHT "IT" "IS"
  individualPixels[24] = 1;
  individualPixels[25] = 1;

  individualPixels[22] = 1;
  individualPixels[21] = 1;

  /* Minutes between 0-5 - Light "O CLOCK" */
  if ((m >= 0 && m < 5)) {
    individualPixels[111] = 1;
    individualPixels[112] = 1;
    individualPixels[113] = 1;
    individualPixels[114] = 1;
    individualPixels[115] = 1;
    individualPixels[116] = 1;
  }

  /* Minutes between 5-10 or 55-60 - Light "FIVE," "MINUTES" */
  if ((m >= 5 && m < 10) || (m >= 55 && m < 60)) {
    individualPixels[48] = 1;
    individualPixels[49] = 1;
    individualPixels[50] = 1;
    individualPixels[51] = 1;

    individualPixels[40] = 1;
    individualPixels[41] = 1;
    individualPixels[42] = 1;
    individualPixels[43] = 1;
    individualPixels[44] = 1;
    individualPixels[45] = 1;
    individualPixels[46] = 1;
  }

  /* Minutes between 10-15 or 50-55 - Light "TEN," "MINUTES" */
  if ((m >= 10 && m < 15) || (m >= 50 && m < 55)) {
    individualPixels[17] = 1;
    individualPixels[18] = 1;
    individualPixels[19] = 1;

    individualPixels[40] = 1;
    individualPixels[41] = 1;
    individualPixels[42] = 1;
    individualPixels[43] = 1;
    individualPixels[44] = 1;
    individualPixels[45] = 1;
    individualPixels[46] = 1;
  }

  /* Minutes between 15-20 or 45-50 - Light "A QUARTER" */
  if ((m >= 15 && m < 20) || (m >= 45 && m < 50)) {
    individualPixels[15] = 1;

    individualPixels[26] = 1;
    individualPixels[27] = 1;
    individualPixels[28] = 1;
    individualPixels[29] = 1;
    individualPixels[30] = 1;
    individualPixels[31] = 1;
    individualPixels[32] = 1;
  }

  /* Minutes between 20-25 or 40-45 - Light "TWENTY," "MINUTES" */
  if ((m >= 20 && m < 25) || (m >= 40 && m < 45)) {
    individualPixels[33] = 1;
    individualPixels[34] = 1;
    individualPixels[35] = 1;
    individualPixels[36] = 1;
    individualPixels[37] = 1;
    individualPixels[38] = 1;

    individualPixels[40] = 1;
    individualPixels[41] = 1;
    individualPixels[42] = 1;
    individualPixels[43] = 1;
    individualPixels[44] = 1;
    individualPixels[45] = 1;
    individualPixels[46] = 1;
  }

  /* Minutes between 25-30 or 35-40 - Light "TWENTY," "FIVE," "MINUTES" */
  if ((m >= 25 && m < 30) || (m >= 35 && m < 40)) {
    individualPixels[33] = 1;
    individualPixels[34] = 1;
    individualPixels[35] = 1;
    individualPixels[36] = 1;
    individualPixels[37] = 1;
    individualPixels[38] = 1;

    individualPixels[48] = 1;
    individualPixels[49] = 1;
    individualPixels[50] = 1;
    individualPixels[51] = 1;

    individualPixels[40] = 1;
    individualPixels[41] = 1;
    individualPixels[42] = 1;
    individualPixels[43] = 1;
    individualPixels[44] = 1;
    individualPixels[45] = 1;
    individualPixels[46] = 1;
  }

  /* Minutes between 30-35 - Light "HALF" */
  if ((m >= 30 && m < 35)) {
    individualPixels[13] = 1;
    individualPixels[14] = 1;
    individualPixels[15] = 1;
    individualPixels[16] = 1;
  }

  /* Minutes between 5-35 - Light "PAST" */
  if ((m >= 5) && (m < 35)) {
    individualPixels[52] = 1;
    individualPixels[53] = 1;
    individualPixels[54] = 1;
    individualPixels[55] = 1;
  }

  /* Minutes between 35-60 - Light "TO" & MODIFY CURRENT HOUR VALUE */
  if (m >= 35) {
    individualPixels[56] = 1;
    individualPixels[57] = 1;
    h++; //Add 1 from current hour
    /*Set time to twelve for hour around midnight, noon */
    if (h == 0) {
      h = 12;
    }
    /*Corner case for 12:35-12:59 */
    if (h == 13) {
      h = 1;
    }
  }

  /* Hour=1 - Light "ONE" */
  if (h == 1) {
    individualPixels[59] = 1;
    individualPixels[60] = 1;
    individualPixels[61] = 1;
  }

  /* Hour=2 - Light "TWO" */
  if (h == 2) {
    individualPixels[62] = 1;
    individualPixels[63] = 1;
    individualPixels[64] = 1;
  }

  /* Hour=3 - Light "THREE" */
  if (h == 3) {
    individualPixels[73] = 1;
    individualPixels[74] = 1;
    individualPixels[75] = 1;
    individualPixels[76] = 1;
    individualPixels[77] = 1;
  }

  /* Hour=4 - Light "FOUR" */
  if (h == 4) {
    individualPixels[72] = 1;
    individualPixels[71] = 1;
    individualPixels[70] = 1;
    individualPixels[69] = 1;
  }

  /* Hour=5 - Light "FIVE" */
  if (h == 5) {
    individualPixels[68] = 1;
    individualPixels[67] = 1;
    individualPixels[66] = 1;
    individualPixels[65] = 1;
  }

  /* Hour=6 - Light "SIX" */
  if (h == 6) {
    individualPixels[78] = 1;
    individualPixels[79] = 1;
    individualPixels[80] = 1;
  }

  /* Hour=7 - Light "SEVEN" */
  if (h == 7) {
    individualPixels[81] = 1;
    individualPixels[82] = 1;
    individualPixels[83] = 1;
    individualPixels[84] = 1;
    individualPixels[85] = 1;
  }

  /* Hour=8 - Light "EIGHT" */
  if (h == 8) {
    individualPixels[86] = 1;
    individualPixels[87] = 1;
    individualPixels[88] = 1;
    individualPixels[89] = 1;
    individualPixels[90] = 1;
  }

  /* Hour=9 - Light "NINE" */
  if (h == 9) {
    individualPixels[100] = 1;
    individualPixels[101] = 1;
    individualPixels[102] = 1;
    individualPixels[103] = 1;
  }

  /* Hour=10 - Light "TEN" */
  if (h == 10) {
    individualPixels[99] = 1;
    individualPixels[98] = 1;
    individualPixels[97] = 1;
  }

  /* Hour=11 - Light "ELEVEN" */
  if (h == 11) {
    individualPixels[96] = 1;
    individualPixels[95] = 1;
    individualPixels[94] = 1;
    individualPixels[93] = 1;
    individualPixels[92] = 1;
    individualPixels[91] = 1;
  }

  /* Hour=12 - Light "TWELVE" */
  if (h == 12) {
    individualPixels[104] = 1;
    individualPixels[105] = 1;
    individualPixels[106] = 1;
    individualPixels[107] = 1;
    individualPixels[108] = 1;
    individualPixels[109] = 1;
  }

  if (currQuoteSet == 0) {
    //   0: Be Kind
    individualPixels[0] = 2;
    individualPixels[1] = 2;

    individualPixels[117] = 2;
    individualPixels[118] = 2;
    individualPixels[119] = 2;
    individualPixels[120] = 2;
  } else if (currQuoteSet == 1) {
    //   1: ; Not One Faileth
    individualPixels[3] = 2;

    individualPixels[5] = 2;
    individualPixels[6] = 2;
    individualPixels[7] = 2;

    individualPixels[9] = 2;
    individualPixels[10] = 2;
    individualPixels[11] = 2;

    individualPixels[123] = 2;
    individualPixels[124] = 2;
    individualPixels[125] = 2;
    individualPixels[126] = 2;
    individualPixels[127] = 2;
    individualPixels[128] = 2;
    individualPixels[129] = 2;
  } else if (currQuoteSet == 2) {
    //   2: XII XXV MMXVIII
    individualPixels[2] = 2;
    individualPixels[4] = 2;
    individualPixels[8] = 2;

    individualPixels[12] = 2;
    individualPixels[20] = 2;
    individualPixels[23] = 2;

    individualPixels[39] = 2;
    individualPixels[47] = 2;
    individualPixels[58] = 2;
    individualPixels[110] = 2;
    individualPixels[119] = 2;
    individualPixels[121] = 2;
    individualPixels[122] = 2;
  } else {
    //   3: Utes
    individualPixels[40] = 2;
    individualPixels[41] = 2;
    individualPixels[42] = 2;
    individualPixels[43] = 2;
  }

  /* Light pixels corresponding to current time */
  for (int i = 0; i < NUMPIXELS; i++) {
    if (individualPixels[i] == 1) {
      pixels.setPixelColor(i, mainColor);
    } else if (individualPixels[i] == 2) {
      pixels.setPixelColor(i, quoteColor);
    }
    else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }

  pixels.show();

}

