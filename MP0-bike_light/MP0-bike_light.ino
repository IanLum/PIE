const uint8_t LED1 = 13;
const uint8_t LED2 = 12;
const uint8_t LED3 = 11;
const uint8_t LED4 = 10;
const uint8_t LED5 = 9;
const uint8_t BUTTON = 8;
uint8_t lights[5] = {LED1, LED2, LED3, LED4, LED5};

const uint16_t SLOW_BLINK_DURATION = 500;
const uint16_t FAST_BLINK_DURATION = 100;
const uint16_t LOOP_DURATION = 100;

const uint16_t BUTTON_PRESS_DELAY = 200;
const uint8_t NUM_MODES = 4;

uint32_t last_button_press;
uint32_t blink_time;
uint8_t light_mode = 0;

int last_button_state = LOW;

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
  uint32_t t;
  t = millis();

  bool button_state = digitalRead(BUTTON);
  if (button_state == HIGH && last_button_state == LOW && t >= last_button_press + BUTTON_PRESS_DELAY) {
    light_mode ++;
    light_mode = light_mode % NUM_MODES;
    last_button_press = t;
  }
  last_button_state = button_state;

  switch(light_mode) {
    case 0: set_all_lights(LOW); break;
    case 1: blink_lights(t, SLOW_BLINK_DURATION); break;
    case 2: blink_lights(t, FAST_BLINK_DURATION); break;
    case 3: loop_lights(t); break;
  }
}

void set_all_lights(bool state) {
  digitalWrite(LED1, state);
  digitalWrite(LED2, state);
  digitalWrite(LED3, state);
  digitalWrite(LED4, state);
  digitalWrite(LED5, state);
}

void blink_lights(uint32_t t, uint16_t blink_duration) {
  if (t >= blink_time + blink_duration) {
    set_all_lights(!digitalRead(LED1));
    blink_time = t;
  }
}

uint8_t active_light_loop = 0;
void loop_lights(uint32_t t) {
  set_all_lights(LOW);
  digitalWrite(lights[active_light_loop], HIGH);
  if (t >= blink_time + LOOP_DURATION) {
    active_light_loop ++;
    active_light_loop = active_light_loop % 5;
    blink_time = t;
  }
}