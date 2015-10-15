/*
 * Zack
 * sep.26
 *
 * Car coding
 *
 * Wireless + motor control
 *
 * motor(number, speed, direction) set given motor to given speed and direction
 *
 * attack(s) run forward with given speed
 *
 * retreat(s) run backwrd with given speed
 *
 * stopper() stop
 *
 *
 * S_print(str)   switch to Serial and print
 *
 * Xcheck(str)  check if is given string return bool
 *
 * X_checker(str, len, count, input, trigger)  check continuous char to string
 */

//lib
#include <SoftwareSerial.h>
#include <AFMotor.h>



//Motor define
AF_DCMotor motor1(2, MOTOR12_64KHZ);  //front left

AF_DCMotor motor2(1, MOTOR12_64KHZ);  //front right

AF_DCMotor motor3(3, MOTOR12_64KHZ);  //back left

AF_DCMotor motor4(4, MOTOR12_64KHZ);  //back right


//varialble-------------------------------------------------

/*
 * Motor direction status
 * 1 FORWARD
 * 2 BACKWARD
 * 0 RELEASE / STOP
 */
int OverAll[] = {0, 0, 0, 0};

char reader;

//Xbee check structer
struct Xchecker {
  String str;
  int len;
  int count;
  bool trigger;
};

Xchecker Xstop = {"stop", 4, 0, false};

Xchecker Xgo = {"go", 2, 0, false};

Xchecker Xretreat = {"retreat",7,0,false};


//Xbee initial -------------------------------------------------
#define TX 10
#define RX 11
SoftwareSerial Xbee(RX, TX); //Xbee's RX TX pin


//Xbee code-------------------------------------------------


//String check using int
bool X_checker(String str, int len, int *count, char input, bool *trigger)
{
  //initial
  //did in parameter

  //invalid check
  if (*trigger == true && input != str[*count])
  {
    *count = 0;
    *trigger = false;
  }

  //trigger
  if (*trigger == false && input == str[0])
  {
    *trigger = true;
    *count = 1;
  }

  //checking
  if (*trigger == true && input == str[*count])
  {
    *count = *count + 1;
  }

  //return
  if (*count == len)
  {
    return true;
    *trigger = false;
    *count = 0;
    Xbee.println();
  }
  else {
    return false;
  }
}

//Switch to Serial and print str
void S_print(String str)
{
  Xbee.end();
  delay(5);
  Serial.begin(9600);

  Serial.println(str);

  Serial.end();
  delay(5);
  Xbee.begin(9600);
}

//Check if input String is given str
bool Xcheck(String str)
{
  if (Xbee.available() && Xbee.readString() == str)
  {
    return true;
  }

  else
  {
    return false;
  }
}


//Motor Code-------------------------------------------------

/*motor(number, speed, direction)
 *
 * number 1-4
 *
 * Speed 0-255
 *
 * Dirction
 * 1 FORWARD
 * 2 BACKWARD
 * 0 RELEASE / STOP
 *
 */
void motor(int n, int s, int d) //number speed direction
{
  uint8_t dir;

  //direction
  switch (d)
  {
    case 1:
      dir = FORWARD;
      break;

    case 2:
      dir = BACKWARD;
      break;

    case 0:
      int i = n - 1;
      //forward check
      if (OverAll[i] == 1)
      {
        motor(i, 100, 2);
      }

      //backwar check
      if (OverAll[i] == 2)
      {
        motor(i, 100, 1);
      }
      delay(50);
      motor(n, 0, 0);

      break;
  }

  //setting
  if (d != 0)
  {
    switch (n)
    {
      case 1:
        motor1.setSpeed(s);
        motor1.run(dir);
        break;

      case 2:
        motor2.setSpeed(s);
        motor2.run(dir);
        break;

      case 3:
        motor3.setSpeed(s);
        motor3.run(dir);
        break;

      case 4:
        motor4.setSpeed(s);
        motor4.run(dir);
        break;

      default:
        break;
    }
  }

  OverAll[n - 1] = d;
}

//stop
void stopper()
{
  //Check running motor and run opposite a bit
  for (int i = 1; i <= 4; i++)
  {
    //forward check
    if (OverAll[i] == 1)
    {
      motor(i, 100, 2);
    }

    //backwar check
    if (OverAll[i] == 2)
    {
      motor(i, 100, 1);
    }
  }

  delay(50);

  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}


//run forward with required speed
void attack(int s)  //speed
{
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);

  motor1.setSpeed(s);
  motor2.setSpeed(s);
  motor3.setSpeed(s);
  motor4.setSpeed(s);
}

//run forward with required speed
void retreat(int s)  //speed
{
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);

  motor1.setSpeed(s);
  motor2.setSpeed(s);
  motor3.setSpeed(s);
  motor4.setSpeed(s);
}

void setup() {
  //Xbee start
  Xbee.begin(9600);
  delay(400);

  S_print("Start searching for my other half...");

  while (!Xbee.available())
  {
    Xbee.println("reaching");
    delay(500);
  }

  S_print("\nConnected");
  Xbee.println("\nConnected");

  //Motor

}

void loop() {
  if (Xbee.available())
  {
    reader = Xbee.read();

    if (X_checker(Xstop.str, Xstop.len, &Xstop.count, reader, &Xstop.trigger))
    {
      Xbee.println();
      Xbee.println("Stopping");
      stopper();
    }

    if (X_checker(Xgo.str, Xgo.len, &Xgo.count, reader, &Xgo.trigger))
    {
      Xbee.println();
      Xbee.println("Go");
      attack(255);
    }
    

    if (X_checker(Xretreat.str, Xretreat.len, &Xretreat.count, reader, &Xretreat.trigger))
    {
      Xbee.println();
      Xbee.println("Retreat");
      retreat(255);
    }
  }
}
