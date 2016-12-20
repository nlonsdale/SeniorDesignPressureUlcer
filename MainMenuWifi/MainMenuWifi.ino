//P Bui, K Dhami, N Lonsdale, H Lor

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LCD.h>
#include <SoftwareSerial.h>
#include <virtuabotixRTC.h>
#include <SPI.h>

float sample[9]={0};
String apiKey = "MTSDQ3QK31ETOQTD";

SoftwareSerial BTserial(2,3); //RX-TX
//SoftwareSerial ser(4,5); // RX, TX

#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

#define WeightRelease 3

#define countto 4

#define relieve 5

LiquidCrystal_I2C  lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin); //I2C pins are A4==SDA, A5==SCL
virtuabotixRTC myRTC(6, 7, 8);

int val0 = 0;     // variable for reading the pin status
int val1 = 0;
int val2 = 0;

int ButtonVal = 0;

double sensorValue[9];
double weight[9];
double voltage[9];


int trigger=10;
double weighthighest=WeightRelease;


double TimerSensor; //Hue
long prevMillis; //hue
long currentMillis; //hue
long prevMillis2; //hue
long currentMillis2; //hue
long oneSecond = 1000;//hue
long wifiDelay = 15000;
double WeightAdded[9];//hue
int TimerCount[9];
int releaseCount;

long prevMillis3;

int t1;
int i = 0;


#define BUTTON_PIN0 22
#define BUTTON_PIN1 24
#define BUTTON_PIN2 26

char c = ' ';

void setup() {
  // put your setup code here, to run once:
  lcd.begin (20, 4);
  // myRTC.setDS1302Time(00, 7, 13, 5, 9, 12, 2016);


  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();

  pinMode(BUTTON_PIN0, INPUT_PULLUP);
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  pinMode (28, INPUT_PULLUP);


  Serial.begin(115200);
  BTserial.begin(9600);
  
  Serial1.begin(115200);//was 115200
  Serial1.println("AT+RST");

}

/**************************************************************************************************************************************/
void loop() {


 // wifisend();
 //voltageN();
  esp_8266();
  myRTC.updateTime();

  currentMillis = millis(); //hue
  NotifyTimer(); //hue

  if (BTserial.available())
  {
    c = BTserial.read();
    Serial.write(c);

  }


  if (Serial.available())
  {
    c = Serial.read();
    BTserial.write(c);

  }

  if (digitalRead(28) == 0)
  {

    BTserial.write('1');
    Serial.print('1'); //Hue

    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("BLUETOOTH");
    lcd.setCursor(2, 2);
    lcd.print("Transmitting BT");


    //delay(1500);
    lcd.clear();
   // BTserial.write('0');

  }

  val0 = digitalRead(BUTTON_PIN0);
  val1 = digitalRead(BUTTON_PIN1);
  val2 = digitalRead(BUTTON_PIN2);

  sensorValue[0] = analogRead(A0);
  sensorValue[1] = analogRead(A1);
  sensorValue[2] = analogRead(A2);
  sensorValue[3] = analogRead(A3);
  sensorValue[4] = analogRead(A4);
  sensorValue[5] = analogRead(A5);
  sensorValue[6] = analogRead(A6);
  sensorValue[7] = analogRead(A7);
  sensorValue[8] = analogRead(A8);

  for (i = 0; i <= 8; i++)
  {

    voltage[i] = sensorValue[i] * (5.0 / 1023.0);

    if (voltage[i] <= 3.5)
    {
      weight[i] = .337 * exp(.8577 * voltage[i]);

    } else if (voltage[i] > 3.5)
    {

      weight[i] = (8.4709 * voltage[i] * voltage[i] * voltage[i] * voltage[i]) - (116.28 * voltage[i] * voltage[i] * voltage[i]) + (590.04 * voltage[i] * voltage[i]) - (1303.6 * voltage[i]) + 1056.4;
    }

    if (voltage[i] <= .5)

    {

      weight[i] = 0;
    }

  }

  if (val0 == 0) {
     lcd.clear();
    ButtonVal = 0;
    trigger=10;
   
  }
  if (val1 == 0) {
       lcd.clear();
    ButtonVal = 1;
    trigger=10;
   
  }
  if (val2 == 0) {
       lcd.clear();
    ButtonVal = 2;
    trigger=10;
   
  }

  switch (ButtonVal) {
    //main menu
    case 0:
      
      lcd.home();
      lcd.print("Main Menu");
      lcd.setCursor(0, 1);
      //lcd.setCursor(0,2); 
      //lcd.print("Triggered Sensor"); lcd.print(trigger);
      triggerSwitch();
      break;
    case 1:
     
       
      //print time second, minute, and hour
      //print text options for buttons
      lcd.home();
      lcd.print("Pressure Points");

      lcd.setCursor(1, 1);
      lcd.print(weight[0]); lcd.print("  "); lcd.print(weight[1]); lcd.print("  "); lcd.print(weight[2]);
      lcd.setCursor(1, 2);
      lcd.print(weight[3]); lcd.print("  "); lcd.print(weight[4]); lcd.print("  "); lcd.print(weight[5]);
      lcd.setCursor(1, 3);
      lcd.print(weight[6]); lcd.print("  "); lcd.print(weight[7]); lcd.print("  "); lcd.print(weight[8]);
      triggerSwitch();
      //lcd.print("Voltage: "); lcd.print(voltage[0]);
      break;
    //change time
    case 2:
      
      lcd.home();
      lcd.print("Time and Date");
      lcd.setCursor(0, 2);
      lcd.print(myRTC.hours); lcd.print(":"); lcd.print(myRTC.minutes); lcd.print(":"); lcd.print(myRTC.seconds);
      lcd.setCursor(0, 3);
      lcd.print(myRTC.month); lcd.print("-"); lcd.print(myRTC.dayofmonth); lcd.print("-"); lcd.print(myRTC.year);

      lcd.setCursor(0, 1);
      triggerSwitch();
      break;
    default:
      lcd.home();
      lcd.print("");
      lcd.setCursor(0, 1);
      triggerSwitch();
  }

}

void NotifyTimer () { //hue
  if ( currentMillis - prevMillis > oneSecond) {
    int x;
    prevMillis = currentMillis;
    if ( weight[0] >= WeightRelease | weight[8] >= WeightRelease | weight[7] >= WeightRelease | weight[6] >= WeightRelease | weight[5] >= WeightRelease | weight[4] >= WeightRelease | weight[3] >= WeightRelease | weight[2] >= WeightRelease | weight[1] >= WeightRelease) {
      for (x = 0; x <= 8; x++) {
        TimerCount[x] = TimerCount[x] + 1;
        WeightAdded[x] = WeightAdded[x] + weight[x];
      }
    }
    else {
      for (x = 0; x <= 8; x++) {
        TimerCount[x] = 0;
        WeightAdded[x] = 0;
      }
    }
    if (TimerCount[1] == countto |TimerCount[2] == countto |TimerCount[3] == countto |TimerCount[4] == countto |TimerCount[5] == countto |TimerCount[6] == countto |TimerCount[7] == countto |TimerCount[8] == countto |TimerCount[9] == countto ){
      BTserial.write('1');

      lcd.clear();
      ButtonVal = 0;
      for (i=0; i<=8;i++)
      {
        
        if ( WeightAdded[i]>weighthighest)
        {
          weighthighest= WeightAdded[i];
          trigger=i;
        }
      }
      Serial.print("Release Pressure off of Sensor: ");
      Serial.print(trigger);
      Serial.println();
      weighthighest=0;
      //trigger=0;
      for (x = 0; x <= 8; x++) {
        TimerCount[x] = 0;
        WeightAdded[x] = 0;
      }
    }
    Serial.print(TimerCount[1]);
    Serial.print("   ");
    Serial.print(weight[0]);
    Serial.println();
  }  
}

void triggerSwitch() {
  currentMillis2 = millis();
  switch (trigger){
    case 0:
    
      lcd.setCursor(0,2); 
      lcd.print("Front Left"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[0] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }
      
    break;
    case 1:
    ButtonVal = 0;
      lcd.setCursor(0,2); 
      lcd.print("Front Center"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[1] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }
      
    break;
    case 2:

    lcd.setCursor(0,2); 
      lcd.print("Front Right"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[2] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }

    break;
    case 3:
    lcd.setCursor(0,2); 
      lcd.print("Middle Left"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[3] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }

    break;
    case 4:
lcd.setCursor(0,2); 
      lcd.print("Middle Center"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[4] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }
      
    break;
    case 5:
      lcd.setCursor(0,2); 
      lcd.print("Middle Right"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[5] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }
      
    break;
    case 6:
      lcd.setCursor(0,2); 
      lcd.print("Back Left"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[6] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }
      
    break;
    case 7:
lcd.setCursor(0,2); 
      lcd.print("Back Center"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[7] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }
    break;
    
    case 8:
    lcd.setCursor(0,2); 
      lcd.print("Back Right"); 
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        if (weight[8] < WeightRelease){
          releaseCount = releaseCount + 1;
          if (releaseCount == relieve){
            trigger = 9;
            releaseCount = 0;
          }
        }
      }
      
    break;
    case 9:
      lcd.setCursor(0,2); 
      lcd.print("Pressure Relieved");
      BTserial.write('0');
      if ( currentMillis2 - prevMillis2 > oneSecond) {
        prevMillis2 = currentMillis2;
        t1 = t1 +1;
        if (t1 == relieve){
          lcd.clear();
          trigger=10;
          t1 = 0;
        }
      }
    break;
    default:
      lcd.setCursor(0,2); 
   
    
  }
}

//void voltageN()
//{
//    sample[0]=0; 
//    sample[1]=0;
//    sample[2]=0;
//    sample[3]=0;
//    sample[4]=0;
//    sample[5]=0;
//    sample[6]=0;
//    sample[7]=0;
//    sample[8]=0;
//  for(int i=0;i<150;i++)
//  {
//    sample[0]+=A0;  //read the value from the sensor
//    sample[1]+=A1;
//    sample[2]+=A2;
//    sample[3]+=weight[3];
//    sample[4]+=weight[4];
//    sample[5]+=weight[5];
//    sample[6]+=weight[6];
//    sample[7]+=weight[7];
//    sample[8]+=weight[8];
//    
//  }
//  sample[0] = sample[0]/150;
//  sample[1] = sample[1]/150;
//  sample[2] = sample[2]/150;
//  sample[3] = sample[3]/150;
//  sample[4] = sample[4]/150;
//  sample[5] = sample[5]/150;
//  sample[6] = sample[6]/150;
//  sample[7] = sample[7]/150;
//  sample[8] = sample[8]/150;
//
//}


void esp_8266()
{

  if(currentMillis - prevMillis3 > wifiDelay)
  {
    prevMillis3 = currentMillis;
 // convert to string
  /*char buf0[10];
  char buf1[10];
  char buf2[10];
  char buf3[10];
  char buf4[10];
  char buf5[10];
  char buf6[10];
  char buf7[10];
  char buf8[10];
  
  
  String weightReading_0 = dtostrf(sample[0], 4, 1, buf0);
  String weightReading_1 = dtostrf(sample[1], 4, 1, buf0);
  String weightReading_2 = dtostrf(sample[2], 4, 1, buf0);
  String weightReading_3 = dtostrf(sample[3], 4, 1, buf0);
  String weightReading_4 = dtostrf(sample[4], 4, 1, buf0);
  String weightReading_5 = dtostrf(sample[5], 4, 1, buf0);
  String weightReading_6 = dtostrf(sample[6], 4, 1, buf0);
  String weightReading_7 = dtostrf(sample[7], 4, 1, buf0);
  String weightReading_8 = dtostrf(sample[8], 4, 1, buf0);
  */
  // TCP connection

  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  Serial1.println(cmd);
 
  if(Serial1.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET /update?api_key=";
  //String getStr1 = "GET /update?api_key=";
  getStr+= apiKey;
  //getStr1+= apiKey;
  
  getStr +="&field1=";
  getStr += String(weight[0]);

   getStr +="&field2=";
  getStr += String(weight[1]);

  

  getStr +="&field3=";
  getStr += String(weight[2]);
 

  getStr +="&field4=";
  getStr += String(weight[3]);


  getStr +="&field5=";
  getStr += String(weight[4]);



  getStr +="&field6=";
  getStr += String(weight[5]);


 
  getStr +="&field7=";
  getStr += String(weight[6]);



  getStr +="&field8=";
  getStr += String(weight[7]);


/*
  getStr +="&field8=";
  getStr += String(weightReading_8);
  */
   getStr += "\r\n\r\n";
   //getStr1 += "\r\n\r\n";



  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  Serial1.println(cmd);

  
  int y = 0;
  for(y=0; y < 8; y++){
    Serial.print(weight[y]);
    Serial.print("  ");
  }
  

  if(Serial1.find(">")){
    Serial1.print(getStr);
   
  }
  else{
    Serial1.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
    
  // thingspeak needs 15 sec delay between updates
  }
}
/*
void wifisend(){

  // enable debug serial
  Serial.begin(9600); 
  // enable software serial
 
  
  // reset ESP8266
  //Serial1.println("AT+RST");

  voltageN();
  esp_8266();
  

  //Serial.begin(9600); 

}
*/
