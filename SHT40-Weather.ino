/*************************************************** 
  SHT4x Humidity & Temp Sensor
  Adafruit 128x64 OLED FeatherWing
  Adafruit Feather M4 Express 

  Show high and low temperature for the last 24 hours. 
  Use an array of 1440 measurments and sort for the highest and lowest values to display.  
  Use buttons to display SHT40 information.
 ****************************************************/

#include <Adafruit_SHT4x.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define VBATPIN A6

float measuredBatteryVoltage = 0.0;
float t, h = 0.0;
float temperature_low, temperature_high = 0.0;
float humidity_low, humidity_high = 0.0;
// float temperature_array[1140] = {0.0};
// float humidity_array[1440] = {0.0};

Adafruit_SHT4x sht4 = Adafruit_SHT4x(); 
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

void setup() {
  Serial.begin(115200);

  // while (!Serial)  // Wait for serial connection
  //   delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("SHT40 and 128x64 OLED Weather Station");
  if (! sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);

  // You can have 3 different precisions, higher precision takes longer
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht4.getPrecision()) {
     case SHT4X_HIGH_PRECISION: 
       Serial.println("High precision");
       break;
     case SHT4X_MED_PRECISION: 
       Serial.println("Med precision");
       break;
     case SHT4X_LOW_PRECISION: 
       Serial.println("Low precision");
       break;
  }

  // You can have 6 different heater settings
  // higher heat and longer times uses more power
  // and reads will take longer too!
  sht4.setHeater(SHT4X_NO_HEATER);
  switch (sht4.getHeater()) {
     case SHT4X_NO_HEATER: 
       Serial.println("No heater");
       break;
     case SHT4X_HIGH_HEATER_1S: 
       Serial.println("High heat for 1 second");
       break;
     case SHT4X_HIGH_HEATER_100MS: 
       Serial.println("High heat for 0.1 second");
       break;
     case SHT4X_MED_HEATER_1S: 
       Serial.println("Medium heat for 1 second");
       break;
     case SHT4X_MED_HEATER_100MS: 
       Serial.println("Medium heat for 0.1 second");
       break;
     case SHT4X_LOW_HEATER_1S: 
       Serial.println("Low heat for 1 second");
       break;
     case SHT4X_LOW_HEATER_100MS: 
       Serial.println("Low heat for 0.1 second");
       break;
  }

  display.begin(0x3C, true); // Address 0x3C default
  if (! display.begin()) {
    Serial.println("Couldn't find OLED");
    while (1) delay(1);
  }
  Serial.println("OLED Begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  // display.display();
  // delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  // display.display();

  display.setRotation(1);
  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.display(); // actually display all of the above
  delay(10);  // Move above display.display(); to stop start glitch?

  // First reading so that values are not equal to 0.0 for high and low determinations. 
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);

  t = temp.temperature;
  h = humidity.relative_humidity;
  // temperature_array[1140] = {t};
  // humidity_array[1440] = {h};
  t = t * 9/5 + 32;
  temperature_low = t;
  temperature_high = t;
  humidity_low = h;
  humidity_high = h;

}

void loop() {
  sensors_event_t humidity, temp;
  
  uint32_t timestamp = millis();
  sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  timestamp = millis() - timestamp;

  t = temp.temperature;
  h = humidity.relative_humidity;

  Serial.print("Temperature: "); Serial.print(t); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(h); Serial.println("% rH");

  Serial.print("Read duration (ms): ");
  Serial.println(timestamp);
  Serial.println();

  // Read the battery voltage. 
  measuredBatteryVoltage = analogRead(VBATPIN);
  measuredBatteryVoltage *= 2;    // Multiply by 2 because of the resistive divide by 2. 
  measuredBatteryVoltage *= 3.3;  // Multiply by the analog reference voltage of 3.3V.
  measuredBatteryVoltage /= 1024; // Divide by 1024 to convert to a voltage.

  // Display to OLED 
  display.clearDisplay();
  display.setCursor(0,0);

  t = t * 9/5 + 32;

  if(t <= temperature_low) {
    temperature_low = t;
  }

  if(t >= temperature_high) {
    temperature_high = t;
  }

  if(h <= humidity_low) {
    humidity_low = h;
  }

  if(h >= humidity_high) {
    humidity_high = h;
  }

  display.print(t);
  display.print(" *F   ");

  display.print(h);
  display.println(" %RH");

  display.println("Low");

  display.print(temperature_low);
  display.print(" *F   ");

  display.print(humidity_low);
  display.println(" %RH");

  display.println("High");

  display.print(temperature_high);
  display.print(" *F   ");

  display.print(humidity_high);
  display.println(" %RH");

  display.println();
  display.print("Battery: ");
  display.print(measuredBatteryVoltage);
  display.print(" Volts");

  delay(10);
  yield();
  display.display(); // actually display all of the above

  delay(1000);

}
