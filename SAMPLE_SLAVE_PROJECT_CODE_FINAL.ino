/*
* The following code for the slave arduino board is responsible for making
* the BoeBot execute certain actions like: traversing across a terrain, object
* detection/avoidance, arm (funnel) movement, buzzing, and bluetooth communication.
*/




#include <SoftwareSerial.h>   //Software Serial Port
#include <Servo.h>            // Include servo library

Servo servoLeft;              // Declare left and right servos
Servo servoRight;
Servo servoFunnel;

#define RxD 7                 //Defines pins used for bluetooth.
#define TxD 6

#define DEBUG_ENABLED  1

SoftwareSerial blueToothSerial(RxD,TxD);

/*
* As well as setting up bluetooth, the setup() method attaches the servo motors
* and sets up the whiskers.
*
* @Parameters: none
* @Returns: void
* @Results:
*            #Setupu bluetooth
*            #Flush serial ports
*            #Set up whiskers
*            #Attahc motors
*
* NOTE: there is a tone played to indicate setup is taking place.
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

 //Setup for robot traversal
  pinMode(7, INPUT);                         // Set right whisker pin to input
  pinMode(5, INPUT);                         // Set left whisker pin to input

  tone(4, 3000, 1000);                       // Play tone for 1 second
  delay(1000);                               // Delay to finish tone

  servoLeft.attach(13);                      // Attach left signal to pin 13
  servoRight.attach(12);                     // Attach right signal to pin 12
  servoFunnel.attach(10);                    // Attach arm signal to pin 10

}

boolean isSlow = false;                 //Should robot should traverse at slow or fast speed
boolean whiskerDetect;                     //Do whiskers detect anything

/*
* The loop() method sends and receives bluetooth signals to and from
* the master board. Depending on what characters are received, the method will perform
* different actions.
*
* @Parameters: none
* @Returns: void
* @Results:
*       ## stop bot from moving
*       ## make bot move
*       ## decide whether the bot moves slowly or not
*       ## read the temperature and send it to the master
*       ## move the arm motor
*       ## sound buzzer tone
*/
void loop()
{
  char bluetoothChar;


  while(1){                                     // Infinite loop

     moveBot(isSlow);                        // Moves the robot, either slow or fast, as specified by master.

    if(blueToothSerial.available())             // If there is data ready in the btooth
    {
      bluetoothChar = blueToothSerial.read();   // Read the data

       // 'c' for Careful.
        if (bluetoothChar == 'c'){              //Determine if bot should drive carfeully, fast, or stop.
          isSlow = true;
        }
       // 'f' for drive Fast.
        else if (bluetoothChar == 'f'){
          isSlow = false;
        }
       // 's' for Stop.
        else if (bluetoothChar == 's'){
          servoLeft.detach();                   // Motors are detached to achieve a complete stop.
          servoRight.detach();

        }
       //'g' for Go.
        else if (bluetoothChar == 'g'){
          servoLeft.attach(13);                 //Motors are attached in case they were detached beforehand.
          servoRight.attach(12);
        }
       //'t' for Temperature
        else if (bluetoothChar == 't'){

       //Sends recorded temperature back to master.
          double temperatureAnalog = analogRead(A0);
          temperatureAnalog = temperatureAnalog * 0.0049;
          int temperature = (temperatureAnalog - 0.5) * 100;
          blueToothSerial.print(temperature);
        }
       // 'p' for plant seeds (arm used to plant seeds).
        else if (bluetoothChar == 'p') {
          armMove();                            //The method causes a third servo motor (the seed planter) to move.
        }
       //'b' for buzzer.
        else if (bluetoothChar == 'b') {
          tone(4, 3000, 1000);
        }

    }
    delay(200);                                //Prevents the while loop from going unecessarily fast.
   }
 }

/*
* The setupBlueToothConnection() function does not have any extra documentation added to it, and
* has remained untouched or altered since it was provided as a project resource.
*/
void setupBlueToothConnection()
{
  //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.begin(38400);
  //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STWMOD=0\r\n");
  //set the bluetooth name (irrelevant)
  blueToothSerial.print("\r\n+STNA=Slave20\r\n");
  // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STOAUT=1\r\n");
  // Auto-connection is not allowed
  blueToothSerial.print("\r\n+STAUTO=0\r\n");
  delay(2000); // This delay is required.
  //make the slave bluetooth inquirable
  blueToothSerial.print("\r\n+INQ=1\r\n");
  delay(2000); // This delay is required.
}

/*
* The maneuver() method moves the left and right servo motors of the BoeBot
* for a certain duration (specified) and at certain speeds (specified).
*
* @Parameters: int 'speedLeft'
* @Parameters: int 'speedRight'
* @Parameters: int 'msTime'
* @Returns: void
* @Result: #Moves servo motors at certain speeds
*          #Moves servo motors in certain directions
*/
void maneuver(int speedLeft, int speedRight, int msTime)
{
  // speedLeft, speedRight ranges: Backward  Linear  Stop  Linear   Forward
  //                               -200      -100......0......100       200
  servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
  servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
  delay(msTime);                                   // Delay for msTime
}

/*
* The moveBot() function is responsible for robot traversal and object avoidance/detection.
*
* @Parameters: Boolean 'isSlow'
* @Returns: void
* @Results: #Moves robot Forward
*           #Adjusts robot speed
*           #Sounds buzzer upon collision
*           #Avoids obstacles by backing up and turning
*/
int speed;                                             //Specifies how fast boebot will move.
void moveBot(boolean isSlow){

    if (isSlow == true)                            //Decides whether bot drives
    {                                                 //slowly or not.
      speed = 100;                                    // Slow speeds for difficult terrain.
    }
    else {
      speed = 200;
    }

    int wLeft = digitalRead(5);                       // Copy left result to wLeft
    int wRight = digitalRead(8);                      // Copy right result to wRight

    if (wLeft == 0 || wRight == 0){                   //Determines if whikers detect something.
      whiskerDetect = true;
    } else {
      whiskerDetect = false;
    }

 //Following code moves the bot.
 //NOTE: Negative speed = forward, positive speed = backward.

    if( !whiskerDetect)                               // If whiskers don't detect anything.
    {
            maneuver(-speed, -speed, speed/10);       // Forward for 20 or 10 ms (depends on speed variable)
    }
    else if(whiskerDetect)                            // If whiskers detect a collision
    {
      tone(4, 3000, 1000);                            // Sounds tone if collision occurs.

      if((wLeft == 0) && (wRight == 0))               // If both whiskers contact
      {
        maneuver(speed, speed, 400);                  // Back up
        maneuver(-speed, speed, 400);                 // Turn left
      }
      else if(wLeft == 0)                             // If only left whisker contact
      {
        maneuver(200, 200, 400);                      // Back up
        maneuver(speed, -speed, 200);                 // Turn right
      }
      else                                            // If only right whisker contact
      {
        maneuver(speed, speed, 400);                  // Back up
        maneuver(-speed, speed, 200);                 // Turn left
      }
    }


}
/*
 * The armMove() method is designed to move a servoMotor in one direction, and then back to the original position, after
 * some delay.
 *
 * @Parameters: none
 * @Returns: void
 * @Results: #Moves arm servo motor
 */
void armMove() {
  servoFunnel.write(90);
  delay(1000);
  servoFunnel.write(160);
}
