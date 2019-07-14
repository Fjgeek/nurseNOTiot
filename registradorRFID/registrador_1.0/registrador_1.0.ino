//LIBRERIAS
/*
Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13
*/
#include <SPI.h> //protocolo SPI
#include <Ethernet.h>
#include <MFRC522.h>
#include  <Wire.h> //protocolo i2c 
#include  <LiquidCrystal_I2C.h>
#define RST_PIN	49   //Pin 9 para el reset del RC522
#define SS_PIN	53   //Pin 7 para el SS (SDA) del RC522 siendo el ethernet usando 10
//CONSTRUCTORES
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522

//DEFINICION DE VARIABLES
byte RFID[15];
char RFIDpalabra[32]="";
bool flag=true;

byte mac[] = { 0xCC, 0xCC, 0xBB, 0xAA, 0xCC, 0xAA };//cc:cc:bb:aa:cc:aa
 
IPAddress ip(192,168,1,102); //IP address for your arduino 192.168.1.102
IPAddress dnServer(192, 168, 1, 1);
// the router's gateway address:
IPAddress gateway(192, 168, 1, 1);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

char server[] = "192.168.1.15"; //IP address of your computer.
String RFIDenviar;

EthernetClient client;

//SUBFUNCIONES 


//convierte un array de char a strings 
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

void datoRFID(){
      if (client.connect(server, 80)) 
      {
      client.print("GET /api/read/"); //GET request to write data to the database.
      client.print(RFIDenviar);
      client.println(" HTTP/1.1"); 
      client.println("Host: 192.168.1.15"); 
      client.println("Connection: close"); 
      client.println(); 
      client.println(); 
      client.stop();
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("REGISTRO CORRECTO");
      delay(5000);
      lcd.clear();
      lcd.home();
      lcd.print("INGRESE");
      lcd.setCursor(0,1);
      lcd.print("NUEVO");
      }
      else
      {
        Serial.println("Connection failed RFID");
        lcd.clear();
        lcd.home();
        lcd.print("FALLO REGISTRO");
      }
}

void leerRFID(){
    	// Revisamos si hay nuevas tarjetas  presentes
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
            Serial.println(RFIDenviar);
            lcd.clear();
            lcd.home();
            lcd.print(RFIDenviar);
            delay(1000);
            datoRFID();
	}
}

//FUNCION DE CONFIGURACION
void setup(){
    Serial.begin(9600);
    Ethernet.begin(mac, ip, dnServer, gateway, subnet);
    delay(5000); //Wait for ethernet to connect.
    lcd.begin(16, 2);
    SPI.begin();        //Iniciamos el Bus SPI
	mfrc522.PCD_Init(); // Iniciamos  el MFRC522
    Serial.println("REGISTRO DE NUEVAS TARJETAS RFID");
    lcd.home();
    lcd.print("REGISTRADOR");
    lcd.setCursor(0,1);//(columna,fila)
    lcd.print("RFID NUEVOS");
}

//FUNCION PRINCIPAL
void loop (){
    leerRFID();
}
