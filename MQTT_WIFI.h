#include <WiFi.h>
#include <PubSubClient.h> //Biblioteca para as publicações via mqtt
#include "displayESP32LoRa.h" //Arquivo com as funções referentes ao display lora

#define WIFISSID "Redmi"
#define PASSWORD "fandangos"
#define TOKEN "BBFF-AfsLIsF4SBgi9FVGGt8gZhMYLmWJpT" //TOKEN do Ubidots
#define VARIABLE_LABEL_TEMPERATURE "temperature" //variável de temperatura criada no ubidots
#define VARIABLE_LABEL_HUMIDITY "humidity" //variável de umidade criada no ubidots
#define VARIABLE_LABEL_VOLTAGE "voltage" //variável de tensão criada no ubidots
#define DEVICE_ID "5f480f4f1d84724efc5ba918" //ID do dispositivo (Device id, também chamado de client name)
#define SERVER "things.ubidots.com" //Servidor do Ubidots

//Porta padrão
#define PORT 1883

//Tópico aonde serão feitos os publish, "esp32-dht" é o DEVICE_LABEL
#define TOPIC "/v1.6/devices/lora_defc"

//Objeto WiFiClient usado para a conexão wifi
WiFiClient ubidots;
//Objeto para publish–subscribe
PubSubClient client(ubidots);


void reconnect() 
{  
  //Loop até que o MQTT esteja conectado
  while (!client.connected()) 
  {
    //sinaliza desconexão do mqtt no display
    showDisplay(0,"Disconnected", true); 
    showDisplay(1, "Trying to reconnect mqtt...", false);
    Serial.println("Attempting MQTT connection...");
    
    //Tenta conectar
    if (client.connect(DEVICE_ID, TOKEN,"")) 
      Serial.println("connected");
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      //Aguarda 2 segundos antes de retomar
      delay(2000);
    }
  }
  //Sinaliza reconexão do mqtt no display
  showDisplay(4,"Reconnected", false); 
}

bool mqttInit()
{
  //Inicia WiFi com o SSID e a senha
  WiFi.begin(WIFISSID, PASSWORD);
 
  //Loop até que o WiFi esteja conectado
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  //Exibe no monitor serial
  Serial.println("Connected to network");

  //Seta servidor com o broker e a porta
  client.setServer(SERVER, PORT);
  
  //Conecta no ubidots com o Device id e o token
  while(!client.connect(DEVICE_ID, TOKEN, ""))
  {
      Serial.println("MQTT - Connect error");
      return false;
  }

  Serial.println("MQTT - Connect ok");
  return true;
}

//Envia valores por mqtt
//Exemplo: {"temperature":{"value":24.50, "context":{"temperature":24.50, "humidity":57.20, voltage: "0.7"}}}
bool sendValues(float temperature, float humidity, float voltage)
{
  char json[250];
 
  //Atribui para a cadeia de caracteres "json" os valores referentes a temperatura e os envia para a variável do ubidots correspondente
  sprintf(json,  "{\"%s\":{\"value\":%02.02f, \"context\":{\"temperature\":%02.02f, \"humidity\":%02.02f, \"voltage\":%02.02f}}}", VARIABLE_LABEL_TEMPERATURE, temperature, temperature, humidity, voltage);  

  if(!client.publish(TOPIC, json))
    return false;

  //Atribui para a cadeia de caracteres "json" os valores referentes a umidade e os envia para a variável do ubidots correspondente
  sprintf(json,  "{\"%s\":{\"value\":%02.02f, \"context\":{\"temperature\":%02.02f, \"humidity\":%02.02f, \"voltage\":%02.02f}}}", VARIABLE_LABEL_HUMIDITY, humidity, temperature, humidity, voltage);

  if(!client.publish(TOPIC, json))
    return false;
  
  //Atribui para a cadeia de caracteres "json" os valores referentes a umidade e os envia para a variável do ubidots correspondente
  sprintf(json,  "{\"%s\":{\"value\":%02.02f, \"context\":{\"temperature\":%02.02f, \"humidity\":%02.02f, \"voltage\":%02.02f}}}", VARIABLE_LABEL_VOLTAGE, voltage, temperature, humidity, voltage);
     
  if(!client.publish(TOPIC, json))
    return false;

  //Se tudo der certo retorna true
  return true;
}
