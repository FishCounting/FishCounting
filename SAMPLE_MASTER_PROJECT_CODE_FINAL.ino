/*
* The following is the code for the master board, in our boebot system.
*
* The code details how the master can sends signals to the slave, and receive signals
* from both the slave and serial.
*
* In particular, it details how the master requests and receives
* the temperature reading from the boebot, and processes it so that
* an LED is lit on the master board.
*/

#include <SoftwareSerial.h>                     // Software Serial Port

#define RxD 7
#define TxD 6

#define DEBUG_ENABLED  1

String slaveAddr = "0,6A,8E,16,C8,11";

SoftwareSerial blueToothSerial(RxD,TxD);

/*
* The setup() method sets everything on the board up,
* so that it is ready for the program to use.
*
* @Parameter input: none
* @Returns: void
* @Result: #Bluetooth connection is setup
*          #Serial and Bluetooth serial are flushed
*          #LED becomes ready to use.
*
*/
void setup()
{
  Serial.begin(9600);
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  setupBlueToothConnection();
  //wait 1s and flush the serial and btooth buffers
  delay(1000);
  Serial.flush();
  blueToothSerial.flush();

  pinMode(11, OUTPUT); // Red
  pinMode(12, OUTPUT); // Green
  pinMode(13, OUTPUT); // Blue
}

/*
* The loop constantly scans the bluetooth and serial ports for input.
*
* If there is input from the bluetooth, then the signals are processed
* and the LED on the masterboard is made to go a certain color.
*
* If there is input from the serial, then it will be sent to the slave board;
* and the LED on the master board could turn on or off.
*
* @Parameters: none
* @Returns: void
* @Result: #Prints data received by bluetooth or serial.
*/
//RGB values.
int R = 0;
int G = 0;
int B = 255;

void loop()
{
  char serialChar;
  int temperature;

  // Infinite loop
  while(1)
  {
  // If there is data pending to be read from the shield
    if(blueToothSerial.available())
    {
      temperature = blueToothSerial.parseInt();

  // Print the data through the Serial channel
      Serial.print("Master Received: ");
      Serial.print(temperature);
      Serial.print("\n");
    }
  // Turn off the LED if Serial request received.
   if (serialChar == 'o') {
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
   }
   else if (serialChar == 't') {

    if (temperature > 24) {
  // Output Green to indicate this is optimal temperature.
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    } else if (temperature > 20) {

  // Output violet/purple to indicate that temperature is slightly below optimal.
    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
    } else {

  // Output red to indicate temperature is obstructive to plant growth.
    digitalWrite(11, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    }
   }

    // If there is data pending to be read from the serial port
    if(Serial.available())
    {
      serialChar  = Serial.read();
      // Send the data through btooth
      blueToothSerial.print(serialChar);
      Serial.print("Master Transmitted: ");
      Serial.print(serialChar);
      Serial.print("\n");
    }
    delay (100);
  }
}

/*
* The method setupBlueToothConnection() [BELOW] was not implemented or altered in
* any way by our team. Hence it has not been further documented beyond what documentation
* it already had initially.
*/

// Function to start the connection. Make sure slaveAddr is set to the right
// value.
void setupBlueToothConnection()
{
  // Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.begin(38400);
  // set the bluetooth work in master mode
  blueToothSerial.print("\r\n+STWMOD=1\r\n");
  // set the bluetooth name (irrelevant)
  blueToothSerial.print("\r\n+STNA=Master20\r\n");
  // Auto-connection is forbidden here
  blueToothSerial.print("\r\n+STAUTO=0\r\n");
  // This delay is required.
  delay(2000);
  blueToothSerial.flush();
  // This delay is required.
  delay(2000);

  //form the full connection command
  Serial.print("Connecting to slave: ");
  Serial.println(slaveAddr);
  //send connection command
  blueToothSerial.print("\r\n+CONN=" + slaveAddr + "\r\n");
  delay(5000);
}
