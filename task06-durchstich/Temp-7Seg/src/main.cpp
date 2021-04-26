#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//define temperature pins
#define DHTPIN 18
#define DHTTYPE DHT11

int latchPin = 2;
int clockPin = 4;
int dataPin = 15;

int comPin[] = {25, 26, 27, 14}; // Common pin (anode) of 4 digit 7-segment display

byte num[] = {
    B00111111, //0
    B00000110, //1
    B01011011, //2
    B01001111, //3
    B01100110, //4
    B01101101, //5
    B01111101, //6
    B00000111, //7
    B01111111, //8
    B01101111  //9
};

byte dot = B10000000;
byte star = B01100011;
byte celsius = B00111001;

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

int digitBuffer[4] = {0};

void writeData(int value)
{
  // Make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  // Make latchPin output high level, then 74HC595 will update data to parallel output
  digitalWrite(latchPin, HIGH);
}

void electDigitalDisplay(byte com)
{
  // Close all single 7-segment display
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(comPin[i], LOW);
  }
  // Open the selected single 7-segment display
  digitalWrite(comPin[com], HIGH);
}

void setup()
{
  //Serial.begin(115200);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < 4; i++)
  {
    pinMode(comPin[i], OUTPUT);
  }

  dht.begin();
}

void loop()
{
  int tempC;

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println("Error reading temperature!");
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println("°C");
    tempC = event.temperature;

    digitBuffer[1] = int(tempC % 10);
    digitBuffer[0] = int((tempC / 10) % 10);
  }

  for (int i = 0; i < 4; i++)
  {
    // Select a single 7-segment display
    electDigitalDisplay(i);

    // Send data to 74HC595
    //writeData(~num[i]);
    Serial.print(digitBuffer[i]);

    if (i == 2)
    {
      writeData(~star);
    }
    else if (i == 3)
    {
      writeData(~celsius);
    }
    else {
      writeData(~num[digitBuffer[i]]);
    }

    Serial.println(" *");

    // Short delay.
    delay(5);
    // Clear the display content
    writeData(0xff);
  }
}