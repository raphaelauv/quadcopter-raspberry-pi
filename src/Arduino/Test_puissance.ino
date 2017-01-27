int high_time=1000;
int low_time=19000;
int current_loop=0;
int motor_output=8;
void setup() {
  // initialize digital pin 8 as an output.
  pinMode(motor_output, OUTPUT);
}

void loop() {
  if(current_loop>250){
    current_loop=0;
    if(high_time>2000){
      high_time=1000;
      low_time=19000;
      }
    else{
      high_time+=50;
      low_time-=50;
      }
    }
  digitalWrite(motor_output, HIGH);   // turn the LED on (HIGH is the voltage level)
  delayMicroseconds(high_time);                   // wait for a second
  digitalWrite(motor_output, LOW);    // turn the LED off by making the voltage LOW
  delayMicroseconds(low_time);
  current_loop++; 
}
