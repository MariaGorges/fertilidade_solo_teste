/************************************************************/
/* Estação Meteorológica e Agrícola com Módulo ESP-01       */
/* Sensores:                                                */
/* - DHT11 (Temperatura e Umidade do Ar) - GPIO2            */
/* - HL69/YI69 (Umidade do Solo) - GPIO2 (analógico)        */
/* - DS18B20 (Temperatura do Solo) - GPIO0                  */
/************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

/* Definições dos sensores */
#define Modelo DHT11
#define Pino_DHT 2      // GPIO2 para DHT11
#define Pino_Solo A0    // GPIO2 (analógico) para HL69/YI69
#define Pino_DS18B20 0  // GPIO0 para DS18B20

/* Configuração WiFi */
const char *nome_da_rede = "fertilidade_solo";
const char *senha = "12345678";

/* Objetos */
ESP8266WebServer servidor(80);
DHT dht(Pino_DHT, Modelo);
OneWire oneWire(Pino_DS18B20);
DallasTemperature sensors(&oneWire);

/* Variáveis para armazenar leituras */
float TempSolo;
int UmidSolo;
String StatusSolo;

void setup() {
  /* Inicializa sensores */
  dht.begin();
  sensors.begin();
  pinMode(Pino_Solo, INPUT);

  /* Configura WiFi */
  WiFi.softAP(nome_da_rede, senha);

  /* Configura servidor web */
  servidor.on("/", Pagina_Requisitada);
  servidor.onNotFound(Pagina_Inexistente);
  servidor.begin();
}

void loop() {

  sensors.requestTemperatures();
  TempSolo = sensors.getTempCByIndex(0);


  int leituraSolo = analogRead(Pino_Solo);
  UmidSolo = map(leituraSolo, 1023, 0, 0, 100);  // Converte para porcentagem

  if (UmidSolo <= 20) {
    StatusSolo = "SOLO SECO";
  } else {
    StatusSolo = "SOLO MOLHADO";
  }

  /* Trata requisições do servidor */
  servidor.handleClient();
  delay(100);  // Pequeno delay para estabilidade
}

void Pagina_Requisitada() {
  servidor.send(200, "text/html", Monta_HTML(TempSolo, StatusSolo));
}

void Pagina_Inexistente() {
  servidor.send(404, "text/html",
                "<H1><!DOCTYPE html>"
                "<html>P&aacute;gina n&atilde;o encontrada"
                "</H1></html>");
}

String Monta_HTML(float tempSolo, String StatusSolo) {
  String ptr = "";
  ptr += "<!DOCTYPE html><html>\n";
  ptr += "<head>\n";
  ptr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>FERTILIDADE DO SOLO</title>\n";
  ptr += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css\">\n";
  ptr += "<style>\n";
  ptr += "html, body { height: 100%; margin: 0; padding: 0; }\n";
  ptr += "body { font-family: 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif; background: linear-gradient(135deg, #f5f7fa 0%, #e4f0f5 100%); display: flex; flex-direction: column; }\n";
  ptr += ".container { flex: 1; width: 95%; max-width: 100%; margin: 0 auto; padding: 10px; display: flex; flex-direction: column; justify-content: center; }\n";
  ptr += ".header { text-align: center; margin-bottom: 5vh; padding-top: 3vh; }\n";
  ptr += ".header h1 { color: #2c3e50; font-size: 9vw; margin-bottom: 1vh; font-weight: 600; }\n";
  ptr += ".header h2 { color: #7f8c8d; font-size: 5vw; font-weight: 400; margin-top: 0; }\n";
  ptr += ".card { background: white; border-radius: 15px; padding: 6vw; margin-bottom: 4vh; box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1); flex: 1; display: flex; flex-direction: column; justify-content: center; }\n";
  ptr += ".card-title { color: #3498db; font-size: 6vw; margin-top: 0; margin-bottom: 4vh; display: flex; align-items: center; justify-content: center; }\n";
  ptr += ".card-title i { margin-right: 3vw; font-size: 7vw; }\n";
  ptr += ".sensor-data { display: flex; justify-content: space-around; text-align: center; margin: 2vh 0; }\n";
  ptr += ".sensor-item { padding: 2vw; flex: 1; }\n";
  ptr += ".sensor-value { font-size: 8vw; font-weight: 600; color: #2c3e50; margin: 1vh 0; display: flex; align-items: center; justify-content: center; }\n";
  ptr += ".sensor-value i { margin-right: 3vw; font-size: 8vw; }\n";
  ptr += ".sensor-label { font-size: 4.5vw; color: #7f8c8d; margin-top: 1vh; }\n";
  ptr += ".status { margin-top: 3vh; padding: 3vh; border-radius: 10px; font-weight: 500; text-align: center; font-size: 5vw; }\n";
  ptr += ".status-dry { background-color: #ffebee; color: #d32f2f; border: 2px solid #d32f2f; }\n";  // Vermelho para solo seco
  ptr += ".status-wet { background-color: #e8f5e9; color: #2e7d32; border: 2px solid #2e7d32; }\n";  // Verde para solo úmido
  ptr += "</style>\n";
  ptr += "<script>\n";
  ptr += "setInterval(loadDoc, 2000);\n";
  ptr += "function loadDoc() {\n";
  ptr += "  var xhttp = new XMLHttpRequest();\n";
  ptr += "  xhttp.onreadystatechange = function() {\n";
  ptr += "    if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "      document.getElementById(\"webpage\").innerHTML = this.responseText;\n";
  ptr += "    }\n";
  ptr += "  };\n";
  ptr += "  xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "  xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<div class=\"container\">\n";
  ptr += "<div class=\"header\">\n";
  ptr += "<h1><i class=\"fas fa-seedling\" style=\"color: #27ae60;\"></i> FERTILIDADE DO SOLO</h1>\n";
  ptr += "<h2>Monitoramento do Solo</h2>\n";
  ptr += "</div>\n";
  
  // Card de Dados do Solo
  ptr += "<div class=\"card\">\n";
  ptr += "<h3 class=\"card-title\"><i class=\"fas fa-tree\" style=\"color: #8e44ad;\"></i> DADOS DO SOLO</h3>\n";
  ptr += "<div class=\"sensor-data\">\n";
  
  // Temperatura
  ptr += "<div class=\"sensor-item\">\n";
  ptr += "<div class=\"sensor-value\"><i class=\"fas fa-temperature-high\" style=\"color: #e74c3c;\"></i> ";
  ptr += (int)tempSolo;
  ptr += "°</div>\n";
  ptr += "<div class=\"sensor-label\">TEMPERATURA</div>\n";
  ptr += "</div>\n";
  
  // Umidade
  ptr += "<div class=\"sensor-item\">\n";
  ptr += "<div class=\"sensor-value\"><i class=\"fas fa-tint\" style=\"color: #3498db;\"></i> ";
  String statusUpper = StatusSolo;
  statusUpper.toUpperCase();
  ptr += statusUpper;
  ptr += "</div>\n";
  ptr += "<div class=\"sensor-label\">UMIDADE</div>\n";
  ptr += "</div>\n";
  
  ptr += "</div>\n";
  
  // Status - CORREÇÃO APLICADA AQUI
  ptr += "<div class=\"status ";
  if (StatusSolo.indexOf("Seco") != -1 || StatusSolo.indexOf("seco") != -1) {  // Verifica em maiúsculo e minúsculo
    ptr += "status-dry\"><i class=\"fas fa-exclamation-triangle\"></i> SOLO SECO - IRRIGAR";
  } else {
    ptr += "status-wet\"><i class=\"fas fa-check-circle\"></i> SOLO ÚMIDO - OK";
  }
  ptr += "</div>\n";
  
  ptr += "</div>\n"; // Fecha card
  ptr += "</div>\n"; // Fecha container
  ptr += "</div>\n"; // Fecha webpage
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}