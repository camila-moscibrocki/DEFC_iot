#include <Arduino.h>
#include <SimpleDHT.h>
#include "displayESP32LoRa.h" //Arquivo display lora
#include "MQTT_Client.h" //Arquivo mqtt

float temperature; //Temperatura DHT
float humidity; //Umidade DHT
float voltage; //Tensão célula
  
const int pinDHT = 22; //Pino DHT número 22
SimpleDHT11 dht11(pinDHT); //valores do sensor DHT

void setup() 
{
  //debug, serial com 115200 bps
  Serial.begin(115200);

  //Inicializa display lora
  if(!loraDisplayConfig())
    ESP.restart();

  //Exibe mensagem do mqtt no display
  showDisplay(0, "Setting up mqtt...", true);
  Serial.println("Setting up mqtt...");

  //Inicializa mqtt
  if(!mqttInit())
  {        
    delay(3000);
    showDisplay(0, "Failed!", false);
    Serial.println("Failed!");
    ESP.restart();
  }
  showDisplay(0, "OK", false);
  Serial.println("OK");
}

//Obtém temperatura e umidade do sensor
bool getClimate()
{  
  int err = SimpleDHTErrSuccess;

  //'temperature' e 'humidity na função chamada 'read2
  if ((err = dht11.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) 
  {
    Serial.print("Read dht11 failed, err=");
    Serial.println(err);

    //Zera valores
    temperature = humidity = 0;
    return false;
  }

  return true;
}

//valores a flag 'success'=treu
//flag 'success'=traço, traço '-'
void showClimate(bool success)
{
  if(success)
  {
    showDisplay(0, "Temperature: "+String(temperature)+" °C", true);
    showDisplay(1, "Humidity: "+String(humidity)+" %", false);
    showDisplay(2, "Voltage: "+String(voltage)+" V", false);
    Serial.println("Temperature: "+String(temperature)+" °C");
    Serial.println("Humidity: "+String(humidity)+" %");
    Serial.println("Voltage: "+String(voltage)+" V");
  }
  else
  {
    showDisplay(0, "Temperature: -", true);
    showDisplay(1, "Humidity: -", false);
    showDisplay(2, "Voltage: -", false);
    Serial.println("Temperature: -");
    Serial.println("Humidity: -");
    Serial.println("Voltage: -");
  }
}

void loop() 
{
  voltage = (float)analogRead(36) / 4096 * 3.9;
  Serial.print(voltage,1);
  Serial.println("v");
  delay(200);
  
  //reconectar
  if(!client.connected())
    reconnect();
  
  //Lê a temperatura e umidade no display passando uma flag 
  if(getClimate())
    showClimate(true);
  else
    showClimate(false);

  //2.5s antes de exibir o status do envio para dar efeito de pisca no display
  delay(2500);  
  if(sendValues(temperature, humidity, voltage))
  {      
    Serial.println("Successfully sent data");
    showDisplay(4,"Successfully sent data", false);
  }
  else
  {      
    Serial.println("Failed to send sensor data");
    showDisplay(4,"Failed to send sensor data", false);
  }    
    
  //2.5s para dar tempo de ler as mensagens acima
  delay(2500);    
}
