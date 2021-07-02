// C++ code
//

//=======
//Setup Varibles
//Each led increments the timer by as set amount default 30 min which gives a max timer between 30min to 2Hr in 30 Min increments
//Change the ledMinites value to adjust this.
const int ledMinites = 1; //value of one led in millis 60000=1min


//LEDs
int leds[] = {2, 3, 4, 5};
int ledOnCount = 0;
int blinkRate = 500;//milliseconds
bool ledsOn = true;

//button sate varibles
const int KEY_PRESSED = HIGH; //state of key being pressed
const unsigned long KEY_DURATION = 1000 * 2; //milliseconds, minimum threshold to test for long presses
const int buttonPin = 9;

int buttonState = 0;
int lastButtonState = 0;

unsigned long buttonPressTime;
bool buttonDown = false;

bool buttonPressed;
bool buttonHeld;

//speaker
int buzz_pin = 10;

//timer
unsigned long timer_millis = 0;
const long increment = ledMinites * 60000; //value of one led in millis 60000=1min

//prop bomb
int bomb_state = 0;
unsigned long countdown_start = 0;

unsigned long currentMillis;
unsigned long previousBlinkMIllis;
unsigned long previousBuzzMIllis;

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting");
  for (byte i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
  }

  pinMode(buzz_pin, OUTPUT);
  pinMode(buttonPin, OUTPUT);
}

void check_button() {
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, react
    if (buttonState == KEY_PRESSED) {
      // The button just got pushed
      buttonPressTime = millis();
      buttonDown = true;

      Serial.println("on");
    } else {
      // the button just got released
      buttonDown = false;
      buttonPressed = true;
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(500);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
  if (buttonDown) {
    String str;
    Serial.println(str + "Hold T: " + (currentMillis - buttonPressTime) + ":" + KEY_DURATION);
  }
  if (buttonDown == true && currentMillis - buttonPressTime >= KEY_DURATION) {
    // we have a long key press
    Serial.println("HOLD");
    buttonDown = false; // reset state so button has to be released before action again

    buttonPressed = false;
    buttonHeld = true;
  }
}


void set_time() {
  blinkRate = 0;
  if (buttonPressed) {

    ledOnCount ++;
    if (ledOnCount > 4) {
      ledOnCount = 0;
    }
    timer_millis = (long)increment * (long)ledOnCount;
    Serial.println((int)ledOnCount);
    Serial.println((int)increment);
    Serial.println(timer_millis);
  }
  if (buttonHeld && ledOnCount > 0) {
    bomb_state++;
  }
}

void wait_to_arm() {
  blinkRate = 500;

  if (buttonHeld) {
    bomb_state++;
  }
}

void count_down() {
  if (countdown_start == 0 ) {
    String str;
    Serial.println(str + "ARMED : " + timer_millis / 1000 + "sec");
    tone(buzz_pin, 329, 250);
    delay(250);
    tone(buzz_pin, 329 + 50, 250);
    delay(250);
    tone(buzz_pin, 329 + 100, 250);
    delay(250);
    countdown_start = millis();
  }

  int elapsed_percent = 100 * (millis() - countdown_start) / timer_millis;
  if (buttonHeld) {
    if (elapsed_percent >= 100) {
      bomb_state += 2;
    } else {
      bomb_state++;
    }
  } else {
    if (millis() - countdown_start >= timer_millis) {
      if (millis() - previousBuzzMIllis > 5000) {
        previousBuzzMIllis = millis();

        //Prop Bomb Goes off
        Serial.println("**BANG**");
        tone(buzz_pin, 600, 5000);
        ledOnCount = 4;
        blinkRate =250;
      }

    }

    if (elapsed_percent < 25 ) {
      ledOnCount = 4;
      blinkRate = 1000;
    } else if (elapsed_percent < 50 ) {
      ledOnCount = 3;
      blinkRate = 750;
    } else if (elapsed_percent < 75 ) {
      ledOnCount = 2;
      blinkRate = 500;
    } else if (elapsed_percent < 99 ) {
      ledOnCount = 1;
      blinkRate = 250;
    } else if (elapsed_percent == 0 ) {
      ledOnCount = 100;
      blinkRate = 500;
    }

  }

}
void disarmed() {
  Serial.println("**DISARMED**");
  ledOnCount = 0;
  tone(buzz_pin, 329 + 100, 250);
  delay(250);
  tone(buzz_pin, 329 + 50, 250);
  delay(250);
  tone(buzz_pin, 329, 250);
  delay(250);

  bomb_state++;

}

void loop()
{
  check_button();
  currentMillis = millis();

  switch (bomb_state) {
    case 0: //Start up
      //Serial.println("State: set");
      set_time();
      break;
    case 1:
      //Serial.println("State: waiting");
      wait_to_arm();
      break;
    case 2:
      //Serial.println("State: Armed");
      count_down();
      break;
    case 3:
      //Serial.println("State: Disarmed");
      disarmed();
      break;
  }


  //Led Control
  if (currentMillis - previousBlinkMIllis > blinkRate) {
    ledsOn = !ledsOn;
    previousBlinkMIllis = currentMillis;

    for (byte i = 0; i < ledOnCount; i = i + 1) {
      digitalWrite(leds[i], (ledsOn) ? HIGH : LOW);
    }
    for (byte i = ledOnCount; i < 4; i = i + 1) {
      digitalWrite(leds[i], LOW);
    }
  }

  //role over bomb state as needed
  if (bomb_state > 3) {
    //reset all
    ledOnCount = 0;
    timer_millis = 0;
    countdown_start = 0;
    bomb_state = 0;
    blinkRate = 500;
  }
  //reset needed varibles for next loop
  buttonPressed = false;
  buttonHeld = false;
}
