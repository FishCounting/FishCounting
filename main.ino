/*PSEUDOCODE

// Link for more advanced wire usage
https://create.arduino.cc/projecthub/ianabcumming/arduino-laser-tripwire-31e473?ref=search&ref_id=laser&offset=4*/

int count = 0;
boolean isLight = false;

void setup() {
	// Activate diode on pin 9
	pinMode(9, OUTPUT);

	// Activate photoresistor
	pinMode(A0, INPUT);

	// Begin communication with serial port
	Serial.begin(9600);
}

void loop(){
	while(1){
		// Turn the laser on
		digitalWrite(9, HIGH);

		// This reads a value from 0 - 1024
		double lightAnalog = analogRead(A0);

		// Sensor unblocked
		if (lightAnalog > 800) {

			// Sensor was previously blocked
			if (isLight) {
				Serial.write(0);
				isLight = false;
			}
			delay (200);
			continue;

		// Sensor blocked
		} else {
				isLight = true;
		}
		delay (200);
	}
}
