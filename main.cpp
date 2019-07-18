#include <Arduino.h>
#include <WiFi.h>
#include <ArtnetWifi.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>
#include <NeoPixelBrightnessBus.h>


WiFiMulti multiple_wifi;
ArtnetWifi artnet;

int dataBuf[4][12];

const int STARTADDRESS = 0;
const int UNIVERSE = 0;

// this example assumes 4 pixels, making it smaller will cause a failure
const uint16_t AMOUNT_OF_PIXELS = 10;

// make sure to set this to the correct pin, ignored for Esp8266
const uint8_t PIN_DATA_OUT = 12; 
  
// four element LED (red, green, blue, white)
NeoPixelBrightnessBus<NeoGrbwFeature, Neo800KbpsMethod> led_strip(AMOUNT_OF_PIXELS, PIN_DATA_OUT);

boolean connect_to_wifi( void )
{
  boolean state = true;
  int i = 0;

  Serial.println( "" );
  Serial.println( "Connecting to WiFi" );
  
  // Wait for connection
  Serial.print( "Connecting" );
  while( multiple_wifi.run() != WL_CONNECTED ) {
    delay( 500 );
    Serial.print( "." );
    if( i > 40 ){
      state = false;
      break;
    }
    i++;
  }
  if( state ) {
    Serial.println( "" );
    Serial.print( "Connected " );
    Serial.print( "IP address: " );
    Serial.println( WiFi.localIP() );
  } else {
    Serial.println( "" );
    Serial.println( "Connection failed." );
  }
  return state;
}

void testLedStrip()
{
  led_strip.SetPixelColor( 0, RgbwColor( 255, 0, 0, 0 ) );
  led_strip.SetPixelColor( 1, RgbwColor( 0, 255, 0, 0 ) );
  led_strip.SetPixelColor( 2, RgbwColor( 0, 0, 255, 0 ) );
  led_strip.SetPixelColor( 4, RgbwColor( 255, 255, 0, 0 ) );
  led_strip.SetPixelColor( 5, RgbwColor( 0, 255, 255, 0 ) );
  led_strip.SetPixelColor( 6, RgbwColor( 255, 0, 255, 0 ) );
  led_strip.SetPixelColor( 3, RgbwColor( 0, 0, 0, 255 ) );
  led_strip.SetPixelColor( 7, RgbwColor( 255, 255, 255, 0 ) );
  led_strip.SetPixelColor( 8, RgbwColor( 1, 1, 1, 1) );
  led_strip.SetPixelColor( 9, RgbwColor( 5, 5, 5, 5 ) );
}
 

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  uint8_t h = data[ STARTADDRESS + 4 ];
  if( true ) {
    testLedStrip();
    led_strip.SetBrightness( h );
    led_strip.Show();
  }
  if( false ) {
    // we use 5 bytes to encode stuff
    uint8_t r = data[ STARTADDRESS + 0 ];
    uint8_t g = data[ STARTADDRESS + 1 ];
    uint8_t b = data[ STARTADDRESS + 2 ];
    uint8_t w = data[ STARTADDRESS + 3 ];
    
    for( int i = 0; i < AMOUNT_OF_PIXELS; i++ ) {
      
      if( i % 2 == 0 ) {
        led_strip.SetPixelColor( i, RgbwColor( r, g, b, w ) );
        led_strip.SetPixelColor( i+1, RgbwColor( b, g, r, w ) );
        led_strip.SetPixelColor( i+2, RgbwColor( g, r, b, w ) );
      } else {
        led_strip.SetPixelColor( i-1, RgbwColor( r, g, b, w ) );
        led_strip.SetPixelColor( i, RgbwColor( b, g, r, w ) );
        led_strip.SetPixelColor( i-2, RgbwColor( g, r, b, w ) );
      }

    }
    led_strip.SetBrightness( h );
    // the following line demonstrates rgbw color support
    // if the NeoPixels are rgbw types the following line will compile
    // if the NeoPixels are anything else, the following line will give an error
    // led_strip.SetPixelColor(3, RgbwColor(colorSaturation));
    led_strip.Show();
  }
}

/**
 * SETUP
 **/
void setup()
{
  // Init serial output
  Serial.begin( 9600 );

  // Add all Wifi access points which the ESP32 should connect to

  // SSID, PASSWORD (for hackerspace wifi) 
  const char* ssid = "Hackerspace";
  const char* password = "********";

  multiple_wifi.addAP( ssid, password );
 
  Serial.println( "Hello." );

  // Connect to WiFi networks
  boolean was_success = connect_to_wifi();

  delay( 1000 );

  if( was_success )
  {
    Serial.println( "WIFI: SUCCESS" );
  } else {
    Serial.println( "WIFI: FAIL" );
  }
  // Setup callback on artnet packet
  artnet.setArtDmxCallback( onDmxFrame );
  artnet.begin();
    
  led_strip.Begin();
  // the following line demonstrates rgbw color support
  // if the NeoPixels are rgbw types the following line will compile
  // if the NeoPixels are anything else, the following line will give an error
  // led_strip.SetPixelColor(3, RgbwColor(colorSaturation));  
    Serial.println( "LED: INITIATED" );
    testLedStrip();
  led_strip.Show();
}

/**
 * MAIN LOOP
 **/
void loop()
{
  artnet.read(); // Read incoming packets
  yield();
}
