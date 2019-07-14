//LIBRERIAS

#include <SPI.h>
#include <Ethernet.h>
#include <MFRC522.h>
#define RST_PIN	49   //Pin 9 para el reset del RC522
#define SS_PIN	53   //Pin 10 para el SS (SDA) del RC522
//CONSTRUCTORES

MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522

//DEFINICION DE PINES
int rojo=7;
int cuartoLUZ=6;
int camaLUZ=5;

//DEFINICION DE VARIABLES
byte RFID[15];
char RFIDpalabra[32]="";
bool flag=false;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // de:ad:be:ef:fe:ed
 
IPAddress ip(192,168,1,101); //IP address for your arduino.
IPAddress dnServer(192, 168, 1, 1);
// the router's gateway address:
IPAddress gateway(192, 168, 1, 1);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

char server[] = "192.168.1.15"; //IP address of your computer.
String arduinoNumber="1";
String RFIDenviar;

EthernetClient client;

//SUBFUNCIONES 

void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

void luces(bool encender){
if (encender==true){
    digitalWrite(cuartoLUZ,LOW);
    digitalWrite(camaLUZ,LOW);
}else{
    digitalWrite(cuartoLUZ,HIGH);
    digitalWrite(camaLUZ,HIGH);
}
}

void datoROJO(){
      luces(true);
      if (client.connect(server, 80)) 
      {
      Serial.println("rojo");
      client.print("GET /api/emergency/request/"); //GET request to write data to the database.
      client.print(arduinoNumber);
      client.println(" HTTP/1.1"); 
      client.println("Host: 192.168.1.15"); 
      client.println("Connection: close"); 
      client.println(); 
      client.println(); 
      client.stop();
      }
      else
      {
        Serial.println("Connection failed rojo");
      }
}

void datoRFID(){
      if (client.connect(server, 80)) 
      {
      Serial.println("rfid");
      client.print("GET /api/emergency/success/"); //GET request to write data to the database.
      client.print(arduinoNumber);
      client.print("/");
      client.print(RFIDenviar);
      Serial.println(RFIDenviar);
      client.println(" HTTP/1.1"); 
      client.println("Host: 192.168.1.15"); 
      client.println("Connection: close"); 
      client.println(); 
      client.println(); 
      client.stop();
      flag=false;
      luces(false);
      }
      else
      {
        Serial.println("Connection failed RFID");
        flag=true;
        luces(true);
      }
}

void leerRFID(bool flag){
    	// Revisamos si hay nuevas tarjetas  presentes
    if (flag==true){
	if ( mfrc522.PICC_IsNewCardPresent()) 
        { 
  		//Seleccionamos una tarjeta 
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  Serial.print("Card UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);
                          RFID[i]=mfrc522.uid.uidByte[i];   
                  } 
                  Serial.println();
                  // Terminamos la lectura de la tarjeta  actual
                  mfrc522.PICC_HaltA();         
            }
            array_to_string(RFID,15,RFIDpalabra);
            String conversor(RFIDpalabra);
            RFIDenviar=conversor.substring(0,8);
            Serial.print(RFIDenviar);
            datoRFID();
	}
    }
}

//FUNCION DE CONFIGURACION
void setup(){
    pinMode(rojo,INPUT);
    pinMode(camaLUZ,OUTPUT);
    pinMode(cuartoLUZ,OUTPUT);
    Serial.begin(9600);
    Ethernet.begin(mac, ip, dnServer, gateway, subnet);
    delay(5000); //Wait for ethernet to connect.
    digitalWrite(camaLUZ,HIGH);
    digitalWrite(cuartoLUZ,HIGH);
    SPI.begin();        //Iniciamos el Bus SPI
	mfrc522.PCD_Init(); // Iniciamos  el MFRC522
    /*
    luces(true);
    delay(2000);
    luces(false);
    */
    Serial.println("Bienvenidos al sistema LLAMADA PACIENTE-ENFERMERIA");
}

//FUNCION PRINCIPAL
void loop (){
    bool boton;
    boton=digitalRead(rojo);
    delay(200);
    if(boton==true){
       datoROJO();
       flag=true;
    }
    leerRFID(flag);
   // Serial.println(flag);
}
