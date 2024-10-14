/* European Daylight Saving Time (DST) */

// The Real Time Clock is kept in local winter time (standard time). This approach is key for the simplicity of this solution.
// This code is for a DST from Last sunday in March 2:00 (am) local time until Last sunday in October 3:00 (am) local time. 
// The DST time can be changed if required, by choosing a different hour in the DateTime function (4th parameter).

#include <RTClib.h>   // https://github.com/adafruit/rtclib 
RTC_DS3231 rtc; 
//RTC_DS1307 rtc;

#define USEDST  true // Use DST (true or false)
#define BAUD   57600 // Baud rate for the serial monitor

DateTime now; 
int lastsec;

DateTime dstclock(DateTime n) { // Return the given (DST adjusted) date and time according to DST settings (for extensive date and time calculations)
  
  DateTime b, e;
  
  b = DateTime(n.year(), 3, 31, 2, 0, 0); // Begin of DST: set on March 31 2:00 (am)
  if (n.month() == 3) b = DateTime(n.year(), 3, 31 - b.dayOfTheWeek(), 2, 0, 0); // Begin of DST: adjusted to last sunday in March 2:00 (am) when actual month is March
  e = DateTime(n.year(), 10, 31, 2, 0, 0); // End of DST: set on October 31 2:00 (am)
  if (n.month() == 10) e = DateTime(n.year(), 10, 31 - e.dayOfTheWeek(), 2, 0, 0); // End of DST: adjusted to last sunday in October 2:00 (am) when actual month is October

  if (USEDST && (n >= b) && (n < e)) n = n + TimeSpan(0,1,0,0); // adjust to standard time if within summertime
  return n;
}; 

DateTime getclock() { // Retrieve the (DST adjusted) date and time
  return dstclock(rtc.now());
}; 

void setclock(DateTime n) { // if the clock is set during summertime then adjust the clock to standard time

  if (USEDST && (n != dstclock(n))) n = n - TimeSpan(0,1,0,0); // if summertime then adjust to the standard time 
  rtc.adjust(n); // Set the clock to standard time
}; 

void setup() {
  // initialise the rtc 
  rtc.begin();
  if (rtc.lostPower()) setclock(DateTime(F(__DATE__), F(__TIME__))); // Set date and time for use with the DS3231 RTC
  //  if (!rtc.isrunning()) setclock(DateTime(F(__DATE__), F(__TIME__))); // Set date and time for use with the DS1307 RTC

  // initialise the serial port 
  Serial.begin(BAUD);
  Serial.println(""); Serial.println(""); Serial.println("----------------- New start ------------------");;

  // Start the DST demonstration
  setclock(DateTime(F(__DATE__), F(__TIME__))); // Set the clock to compile date and time

  // show standard (winter) time
  now = rtc.now(); // retreive the standard time
  Serial.print("     Actual standard time: ");
  Serial.print(now.timestamp(DateTime::TIMESTAMP_DATE)); Serial.print(" ");  // print the date
  Serial.println(now.timestamp(DateTime::TIMESTAMP_TIME));                   // print the time 

  // show DST corrected time (only when in DST period, otherwise this equals the standard time)
  Serial.print("Actual DST corrected time: ");
  now = getclock(); // get DST corrected time 
  Serial.print(now.timestamp(DateTime::TIMESTAMP_DATE)); Serial.print(" ");  // print the date
  Serial.print(now.timestamp(DateTime::TIMESTAMP_TIME)); Serial.println(""); // print the time
  Serial.println("");
  delay(5000); 

  Serial.println("- Demonstration of the DST activation -");
  Serial.println("---------------------------------------");
  Serial.println("Last standard time: 2024-03-31 01:59:59");
  Serial.println("    First DST time: 2024-03-31 03:00:00");
  Serial.println("");
  delay(5000); 
  setclock(DateTime(2024, 3, 31, 1, 59, 45)); // Set the clock 15 seconds before DST starts
  lastsec = now.second(); 
}

void loop() {
  now = getclock(); // read the time from the RTC and adjust for DST or
  if (lastsec != now.second()) { // if a new second
    Serial.print(now.timestamp(DateTime::TIMESTAMP_DATE)); Serial.print(" ");  // print the date
    Serial.print(now.timestamp(DateTime::TIMESTAMP_TIME)); Serial.println(""); // print the time
    lastsec = now.second(); // store the current second
  }
}
