//Copyright for used arduino-CAN library for Sandeep Mistry
//Github: https://github.com/sandeepmistry

#include <CAN.h>

///SENDER
enum Weather { SUNNY, RAINY };

enum RainIntensity { SMALL, MEDIUM, HEAVY};

int speedLimitList[5] = {20,40,60,80,100};

Weather weatherState = SUNNY;
RainIntensity rainIntensity = SMALL;
bool isRaining = false;
uint8_t speedLimit = 40;
uint8_t lock = 0;

void switchWeatherState(Weather weatherState) {
  if (weatherState == SUNNY) {
    weatherState = RAINY;
  }
  else {
    weatherState = SUNNY;
  }
}

char* ToStringWeather(Weather weather) {
  switch(weather)
  {
    case SUNNY:   return "s";
    case RAINY:   return "r";
    default:      return "s";
  }
}

char* ToStringRainIntensity(RainIntensity intensity) {
  switch(intensity)
  {
    case SMALL:   return "m";
    case MEDIUM:  return "m";
    case HEAVY:   return "h";
    default:      return "m";
  }
}

uint8_t changeSpeedLimit(long newLimit) {
  switch(newLimit)
  {
    case 20:  return 1;
    case 40:  return 2;
    case 60:  return 3;
    case 80:  return 4;
    case 100: return 5;
    default:  return 2;
  }
}

char writeWeather(Weather weatherState) {
  char* weather = ToStringWeather(weatherState);
  return *weather;
}

char writeIntensity(RainIntensity rainIntensity) {
  char* intensity = ToStringRainIntensity(rainIntensity);
  return *intensity;
}
long shuffleNewLimit() {
  long randInt = random(0,5);
  long newLimit = speedLimitList[randInt];
  Serial.print("\nNew limit is: ");
  Serial.print(newLimit);
  return newLimit;
}

void changeRainIntensity() {
  long randInt = random(0,3);
  switch(randInt) {
    case 0:   rainIntensity = SMALL;
    case 1:   rainIntensity = MEDIUM;
    case 2:   rainIntensity = HEAVY;
    default:  rainIntensity = SMALL;
  }
}

void setup() {

  Serial.begin(9600);
  while(!Serial);

  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
}
}
 
void loop() {
  delay(5000);
  Serial.println("\nSending weather data... ");
  if (isRaining) {
    weatherState = SUNNY;
    changeRainIntensity();
    Serial.println("\n---------------------------------------------");
    CAN.beginPacket(0x12);
    lock = 1;
    CAN.write(writeWeather(weatherState));
    Serial.println("Weather data sent");
    CAN.endPacket();
    delay(5000);
    while(lock == 1){
      int packetSize = CAN.parsePacket();
      if(packetSize) {
        if(CAN.packetId() == 0x12) {
          lock = CAN.read();
        }
      }
    }
    Serial.println("\n---------------------------------------------");
    CAN.beginPacket(0x13);
    lock = 1;
    CAN.write(writeIntensity(rainIntensity));
    Serial.println("Rain intensity sent");
    CAN.endPacket();
    isRaining = false;
    delay(5000);
    while(lock == 1){
      int packetSize = CAN.parsePacket();
      if(packetSize) {
        if(CAN.packetId() == 0x12) {
          lock = CAN.read();
        }
      }
    }
    delay(5000);
  }
  else {
  Serial.println("\n---------------------------------------------");
  CAN.beginPacket(0x12);
  lock = 1;
  CAN.write(writeWeather(weatherState));
  Serial.println("Weather data sent");
  CAN.endPacket();
  isRaining = true;
  while(lock == 1){
    int packetSize = CAN.parsePacket();
    if(packetSize) {
      if(CAN.packetId() == 0x12) {
        lock = CAN.read();
        }
      }
    }
  delay(5000);
  }
  Serial.println("\n---------------------------------------------");
  Serial.println("Sending Speedlimit.. ");
  CAN.beginPacket(0x14);
  lock = 1;
  speedLimit = changeSpeedLimit(shuffleNewLimit());
  CAN.write(speedLimit);
  Serial.println("\nSpeedlimit sent");
  CAN.endPacket();
  while(lock == 1){
  int packetSize = CAN.parsePacket();
  if(packetSize) {
     if(CAN.packetId() == 0x12) {
      lock = CAN.read();
    }
  }
}
delay(5000);
}
