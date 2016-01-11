/*
1. setup the adruino and make sure bluetooth RX is on pin 3 and bluetooth Tx is on pin 2
 this script does not need you to unplug the bluetooth when uploading scripts
 2. upload the .ino script from adruino ide
 3. run the phone app v1.2
 4. click on Open and wait for "Bluetooth Opened" message
 5. type 22 in text field before Set Light(get"L|22")
 6. click on Set Light button
 7. watch the log. You can see the string on adruino serial monitor as well.
 8. Play with other fields.
 */
 
int keepsending= 1;
 
#include <SoftwareSerial.h>
#include <SD.h>
//int light=0;
File myFile;
int last_time_4= 0;
int bluetoothTx = 7;
int bluetoothRx = 8;
String command="";
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);
int vib_max=0; //change to something higher**************************************
int brightness_max=0; //change to somethign higher*****************************
int Sound_max=0;//change to somethign higher*****************************
int lines=0;
void setup()
{
  //Setup usb serial connection to computer
  Serial.begin(9600);
  btsetup();
  Serial.print("Initializing SD card...");
  pinMode(4, OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void loop()
{ 
  bt_comunicate();
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
      parseCMD(command);
      bluetooth.println("Received Command: "+command);
      command="";
    }
  }

  //Read from usb serial to bluetooth
  if(Serial.available())
  {
    char toSend = (char)Serial.read();
    bluetooth.print(toSend);
  }
}

void parseCMD(String cmd)
{

  if (cmd.substring(0,2)=="s|"){
    Serial.println("try open test");
      myFile = SD.open("test.txt");
    if (myFile) {
       Serial.println("test.txt:");

      // read from the file until there's nothing else in it:
      //while (myFile.available()) 
      if (myFile==1)
      {
        Serial.println("pppprinting------2");
        
        while(keepsending==1){
          if (millis()>last_time_4+10){
          last_time_4=millis();
          int pri=myFile.read();
           if (pri==-1){
            keepsending= 0;
            }
            Serial.println(pri);
            bluetooth.print("Pdata|");
            bluetooth.println(pri);
          }
          
        }
        bluetooth.println("PdataEnd");
        // bluetooth.println("DT|"+myFile.read());
      }
      // close the file:
      myFile.close();
    } 
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt for printing");
    }
  }
}





