// Set GPIO pins
const uint8_t LED1 = 13;
const uint8_t LED2 = 12;
const uint8_t LED3 = 11;
const uint8_t LED4 = 10;
const uint8_t LED5 = 9;
const uint8_t BUTTON = 8;
const uint8_t POT = A0;
uint8_t lights[5] = {LED1, LED2, LED3, LED4, LED5};

// Set durations (in ms) of various light modes
const uint16_t SLOW_BLINK_DURATION = 500;
const uint16_t FAST_BLINK_DURATION = 100;
const uint16_t LOOP_DURATION = 100;

// Delay between allowed button presses
const uint16_t BUTTON_PRESS_DELAY = 200;

// Number of light modes to cycle through
const uint8_t NUM_MODES = 5;

// Event-time trackers
uint32_t last_button_press;
uint32_t blink_time;

// Tracks the light mode
uint8_t light_mode = 0;

// Modifies the speed of all modes
float speed_multiplier;

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop() {
  uint32_t t = millis();
  // Update light_mode if button is pressed
  detect_button_press(t);
  // Update speed multiplier
  read_pot();
  // Match light mode to various light mode functions
  switch(light_mode) {
    case 0: set_all_lights(LOW); break;
    case 1: blink_lights(t, SLOW_BLINK_DURATION); break;
    case 2: blink_lights(t, FAST_BLINK_DURATION); break;
    case 3: loop_lights(t); break;
    case 4: bounce_lights(t); break;
  }
}

// Detect if the button was just pressed, if so, tick light_mode by 1
int last_button_state = LOW;
void detect_button_press(uint32_t t) {
  bool button_state = digitalRead(BUTTON);
  // If the button is high and was previously low, and hasn't been pressed within the BUTTON_PRESS_DELAY time
  if (button_state == HIGH && last_button_state == LOW && t >= last_button_press + BUTTON_PRESS_DELAY) {
    // Tick by 1, mod 5
    light_mode ++;
    light_mode = light_mode % NUM_MODES;
    last_button_press = t;
  }
  last_button_state = button_state; 
}

// Set the speed multiplier based on the potentiometer
void read_pot() {
  // Reads the potentiometer as a value between 0 and 1024
  // Normalize to 0 to 2: zero times the wait duration or double the wait duration
  speed_multiplier = analogRead(A0) / 512.;
}

// Set LED 1, 2, 3, 4, and 5 the same state
void set_all_lights(bool state) {
  digitalWrite(LED1, state);
  digitalWrite(LED2, state);
  digitalWrite(LED3, state);
  digitalWrite(LED4, state);
  digitalWrite(LED5, state);
}

// Blink all lights with a given wait duration
void blink_lights(uint32_t t, uint16_t blink_duration) {
  // If the lights haven't blinked for the blink duration
  if (t >= blink_time + blink_duration * speed_multiplier) {
    // Turn the lights on if off, off if on
    set_all_lights(!digitalRead(LED1));
    blink_time = t;
  }
}

// Turn on one light at a time in a looping pattern
uint8_t active_light = 0;
void loop_lights(uint32_t t) {
  // Turn off all lights
  set_all_lights(LOW);
  // Turn on the active light by idx in the lights array
  digitalWrite(lights[active_light], HIGH);
  // If the active light hasn't changed for the LOOP_DURATION, set the next light to active
  if (t >= blink_time + LOOP_DURATION * speed_multiplier) {
    // Tick the active light idx by 1, mod 5
    active_light ++;
    active_light = active_light % 5;
    blink_time = t;
  }
}

// Turn on one light at a time in a bouncing pattern
bool ascending = true;
void bounce_lights(uint32_t t) {
  // Turn off all lights
  set_all_lights(LOW);
  // Turn on the active light by idx in the lights array
  digitalWrite(lights[active_light], HIGH);
  // If the active light hasn't changed for the LOOP_DURATION, set the next light to active
  if (t >= blink_time + LOOP_DURATION * speed_multiplier) {
    // If the active light is at the lower end, begin ascending, if the active light is at the upper end, begin descending
    switch(active_light) {
      case 0: ascending = true; break;
      case 4: ascending = false; break;
    }
    // If ascending, tick active light idx by 1
    if (ascending) {
      active_light ++;
     // If descending, tick active light idx by -1
    } else {
      active_light --;
    }
    blink_time = t;
  }
}