/*PSEUDOCODE

//variables
Int count = 0;

Void setup(){
  //we need to set the laser as an output
  // X is a pin within a specific range -> need to check documentation for specific board
  pinMode(X,OUTPUT);
  Serial.begin(9600);
}

lightFlag = false;

Void loop(){
  while(1){
   //turn the laser on
   digitalWrite(X,HIGH);
  /*1. read the photoresistor
  2. IF STATEMENT
  	A. value is above a certain threshold, no change -> go to end of loop
    if (lightFlag) {
      count++;
      lightFlag = false;
    }
  	B. if value is below a certain threshold
		  I. keep on reading the values on photoresistor
      lightFlag = true;
    Repeat loop
  }
}


// Link for more advanced wire usage
https://create.arduino.cc/projecthub/ianabcumming/arduino-laser-tripwire-31e473?ref=search&ref_id=laser&offset=4*/

// Socket library may need to be imported
int count = 0;
boolean isLight = false;

void setup() {
  // Set laser up
  pinMode(11, OUTPUT);
  pinMode(A0, INPUT);

  Serial.begin(9600);
}



void loop(){
	while(1){
   //turn the laser on
		digitalWrite(12, HIGH);
		// This reads a value from 0 - 1024
		double lightAnalog = analogRead(A0);
		if (lightAnalog > 800)
			if (isLight) {
				count++;
				isLight = false;
			}
			delay (200);
			continue;
		else {
			isLight = true;
		}
		delay (200);
	}
}
