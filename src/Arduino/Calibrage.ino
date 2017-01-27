/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified 2 Sep 2016
  by Arturo Guadalupi
  
  modified 8 Sep 2016
  by Colby Newman
*/
int high_time=2000;
int current_time=0;
int motor_output=8;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 8 as an output.
  pinMode(motor_output, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  if(current_time<250){
    current_time++;
    digitalWrite(motor_output, HIGH);   // turn the LED on (HIGH is the voltage level)
    delayMicroseconds(1200);                   // wait for a second
    digitalWrite(motor_output, LOW);    // turn the LED off by making the voltage LOW
    delayMicroseconds(18000);
   }
  else {
    digitalWrite(motor_output, HIGH);   // turn the LED on (HIGH is the voltage level)
    delayMicroseconds(1000);                    // wait for a second
    digitalWrite(motor_output, LOW);    // turn the LED off by making the voltage LOW
    delayMicroseconds(19000);
  }
}
