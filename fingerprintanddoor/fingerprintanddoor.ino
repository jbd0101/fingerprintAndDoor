
#include <Adafruit_Fingerprint.h>
#include <Servo.h>

SoftwareSerial mySerial(2, 3);

Servo door;

int pos = 0;    // variable to store the servo position
int detector = 4;
boolean needToClose=false;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  openDoor();
  return finger.fingerID; 
}

void openDoor(){
  for (pos = 65; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    door.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  needToClose=true;
}

void closeDoor(){
  for (pos = 0; pos <= 60; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    door.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  needToClose=false;
}

void closeDoorIfAllowed(){
  int stat = digitalRead(detector);
  if(stat==0){
    closeDoor();
  }
}

void setup()  
{
  Serial.begin(9600);
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  pinMode(detector,INPUT_PULLUP);

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
   door.attach(9);

  closeDoor();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}

void loop()                     // run over and over again
{
  if(needToClose){
    closeDoorIfAllowed();
  }else{
    getFingerprintIDez();

  }
  delay(250);            //don't ned to run this at full speed.
}
