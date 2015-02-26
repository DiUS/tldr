#include <Wire.h>
#include <LSM303.h>
#include <ZumoMotors.h>
#include <SoftwareSerial.h>

bool debug = false;

SoftwareSerial bt(2,3); // RX, TX
ZumoMotors motors;
LSM303 compass;

void setup() {
  bt.begin(9600);
  Serial.begin (9600);
  Wire.begin();
  compass.init (LSM303::device_D, LSM303::sa0_auto);
  compass.enableDefault ();
  compass.m_min = (LSM303::vector<int16_t>){ -5388,  -4831, +15795};
  compass.m_max = (LSM303::vector<int16_t>){ -1401,  +1225, +22506};
}

#define GRANULARITY 10

int speed = 300;
int right_turn_speed = 272;
int left_turn_speed = 260;

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

bool close_enough (float where, float wanted)
{
  float delta = abs (where - wanted);
  return (delta < 3);
}

void forward (int n)
{
  RunFor (n, speed, speed);
}
void reverse (int n)
{
  RunFor (n, -speed, -speed);
}
void left (int n)
{
  /*delay (200);
  compass.read ();
  float where = compass.heading ();
  float to = where - n;
  if (to < 0)
    to += 360;
  while (!close_enough (where, to))
  {
    RunFor (3, -left_turn_speed, left_turn_speed);
    delay (250);
    compass.read ();
    where = compass.heading ();
  }*/
  RunFor (n, -left_turn_speed, left_turn_speed);
}
void right (int n)
{
  RunFor (n, right_turn_speed, -right_turn_speed);
}


bool interpret_cmd ()
{
  if (cmdidx == 0)
    return true; // empty command, just print new prompt

  bool valid = true;
  if (strncmp ("fd ", cmdbuf, 3) == 0)
    forward (atoi (cmdbuf + 3));
  else if (strncmp ("rt ", cmdbuf, 3) == 0)
    right (atoi (cmdbuf + 3));
  else if (strncmp ("lt ", cmdbuf, 3) == 0)
    left (atoi (cmdbuf + 3));
  else if (strncmp ("bk ", cmdbuf, 3) == 0)
    reverse (atoi (cmdbuf + 3));
  else if (strncmp ("sp ", cmdbuf, 3) == 0)
    speed = atoi (cmdbuf + 3);
  else if (strncmp ("rs ", cmdbuf, 3) == 0)
    right_turn_speed = atoi (cmdbuf + 3);
  else if (strncmp ("ls ", cmdbuf, 3) == 0)
    left_turn_speed = atoi (cmdbuf + 3);
  else if (strncmp ("debug ", cmdbuf, 6) == 0)
    debug = atoi (cmdbuf + 6);
  else
    valid = false;
  return valid;
}

void loop() {
  int c = -1;
  Stream *s = 0;
  if (bt.available ())
  {
    c = bt.read ();
    if (debug) bt.write (c);
    s = &bt;
  }
  else if (Serial.available ())
  {
    c = Serial.read ();
    if (debug) Serial.write (c);
    s = &Serial;
  }
  if (c != -1)
  {
    if (c == '\r' || c == '\n')
    {
      if (!overflow)
      {
        s->println (interpret_cmd () ? "0" : "1");
/*        delay (200);
        compass.read ();
        s->println (compass.heading ());*/
      }
      else
      {
        s->println ("2");
        overflow = false;
      }
      cmdidx = 0;
      memset (cmdbuf, 0, sizeof (cmdbuf));
      if (debug)
      {
        bt.print ("# ");
        Serial.print ("# ");
      }
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

