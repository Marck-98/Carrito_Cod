#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// Configuración WiFi
const char* ssid = "LabsITP";
const char* password = "Advanced12345#";

// URL de la API
const char* apiUrl = "http://54.90.176.54:5000/iot/last";

// Pines del sensor ultrasónico
const int EchoPin = 33;    // Pin conectado al Echo del sensor
const int TriggerPin = 25; // Pin conectado al Trigger del sensor

// Pines de los motores
const int motorDelanteroAdelante = 12;
const int motorDelanteroAtras = 14;
const int motorTraseroAdelante = 27;
const int motorTraseroAtras = 26;

// Pines de los LEDs
const int ledPin1 = 18; // Derecha enfrente
const int ledPin2 = 5;  // Izquierda enfrente
const int ledPin3 = 13; // Amarillo Izquierdo de atrás
const int ledPin4 = 4;  // Rojo Izquierdo de atrás
const int ledPin5 = 2;  // Amarillo Derecho de atrás
const int ledPin6 = 15; // Rojo Derecho de atrás

// Pines y configuración del servo
#define PINSERVO 32
Servo servoMotor;

// Variables para el parpadeo de LEDs
unsigned long previousMillis = 0;
int ledState = LOW;
bool parpadeoActivo = false;
int currentStatus = -1;

// Configuración de los movimientos
void setup() {
  Serial.begin(115200);

  // Configurar pines de motores como salidas
  pinMode(motorDelanteroAdelante, OUTPUT);
  pinMode(motorDelanteroAtras, OUTPUT);
  pinMode(motorTraseroAdelante, OUTPUT);
  pinMode(motorTraseroAtras, OUTPUT);

  // Configurar pines de los LEDs
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  pinMode(ledPin6, OUTPUT);

  // Configurar pines del sensor ultrasónico
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  // Iniciar el servo
  servoMotor.attach(PINSERVO);

  // Conectar a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado.");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(apiUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Respuesta de la API:");
      Serial.println(payload);

      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        int status = doc["status"];
        if (status != currentStatus) {
          currentStatus = status;
          Serial.print("Nueva acción recibida: ");
          Serial.println(status);
          procesarStatus(status);
        }
      } else {
        Serial.print("Error al parsear JSON: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("Error en la solicitud HTTP: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Desconectado de WiFi");
  }

  // Comprobar la distancia con el sensor ultrasónico para detener el avance si hay un obstáculo
  if (currentStatus == 2) {  // Solo si está en modo avanzar
    float distancia = ping(TriggerPin, EchoPin);
    if (distancia < 50.0) {
      detenerMotores();
      explorarYBuscarSalida();
      currentStatus = 0;  // Cambia el estado para evitar que reinicie el avance automáticamente
    }
  }

  // Lógica de parpadeo sin bloquear
  if (parpadeoActivo) {
    unsigned long currentMillis = millis();
    int interval = (currentStatus == 8) ? 100 : 500;  // Intervalo rápido al retroceder

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;

      switch (currentStatus) {
        case 8:
          digitalWrite(ledPin1, ledState);
          digitalWrite(ledPin2, ledState);
          digitalWrite(ledPin4, ledState);
          digitalWrite(ledPin6, ledState);
          break;
        case 4:
          digitalWrite(ledPin2, ledState);
          digitalWrite(ledPin3, ledState);
          break;
        case 6:
          digitalWrite(ledPin1, ledState);
          digitalWrite(ledPin5, ledState);
          break;
      }
    }
  }
}

void procesarStatus(int status) {
  detenerMotores();
  apagarLeds();

  switch (status) {
    case 1:
      girarIzquierda(1000);  // Girar a la izquierda 360 grados
      parpadearLEDs(ledPin2, ledPin3, 500); // Parpadear LEDs 5 y 3
      break;
    case 2:
      avanzar();  // Iniciar avance sin detener
      break;
    case 3:
      girarDerecha(1000);  // Girar a la derecha 360 grados
      parpadearLEDs(ledPin1, ledPin5, 500); // Parpadear LEDs 1 y 2
      break;
    case 4:
      girarIzquierda(500);  // Girar a la izquierda 90 grados
      parpadearLEDs(ledPin2, ledPin3, 500); // Parpadear LEDs 5 y 3 por 10 segundos
      delay(10000); // Esperar 10 segundos
      break;
    case 5:
      encenderLedsDetener(); // Encender LEDs sin parpadeo
      break;
    case 6:
      girarDerecha(500);  // Girar a la derecha 90 grados
      parpadearLEDs(ledPin1, ledPin2, 500); // Parpadear LEDs 1 y 2 por 10 segundos
      delay(10000); // Esperar 10 segundos
      break;
    case 8:
      retroceder();  // Iniciar retroceso sin detener
      parpadearLEDs(ledPin4, ledPin6, 100); // Parpadear LEDs 4 y 6
      break;
    default:
      Serial.println("Acción desconocida");
      break;
  }
}

float ping(int TriggerPin, int EchoPin) {
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  unsigned long duration = pulseIn(EchoPin, HIGH);
  return float(duration) / 58.2;
}

void explorarYBuscarSalida() {
  servoMotor.write(45);
  delay(500);
  float distanciaIzquierda = ping(TriggerPin, EchoPin);

  servoMotor.write(135);
  delay(500);
  float distanciaDerecha = ping(TriggerPin, EchoPin);

  servoMotor.write(90);
  delay(500);

  if (distanciaIzquierda > 20.0) {
    girarIzquierda(500);
  } else if (distanciaDerecha > 20.0) {
    girarDerecha(500);
  } else {
    retroceder();
    girar180();
  }
}

void girar180() {
  girarIzquierda(1000);  // Gira 180 grados con un delay de 1000 ms
}

void avanzar() {
  digitalWrite(motorDelanteroAdelante, HIGH);
  digitalWrite(motorTraseroAdelante, HIGH);
}

void retroceder() {
  digitalWrite(motorDelanteroAtras, HIGH);
  digitalWrite(motorTraseroAtras, HIGH);
}

void girarIzquierda(int tiempo) {
  digitalWrite(motorDelanteroAdelante, HIGH);
  digitalWrite(motorTraseroAtras, HIGH);
  delay(tiempo);
  detenerMotores();
}

void girarDerecha(int tiempo) {
  digitalWrite(motorDelanteroAtras, HIGH);
  digitalWrite(motorTraseroAdelante, HIGH);
  delay(tiempo);
  detenerMotores();
}

void detenerMotores() {
  digitalWrite(motorDelanteroAdelante, LOW);
  digitalWrite(motorDelanteroAtras, LOW);
  digitalWrite(motorTraseroAdelante, LOW);
  digitalWrite(motorTraseroAtras, LOW);
}

void encenderLedsDetener() {
  digitalWrite(ledPin4, HIGH);
  digitalWrite(ledPin6, HIGH);
  digitalWrite(ledPin3, HIGH);
  digitalWrite(ledPin5, HIGH);
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  parpadeoActivo = false;
}

void iniciarParpadeo() {
  parpadeoActivo = true;
}

void apagarLeds() {
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  digitalWrite(ledPin4, LOW);
  digitalWrite(ledPin5, LOW);
  digitalWrite(ledPin6, LOW);
}

void parpadearLEDs(int led1, int led2, int intervalo) {
  if (!parpadeoActivo) {
    parpadeoActivo = true;
    previousMillis = millis();
  }
  digitalWrite(led1, ledState);
  digitalWrite(led2, ledState);
}
