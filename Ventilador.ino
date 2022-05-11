#include <Adafruit_NeoPixel.h>

#define LED_PIN   6
#define LED_COUNT 24
#define staticDelayTime 50
int bladePosition = 0;
int delayTime = 50;
int maxDelayWait = 1500;

enum fanState {
  LEFT,
  RIGHT,
  STILL
};

fanState orientation = LEFT;
fanState queuedOrientation = orientation;
bool changingState = false;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  strip.begin();
  strip.show();
  strip.setBrightness(50);
  attachInterrupt(digitalPinToInterrupt(2), changeFan, RISING);
}

void loop()
{
  refreshDisplay();
  if(changingState || delayTime != staticDelayTime){
  	stateChange();
  }
  bladeTurn();
  delay(delayTime);
}

void stateChange(){
  
  if(orientation == STILL && changingState){
    changingState = false;
    orientation = queuedOrientation;
    delayTime = staticDelayTime + maxDelayWait;
  }
  
  if(changingState){
    delayTime += 100;
    if(delayTime > maxDelayWait){
      changingState = false;
      orientation = queuedOrientation;
      
      if(orientation == STILL){
        delayTime = staticDelayTime;
      }
    }
  }else{
      delayTime -= 100;
    if(delayTime < staticDelayTime){
      delayTime = staticDelayTime;
    }
  }
}

void bladeTurn(){
  
  if(orientation == RIGHT){
    bladePosition++;
  }
  if(orientation == LEFT){
    bladePosition--;
  }

  if(bladePosition >= LED_COUNT){
    bladePosition = 0;
  }
  if(bladePosition < 0){
    bladePosition = LED_COUNT-1;
  }
  
}

void refreshDisplay(){  
  strip.clear();
  strip.setPixelColor(bladePosition, strip.Color(255, 0, 255)); 
  strip.show();
}

void changeFan(){
  if(orientation == queuedOrientation){
    switch(analogRead(A0)){
      case 853: queuedOrientation = STILL; break;
      case 890: queuedOrientation = RIGHT; break;
      case 930: queuedOrientation = LEFT ; break;
    }
  }
  if(orientation != queuedOrientation){
  	changingState = true;
  }
}