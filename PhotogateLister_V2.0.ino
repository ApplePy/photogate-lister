//Copyright Andy Yang (2012), Darryl Murray (2014), and D. Robertson



//used for analog communication
#include <Wire.h>
//divisor to correct for different clock speed
#define CORRECTION_FACTOR 16
//speed of number cycling, in millis
#define CHANGE_SPEED 90 //**EDIT
//the display is on this address
const byte saa1064 = 59; //address of the display chip //Previously 0x76 >> 1;
const int LED1 = 9; //yellow LED address
const int LED2 = 10; //red LED address
const int PHOTOGATE = 8; //photogate address
//These are arrays containing the hex required to display digits 0-9, and A-Z
//hex 0-F for a display digit
//the first two and last two digits weren't wired the same, so I have 2
//different configurations. For future boards, these might all need changing
//NOTE: the left-most digits are controlled by digits2/alpha2, the right-most digits are controlled by digits1/alpha1
const byte digits1[10] = {
  1+2+4+8+16+32, 2+4, 1+2+64+16+8, 1+2+64+4+8, 32+64+2+4, 1+32+64+4+8, 1+32+64+4+8+16, 1+2+4, 1+32+2+64+4+16+8, 1+32+64+2+4+8};
const byte digits2[10] = {
  128+64+32+16+8+4, 64+32, 128+64+2+8+16, 128+64+2+32+16, 4+2+64+32, 128+4+2+32+16, 128+4+8+16+32+2, 128+64+32, 128+4+8+16+32+64+2, 128+4+2+64+32+16};
const byte alpha1[26] = {
  1+2+4+16+32+64 /*A*/, 1+2+4+8+16+32+64/*B*/, 1+8+16+32/*C*/, 1+2+4+8+16+32/*D*/, 1+8+16+32+64/*E*/, 1+16+32+64/*F*/, 1+4+8+16+32+64/*G*/, 2+4+16+32+64/*H*/, 16+32/*I*/, 2+4+8+16/*J*/, 128/*K (placeholder)*/, 8+16+32/*L*/, 128/*M (placeholder)*/, 128/*N (placeholder)*/, 1+2+4+8+16+32/*O*/, 1+2+16+32+64/*P*/, 128/*Q (placeholder)*/, 128/*R (placeholder)*/, 1+4+8+32+64/*S*/, 128/*T (placeholder)*/, 2+4+8+16+32/*U*/, 128/*V (placeholder)*/, 128/*W (placeholder)*/, 128/*X (placeholder)*/, 128/*Y (placeholder)*/, 1+2+8+16+64/*Z (bad)*/};
const byte alpha2[26] = {
  128+64+32+8+4+2 /*A*/, 128+64+32+16+8+4+2/*B*/, 128+16+8+4/*C*/, 128+64+32+16+8+4/*D*/, 128+16+8+4+2/*E*/, 128+8+4+2/*F*/, 128+32+16+8+4+2/*G*/, 64+32+8+4+2/*H*/, 8+4/*I*/, 64+32+16/*J*/, 1/*K (placeholder)*/, 16+8+4/*L*/, 1/*M (placeholder)*/, 1/*N (placeholder)*/, 128+64+32+16+8+4/*O*/, 128+64+8+4+2/*P*/, 1/*Q (placeholder)*/, 1/*R (placeholder)*/, 128+32+16+4+2/*S*/, 1/*T (placeholder)*/, 64+32+16+8+4/*U*/, 1/*V (placeholder)*/, 1/*W (placeholder)*/, 1/*X (placeholder)*/, 1/*Y (placeholder)*/, 128+64+16+8+2/*Z (bad)*/};
//timer variables
int val = 0;
unsigned long t = 0;
unsigned long pauseCount = 0;
double dt[32] = {
  0}; //**EDIT
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

  //Quick Wire writing tutorial:
  //First write command sets the properties of the screen; other than the initial screen config, just write "1"
  //Second through fifth write command are for writing digits to the screen (see digits1/2 and alpha1/2 arrays for pre-defined numbers/letters)

  //Initial screen configuration information:
  //screen bits, from most to least significant (left to right):
  //first bit is reserved
  //next 3 bits control brightness/current (12mA, 6mA, and 3mA, respectively)
  //next bit switches all segments on for segment test
  //next bit controls whether digits 1 and 3 are blanked (0) or not (1)
  //next bit controls whether digits 2 and 4 are blanked (0) or not (1)
  //last bit controls whether the device is in static or dynamic mode
  //static mode is the continuous display of digits 1 and 2
  //dyanmic mode is the alternating display of digits 1+3 and 2+4
  //see Philips Semiconductors part SAA1064 data sheet for more information

  //setting intial screen configuration (see comment block above for explanation)
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
  lucasLogo();
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
    delay(5);
  }
}

//Scrolls the words "LUCAS SCI" across the screen
void lucasLogo()
{
  for (unsigned int timing = 0; timing < 14; timing++)
  {
    switch(timing)
    {
    case 0:
    case 13:
      //blank
      displayChars(-1, -1, -1, -1, -1, -1, -1, -1);
      break;
    case 1:
      //_ _ _ L
      displayChars(0, 0, 0, 11, -1, -1, -1, 1);
      break;
    case 2:
      //_ _ L U
      displayChars(0, 0, 11, 20, -1, -1, 1, 1);
      break;
    case 3:
      //_ L U C
      displayChars(0, 11, 20, 2, -1, 1, 1, 1);
      break;
    case 4:
      //L U C A
      displayChars(11, 20, 2, 0, 1, 1, 1, 1);
      break;
    case 5:
      //U C A S
      displayChars(20, 2, 0, 18, 1, 1, 1, 1);
      break;
    case 6:
      //C A S _
      displayChars(2, 0, 18, -1, 1, 1, 1, -1);
      break;
    case 7:
      //A S _ S
      displayChars(0, 18, -1, 18, 1, 1, -1, 1); 
      break;
    case 8:
      //S _ S C
      displayChars(18, -1, 18, 2, 1, -1, 1, 1);
      break;
    case 9:
      //_ S C I
      displayChars(-1, 18, 2, 8, -1, 1, 1, 1);
      break;
    case 10:
      //S C I _
      displayChars(18, 2, 8, -1, 1, 1, 1, -1);
      break;
    case 11:
      //C I _ _
      displayChars(2, 8, -1, -1, 1, 1, -1, -1);
      break;
    case 12:
      //I _ _ _
      displayChars(8, -1, -1, -1, 1, -1, -1, -1);
      break;
    }
    delay (9);
  }
}

//display a fixed-point decimal x, with a decimal point at location decimal and precision precision
void displayFixed(float x, int decimal, int precision){
  int p10[4] = {
    1, 10, 100, 1000  };

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


//Displays text or digits to screen using the digits and alpha arrays
//USAGE: going from left to right, vars screen1-4 specifies the character from the digits/alpha array you want to display
//eg: if screen1 = 1, then it will display either "1" or "B")
//vars type1-4 specify the type to display: -1 = blank screen spot, 0 = digits, 1 = characters);
//FUTURE ADDITIONS: -support for the decimal/dot
//NOTE: the character display is in the wrong order because of how the screen places things on the display:
//it always displays on the right-most screen spot first, then the left
void displayChars(int screen1, int screen2, int screen3, int screen4, int type1, int type2, int type3, int type4)
{
  //Start up display
  Wire.beginTransmission(saa1064);
  Wire.write(1);

  //write character 2
  if (type2 == -1)
    Wire.write(0);
  else if (type2 == 0)
    Wire.write(digits2[screen2]);
  else if (type2 == 1)
    Wire.write(alpha2[screen2]);

  //write character 1
  if (type1 == -1)
    Wire.write(0);
  else if (type1 == 0)
    Wire.write(digits2[screen1]);
  else if (type1 == 1)
    Wire.write(alpha2[screen1]);

  //write character 4  
  if (type4 == -1)
    Wire.write(0);
  else if (type4 == 0)
    Wire.write(digits1[screen4]);
  else if (type4 == 1)
    Wire.write(alpha1[screen4]);

  //write character 3  
  if (type3 == -1)
    Wire.write(0);
  else if (type3 == 0)
    Wire.write(digits1[screen3]);
  else if (type3 == 1)
    Wire.write(alpha1[screen3]);

  Wire.endTransmission();
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

    //Display last time recorded for a short while and update t as to not affect the else block's code
    if ((millis()-pauseCount)/CHANGE_SPEED < 1 && indices > 0)
    {
      displayFloat(dt[indices - 1], 4);
      t = millis();
    }
    else
    {  
      //display registered times
      if(indices){ //list has items
        //pick an index to display based on time
        int index = ((millis()-t)/(CHANGE_SPEED * 2)) % indices;
        int numOrSpeed = ((millis()-t)/CHANGE_SPEED) % 2;
        //flash red LED on the first index
        if(index == 0) digitalWrite(LED2, HIGH);
        else           digitalWrite(LED2,  LOW);
        //display
        if (numOrSpeed == 0) displayChars(-1, (index + 1)/10 % 10, (index + 1) % 10, -1, -1, 0, 0, -1);
        else displayFloat(dt[index], 4);
      }
      else{ //no items in the list yet
        displayFloat(0,4);
      }
    }
  }
  else{ //if the gate is blocked
    //show the time that has passed since the counter was last updated
    if(!active){
      active = 1;
      //update counter
      t = millis();
      //update the pause counter
      pauseCount = t;
    }
    dt[indices] = 0.001*(millis()-t)*CORRECTION_FACTOR;
    displayFloat(dt[indices], 4);
  }
}
