/*----------------------------------------------------------


  ----------------------------------------------------------*/
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include "SparkFunCCS811.h"



#include "U8glib.h"

#include <MHZ19_uart.h>

ClosedCube_HDC1080 hdc1080;

//#define CCS811_ADDR 0x5B //Default I2C Address
#define CCS811_ADDR 0x5A //Alternate I2C Address

CCS811 mySensor(CCS811_ADDR);

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 


const int rx_pin = 9;	//SoftSerial rx pin no
const int tx_pin = 10;	//SoftSerial tx pin no

int co2ppm, voc;
float  temp, rh;


MHZ19_uart mhz19;



void draw(void) {

  u8g.setFont(u8g_font_fub25n); //  courB24n // 707bytes
  u8g.setPrintPos(0, 26); 
  u8g.print(co2ppm);
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr(88,10, "ppm");
  u8g.drawStr(88,24, "CO2");
  
  u8g.drawHLine(0,29, 127);

  u8g.setPrintPos(0, 44); 
  u8g.print("T:");
  u8g.print(temp,1);
  
 // u8g.setPrintPos(0, 42); 
  u8g.print("C RH%:");
  u8g.print(rh,1);
  u8g.setPrintPos(0, 56); 
  u8g.print("TVOC:");
  u8g.print(voc);
} //draw


void setup(void) {

  Serial.begin(9600);

  hdc1080.begin(0x40);

  CCS811Core::status returnCode = mySensor.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println("CSS811.begin() returned with an error.");
  //  while (1); //Hang if there was a problem.
  }

  Serial.print("Manufacturer 1080 ID=0x");
  Serial.print(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("   Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
	
  mhz19.begin(rx_pin, tx_pin);
  Serial.print("MH-Z19 begin!");
//  mhz19.setAutoCalibration(false);
 // Serial.print("MH-Z19 off autocalibration");

  // flip screen, if required
  // u8g.setRot180();
}

void loop(void) {
 
 
  co2ppm = mhz19.getPPM();
// temp = mhz19.getTemperature();

  temp = hdc1080.readTemperature();
  rh   = hdc1080.readHumidity();
  if (mySensor.dataAvailable())
  {
    //If so, have the sensor read and calculate the results.
    //Get them later
    mySensor.readAlgorithmResults();
    voc = mySensor.getTVOC();
    Serial.print("CO2[");
    //Returns calculated CO2 reading
    Serial.print(mySensor.getCO2());
    Serial.print("] tVOC[");
    //Returns calculated TVOC reading
    Serial.print(voc);
    Serial.print("] millis[");
    //Simply the time since program start
    Serial.print(millis());
    Serial.print("]");
    Serial.println();
  }


  Serial.print("T=");
  Serial.print(temp);
  Serial.print("C, RH=");
  Serial.print(rh);
  Serial.println("%");

  Serial.print("co2: "); Serial.print(co2ppm);
  Serial.print("   temp: "); Serial.print(temp);
  Serial.print("   MH-Z19 status:");Serial.println(mhz19.getStatus());


  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  
  
  delay(3000);
}

