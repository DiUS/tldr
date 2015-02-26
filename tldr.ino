#include <ZumoMotors.h>
#include <SoftwareSerial.h>

#if 1
SoftwareSerial bt(2,3); // RX, TX
#else
# define bt Serial
#endif

int thisByte = 33;

void setup() {
  bt.begin(9600);
  bt.print ("# ");
}

#define GRANULARITY 100

void RunFor (int n, int speed_a, int speed_b)
{
  // ZumoMotors.setSpeeds (speed_a, speed_b);
  delay (n * GRANULARITY);
  // ZumoMotors.setSpeeds (0, 0);
}

#define CMD_MAX_LEN 10
char cmdbuf[CMD_MAX_LEN + 1] = { 0, };
unsigned char cmdidx = 0;
bool overflow = false;

void forward (int n)
{
  bt.print ("Forward...");
  RunFor (n, 10, 10);
  bt.println ("done");
}
void reverse (int n)
{
  bt.print ("Reverse...");
  RunFor (n, -10, -10);
  bt.println ("done");
}
void left (int n)
{
  bt.print ("Left..");
  RunFor (n, -5, 5);
  bt.println ("done");
}
void right (int n)
{
  bt.print ("Right...");
  RunFor (n, 5, -5);
  bt.println ("done");
}


void interpret_cmd ()
{
  bool valid = true;
  if (strncmp ("fd ", cmdbuf, 3) == 0)
    forward (atoi (cmdbuf + 3));
  else if (strncmp ("rt ", cmdbuf, 3) == 0)
    right (atoi (cmdbuf + 3));
  else if (strncmp ("lt ", cmdbuf, 3) == 0)
    left (atoi (cmdbuf + 3));
  else if (strncmp ("rv ", cmdbuf, 3) == 0)
    reverse (atoi (cmdbuf + 3));
  else
    valid = false;
  bt.println (valid ? "ok" : "error");
}

void loop() {
  if (Serial.available ())
  {
    int c = Serial.read ();
    bt.write (c);
    if (c == '\n')
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

