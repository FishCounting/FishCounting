/*PSEUDOCODE

// Link for more advanced wire usage
https://create.arduino.cc/projecthub/ianabcumming/arduino-laser-tripwire-31e473?ref=search&ref_id=laser&offset=4*/


void setup() {
	// Activate diode on pin 9
	pinMode(9, OUTPUT);

	// Activate photoresistor
	pinMode(A0, INPUT);

	// Activate diode on pin 9
	pinMode(10, OUTPUT);

	// Activate photoresistor
	pinMode(A1, INPUT);

	// Activate diode on pin 9
	pinMode(11, OUTPUT);

	// Activate photoresistor
	pinMode(A2, INPUT);

	// Activate diode on pin 9
	pinMode(12, OUTPUT);

	// Activate photoresistor
	pinMode(A3, INPUT);

	// Activate diode on pin 9
	pinMode(13, OUTPUT);

	// Activate photoresistor
	pinMode(A4, INPUT);

	// Begin communication with serial port
	Serial.begin(9600);
}

void loop(){

	int countTime = 0;
	bool isLight = false;
	int maxHeight = 0;
	int numLaser = 5;
	int tempCount = 0;

	while(1){
		// Turn the laser on (middle laser)
		digitalWrite(9, HIGH);

		// This reads a value from 0 - 1024
		double lightAnalog = analogRead(A0);

		// Sensor unblocked
		if (lightAnalog > 800) {

			// Sensor was previously blocked
			if (isLight) {
				// Time is written to serial in terms of milliseconds
				Serial.write(countTime * 50);
				Serial.write(maxHeight);
				isLight = false;
				countTime = 0;
				maxHeight = 0;

			}
			delay (50);
			continue;

		// Sensor blocked
		} else {
				tempCount = 0;
				double lightAnalog1 = analogRead(A1);
				double lightAnalog2 = analogRead(A2);
				double lightAnalog3 = analogRead(A3);
				double lightAnalog4 = analogRead(A4);
				double lightAnalog5 = analogRead(A5);

				if (lightAnalog1 < 800) {
					tempCount++;
				}
				if (lightAnalog2 < 800) {
					tempCount++;
				}
				if (lightAnalog3 < 800) {
					tempCount++;
				}
				if (lightAnalog4 < 800) {
					tempCount++;
				}
				if (lightAnalog5 < 800) {
					tempCount++;
				}
				if (tempCount > maxHeight) {
					maxHeight = tempCount;
				}
				countTime++;
				isLight = true;
		}
		delay (50);
	}
}
