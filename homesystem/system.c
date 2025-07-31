int pirPin = 2;          // PIR sensor output connected to digital pin 2
int ledPin = 8;          // LED connected to digital pin 8
int buzzerPin = 9;       // Buzzer connected to digital pin 9

struct SensorLog {
  unsigned long timestamp;
  int motionStatus;
};

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Smart Home System Initialized...");
}

void loop() {
  int motion = digitalRead(pirPin);
  unsigned long currentTime = millis();  // Timestamp in ms

  // Allocate memory for a sensor log
  SensorLog* logEntry = (SensorLog*)malloc(sizeof(SensorLog));
  if (logEntry != NULL) {
    logEntry->timestamp = currentTime;
    logEntry->motionStatus = motion;

    // Log to Serial Monitor
    Serial.print("Time: ");
    Serial.print(logEntry->timestamp);
    Serial.print(" ms - Motion: ");
    if (logEntry->motionStatus == HIGH) {
      Serial.println("DETECTED");
    } else {
      Serial.println("NONE");
    }

    // Trigger actions
    if (motion == HIGH) {
      digitalWrite(ledPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }

    // Free memory
    free(logEntry);
  } else {
    Serial.println("Memory allocation failed.");
  }

  delay(1000); // Wait 1 second before next read
}

