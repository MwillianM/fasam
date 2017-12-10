#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> //5.7.0

// Dados do WiFi
const char* rede = "";//Preencher
const char* senha = "";//Preencher

// Dados MQTT
WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);
const char* servdr = "";//Preencher (cmd -> netstat)
const char* canal = "";//Preencher

// Dados Gerais
unsigned long cont = 0;
unsigned long agora = millis();
int atraso = 5000;//Milisegundos
int led = 2;
int ledEstado = 1;

// Loop MQTT
void callback(char* topic, byte* payload, unsigned int length){
  // Parse do Json
  DynamicJsonBuffer jsonBuffer;
  String json = (char*)payload;
  JsonObject& rootRead = jsonBuffer.parseObject(json);
  // Leitura do Json
  long c = rootRead["comando"];
  Serial.println(json);
  // Atuacao
  if(c == 1){
    // Mudar estado
    if(ledEstado == 0){//Estado invertido no led 2
      digitalWrite(led, HIGH);
      Serial.println("Ligado!");
      ledEstado = 1;
    }else{
      digitalWrite(led, LOW);
      Serial.println("Desligado!");
      ledEstado = 0;
    }
  }else{
    if(c == 0){
      // Obter estado
      if (ledEstado == 1){
        Serial.println("Estou Ligado!");
      }else{
        Serial.println("Estou Desligado!");
      }
    }else{
      // Ajuda
      Serial.println("==========================");
      Serial.println("Comandos:");
      Serial.println("{comando:1} -> Mudar estado");
      Serial.println("{comando:0} -> Obter estado");
      Serial.println("==========================");
    }
  }
}

void setup(){
  // Iniciando Placa
  delay(1000);
  Serial.begin(115200);
  delay(3000);
  pinMode(led,OUTPUT);
  Serial.println("Funcionando!");

  // Conectando a WiFi
  Serial.print("Conectando a ");
  Serial.println(rede);
  WiFi.mode(WIFI_STA); //Modo Servidor-Cliente
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(rede, senha);
    Serial.print(WiFi.status());
    delay(500);
  }
  Serial.println("\nWiFi conectado!");  
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Servidor MQTT
  MQTT.setServer(servdr, 1883);
  MQTT.setCallback(callback);
}

// Conectando ao MQTT
void reconnect(){
  while (!MQTT.connected()) {
    Serial.print("Conectando a ");
    Serial.println(servdr);
    if (MQTT.connect(servdr)) {
      MQTT.subscribe(canal);
      Serial.print("Inscrito no canal ");
      Serial.println(canal);
    } else {
      Serial.print(".");
      delay(3000);
    }
  }
}

void loop(){
// Loop MQTT
  reconnect();
  MQTT.loop();
// Simulando delay
  unsigned long agora = millis();
  if((cont + atraso) < agora){
    cont = agora;
    Serial.print(".");
  }
}
