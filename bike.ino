// bike.ino
// Cycling Sim Demo — single Arduino sketch.
// Reads:
//   - Pedal Hall sensor on D2 (interrupt) → RPM
//   - Steering potentiometer on A0 → angle (-1.0 to +1.0)
// Sends CSV over Serial at 20 Hz: "RPM,ANGLE\n" (e.g. "62.3,-0.42")
//
// Wiring:
//   Hall sensor (e.g. A3144 / KY-003):
//     VCC  → 5V
//     GND  → GND
//     OUT  → D2  (interrupt-capable pin)
//   Potentiometer (10kΩ):
//     Outer pin 1 → 5V
//     Wiper       → A0
//     Outer pin 2 → GND
//
// Open the HTML page in Chrome/Edge, click "Connect", select the Arduino port.

const uint8_t HALL_PIN  = 2;     // interrupt-capable on Uno/Nano (INT0)
const uint8_t POT_PIN   = A0;
const uint16_t SEND_HZ  = 20;
const uint16_t POT_MIN  = 0;     // calibrate if needed
const uint16_t POT_MAX  = 1023;
const uint16_t RPM_WINDOW_MS = 1000;  // recompute RPM over 1s rolling window

volatile unsigned long pulseCount = 0;
unsigned long lastSampleAt = 0;
unsigned long lastPulseSampleAt = 0;
unsigned long lastPulseSnapshot = 0;
float currentRPM = 0;

void onPedalPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), onPedalPulse, FALLING);
  pinMode(POT_PIN, INPUT);
  lastSampleAt = millis();
  lastPulseSampleAt = millis();
}

void loop() {
  unsigned long now = millis();

  // Recompute RPM every RPM_WINDOW_MS ms by sampling pulse delta.
  if (now - lastPulseSampleAt >= RPM_WINDOW_MS) {
    noInterrupts();
    unsigned long snapshot = pulseCount;
    interrupts();

    unsigned long pulses = snapshot - lastPulseSnapshot;
    float seconds = (now - lastPulseSampleAt) / 1000.0;
    // 1 magnet on the wheel = 1 pulse per revolution.
    currentRPM = (pulses / seconds) * 60.0;

    lastPulseSnapshot = snapshot;
    lastPulseSampleAt = now;
  }

  // Send sample over serial at SEND_HZ rate.
  if (now - lastSampleAt >= (1000 / SEND_HZ)) {
    int raw = analogRead(POT_PIN);
    // Normalize potentiometer to -1.0 (full left) ... +1.0 (full right).
    float norm = (float)(raw - POT_MIN) / (float)(POT_MAX - POT_MIN);  // 0..1
    float angle = norm * 2.0 - 1.0;                                    // -1..1
    if (angle < -1.0) angle = -1.0;
    if (angle >  1.0) angle =  1.0;

    Serial.print(currentRPM, 2);
    Serial.print(',');
    Serial.println(angle, 3);

    lastSampleAt = now;
  }
}
