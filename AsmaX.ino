#include <DHT.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
LiquidCrystal lcd(48, 49, 50, 51, 52, 53);
#define DHT11_PIN 46
#define LED_PIN 28
#define BUTTON_PIN 32

DHT dht(DHT11_PIN, DHT11);
SoftwareSerial SIM900(7, 8); 
#define MQ2pin 0

float air;  //variable to store sensor value
float temp, hum;
float high_temp_thres= 29, high_hum_thres= 65, high_air_thres= 100, high_pulse_thres= 120, low_temp_thres= 26.1, low_hum_thres= 30, low_air_thres= 51, low_pulse_thres= 40;
int count;
const int PulseWire = A5;
const int LED13 = 13; // The on-board Arduino LED, close to PIN 13.
int myBPM;
int Threshold = 550;

PulseSensorPlayground pulseSensor;


void setup(){
  lcd.begin(16, 2);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pulseSensor.analogInput(PulseWire);   
  // pulseSensor.blinkOnPulse(LED);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold); 
  if(pulseSensor.begin())
  {
    Serial.println("pulse sensing in action!");
  }
  // Arduino communicates with SIM900 GSM shield at a baud rate of 19200
  SIM900.begin(19200);
  // time to  GSM shield to log on to network
  delay(5000);
  count = 0;
} 

void loop(){
  digitalWrite(LED_PIN, LOW);
  readTempHum();
  readAirSensor();
  // delay(20000);
  readPulse();
// if hum / temp / air / pulse  > thres
// 	turn red
  if (hum > high_hum_thres || temp > high_temp_thres || air > high_air_thres || myBPM  > high_pulse_thres|| hum < low_hum_thres || temp < low_temp_thres || air < low_air_thres ||  myBPM  < low_pulse_thres)
  {
    digitalWrite(LED_PIN, HIGH);               
  }
  // print message when humidity is critical
  if ( hum < low_hum_thres)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print ("Alert!");
    lcd.setCursor(0,1);
    lcd.print ("low humidity");
  }
  if (hum > high_hum_thres)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print ("Alert!");
    lcd.setCursor(0,1);
    lcd.print ("high humidity");
    delay(5000);
    
  }
  // print message when temperature is critical
  if ( temp < low_temp_thres)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print ("Alert!");
    lcd.setCursor(0,1);
    lcd.print ("low temperature");
  }
  if (temp > high_temp_thres)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print ("Alert!");
    lcd.setCursor(0,1);
    lcd.print ("high temperature");
    delay(5000);
    // lcd.clear();
  }
  // print message when air is critical
  if ( air < low_air_thres || air > high_air_thres)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print ("Alert! Poor");
    lcd.setCursor(0,1);
    lcd.print ("air Quality");
  }
  // print message when pulse is critical
  if ( myBPM > high_pulse_thres)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print ("Alert! Quick");
    lcd.setCursor(0,1);
    lcd.print ("heart beats !");
    delay(3000);
  }
  if ( myBPM < low_pulse_thres)
  {
    count++;
    if (count >= 5) 
    {
      sendSMS("Please help! Your patient's heart is failing!");
      delay(30000);
      count = 0;
    }               
          
  }
  else
  {
    count = 0;
  }  
//when button is clicked, send sms
  if (digitalRead(BUTTON_PIN) == LOW) 
  {
    sendSMS("Please help! I am having a heart attack!");
  }
}
void sendSMS(char msg[]) {
  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);

  // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
  // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
  SIM900.println("AT+CMGS=\"+250734487843\""); 
  delay(100);
  
  SIM900.println(msg); 
  SIM900.println("Temp = ");
  SIM900.println(temp);
  SIM900.println("hum = ");
  SIM900.println(hum);
  SIM900.println("pulse = ");
  SIM900.println(myBPM);
  delay(100);

  // End AT command with a ^Z, ASCII code 26
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  // Give module time to send SMS
  delay(5000); 
}
void readPulse() {

  // lcd.print("heart");
	// if (pulseSensor.sawStartOfBeat()) 
  {            // Constantly test to see if "a beat happened".
    myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                               // "myBPM" hold this BPM value now. 
    // Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
     Serial.print("BPM: ");                        // Print phrase "BPM: " 
     Serial.println(myBPM);                        // Print the value inside of myBPM. 
    lcd.clear();
	  lcd.setCursor(0,0);	
	  lcd.print("BPM: ");
	  lcd.setCursor(0,1);
	  lcd.print(myBPM);

	  delay(5000);
}

  // delay(20);    
}
void readAirSensor() {
	air = analogRead(MQ2pin); // read analog input pin 0
 
	lcd.setCursor(0,0);	
	lcd.print("Air Q: ");
	lcd.setCursor(0,12);
	lcd.print(air);
	
	delay(2000); // wait 2s for next reading
}

void readTempHum()
{
  temp = dht.readTemperature();
  hum = dht.readHumidity();
   lcd.clear();
  lcd.setCursor(7,0); 
  lcd.print("T:");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(7,1);
  lcd.print("H: ");
  lcd.print(hum);
  lcd.print("%");
  // delay(3000);
}
