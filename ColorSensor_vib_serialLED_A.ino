// arduino with color-sensor, vibrators, and serial LED
// I2C color-sensor made by seeed technology inc.
//
/*
 * Copyright (c) 2015 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Wuruibin
 * Modified Time: June 2015
 * Description: This demo can measure the color chromaticity of ambient light or the color of objects,
 *         and via Chainable RGB LED Grove displaying the detected color.
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Wire.h>
#include <GroveColorSensor.h>
//#include <MsTimer2.h>

//serial LED
//http://mag.switch-science.com/2013/04/01/fullcolor_serialled_tape/
#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
// NEO_GRB     Pixels are wired for GRB bitstream   // NEO_RGB     Pixels are wired for RGB bitstream
// NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)  // NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)

#define LEDNUM 1  // serial LED 1個
const int serialLEDPin = 9; //serial LED接続
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDNUM, serialLEDPin, NEO_GRB + NEO_KHZ800);

//vibrator
uint8_t VibRedPin   = 6;  //PWM output to vibration motor for RED
uint8_t VibGreenPin = 5;  //PWM output to vibration motor for GREEN
uint8_t VibBluePin  = 3;  //PWM output to vibration motor for BLUE
uint8_t vibratorNumber = 0;  //MsTimer switch ON vibration motors one by one periodically


void setup()
{
  Serial.begin(9600);
//  Serial.begin(115200);
  Wire.begin();
  
  strip.begin();
  strip.show(); // Initialize serial LED, all pixels to 'off'
  
//  MsTimer2::set(500, vibratorTimer); // 0.5sec period
//  MsTimer2::start();  //使用保留
}


void loop()
{
  int red,  green,  blue; // raw data
  int redC, greenC, blueC; // corrected data
  
  GroveColorSensor colorSensor;
  colorSensor.ledStatus = 1;              // When turn on the color sensor LED, ledStatus = 1; When turn off the color sensor LED, ledStatus = 0.
  while(1)
  {
    colorSensor.readRGB(&red, &green, &blue); //Read RGB values to variables.
    redC   = constrain(  red * 2.22, 0, 255); //白色折り紙の読み取り値を200にする補正値
    greenC = constrain(green * 2.38, 0, 255);
    blueC  = constrain( blue * 8.70, 0, 255);

    //analogWrite value: between 0 (always off) and 255 (always on).
    analogWrite(VibRedPin,   redC);
    analogWrite(VibGreenPin, greenC);
    analogWrite(VibBluePin,  blueC);

//    delay(300);
    Serial.print("corrected RGB( ");
    Serial.print(redC,DEC);
    Serial.print(", ");
    Serial.print(greenC,DEC);
    Serial.print(", ");
    Serial.print(blueC,DEC);
    Serial.println(" )");
    
    colorSensor.clearInterrupt();

    for(int i = 0; i<strip.numPixels(); i++)
    {
      strip.setPixelColor(i, redC / 4, greenC / 4, blueC / 4);  //補正後のRGBでSerialLEDを点灯、明る過ぎないよう調整
      strip.show();
    }
  }
}

void vibratorTimer()   //バイブレーターを定期的に停止させて振動有無を際立たせる。
{
  uint8_t i;
  uint8_t n;
  i = vibratorNumber % 3;
  switch (i) {
    case 0:
      n = VibRedPin;
      break;
    case 1:
      n = VibGreenPin;
      break;
    case 2:
      n = VibBluePin;
      break;
  }
  analogWrite(n, 0);  // PWM 0で停止
  delay(100);  // 停止時間

  if(vibratorNumber == 2){
    vibratorNumber = 0;
  }
  else{
    vibratorNumber = vibratorNumber + 1;
  }

}
