
unsigned long int lastt= 3000;
unsigned long int ct= 3000;

String cc="";

#include <SD.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
int voice=25;//need change**********************************************************************************************************************************************
int bluetoothTx = 7;
int bluetoothRx = 8;
String command="";
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);
int vib_max=80; //change to something higher***************************************************
int brightness_max=254; //change to somethign higher*********************************************
int Sound_max=28;//change to somethign higher****************************************************

File myFile;
int v = 0;
int sensorPin = A4;
int stopw = 0;
String in="";
const int pressure_record_interval = 1000;//need to change before final****************************************************************************************************************************************
int pressureled = A5;
unsigned long time;
unsigned long int sec=0;
unsigned long int minu= 0;
unsigned long int hour=0;
int last20[20];
String settingTimeString = "";
long int sleeptime=0;
long int pressure_last_time=1;

//pin definitions.  must be PWM-capable pins!
const int redPin = 9;
const int greenPin = 5;
const int bluePin = 6;
const int motorPin = 10 ;
//maximum duty cycle to be used on each led for color balancing.  
//if "white" (R=255, G=255, B=255) doesn't look white, reduce the red, green, or blue max value.
const int max_red = 255;
const int max_green = 90;
const int max_blue = 100;
const int max_motor= 65;
int morning_updateInterval=50; //the bigger the slower it increase 
int currentmotor=0;
int led_on=0;
int motor_on=0;
int color[3] = {
  1, 1, 1}; //array to store led brightness values
int currentcolor[3] = {
  1,1, 1  }; 
int alarm_onoff=1;//1 means the "stop alarm button" haven't been pressed
int alarm_before_wakeup=4000;//(10minutes is the best)the alarm clock will start at this amount of time before finish counting sleeptime************************************************
int const motor_increase_unit=10;
int motor_increase_rate=1;
int LEDincrease_rate=1;
long int motor_lastUpdate=0;
long int LEDlastUpdate=0;



int wakeup_start_state(){//if currenttime is wakeup_preocess_time before sleeptime
  Serial.print("                                                                                      10*1000-millis()=");
  Serial.println(sleeptime-alarm_before_wakeup);
  Serial.println(sleeptime-alarm_before_wakeup<millis());
  if (sleeptime-alarm_before_wakeup<millis()&&(sleeptime-alarm_before_wakeup*1/3>millis())&&alarm_onoff==1){
    //if (10*1000-5*1000<millis()){
    return 1;
  }
  if ((sleeptime-alarm_before_wakeup*1/3<=millis())&&(sleeptime-alarm_before_wakeup*2/3>millis())&&alarm_onoff==1){
    return 2;
  }
  if ((sleeptime-alarm_before_wakeup*2/3<=millis())&&alarm_onoff==1){
    return 3;
  }
  else{
    return 0;
  }
}

void morningLED()   
{
  unsigned long currentMillis = millis();

  if((wakeup_start_state()>=1)&&(led_on==0)){

    if(((millis() - LEDlastUpdate) > morning_updateInterval)&&(LEDincrease_rate<=254))  {
      LEDlastUpdate = millis();
      LEDincrease_rate+=1;

      //for(int increase_rate=1;increase_rate<=255;increase_rate++){
      currentcolor[0]=color[0]*LEDincrease_rate;
      currentcolor[1]=color[1]*LEDincrease_rate;
      currentcolor[2]=color[2]*LEDincrease_rate;
      Serial.print("LEDbirghtness: ");
      Serial.println(currentcolor[0]);
      for (int colornum=0;colornum<3;colornum++){
        if (currentcolor[colornum]>=254) {
          currentcolor[colornum]=254;
          led_on=1;
        }
      }
      analogWrite(redPin, map(brightness_max, 0, 255, 0, max_red));
      analogWrite(greenPin, map(currentcolor[1], 0, 255, 0, max_green));
      analogWrite(bluePin, map(currentcolor[2], 0, 255, 0, max_blue));
      //delay(35);
    }
  }
  if((wakeup_start_state()>=1)&&(led_on==1)){
    analogWrite(redPin, map(currentcolor[0], 0, 255, 0, max_red));
    analogWrite(greenPin, map(currentcolor[1], 0, 255, 0, max_green));
    analogWrite(bluePin, map(currentcolor[2], 0, 255, 0, max_blue));
    LEDincrease_rate=1;
  }
}
void morningVibrate()   //这个就是一个普通function
{
  unsigned long currentMillis = millis();
  if (wakeup_start_state()<1) {analogWrite(motorPin,0);}
  if(wakeup_start_state()>=1){
        Serial.println("motor on");
    analogWrite(motorPin, map(vib_max, 0, 254, 34, max_motor));
  }

}
//if currenttime is wakeup_preocess_time before sleeptime

void getinput()
{
  in = "";
  while (Serial.available() > 0) 
  {
    in += char(Serial.read());
    delay(2);
  }

}
void readfile(){
  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    while (myFile.available()) {// read from the file
      Serial.write(myFile.read());
    }
    myFile.close();// close the file:
  } 
  else {// if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
void flashPressureIndicationLED(){
  if (v>350)digitalWrite(pressureled, LOW);
  else digitalWrite(pressureled, HIGH);

}

void checktesttxt(){
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile)  SD.remove("test.txt"); 
  else Serial.println("error opening test.txt");

  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to test.txt...");
    //myFile.println("Sleep record:");
    myFile.close();
    Serial.println("done.");
  } 
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
void checksettingtxt(){
  String settingTimeString ="";
  myFile = SD.open("setting.txt");
  if (myFile) {
    Serial.println("reading from setting.txt...");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      int settingread=myFile.read();
      if (isDigit(settingread))//means when it see a number
      {
        settingTimeString += char(settingread);  
      }

    }
    Serial.print("sleeptime is:");
    Serial.println(settingTimeString);
    sleeptime= settingTimeString.toInt();
    myFile.close();
  } 
  else {
    Serial.println("error opening setting.txt");
  }
}
void record_pressure_each_interval(){
  if (ct - pressure_last_time>=pressure_record_interval){
    pressure_last_time=ct;
    myFile = SD.open("test.txt", FILE_WRITE);    
    if (myFile==1&&stopw==0) {
      myFile.println(v);
      Serial.println("writing*****");
      //delay (pressure_record_interval);
      /*
      getinput();
       if (in=="end"){
       myFile.println("end");
       stopw=1;
       }
       else if (in=="clear"){  
       Serial.println("Removing test.txt...");
       SD.remove("test.txt");
       stopw=1;
       }
       if (in=="time"){
       checktime();
       }
       if (sleeptime<=millis()){
       stopw=1;
       }
       */
      myFile.close();
    } 

    else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
      //delay (600);
    }
  }
}
void morningmusic(){
  mp3_set_volume (Sound_max);
  //mp3_pause();
  if(wakeup_start_state()>=3){
    mp3_play ();
  }
}
void btsetup(){
  //Setup Bluetooth serial connection to android
  bluetooth.begin(115200);
  bluetooth.print("$$$");
  delay(100);
  bluetooth.println("U,9600,N");
  bluetooth.begin(9600);
}
void bt_comunicate(){
  //Read from bluetooth and write to usb serial 
  if(bluetooth.available())
  {
    char toSend = (char)bluetooth.read();
    Serial.print(toSend);
    if(toSend)
    {
      command+=toSend;
    }
    if(command.indexOf('\n')!=-1)//when there is no'\n' in command
    {
      bluetooth.println("Received Command: "+command);
      cc=command;
      Serial.println(cc);
      command="";//this line moved to parseCMD()
    }
  }

  //Read from usb serial to bluetooth
  if(Serial.available())
  {
    char toSend = (char)Serial.read();
    bluetooth.print(toSend);
  }
}

void parseCMD()
{
  String cmd=cc.substring(0,2);
  int num =cc.substring(2).toInt();
  Serial.println(num);

  if (cmd=="L|"){
    brightness_max=num;
    Serial.print("change bright=");
    Serial.println(brightness_max);
  }
  if (cmd=="V|"){
    vib_max=num;
    Serial.print("change vib_max=");
    Serial.println(vib_max);
  }
  if (cmd=="S|"){
    Sound_max=num;
        Serial.print("change sound_max=");
    Serial.println(Sound_max);
  }

}


//

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(pressureled, OUTPUT);  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  analogWrite(motorPin,0);

  btsetup();
  mp3_set_serial (Serial);    //set Serial for DFPlayer-mini mp3 module 
  mp3_set_volume (Sound_max);
  mp3_play (1);
  delay(2000);
  mp3_pause ();


  Serial.print("Initializing SD card...");
  pinMode(4, OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
   checktesttxt();
   checksettingtxt();
  sleeptime=24840000;//need to change before final************************************************************
}
void loop()
{
  mp3_set_volume (Sound_max);
  //mp3_pause();
  if(wakeup_start_state()>=3){
    mp3_play ();
  }
}
/*
void loop()
{
  ct=millis();
  if ((ct<3200)&&(ct<sleeptime-alarm_before_wakeup)){
   // checksettingtxt();
   analogWrite(motorPin,0);
  }
  if ((lastt+200<ct)&&(ct<sleeptime-alarm_before_wakeup)){
    lastt=ct;
    Serial.println("--------------------------");
    bt_comunicate();  
    v = analogRead(sensorPin);  
    flashPressureIndicationLED(); 
    Serial.print("read pressure:");
    Serial.println(v); 
    record_pressure_each_interval();
    parseCMD();
    analogWrite(motorPin,0);
  }

  if ((lastt+200<ct)&&(ct>sleeptime-alarm_before_wakeup)&&(alarm_onoff==1))
  {
    lastt=ct;  
        if (analogRead(A1)<=5){
      alarm_onoff=0;

    }
    Serial.println("--------------------------");
    bt_comunicate();  
    v = analogRead(sensorPin);  
    flashPressureIndicationLED(); 
    Serial.print("read pressure:");
    Serial.println(v); 
    record_pressure_each_interval();
    morningLED();
    morningVibrate();
    morningmusic();
    parseCMD();
  }
    if ((lastt+200<ct)&&(ct>sleeptime-alarm_before_wakeup)&&(alarm_onoff==0))
  {
    lastt=ct;
    Serial.println("--000000");
    mp3_pause();
    bt_comunicate();
  }
}

*/



