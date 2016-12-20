

#include <SoftwareSerial.h>

// LED 
int ledPin = 13;
int sensor_pin=A0;  // variable for sensor
float sample=0;


// replace with your channel's thingspeak API key
String apiKey = "";

// connect 2 to TX of Serial USB
// connect 3 to RX of serial USB
SoftwareSerial ser(4,5); // RX, TX

// this runs once
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);    

  // enable debug serial
  Serial.begin(115200); 
  // enable software serial
  ser.begin(115200);
  
  // reset ESP8266
  ser.println("AT+RST");
}


// the loop 
void loop() {
  

  voltage();
  esp_8266();
}
void voltage()
{
    sample=0; 
  for(int i=0;i<150;i++)
  {
    sample+=analogRead(sensor_pin);  //read the value from the sensor
    delay(2);
  }
  sample = sample/150;
 
  
}
void esp_8266()
{
 // convert to string
  char buf[32];
  String strVolt = dtostrf(sample, 4, 1, buf);
  Serial.print(strVolt);
  Serial.println(" V");
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);
   
  if(ser.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field4=";
  getStr += String(strVolt);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);

  if(ser.find(">")){
    ser.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
    
  // thingspeak needs 15 sec delay between updates
  delay(16000);  
}
