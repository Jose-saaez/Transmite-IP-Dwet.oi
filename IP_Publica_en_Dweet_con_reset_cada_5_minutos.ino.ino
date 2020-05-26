//Transmite la IP publica a http://dweet.io/get/latest/dweet/for/Nombre_De_Dato_A_Enviar
// En este caso la IP publica 
//Utiliza WiFiManager para transportarlo a distintos lugares y configurar El punto de acceso dinamicamente
//posee la funcionalidad ESP.restart() en caso de perder la conectividad wifi de la red local, se autoreconecta con un timeout de 180 segundos
// tambien se resetea si no obtiene la IP del servicio api.ipify.org

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager PROBADO CON VERSION 14.0

String dweetThingName("Nombre_De_Dato_A_Enviar");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  } 

  //if you get here you have connected to the WiFi
  Serial.println("connectado a la red local...yeey :)");
  Serial.println("IP local: ");
  Serial.println(WiFi.localIP());
 
}

//*************************************
//  FUNCION getIp()
//*************************************

String getIp()   // se conecta a la api de ipify.org para obtener la IP publica --->
{
  WiFiClient client;
  if (client.connect("api.ipify.org", 80)) 
  {
      Serial.println("connected");
      client.println("GET / HTTP/1.0");
      client.println("Host: api.ipify.org");
      client.println();
  } else {
      Serial.println("Conexion a ipify.org fallida, reseteando...");
      delay(3000);
      ESP.restart();
      return String();
  }
  delay(5000);
  String line;
  while(client.available())
  {
    line = client.readStringUntil('\n');
    Serial.println(line);
  }
  return line;
}

//**************************************************
//  FUNCION dweet( String ip) devuelve TRUE o FALSE
//**************************************************

bool dweet( String ip)  // esto es como escribir un tweet para iothing
{

  WiFiClient client;
  if (!client.connect("dweet.io", 80)) 
  {
    Serial.println("dweet fallido");
    return false;
  }  
  else 
  {
    Serial.println("Transmitiendo ip address... " + dweetThingName +" [" + ip + "]" );
    
    client.println( String("GET /dweet/for/" + dweetThingName + "?IPaddress=") + ip + " HTTP/1.1");
    client.println( "Host: dweet.io" );
    client.println( "Connection: close");
    client.println("");
    client.println("");
    
    delay(1000);
  
    // Read all the lines of the reply from server and print them to Serial
    while(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    };
    return true;
  }; 
}

//**************************************************
//  LOP PRINCIPAL *******************>>>>>>>>>>>
//**************************************************


void loop() {
  String ip = getIp();
  
  if ( ip != String() )
    dweet( ip ) ;
  // transmite la IP cada 5 minutos
  Serial.println("Retardo de 5 minutos para Re-transmitir IP");
  delay(300000);
}
