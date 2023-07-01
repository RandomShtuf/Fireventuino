// Import dependencies
#include <SoftwareSerial.h>

// define rx and tx
#define RX 3
#define TX 2

// set up info for wifi module
String AP = "SSID of Your WiFi";
String PASS = "PASSWORD of Your WiFi";
String API = "API KEY";
String HOST = "api.thingspeak.com";
String PORT = "80";
String gas = "field1";
String temperature = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false;

// set up sensor information
int valSensor = 1;
int gasPin = A0;    // Analog pin connected to the AOUT pin of MQ-5 sensor
int digitalPin = 4; // Digital pin connected to the DOUT pin of MQ-5 sensor
int smokeThreshold = 75;
int buzzerPin = 9;  // Digital pin connected to the DOUT pin of buzzer
int lm35Pin = A1;   // Analog pin connected to the AOUT pin of lm35
float tempThreshold = 36.00;

// set up serial communication for wifi
SoftwareSerial esp8266(RX,TX);

void setup() {
  // initialize sensors, buzzer and serial communication
  Serial.begin(9600);    // Initialize serial communication
  pinMode(digitalPin, INPUT); // Set the digital pin as an input
  pinMode(buzzerPin, OUTPUT);
  esp8266.begin(115200);

  // initialize wifi
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
  // read sensor inputs
  int gasValue = analogRead(gasPin); // Read the analog value from the sensor
  int digitalValue = digitalRead(digitalPin); // Read the digital value from the sensor
  int temperatureValue = analogRead(lm35Pin);

  float temperatureC = (temperatureValue * 5.0 / 1023.0) * 100;

  Serial.print("Analog Value: ");
  Serial.println(gasValue); // Print the analog value
  Serial.print("Digital Value: ");
  Serial.println(digitalValue); // Print the digital value
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  // send sensor readings to thingspeak
  String getData = "GET /update?api_key="+ API +"&"+ gas +"="+String(gasValue)+"&"+ temperature +"="+String(temperatureC);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);delay(1500);countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");

  // trigger buzzer when smoke is detected and temperature is high
  if (gasValue > smokeThreshold && temperatureC > tempThreshold) {
    digitalWrite(buzzerPin, HIGH);
    delay(1000);

    digitalWrite(buzzerPin, LOW);
    delay(1000);

    digitalWrite(buzzerPin, HIGH);
    delay(1000);

    digitalWrite(buzzerPin, LOW);
    delay(1000);

    digitalWrite(buzzerPin, HIGH);
    delay(1000);

    digitalWrite(buzzerPin, LOW);
    delay(1000);

    digitalWrite(buzzerPin, HIGH);
    delay(1000);

    digitalWrite(buzzerPin, LOW);
    delay(1000);

    digitalWrite(buzzerPin, HIGH);
    delay(1000);

    digitalWrite(buzzerPin, LOW);
    delay(1000);
  }



  delay(1000); // Delay for 1 second
}

// define functions
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }