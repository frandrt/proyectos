
#include <EthernetESP32.h>
#include <WebServer.h>
#include <Ticker.h>
#include "data.h"
#define VSPI 1
//VARIABLES PARA MANEJAR MODULO LAN DE COMUNICACION SPI W5500
const int VSPI_MISO = 13;
const int VSPI_MOSI = 11;
const int VSPI_SCLK = 12;
const int VSPI_SS = 10;
const int W5500_INT = 4;
//DECLARACION DE PIN DE ENTRADA 
const int pinAC1 = 5;
const int pinAC2 = 6;
const int pinDCR = 7;
const int pinDCB = 8;
const int pinT = 9;
//DECLARACION DE VARIABLES PARA MEDIR 
float AC_1=210;
float AC_2;
float DC_R=48;
float DC_B=50;
float T=18;
//BANDERAS PARA DE INDENFICACION DE ALARMA
bool bandera_AC = false; 
bool bandera_DCR = false; 
bool bandera_DCB = false; 
bool bandera_T = false; 

//CONFIGURACION IP ESTATICA
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
W5500Driver driver(VSPI_SS,W5500_INT,-1);
SPIClass SPI1(VSPI);
IPAddress ip(10, 10, 10, 210);


String nuevaPagina = Pagina;
WebServer server(80);  //puerto de servidor
void mensajeBase() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", Pagina);
}

void mensajeError() {
  String mensaje = "<h1>404</h1>";
  mensaje += "Pagina No encontrada</br>";
  mensaje += "Intenta otra pagina</br>";
  server.send(404, "text/html", mensaje);
}

void setup() {
  Serial.begin(115200);
  
  // CONFIGURAMOS SPI PARA ETEHERNET MODULE W5500
 SPI1.begin(12, 13, 11);

  driver.setSPI(SPI1);
  driver.setSpiFreq(20);

  Ethernet.init(driver);
  Ethernet.begin(mac, ip);
  Serial.print("Assigned IP: ");
  Serial.println(Ethernet.localIP());



  // Start the server and define routes
  server.on("/", mensajeBase);
  server.on("/MEDICIONES", HTTP_GET, handle_mediciones);   //Ruta para enviar estado de sensores AC/DC RECTIFICADOR/DC BATERIAS/TEMPERATURA
  server.on("/ALARMAS", HTTP_GET, handle_alarmas_activas);
  server.onNotFound(mensajeError);


  server.begin();
  Serial.println("Ethernet iniciado");
//CONFIGURACION DE GPIO DE ENTRADAS ADC 
  pinMode(pinAC1, INPUT);
  pinMode(pinAC2, INPUT);
  pinMode(pinDCR, INPUT);
  pinMode(pinDCB, INPUT);
  pinMode(pinT, INPUT);
}
 /*AC_1 = analogRead(pinAC1); 
    DC_R = analogRead(pinDCR); 
    DC_B = analogRead(pinDCB); 
    T = analogRead(pinT);*/
void loop() {

  server.handleClient();
   
  verificar_Variable(AC_1, 210, 230,"Voltaje de red AC ","Vrms", bandera_AC);
  verificar_Variable(DC_R, 45, 51,"Voltaje de rectificador", "Vdc",bandera_DCR); 
  verificar_Variable(DC_B, 48, 56,"Voltaje de banco de baterias", "Vdc", bandera_DCB); 
  verificar_Variable(T, 11, 30, "Temperatura", "°C",bandera_T);
  
}

void verificar_Variable(float var, float limiteA, float limiteB, const char* tipo,const char* magnitud, bool bandera) {
  if (bandera == 0) {
    if (var < limiteA) {
      Serial.print("ALERTA: ");
      Serial.print(tipo);
      Serial.print(" menor que ");
      Serial.println(limiteA);
      Serial.print(magnitud);
      Serial.print("Fecha de activación: ");
      bandera = 1;
      // Agrega aquí el código para imprimir la fecha de activación
    } else if (var > limiteB) {
      Serial.print("ALERTA: ");
      Serial.print(tipo);
      Serial.print(" mayor que ");
      Serial.println(limiteB);
      Serial.print(magnitud);
      Serial.print("Fecha de activación: ");
      bandera = 1;
      // Agrega aquí el código para imprimir la fecha de activación
    }
  } else if (limiteA < var && var < limiteB) {
    Serial.print(tipo);
    Serial.println(" dentro de los límites.");
    bandera = 0;
  }
}
void handle_alarmas_activas(){
//funcion que envia las alarmas activas 
//descripcionalarma/fechay hora/tiempo desde que esta activa se activaliza
}

void handle_mediciones(){

 server.send(200, "text/plain", "AC_1=" + String(AC_1) + "&DC_R=" + String(DC_R) + "&DC_B=" + String(DC_B) + "&T=" + String(T));

}
