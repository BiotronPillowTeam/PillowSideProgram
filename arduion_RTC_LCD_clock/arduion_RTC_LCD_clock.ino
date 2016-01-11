#include "Wire.h"
#include <LiquidCrystal.h>
#define DS1307_I2C_ADDRESS 0x68
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int i;
 
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}
 void setDateDs1307()                
{
     byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    byte test;
    byte zero;
    second = (byte) ((Serial.read() - 48) * 10 + (Serial.read() - 48)); // Use of (byte) type casting and ascii math to achieve result.  
    minute = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
   hour  = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
   dayOfWeek = (byte) (Serial.read() - 48);
   dayOfMonth = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
   month = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
   year= (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
   Wire.beginTransmission(DS1307_I2C_ADDRESS);
   Wire.write(zero);
   Wire.write(decToBcd(second) & 0x7f);    // 0 to bit 7 starts the clock
   Wire.write(decToBcd(minute));
   Wire.write(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                   // bit 6 (also need to change readDateDs1307)
   Wire.write(decToBcd(dayOfWeek));
   Wire.write(decToBcd(dayOfMonth));
   Wire.write(decToBcd(month));
   Wire.write(decToBcd(year));
   Wire.endTransmission();
}
void initialize_timer(){
  Wire.beginTransmission(DS1307_I2C_ADDRESS);   // 255 will be the init value and 0 will be considered 
                                                            // an error that occurs when the RTC is in Battery mode.
   Wire.write(0x08); // Set the register pointer to be just past the date/time registers.
   for (i = 1; i <= 24; i++) {
       Wire.write(0Xff);
      delay(10);
   }   
   Wire.endTransmission();
   Wire.beginTransmission(DS1307_I2C_ADDRESS);   
   Wire.write(0x21); // Set the register pointer to 33 for second half of registers. Only 32 writes per connection allowed.
   for (i = 1; i <= 33; i++) {
       Wire.write(0Xff);
      delay(10);
   }   
   Wire.endTransmission();
   //getDateDs1307();
     byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    String s, m, d, mth, h;
   getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
   Serial.println(": RTC1307 Initialized Memory");
}
void getDateDs1307(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
 
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
 
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}
 
 
void setup()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  Wire.begin();
  // AMEND IF YOUR USING A DIFFERENT LCD SCREEN //
  lcd.begin(16, 2);
  Serial.begin(9600);
    //add if the time reading is not changing, initialize it
    initialize_timer();
  delay(400);
  setDateDs1307();
  delay(400);
}
 
void loop()
{


  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  String s, m, d, mth, h;
   
  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
 
  if (second < 10) { s = "0" + String(second); } else { s = String(second); }
  if (minute < 10) { m = "0" + String(minute); } else { m = String(minute); }
  h = String(hour);
  if (dayOfMonth < 10) { d = "0" + String(dayOfMonth); } else { d = String(dayOfMonth); }
  if (month < 10) { mth = "0" + String(month); } else { mth = String(month); }
   
  char* days[] = { "NA", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
  
   //delay(100);
  lcd.clear();
  // JUMP TO CENTER ON A 16X2 SCREEN //
  lcd.setCursor(4,0);
  // CHANGE THE FOLLOWING TO SET THE DATE IN TO YOUR PREFERED ORDER //
  lcd.print(h + ":" + m + ":" + s);
  Serial.println(h + ":" + m + ":" + s);
  // NEXT LINE, 1 SPACE IN FROM THE LEFT //
  lcd.setCursor(1,1);
  // PREFIX THE 20 AS THE RTC CHIP ONLY USES 2 DIGITS FOR THE YEAR //
  lcd.print(String(days[dayOfWeek]) + " " + d + "/" + mth + "/20" + year);
   
  delay(1000); // Wait 1 second
}
