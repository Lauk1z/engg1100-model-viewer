// ENGG1100 dual-winch controller
// Board: Arduino UNO R4 Minima
// Driver: Keyestudio KS0063 / Jaycar XC4492 (L298N)
// Rocker switch: I = pay out, O = stop, II = reel in

#include <Arduino.h>

enum class WinchMode : uint8_t {
  STOPPED,
  PAY_OUT,
  REEL_IN
};

// L298N pin assignment from the KS0063/XC4492 example wiring.
constexpr uint8_t MOTOR_A_IN1 = 5;
constexpr uint8_t MOTOR_A_IN2 = 6;
constexpr uint8_t MOTOR_A_ENABLE = 9;
constexpr uint8_t MOTOR_B_IN3 = 7;
constexpr uint8_t MOTOR_B_IN4 = 8;
constexpr uint8_t MOTOR_B_ENABLE = 10;

// Connect the switch common terminal to GND. The two throws go to D2/D3.
// INPUT_PULLUP makes an open or disconnected input read HIGH (safe stop).
constexpr uint8_t SWITCH_I_PIN = 2;
constexpr uint8_t SWITCH_II_PIN = 3;

constexpr uint8_t MOTOR_PWM = 180;              // 0-255; start testing at 180.
constexpr unsigned long DEBOUNCE_MS = 35;
constexpr unsigned long REVERSE_DEAD_TIME_MS = 150;

// The prototype motors face opposite directions. If one rope moves the wrong
// way during the unloaded test, change only that motor's value below.
constexpr bool MOTOR_A_REVERSED = false;
constexpr bool MOTOR_B_REVERSED = true;

WinchMode candidateMode = WinchMode::STOPPED;
WinchMode activeMode = WinchMode::STOPPED;
unsigned long candidateSinceMs = 0;

WinchMode readSwitch() {
  const bool positionI = digitalRead(SWITCH_I_PIN) == LOW;
  const bool positionII = digitalRead(SWITCH_II_PIN) == LOW;

  // Centre O gives HIGH/HIGH. LOW/LOW is treated as a wiring fault.
  if (positionI == positionII) {
    return WinchMode::STOPPED;
  }
  return positionI ? WinchMode::PAY_OUT : WinchMode::REEL_IN;
}

void stopMotors() {
  analogWrite(MOTOR_A_ENABLE, 0);
  analogWrite(MOTOR_B_ENABLE, 0);
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, LOW);
  digitalWrite(MOTOR_B_IN3, LOW);
  digitalWrite(MOTOR_B_IN4, LOW);
}

void driveMotor(uint8_t input1, uint8_t input2, uint8_t enablePin,
                bool forward, bool reversed) {
  const bool effectiveForward = forward != reversed;
  digitalWrite(input1, effectiveForward ? HIGH : LOW);
  digitalWrite(input2, effectiveForward ? LOW : HIGH);
  analogWrite(enablePin, MOTOR_PWM);
}

void startMode(WinchMode mode) {
  if (mode == WinchMode::STOPPED) {
    stopMotors();
    return;
  }

  // PAY_OUT is the reference direction; REEL_IN is its reverse.
  const bool forward = mode == WinchMode::PAY_OUT;
  driveMotor(MOTOR_A_IN1, MOTOR_A_IN2, MOTOR_A_ENABLE,
             forward, MOTOR_A_REVERSED);
  driveMotor(MOTOR_B_IN3, MOTOR_B_IN4, MOTOR_B_ENABLE,
             forward, MOTOR_B_REVERSED);
}

void applyMode(WinchMode newMode) {
  // Always remove drive before changing direction.
  stopMotors();
  if (newMode != WinchMode::STOPPED) {
    delay(REVERSE_DEAD_TIME_MS);
    // Do not restart if the operator moved the switch during the dead time.
    if (readSwitch() != newMode) {
      activeMode = WinchMode::STOPPED;
      return;
    }
    startMode(newMode);
  }
  activeMode = newMode;
}

void printMode(WinchMode mode) {
  switch (mode) {
    case WinchMode::PAY_OUT:
      Serial.println("I: PAY OUT / RELEASE ROPE");
      break;
    case WinchMode::REEL_IN:
      Serial.println("II: REEL IN / RETRACT ROPE");
      break;
    default:
      Serial.println("O: STOP");
      break;
  }
}

void setup() {
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_A_ENABLE, OUTPUT);
  pinMode(MOTOR_B_IN3, OUTPUT);
  pinMode(MOTOR_B_IN4, OUTPUT);
  pinMode(MOTOR_B_ENABLE, OUTPUT);
  pinMode(SWITCH_I_PIN, INPUT_PULLUP);
  pinMode(SWITCH_II_PIN, INPUT_PULLUP);

  stopMotors();
  Serial.begin(115200);
  Serial.println("ENGG1100 winch controller ready; outputs are stopped.");

  candidateMode = readSwitch();
  candidateSinceMs = millis();
}

void loop() {
  const WinchMode rawMode = readSwitch();
  const unsigned long now = millis();

  if (rawMode != candidateMode) {
    candidateMode = rawMode;
    candidateSinceMs = now;
  }

  if (candidateMode != activeMode &&
      now - candidateSinceMs >= DEBOUNCE_MS) {
    applyMode(candidateMode);
    printMode(candidateMode);
  }
}
