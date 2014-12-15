//used for analog communication
#include <Wire.h>
//divisor to correct for different clock speed
#define CORRECTION_FACTOR 16
//speed of number cycling, in millis
#define CHANGE_SPEED 90 //**EDIT
//the display is on this address
byte saa1064 = 0x76 >> 1;
int LED1 = 9; //yellow LED address
int LED2 = 10; //red LED address
int PHOTOGATE = 8; //photogate address
//hex 0-F for a display digit
//the first two and last two digits weren't wired the same, so I have 2
//different configurations. For future boards, these might all need changing
byte digits1[10] = {1+2+4+8+16+32, 2+4, 1+2+64+16+8, 1+2+64+4+8, 32+64+2+4, 1+32+64+4+8, 1+32+64+4+8+16, 1+2+4, 1+32+2+64+4+16+8, 1+32+64+2+4+8};
byte digits2[10] = {128+64+32+16+8+4, 64+32, 128+64+2+8+16, 128+64+2+32+16, 4+2+64+32, 128+4+2+32+16, 128+4+8+16+32+2, 128+64+32, 128+4+8+16+32+64+2, 128+4+2+64+32+16};
//timer variables
int val = 0;
unsigned long t = 0;
double dt[32] = {0}; //**EDIT
int indices = 0;
byte active = 0;

//runs on boot
void setup(){
  //set LEDs for writing
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  //set photogate for reading
  pinMode(PHOTOGATE, INPUT);
  
  //setup the display
  Wire.begin();
  Wire.beginTransmission(saa1064);
  //Null byte means we're setting up
  Wire.write((byte)B00000000);
  //bits, from most to least significant:
  //first bit is reserved
  //next 3 bits control brightness
  //I forget the rest. lol.
  Wire.write((byte)B00100111);
  //clear all 4 digits
  Wire.write((byte)B00000000);
  Wire.write((byte)B00000000);
  Wire.write((byte)B00000000);
  Wire.write((byte)B00000000);
  Wire.endTransmission();
  
  
  //turn off the LEDs (in case they were on)
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  throb();//**EDIT
}

//loop through each segment in the displays
//for debugging, in case the displays were wired
//differently.
void throb(){
  byte i;
  for(i=128; i>0;i>>=1){
    Wire.beginTransmission(saa1064);
    Wire.write((byte)1);
    Wire.write(i);
    Wire.write(i);
    Wire.write(i);
    Wire.write(i);
    Wire.endTransmission();
    delay(14);
  }
}

//display a fixed-point decimal x, with a decimal point at location decimal and precision precision
void displayFixed(float x, int decimal, int precision){
  int p10[4] = {1, 10, 100, 1000};
  
  unsigned long xx = x * p10[decimal];
  if(xx > 9999) xx = 0;
  
  byte d1, d2, d3, d4;
  d1 = (xx) % 10;
  d2 = (xx/10) % 10;
  d3 = (xx/100) % 10;
  d4 = (xx/1000) % 10;
  
  Wire.beginTransmission(saa1064);
  
  int maxplace = decimal - precision;
  Wire.write((byte)(maxplace + 1));
  if(maxplace < 3) Wire.write((byte)digits2[d3] | 1*(decimal == 2 && maxplace != 2) );
  if(maxplace < 4) Wire.write((byte)digits2[d4] | 1*(decimal == 3 && maxplace != 3) );
  
  if(maxplace < 1) Wire.write((byte)digits1[d1]);
  if(maxplace < 2) Wire.write((byte)digits1[d2] | 128*(decimal == 1 && maxplace != 1) );
  Wire.endTransmission();
}

//display a floating point decimal x with sig significant digits
void displayFloat(float x, int sig){
  if     (x > 999.9) displayFixed(x, 0, sig-4);
  else if(x > 99.99) displayFixed(x, 1, sig-3);
  else if(x > 9.999) displayFixed(x, 2, sig-2);
  else               displayFixed(x, 3, sig-1);
}



//runs after setup over and over
void loop(){
  //the yellow LED displays the photogate state
  digitalWrite(LED1, !digitalRead(PHOTOGATE));
  
  //if the gate is open
  if(digitalRead(PHOTOGATE) == HIGH){
    if(active){
      active = 0;
      indices++;
    }
    
    //display registered times
    if(indices){ //list has items
      //pick an index to display based on time
      int index = ((millis()-t)/CHANGE_SPEED) % indices;
      //flash red LED on the first index
      if(index == 0) digitalWrite(LED2, HIGH);
      else           digitalWrite(LED2,  LOW);
      //display
      displayFloat(index+1,1); //<<ADDED LINE
      displayFloat(dt[index], 4);
    }else{ //no items in the list yet
      displayFloat(0,4);
    }
  }else{ //if the gate is blocked
    //show the time that has passed since the counter was last updated
    if(!active){
      active = 1;
      //update counter
      t = millis();
    }
    dt[indices] = 0.001*(millis()-t)*CORRECTION_FACTOR;
    displayFloat(dt[indices], 4);
  }
}

