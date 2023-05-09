// This code includes necessary libraries and sets up the pins for different sensors and components

// DHT library for reading temperature and humidity from DHT11 sensor
#include <DHT.h>

// LiquidCrystal library for controlling LCD screen
#include <LiquidCrystal.h>

// SoftwareSerial library for serial communication with SIM900 module
#include <SoftwareSerial.h>

#define USE_ARDUINO_INTERRUPTS true

// PulseSensorPlayground library for measuring heart rate from pulse sensor
#include <PulseSensorPlayground.h>

// Define LCD object with pin numbers
LiquidCrystal lcd(48, 49, 50, 51, 52, 53);

// Define pin numbers for different sensors and components
#define DHT11_PIN 46
#define LED_PIN 28
#define BUTTON_PIN 32
#define MQ2pin 0
const int PulseWire = A5;
const int LED13 = 13; // The on-board Arduino LED, close to PIN 13.

// Initialize DHT object with pin number and sensor type
DHT dht(DHT11_PIN, DHT11);

// Initialize SoftwareSerial object with pin numbers for SIM900 module
SoftwareSerial SIM900(7, 8);

// Initialize variables to store sensor values and threshold values
float air;
float temp, hum;
float high_temp_thres= 29, high_hum_thres= 65, high_air_thres= 100, high_pulse_thres= 120, low_temp_thres= 26.1, low_hum_thres= 30, low_air_thres= 51, low_pulse_thres= 40;
int count;
int myBPM;
int Threshold = 550;

// Initialize PulseSensorPlayground object with pulse wire pin number
PulseSensorPlayground pulseSensor;



// Initialize LCD, DHT11 sensor, LED pin, button pin, and pulse sensor
void setup(){
lcd.begin(16, 2); // Initialize LCD with 16 columns and 2 rows
dht.begin(); // Initialize DHT11 sensor
pinMode(LED_PIN, OUTPUT); // Set LED pin as an output
pinMode(BUTTON_PIN, INPUT); // Set button pin as an input
pulseSensor.analogInput(PulseWire); // Set the analog input pin for the pulse sensor
// pulseSensor.blinkOnPulse(LED); // Enable auto-blinking of Arduino's LED with heartbeat
pulseSensor.setThreshold(Threshold); // Set the threshold value for the pulse sensor
// If pulse sensor is successfully initialized, print message
if(pulseSensor.begin())
{
Serial.println("Pulse sensing in action!");
}
// Set baud rate for GSM shield to 19200 and initialize communication
SIM900.begin(19200);
// Wait for GSM shield to log on to network
delay(5000);
// Set counter variable to 0
count = 0;
}


// The main loop that runs continuously
void loop(){
// Turn off LED
digitalWrite(LED_PIN, LOW);

// Read sensor values
readTempHum();
readAirSensor();
readPulse();

// Check if sensor values are above or below threshold values
if (hum > high_hum_thres || temp > high_temp_thres || air > high_air_thres || myBPM > high_pulse_thres || hum < low_hum_thres || temp < low_temp_thres || air < low_air_thres || myBPM < low_pulse_thres)
{
// Turn on LED if any value is above or below threshold
digitalWrite(LED_PIN, HIGH);
}

// Alert messages for different sensor values
if (hum < low_hum_thres)
{
// Display low humidity alert message
lcd.clear();
lcd.setCursor(0,0);
lcd.print ("Alert!");
lcd.setCursor(0,1);
lcd.print ("low humidity");
}
if (hum > high_hum_thres)
{
// Display high humidity alert message and wait for 5 seconds
lcd.clear();
lcd.setCursor(0,0);
lcd.print ("Alert!");
lcd.setCursor(0,1);
lcd.print ("high humidity");
delay(5000);
}
if (temp < low_temp_thres)
{
// Display low temperature alert message
lcd.clear();
lcd.setCursor(0,0);
lcd.print ("Alert!");
lcd.setCursor(0,1);
lcd.print ("low temperature");
}
if (temp > high_temp_thres)
{
// Display high temperature alert message and wait for 5 seconds
lcd.clear();
lcd.setCursor(0,0);
lcd.print ("Alert!");
lcd.setCursor(0,1);
lcd.print ("high temperature");
delay(5000);
}
if (air < low_air_thres || air > high_air_thres)
{
// Display poor air quality alert message
lcd.clear();
lcd.setCursor(0,0);
lcd.print ("Alert! Poor");
lcd.setCursor(0,1);
lcd.print ("air Quality");
}
if (myBPM > high_pulse_thres)
{
// Display quick heart beats alert message and wait for 3 seconds
lcd.clear();
lcd.setCursor(0,0);
lcd.print ("Alert! Quick");
lcd.setCursor(0,1);
lcd.print ("heart beats !");
delay(3000);
}
if (myBPM < low_pulse_thres)
{
// If pulse rate is below threshold, send SMS for help after counting for 5 loops
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
// Reset count if pulse rate is above threshold
count = 0;
}

// Send SMS for help if button is clicked
if (digitalRead(BUTTON_PIN) == LOW)
{
sendSMS("Please help! I am having a heart attack!");
}
}

// This function is used to send an SMS message to a specified mobile number.

void sendSMS(char msg[]) {
// AT command to set SIM900 to SMS mode
SIM900.print("AT+CMGF=1\r");
delay(100);

// REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
// USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
SIM900.println("AT+CMGS="+250734487843"");
delay(100);

// Print the message to be sent, along with the temperature, humidity and pulse values.
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

/ This function is used to read the pulse sensor and display the BPM on the serial monitor and the LCD display.

void readPulse() {

// lcd.print("heart"); // This line is commented out and has no effect on the code.

// Check if the pulse sensor detected a heartbeat.
if (pulseSensor.sawStartOfBeat()) {
// If a heartbeat is detected, get the BPM value.
myBPM = pulseSensor.getBeatsPerMinute();
// Clear the LCD display and print the BPM value on the first row.
lcd.clear();
lcd.setCursor(0,0);
lcd.print("BPM: ");
lcd.setCursor(0,1);

// Print the BPM value on the second row of the LCD display.
lcd.print(myBPM);

// Wait for 5 seconds before reading the pulse sensor again.
delay(5000);	
}
		
// readAirSensor() - Function to read the air quality sensor

void readAirSensor() {
  air = analogRead(MQ2pin); // Read analog input pin 0 and store in air variable

  // Update LCD display with air quality value
  lcd.setCursor(0,0); // Set the cursor to the first row and first column
  lcd.print("Air Q: "); // Print "Air Q: " text
  lcd.setCursor(0,12); // Set the cursor to the first row and twelfth column
  lcd.print(air); // Print the air quality value

  delay(2000); // Wait for 2 seconds before taking next reading
}

void readTempHum()
{
  // Read temperature and humidity from DHT sensor
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  
  // Clear LCD screen
  lcd.clear();
  
  // Set cursor position to column 7, row 0
  lcd.setCursor(7, 0);
  
  // Print "T:" to LCD
  lcd.print("T:");
  
  // Print temperature value to LCD
  lcd.print(temp);
  
  // Print degree symbol to LCD
  lcd.print((char)223);
  
  // Print "C" to LCD
  lcd.print("C");
  
  // Set cursor position to column 7, row 1
  lcd.setCursor(7, 1);
  
  // Print "H:" to LCD
  lcd.print("H:");
  
  // Print humidity value to LCD
  lcd.print(hum);
  
  // Print "%" to LCD
  lcd.print("%");
  
  // Uncomment the following line if you want to add a delay of 3 seconds before returning
  // This can be useful if you want to limit the frequency of sensor readings
  //delay(3000);
}
