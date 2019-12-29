/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

const char trame[] = "\x02"
"ADCO 524563565245 /\n"
"OPTARIF BBR( S\n"
"ISOUSC 30 9\n"
"BBRHCJB 012000135 )\n"
"BBRHPJB 009920022 B\n"
"BBRHCJW 000802009 E\n"
"BBRHPJW 000234878 _\n"
"BBRHCJR 000027700 =\n"
"BBRHPJR 000055029 O\n"
"PTEC HPJB P\n"
"DEMAIN BLEU V\n"
"IINST1 001 I\n"
"IINST2 000 I\n"
"IINST3 004 N\n"
"IMAX1 060 6\n"
"IMAX2 060 7\n"
"IMAX3 060 8\n"
"PMAX 05695 ?\n"
"PAPP 01220 &\n"
"HHPHC A ,\n"
"MOTDETAT 000000 B\n"
"PPOT 00 #\n"
"\x03";

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(1200, SERIAL_7E1);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  Serial.print(trame);
}
