// arduino with color-sensor, vibrators, and serial LEDs
// I2C color-sensor made by seeed technology inc.
//
/*
 * Copyright (c) 2015 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Wuruibin
 * Modified Time: June 2015
 * Description: This demo can measure the color chromaticity of ambient light or the color of objects,
 * 				and via Chainable RGB LED Grove displaying the detected color.
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
#include <MsTimer2.h>

static boolean output = HIGH;

// serial LED 10個用
#define LEDNUM 10  // serial LED number
const int serialLEDPin = 9; //serial LED接続

//serial LED
//http://mag.switch-science.com/2013/04/01/fullcolor_serialled_tape/
#include <Adafruit_NeoPixel.h>
#define MAX_VAL 255  // 0 to 255 for brightness  元24, 24,60,120,240より選択
#define DELAY_TIME 20
#define DELAY_TIME2 80
#define LEDOFFSET 3  // serial LED start number offset

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_GRB     Pixels are wired for GRB bitstream   //   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)  //   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDNUM, serialLEDPin, NEO_GRB + NEO_KHZ800);

uint8_t VibRedPin = 6;   //PWM output to vibration motor for RED
uint8_t VibGreenPin = 5;  //PWM output to vibration motor for GREEN
uint8_t VibBluePin = 3;  //PWM output to vibration motor for BLUE

uint8_t vibratorNumber = 0;  //MsTimer switch ON vibration motors one by one periodically

void setup()
{
  Serial.begin(9600);
//	Serial.begin(115200);
	Wire.begin();
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  MsTimer2::set(500, vibratorON); // 500ms period
  MsTimer2::start();

 
}

void loop()
{
  int red, green, blue; // raw data
  int redC, greenC, blueC; // corrected data
  
	GroveColorSensor colorSensor;
	colorSensor.ledStatus = 1;							// When turn on the color sensor LED, ledStatus = 1; When turn off the color sensor LED, ledStatus = 0.
	while(1)
	{
		colorSensor.readRGB(&red, &green, &blue);		//Read RGB values to variables.
   redC = constrain(red, 0, 255);
   greenC = constrain(green, 0, 255);
   blueC = constrain(blue * 3.5, 0, 255);

    //analogWrite value: between 0 (always off) and 255 (always on).
    analogWrite(VibRedPin, redC);
    analogWrite(VibGreenPin, greenC);
    analogWrite(VibBluePin, blueC);

//		delay(300);
//		Serial.print("The RGB value are: RGB( ");
    Serial.print("RGB( ");
		Serial.print(redC,DEC);
		Serial.print(", ");
		Serial.print(greenC,DEC);
		Serial.print(", ");
		Serial.print(blueC,DEC);
	  Serial.println(" )");
		colorSensor.clearInterrupt();

		for(int i = 0; i<strip.numPixels(); i++)
		{
      //strip.setPixelColor(i, red, green, blue);
      strip.setPixelColor(i, redC, greenC, blueC);
      strip.show();
		}
	}
}

void vibratorON() 	//バイブレーターがPWM120/255未満では起動失敗可能性あり。定期的に起動させて低速運転状態にしておく。
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
  analogWrite(n, 150);	// PWM 150/255で確実にバイブレーターは起動する
  delay(50);	// 起動したと振動ではわからない時間を設定、消費電力との見合いで。
  analogWrite(n, 70);	// PWM 70/255で確実にバイブレーターは起動していても感じない状態にまで下げる、消費電力との見合いで。

  if(vibratorNumber == 2){
    vibratorNumber = 0;
  }
  else{
    vibratorNumber = vibratorNumber + 1;
  }

}

