// Definición de pines
const int TRIG_PIN = 2;
const int ECHO_PIN = 2;
const int RELAY_PIN = 4;

// Constantes para el sensor ultrasónico
const float SOUND_SPEED = 0.034;  // velocidad del sonido en cm/microsegundo
const float CONTAINER_HEIGHT = 20.0;  // altura del contenedor en cm
const float THRESHOLD = 15.0;  // nivel de líquido que activa el sistema (cm)

// Variables de estado
bool waiting_for_fill = true;
unsigned long pump_start_time = 0;
const unsigned long PUMP_DURATION = 30000;  // 30 segundos en millisegundos

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  
  // Configurar pines
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Asegurar que la bomba está apagada al inicio
  digitalWrite(RELAY_PIN, LOW);
}

float measureDistance() {
  // Generar pulso ultrasónico
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Medir el tiempo de retorno
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calcular distancia
  return duration * SOUND_SPEED / 2;
}

void loop() {
  // Verificar si hay datos disponibles en el puerto serial
  if (Serial.available() > 0) {
    char received = Serial.read();
    if (received == 'B') {
      // Activar la bomba
      digitalWrite(RELAY_PIN, HIGH);
      pump_start_time = millis();
      waiting_for_fill = false;
    }
  }
  
  // Si la bomba está activa, verificar si ya pasaron los 30 segundos
  if (!waiting_for_fill && (millis() - pump_start_time >= PUMP_DURATION)) {
    digitalWrite(RELAY_PIN, LOW);
    waiting_for_fill = true;
  }
  
  // Si estamos esperando que se llene el contenedor
  if (waiting_for_fill) {
    float distance = measureDistance();
    float liquid_level = CONTAINER_HEIGHT - distance;
    
    // Si el nivel de líquido alcanza el umbral
    if (liquid_level >= THRESHOLD) {
      Serial.write('A');
      delay(1000);  // Evitar envíos múltiples
    }
  }
  
  delay(100);  // Pequeña pausa para estabilidad
}
