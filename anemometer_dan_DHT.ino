#include "DHT.h"
#include <SPI.h> //for the SD card module
#include <SD.h> // for the SD card
#include <RTClib.h>
#define DHTPIN 3     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// Pin definitions
# define windPin 2 // Receive the data from sensor
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Constants definitions
const float pi = 3.14159265; // pi number
long period = 10000; // Measurement period (miliseconds)
long delaytime = 5000; // Time between samples (miliseconds)
int radio = 90; // Distance from center windmill to outer cup (mm)
int jml_celah = 18; // jumlah celah sensor

float h, t, f;

// Variable definitions
unsigned int Sample = 0; // Sample number
unsigned int counter = 0; // B/W counter for sensor
unsigned int RPM = 0; // Revol utions per minute
float speedwind = 0; // Wind speed (m/s)
DHT dht(DHTPIN, DHTTYPE);

int Reset = 12;
const int chipSelect = 53; 

//anyar
long sampling_time_ms = delaytime;
long log_time_ms = period;
long prev_log_time = 0;
long prev_sampling_time = 0;

int cal_cnt = 0;
// Create a file to store the data
File myFile;

// RTC
RTC_DS1307 rtc;

void setup(){

if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
Serial.println(F("SSD1306 allocation failed"));
for(;;); // Don't proceed, loop forever
}
  
// Set the pins
pinMode(2, INPUT);
  digitalWrite(2, HIGH);

// sets the serial port to 9600
Serial.begin(9600);
Serial.println("DHTxx test!");

// Splash screen
Serial.println("ANEMOMETER");
Serial.println("**********");
Serial.println("Based on depoinovasi anemometer sensor");
Serial.print("Sampling period: ");
Serial.print(period/1000);
Serial.print(" seconds every ");
Serial.print(delaytime/1000);
Serial.println(" seconds.");
Serial.println("** You could modify those values on code **");
Serial.println();
dht.begin();

 // setup for the RTC
  while (!Serial); // for Leonardo/Micro/Zero
 if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


  // setup for the SD card
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    digitalWrite(Reset, HIGH);
    delay(200); 
    pinMode(Reset, OUTPUT);
    delay(200);
    digitalWrite(Reset, LOW);
    //delay (10000);
    //digitalWrite(resetPin, HIGH);
    return;
  }
  Serial.println("initialization done.");

  //open file
  myFile = SD.open("DATA.txt", FILE_WRITE);

  // if the file opened ok, write to it:
  if (myFile) {
    Serial.println("File opened ok");
    // print the headings for our data
    myFile.println("Date,Time,RPM,WindSpeed,Temperature,Humidity");
  }
  myFile.close();
  displayOLED();
}

void loop(){
 //anyar
  unsigned long current_sampling_time;
  current_sampling_time = millis();
  unsigned long current_time;
  current_time = millis();  

  //anyar
  if ((current_sampling_time - prev_sampling_time) > sampling_time_ms) {
  prev_sampling_time = current_sampling_time;
  samplingSensor();
  displayOLED();
  }

  if ((current_time - prev_log_time) > log_time_ms) {
  prev_log_time = current_time;  
  loggingTime();
  loggingSensor();
  }
}

void displayOLED() 
{

display.clearDisplay();
display.setTextSize(1); // Normal 1:1 pixel scale
display.setTextColor(WHITE); // Draw white text
display.setCursor(70,0); // Start at top-left corner
DateTime now = rtc.now();
display.print(now.hour());
display.print(":");
display.print(now.minute());
display.print(":");
display.println(now.second());
display.print("Suhu: ");
display.print(t); // Draw 'inverse' text
display.println(" *C");
display.print("Klmbpn: ");
display.print(h);
display.println(" %");
display.print("Kec Angin: ");
display.print(speedwind);
display.println(" m/s");//tulis tulisan "BETTER"
display.display();
}

void loggingSensor() {
  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile) {
    Serial.println("open with success");
    //myFile.print(average);
    //myFile.print(",");
    myFile.print(RPM);
    myFile.print(",");
    myFile.print(speedwind);
    myFile.print(",");
    myFile.print(t);
    myFile.print(",");
    myFile.print(h);
    myFile.println(",");    
    }
  else {
    Serial.println("failed to write");
    //delay(3000);
    //digitalWrite(resetPin, HIGH);
    digitalWrite(Reset, HIGH);
    delay(200); 
    pinMode(Reset, OUTPUT);
    delay(200);
    digitalWrite(Reset, LOW);
    }
  myFile.close();
}

void loggingTime() {
 DateTime now = rtc.now();
  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    display.print(now.hour(), DEC);
    display.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(",");
    display.print(now.second(), DEC);
  }
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.println(now.day(), DEC);
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  display.print(now.hour(), DEC);
  display.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  display.print(now.minute(), DEC);
  display.print(':');
  Serial.println(now.second(), DEC);
  display.print(now.second(), DEC);
  myFile.close();
  //delay(1000);
}

void samplingSensor() {
  Sample++;
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(Sample);
  Serial.print(": Start measurement…");
  windvelocity();
  Serial.println(" finished.");
  Serial.print("Counter: ");
  Serial.print(counter);
  Serial.print("; RPM: ");
  RPMcalc();
  Serial.print(RPM);
  Serial.print("; Wind speed: ");
  WindSpeed();
  Serial.print(speedwind);
  Serial.print(" [m/s]");
  Serial.println();
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Humidity : ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
 // Wait a few seconds between measurements.
  delay(2000);   
}

// Measure wind speed
void windvelocity()
{
speedwind = 0;
counter = 0;
attachInterrupt(0, addcount, CHANGE);
unsigned long millis();
long startTime = millis();
while(millis() < startTime + period) {}

detachInterrupt(1);
}

void RPMcalc()
{
RPM=((counter/jml_celah)*60)/(period/1000)/2; // Calculate revolutions per minute (RPM)
}

void WindSpeed()
{
speedwind = ((2 * pi * radio * RPM)/60) / 1000; // Calculate wind speed on m/s
}


void addcount()
{
counter++;
}
