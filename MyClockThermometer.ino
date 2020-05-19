/*
 * Arbedclother - Arduino Bedside Clock with Thermometer.
 *
 * An Arduino bedside clock with thermometer and auto backlight dimming.
 * 
 * LCD (2 rows / 16 characters per row):
 *      Displays temperature (from a thermistor). Refreshed every 60 seconds.
 *      Displays current date and time (from an RTC module). Refreshed every second.
 *      Backlight is automatically dimmed by reading light in the room with a photoresistor.
 *        The darker the room, the higher is the backlight intensity and the other way around.
 *        
 *      Example of what is displayed:
 *               +------------------+
 *               | Wednesday 18 May |
 *               | 21.49 C    16:32 |
 *               +------------------+
 * 
 * Bill Of Materials (BOM):
 *    Arduino Uno
 *    LCD1602
 *    DS3231 RTC
 *    Thermistor
 *    Photoresistor
 *    1K resistor
 *    10K resistor
 *    3K3 resistor
 */

/* ---------------------------LICENSE NOTICE-------------------------------- 
 *  MIT License
 *  
 *  Copyright (c) 2020 David Asta
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <Arduino.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <LiquidCrystal.h>

/////////////////////////////////////////////////////////////////////////
#define PIN_TEMP        0
#define PIN_PHOTORES    1
#define PIN_BACKLIGHT   6
#define PIN_LCD_BS      7
#define PIN_LCD_E       8
#define PIN_LCD_D4      9
#define PIN_LCD_D5      10
#define PIN_LCD_D6      11
#define PIN_LCD_D7      12

#define REFRESHTEMPSECS 60  // How many seconds between temperature value refresh in LCD

/////////////////////////////////////////////////////////////////////////
/*
int PIN_TEMP = 0;
int PIN_BACKLIGHT = 6;
int PIN_PHOTORES = 1;
*/


const char *monthName[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dec"};
const char *dowName[] = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
int counter_seconds;  // Seconds counter to compare with REFRESHTEMPSECS
//int REFRESHTEMPSECS = 60;
float temp;
char *month_name;
char lineLCD[15];
int reading;
RtcDS3231<TwoWire> rtc(Wire);

//                BS  E  D4 D5  D6 D7
//LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
LiquidCrystal lcd(PIN_LCD_BS, PIN_LCD_E, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

/////////////////////////////////////////////////////////////////////////
void setup(){
  pinMode(PIN_BACKLIGHT, OUTPUT);
//  analogWrite(PIN_BACKLIGHT, 50);
  
  counter_seconds = 0;
  
  lcd.begin(16, 2);

  rtc.Begin();

  temp = getTemp();
}

/////////////////////////////////////////////////////////////////////////
void loop(){
  /* Set backligh intensity based on how much light there is in the room */
  reading  = analogRead(PIN_PHOTORES);  
  analogWrite(PIN_BACKLIGHT, reading / 10);

  /* Get date and time from RTC module */
  RtcDateTime now = rtc.GetDateTime();

  /* LCD line 1 */
  lcd.setCursor(0, 0);
  lcd.print(dowName[now.DayOfWeek()]);    // Day of the Week
  lcd.setCursor(10, 0);
  lcd.print(now.Day());                   // Day
  lcd.setCursor(13, 0);
  lcd.print(monthName[now.Month()]);      // Month

  /* LCD line 2 */
  if(counter_seconds == REFRESHTEMPSECS){
    /* Only red temperature every x seconds */
    counter_seconds = 0;
    temp = getTemp();
  }

  lcd.setCursor(0, 1);
  lcd.print(temp);                        // Current temperature
  lcd.setCursor(6, 1);
  lcd.print("C");                         // Celsius symbol

  lcd.setCursor(11, 1);

  sprintf(lineLCD,
          "%02u %02u",
          now.Hour(),                     // Hour
          now.Minute()                    // Minutes
          );
          
  lcd.print(lineLCD);

  /* To have the effect of a time separator blinking
   * we print it only every other second
   */
  lcd.setCursor(13, 1);
  if(now.Second() % 2) lcd.print(":");    // Time separator
  else lcd.print(" ");
          

  delay(1000); // one second delay
  counter_seconds++;
}

/////////////////////////////////////////////////////////////////////////
float getTemp(){
  /*
   * Read the thermistor value,
   * convert it to Kelvin and then to Celsius
   */
  int tempReading;
  double tempK;
  float tempC;

  tempReading = analogRead(PIN_TEMP);
  tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );
  tempC = tempK - 273.15;

  return tempC;
}
