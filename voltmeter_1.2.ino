//Voltmeter Soldering Kit | Version 1.2, October 2020 | Developed by: Jim Heaney, Venator Technologies
//Released Open-Source under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA). See terms here: https://creativecommons.org/licenses/by-nc-sa/4.0/
//More infomration & To Purchase: www.venatormfg.com/voltmeter
//Code for Red LED and 7-Segment Display

//Calibration Numbers:
const float vstep = 0.0531; //Voltage per 1 step of analog measurement. Default: 0.0531
const float offset = 0.156; //Non-zero measurements are off by a consistent value. Default: 0.156
const int holdintensity = 3; //Higher number reduces the brightness of the "hold" LED. Default: 3
const int LEDmode = 0; // Set to 0 for common-anode displays (red), 1 for common-cathode displays (green, blue)

//Declate all of the pin assignments
const int dig1 = 10;
const int dig2 = 11;
const int dig3 = 12;
const int a = 8;
const int b = 6;
const int c = 3;
const int d = 4;
const int e = 5;
const int f = 7;
const int g = 2;
const int dot = 9;
const int LED = A2;
const int button = A1;
const int sense = A0;


//Declare all of the variables that we need
long analogread;
long sumanalogread;
long filteredanalog;
long oldanalog;
int i;
int temp = 0;
bool hold = 0;
bool newpress;
float voltage;
int intvoltage;
int voltage100;
int voltage10;
int voltage1;

void setup() {
  // The setup code, that will run once. 

  //Set all of the pins to either outputs or inputs
  pinMode(dig1, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(button, INPUT);
  pinMode(dot, OUTPUT);
  pinMode(sense, INPUT);

  //By default, set the display off
  if(LEDmode == 0){
    digitalWrite(dig1, LOW);
    digitalWrite(dig2, LOW);
    digitalWrite(dig3, LOW);
    digitalWrite(a, HIGH);
    digitalWrite(b, HIGH);
    digitalWrite(c, HIGH);
    digitalWrite(d, HIGH);
    digitalWrite(e, HIGH);
    digitalWrite(f, HIGH);
    digitalWrite(g, HIGH);
    digitalWrite(dot, HIGH);
  } else{
    digitalWrite(dig1, HIGH);
    digitalWrite(dig2, HIGH);
    digitalWrite(dig3, HIGH);
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    digitalWrite(d, LOW);
    digitalWrite(e, LOW);
    digitalWrite(f, LOW);
    digitalWrite(g, LOW);
    digitalWrite(dot, LOW);
  }
}

void loop() {
  // The main code that will run repeatedly.

  //Step 1: Read the analog value of the "sense" pin
  analogread = analogRead(sense);
  filteredanalog = analogread;

  //Step 2: Check the hold button status
  if ((digitalRead(button) == 0) && (newpress == 1)){ //If the button is not pressed, but it was just a moment ago, toggle hold on or off.
    if (hold == 1){
      oldanalog = filteredanalog;
      hold = 0;
    }
    else{
      hold = 1;
    }
    newpress = 0;
  }
  if (digitalRead(button) == 1){ //If the button is pressed, set the newpress variable to 1. This stops the 328P from interperting you holding the button down as millions of rapid button presses.
    newpress = 1;
  }
  if (hold == 1){ //If in hold mode, turn on the hold LED. The code below also allows the LED's intensity to be set by modifying its duty cycle, i.e. how often it is on. By default, the LED will only be on 1/3 of the time. 
    temp = temp + 1;
    if(temp >= holdintensity){
      temp == 0;
      digitalWrite(LED, HIGH);
    }
    digitalWrite(LED, LOW);
  }
  if (hold == 0){ //If no longer in hold mode, turn off the LED 
    digitalWrite(LED, LOW);
  }

  //Step 3: See if the new voltage reading is close to or less than the old voltage reading
  if (hold == 1){ //If in hold mode, only update the voltage if the new one is higher.
    if(filteredanalog > oldanalog){
      oldanalog = filteredanalog; 
    }
    voltage = oldanalog * vstep; //Set the voltage by multiplying the analog value by the calibration "vstep" value.
  }
  if (hold == 0){ //If not in hold mode, see if the voltage is significantly different than just a moment ago. This helps to reduce flickering in the screen if a voltage source is right at the edge of an analog measurement.
    if(abs(filteredanalog - oldanalog) < 2){
      voltage = oldanalog * vstep;
      oldanalog = filteredanalog;
    }
    else{
      voltage = filteredanalog * vstep; //If there is a significant difference, use the new analog measurement as the voltage.
    }
  }
  if (voltage == 0){
  }
  else{
    voltage = voltage + offset; //If the voltage is not 0, apply an offset value to compensate for voltage drop.
  }
  
  //Step 4: Display the voltage reading 
  if (voltage < 10){ //If the measurement is less than 10, use 2 of the digits as decimals.
  intvoltage = voltage * 100;
  }
  else{
    intvoltage = voltage * 10; //If the voltage is greater than or equal to 10, use only 1 of the digits as a decimal.
  }
  voltage100 = (intvoltage / 100) % 10; //Take the first digit of the voltage measurement and write it to the screen.
  segwrite(voltage100);
  if(voltage < 10){
    if (LEDmode == 0){
      digitalWrite(dot, LOW); //If the voltage is less than 10, turn on this decimal as well.
    } else{
        digitalWrite(dot, HIGH);
    }
  }
  if(LEDmode == 0){
    digitalWrite(dig1, HIGH); //Quickly turn on and off the digit to set the number,
    digitalWrite(dig1, LOW);
    digitalWrite(dot, HIGH);
  } else{
      digitalWrite(dig1, LOW);
      digitalWrite(dig1, HIGH);
      digitalWrite(dot, LOW);
  }
  voltage10 = (intvoltage / 10) % 10; //Repeat the above process for the second digit.
  segwrite(voltage10);
  if((voltage >= 10) && (voltage < 100)){
    if (LEDmode == 0){
      digitalWrite(dot, LOW);
    } else {
        digitalWrite(dot, HIGH);
    }
  }
  if (LEDmode == 0){
    digitalWrite(dig2, HIGH);
    digitalWrite(dig2, LOW);
    digitalWrite(dot, HIGH);
  } else{
      digitalWrite(dig2, LOW);
      digitalWrite(dig2, HIGH);
      digitalWrite(dot, LOW);
  }
  voltage1 = (intvoltage) % 10; //Repeat the above step for the final digit.
  segwrite(voltage1);
  if (voltage >= 100){
    if (LEDmode == 0){
      digitalWrite(dot, LOW);
    } else{
        digitalWrite(dot, HIGH);
    }
  }
  if (LEDmode == 0){
    digitalWrite(dig3, HIGH);
    digitalWrite(dig3, LOW);
    digitalWrite(dot, HIGH);
  } else{
    digitalWrite(dig3, LOW);
    digitalWrite(dig3, HIGH);
    digitalWrite(dot, LOW);
  }
}

void segwrite(int y){ //This is the function that sets our screen's number. Depending on the value of the input, y, it will set some segments LOW (the ones it should turn on), and some HIGH (the ones that should be off).
  if(LEDmode == 0){
    if (y == 0){
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, HIGH);
    }
    if (y == 1){
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
    }
    if (y == 2){
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, HIGH);
      digitalWrite(g, LOW);
    }
    if (y == 3){
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, LOW);
    }
    if (y == 4){
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
    }
    if (y == 5){
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, HIGH);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
    }
    if (y == 6){
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
    }
    if (y == 7){
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
    }
    if (y == 8){
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
    }
    if (y == 9){
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
    }
  } else{
    if (y == 0){
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, LOW);
    }
    if (y == 1){
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
    }
    if (y == 2){
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, LOW);
      digitalWrite(g, HIGH);
    }
    if (y == 3){
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, HIGH);
    }
    if (y == 4){
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
    }
    if (y == 5){
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, LOW);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
    }
    if (y == 6){
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
    }
    if (y == 7){
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
    }
    if (y == 8){
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
    }
    if (y == 9){
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
    }    
  }
}
