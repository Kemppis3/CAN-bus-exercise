//Copyright for used arduino-CAN library for Sandeep Mistry
//Github: https://github.com/sandeepmistry

#include <CAN.h>

//RECEIVER

enum Wipers { INACTIVE, SLOW, NORMAL, FAST };

Wipers wiperSpeed = INACTIVE;

long speedLimit = 40;
long currentSpeed = 40;
uint8_t lock = 0;

void activateWhipers(char status) {
  if (&status == "r") {
    wiperSpeed = SLOW;
  }
lock = 0;
}

void changeWiperSpeed(char intensity) {
  if(&intensity == "m") {
    wiperSpeed = NORMAL;
  }
  if(&intensity == "h") {
    wiperSpeed = FAST;
  }
lock = 0;
}

uint8_t decodeNewLimit(int newSpeedLimit) {
    switch(newSpeedLimit){
      case 1:  return 20;
      case 2:  return 40;
      case 3:  return 60;
      case 4:  return 80;
      case 5:  return 100;
  }
}


void updateCurrentLimitAndSpeed(uint8_t newSpeedLimit) {
  if(newSpeedLimit == speedLimit) {
    Serial.println("\nSpeedlimit stayed the same");
  }
  else {
    speedLimit = newSpeedLimit;
    if (speedLimit < currentSpeed) {
      Serial.println("\nSlowing down...");
      currentSpeed = speedLimit;
    }
    else if (speedLimit > currentSpeed) {
      Serial.println("\nAccelerating...");
      currentSpeed = speedLimit;
    }
  }
lock = 0;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Receiver");

  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  
  int packetSize = CAN.parsePacket();

  if(packetSize && lock == 0) {

    Serial.print("Packet received with id: ");
    Serial.print(CAN.packetId(), HEX);

    //Ignore packet
    if (CAN.packetRtr()) {
      Serial.print("RTR received with data length code: ");
      Serial.print(CAN.packetDlc());
      Serial.println("Skipping...");
    }

    else {
      if (CAN.packetId() == 0x12){
        lock = 1;
        Serial.println("\nWeather change received");
        activateWhipers((char)CAN.read());
        CAN.beginPacket(0x12);
        CAN.write(0x0);
        CAN.endPacket();
      }
      else if (CAN.packetId() == 0x13) {
        lock = 1;
        Serial.println("\nRain intensity update received");
        changeWiperSpeed((char)CAN.read());
        CAN.beginPacket(0x12);
        CAN.write(0x0);
        CAN.endPacket();
      }
      else if (CAN.packetId() == 0x14) {
        lock = 1;
        Serial.println("\nSpeedlimit change received");
        updateCurrentLimitAndSpeed(decodeNewLimit(CAN.read()));
        CAN.beginPacket(0x12);
        CAN.write(0x0);
        CAN.endPacket();
      }
      else {
        Serial.print("\nPacket corrupted...");
        Serial.println("Skipping...");
      }
    }
  Serial.println("\n---------------------------------------------");
  }
}