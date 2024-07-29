#include <Arduino.h>
#include <FastLED.h>
#include "TFT_eSPI.h"
#include "SPI.h"
#include "Adafruit_AHTX0.h"

// Макросы для задания цветов температуры и влажности
#define TEMP_COLOR TFT_RED
#define HUM_COLOR TFT_SKYBLUE

// Объекты
TFT_eSPI tft = TFT_eSPI();
Adafruit_AHTX0 aht;
MbedI2C i2c(26, 27);

// Параметры времени
unsigned long previousMillis = 0; 
const long INTERVAL = 10000; // Интервал переключения между температурой и временем
bool displayTemp = true;

unsigned long lastSecondUpdate = 0; 
const long SECOND_INTERVAL = 1000; 
int hours = 0;
int minutes = 0;
int seconds = 0;

// Прототипы функций
void setupDisplay();
void checkButtons();
void updateClock();
void displayData();
bool isButtonPressed(int pin);

void setup() {
  pinMode(4, INPUT_PULLUP); // Кнопка изменения часов
  pinMode(5, INPUT_PULLUP); // Кнопка изменения минут

  setupDisplay();
  
  if (!aht.begin(&i2c)) {
    tft.drawString("AHT ERR", 0, 55, 4);
    while (1) delay(10);
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  checkButtons();

  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;
    displayTemp = !displayTemp;
    tft.fillScreen(TFT_BLACK); // Очистка экрана перед сменой данных
  }

  if (currentMillis - lastSecondUpdate >= SECOND_INTERVAL) {
    lastSecondUpdate = currentMillis;
    updateClock();
  }

  displayData();
  delay(500); // Период обновления данных
}

void setupDisplay() {
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);
  tft.drawString("AHT start", 0, 55, 4);
  delay(1500);
}

void checkButtons() {
  if (isButtonPressed(4)) {
    hours = (hours + 1) % 24;
    delay(200); // Задержка для предотвращения дребезга
  }
  if (isButtonPressed(5)) {
    minutes = (minutes + 1) % 60;
    delay(200); // Задержка для предотвращения дребезга
  }
}

bool isButtonPressed(int pin) {
  return (digitalRead(pin) == LOW);
}

void updateClock() {
  seconds++;
  if (seconds >= 60) {
    seconds = 0;
    minutes++;
    if (minutes >= 60) {
      minutes = 0;
      hours++;
      if (hours >= 24) {
        hours = 0;
      }
    }
  }
}

void displayData() {
  char text[20];
  
  if (displayTemp) {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp); // Обновление данных о температуре и влажности
    
    // Вывод температуры
    sprintf(text, "%.1f C", temp.temperature);
    tft.setTextColor(TEMP_COLOR, TFT_BLACK);
    tft.drawString(text, 0, 0, 7);
    
    // Вывод влажности
    sprintf(text, "Hum - %.1f %%", humidity.relative_humidity);
    tft.setTextColor(HUM_COLOR, TFT_BLACK);
    tft.drawString(text, 0, 55, 4);
  } else {
    // Вывод времени
    sprintf(text, "%02d:%02d", hours, minutes);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(text, 0, 0, 7);

    // Вывод секунд
    sprintf(text, "Sec: %02d", seconds);
    tft.setTextColor(HUM_COLOR, TFT_BLACK);
    tft.drawString(text, 0, 55, 4);
  }
}
