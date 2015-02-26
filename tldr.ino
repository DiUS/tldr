#include <ZumoMotors.h>
#include <SoftwareSerial.h>

SoftwareSerial bt(2,3); // RX, TX
ZumoMotors motors;

void setup() {
  bt.begin(9600);
  bt.print ("# ");
  Serial.begin (9600);
  Serial.print ("# ");
}

#define GRANULARITY 100
#define SPEED 200

void RunFor (int n, int speed_a, int speed_b)
{
  motors.setSpeeds (speed_a, speed_b);
  delay (n * GRANULARITY);
  motors.setSpeeds (0, 0);
}

#define CMD_MAX_LEN 10
char cmdbuf[CMD_MAX_LEN + 1] = { 0, };
unsigned char cmdidx = 0;
bool overflow = false;

void forward (int n)
{
  bt.print ("Forward...");
  RunFor (n, SPEED, SPEED);
  bt.println ("done");
}
void reverse (int n)
{
  bt.print ("Reverse...");
  RunFor (n, -SPEED, -SPEED);
  bt.println ("done");
}
void left (int n)
{
  bt.print ("Left..");
  RunFor (n, -SPEED, SPEED);
  bt.println ("done");
}
void right (int n)
{
  bt.print ("Right...");
  RunFor (n, SPEED, -SPEED);
  bt.println ("done");
}


void interpret_cmd ()
{
  if (cmdidx == 0)
    return; // empty command, just print new prompt

  bool valid = true;
  if (strncmp ("fd ", cmdbuf, 3) == 0)
    forward (atoi (cmdbuf + 3));
  else if (strncmp ("rt ", cmdbuf, 3) == 0)
    right (atoi (cmdbuf + 3));
  else if (strncmp ("lt ", cmdbuf, 3) == 0)
    left (atoi (cmdbuf + 3));
  else if (strncmp ("bk ", cmdbuf, 3) == 0)
    reverse (atoi (cmdbuf + 3));
  else
    valid = false;
  bt.println (valid ? "ok" : "error");
}

void loop() {
  int c = -1;
  if (bt.available ())
  {
    c = bt.read ();
    bt.write (c);
  }
  else if (Serial.available ())
  {
    c = Serial.read ();
    Serial.write (c);
  }
  if (c != -1)
  {
    if (c == '\r' || c == '\n')
    {
      if (!overflow)
        interpret_cmd ();
      else
      {
        bt.println ("overflow");
        overflow = false;
      }
      cmdidx = 0;
      memset (cmdbuf, 0, sizeof (cmdbuf));
      bt.print ("# ");
    }
    else
    {
      if (cmdidx < CMD_MAX_LEN)
        cmdbuf[cmdidx++] = (char)c;
      else
        overflow = true;
    }
  }
}

