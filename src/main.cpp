#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "./libs/util.h"

#define RXp2 16
#define TXp2 17

// Pin definitions for ESP32
#define SCLK 22
#define MOSI 23
#define CS 4
#define DC 2
#define RST 15

#define TELA_PRINCIPAL 0
#define TELA_RELE_200A 1
#define TELA_TRAVA 2
#define TELA_DESTRAVA 3
#define TELA_CC 4

#define pIgnicao 21
#define pMotor 19
#define pAux 18
#define pBt1 32
#define pBt2 26
#define pBtTrava 35
#define pBtDestrava 34

#define getALL 86
#define setReleH 82
#define setReleL 114
#define setCCH 67
#define setCCL 99
#define setIgnicaoH 73
#define setIgnicaoL 105
#define setMotorH 77
#define setMotorL 109
#define setGasH 71
#define setGasL 103
#define setAguaH 65
#define setAguaL 97
#define setPositH 80
#define setPositL 112

int stRele200a = LOW;
int stReleCC = LOW;
int stGas = HIGH;
int stAgua = LOW;
int stIgnicao = LOW;
int stMotor = LOW;

int bt1last = HIGH;
int bt2last = HIGH;
int btTravlast = HIGH;
int btDeslast = HIGH;

int bt1mili = 0;
int bt2mili = 0;
int btTravmili = 0;
int btDesmili = 0;

int loopLed = 0;
int loopBtn = 0;
int loopSetSerial = 0;

int screen = -1;

Adafruit_ST7735 tft = Adafruit_ST7735(CS, DC, MOSI, SCLK, RST);

void setup() {
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  pinMode(pBt1, INPUT);
  pinMode(pBt2, INPUT);
  pinMode(pBtTrava, INPUT);
  pinMode(pBtDestrava, INPUT);
  pinMode(pIgnicao, INPUT);
  pinMode(pMotor, INPUT);
  pinMode(pAux, INPUT);

  tft.initR(INITR_MINI160x80_PLUGIN);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(25,40);
  tft.setTextColor(ST7735_ORANGE);
  tft.setTextSize(2);
  tft.println("El Patron");
}

void tela(int newScreen) {
  if (newScreen > TELA_CC || newScreen < TELA_PRINCIPAL) {
    screen = TELA_PRINCIPAL;
  } else {
    screen = newScreen;
  }

  tft.setTextSize(2);

  if (screen == TELA_PRINCIPAL) {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(3,5);

    tft.setCursor(3, 20);
    if (stRele200a == HIGH) {
      tft.setTextColor(ST77XX_GREEN);
    } else {
      tft.setTextColor(ST77XX_RED);
    }
    tft.print("200a ");

    if (stReleCC == HIGH) {
      tft.setTextColor(ST77XX_GREEN);
    } else {
      tft.setTextColor(ST77XX_RED);
    }
    tft.print("Corta");

    tft.setCursor(3, 35);
    if (stAgua == HIGH) {
      tft.setTextColor(ST77XX_GREEN);
    } else {
      tft.setTextColor(ST77XX_RED);
    }
    tft.print("Bomba ");

    if (stGas == HIGH) {
      tft.setTextColor(ST77XX_GREEN);
    } else {
      tft.setTextColor(ST77XX_RED);
    }
    tft.print("Gas");

    tft.setCursor(3, 50);
    if (stMotor == HIGH) {
      tft.setTextColor(ST77XX_GREEN);
    } else if (stIgnicao) {
      tft.setTextColor(ST77XX_ORANGE);
    } else {
      tft.setTextColor(ST77XX_WHITE);
    }
    tft.print("Motor");

  } else if (screen == TELA_RELE_200A) {
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(10,20);
    tft.println("Casa<->Carro");

    if (stRele200a == HIGH) {
      tft.setCursor(55,55);
      tft.setTextColor(ST77XX_GREEN);
      tft.println("Ligado");
    } else {
    tft.setCursor(40,55);
      tft.setTextColor(ST77XX_RED);
      tft.println("Deligado");
    }
    
  } else if (screen == TELA_CC) {
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(80,20);
    tft.println("CC");

    if (stReleCC == HIGH) {
      tft.setCursor(55,55);
      tft.setTextColor(ST77XX_GREEN);
      tft.println("Ligado");
    } else {
    tft.setCursor(40,55);
      tft.setTextColor(ST77XX_RED);
      tft.println("Deligado");
    }
  } else if (screen == TELA_TRAVA) {
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(55, 35);
    tft.println("TRAVA");
  } else if (screen == TELA_DESTRAVA) {
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(30, 35);
    tft.println("DESTRAVA");
  }

  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.setTextColor(ST77XX_CYAN);
  tft.println(screen);
}

void botoes(int bt1, int bt2, int btTrava, int btDestrava) {
  if (bt1 > 0) {
    if (bt1 > 5000) {
      tela(TELA_CC);
    } else if (screen == TELA_RELE_200A) {
      tela(0);
    } else {
      tela(screen + 1);
    }
  } else if (bt2 > 0) {
    if (screen == TELA_RELE_200A) {
      if (stRele200a == HIGH) {
        stRele200a = LOW;
      } else {
        stRele200a = HIGH;
      }

      Serial2.write(stRele200a ? setReleH : setReleL);

      tela(TELA_RELE_200A);
    } else if (screen == TELA_CC) {
      if (bt2 > 2000 && stReleCC == LOW) {
        stReleCC = HIGH;
      } else {
        stReleCC = LOW;
      }

      Serial2.write(stReleCC ? setCCH : setCCL);

      tela(TELA_CC);
    } else {
      tela(screen - 1);
    }
  } else if (btTrava > 0) {
    int lastScreen = screen;

    tela(TELA_TRAVA);
    delay(2000);
    tela(lastScreen);
  } else if (btDestrava > 0) {
    int lastScreen = screen;

    tela(TELA_DESTRAVA);
    delay(2000);
    tela(lastScreen);
  }
}

void processarSerial() {
  while (Serial2.available() > 0) {
    int serialByte = Serial2.read();
    bool mudou = false;

    switch (serialByte) {
      case setReleH:
        if (!stRele200a) { mudou = true; }
        stRele200a = HIGH;
        break;

      case setReleL:
        if (stRele200a) { mudou = true; }
        stRele200a = LOW;
        break;

      case setCCH:
        if (!stReleCC) { mudou = true; }
        stReleCC = HIGH;
        break;

      case setCCL:
        if (stReleCC) { mudou = true; }
        stReleCC = LOW;
        break;

      case setGasH:
        if (!stGas) { mudou = true; }
        stGas = HIGH;
        break;

      case setGasL:
        if (stGas) { mudou = true; }
        stGas = LOW;
        break;

      case setAguaH:
        if (!stAgua) { mudou = true; }
        stAgua = HIGH;
        break;

      case setAguaL:
        if (stAgua) { mudou = true; }
        stAgua = LOW;
        break;
    }

    if (mudou) {
      tela(screen);
    }
  }

  if (millis() - loopSetSerial > 751) {
    Serial2.write(stIgnicao ? setIgnicaoH : setIgnicaoL);
    Serial2.write(stMotor ? setMotorH : setMotorL);
    Serial2.write(getALL);

    loopSetSerial = millis();
  }
}

void loop() {
  int tempoBt1 = 0;
  int tempoBt2 = 0;
  int tempoBtTrava = 0;
  int tempoBtDestrava = 0;

  if (millis() - loopBtn > 50) {
    tempoBt1 = pressBotao(pBt1, HIGH, &bt1last, &bt1mili);
    tempoBt2 = pressBotao(pBt2, HIGH, &bt2last, &bt2mili);
    tempoBtTrava = pressBotao(pBtTrava, HIGH, &btTravlast, &btTravmili);
    tempoBtDestrava = pressBotao(pBtDestrava, HIGH, &btDeslast, &btDesmili);

    loopBtn = millis();
  }

  botoes(tempoBt1, tempoBt2, tempoBtTrava, tempoBtDestrava);
  processarSerial();
}
