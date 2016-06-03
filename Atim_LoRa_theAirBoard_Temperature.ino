
// ---------------------------------------------------------------------
// Include
// ---------------------------------------------------------------------
#include <arm.h> //ATIM library for LoRaWAN connection
#include <SoftwareSerial.h> // More information at https://www.arduino.cc/en/Reference/SoftwareSerial
#include <math.h> // For temperature calculation


// We define the pins for the software Serial that will allows us to
// debug our code.
SoftwareSerial mySerial(10, 11); // Pin 10 will work as RX and Pin 11 as TX


// ---------------------------------------------------------------------
// Define
// ---------------------------------------------------------------------
// The 3 colours LED on the Airboard is driven by 3 different digital pins:
// PIN 5 is Green colour, PIN 9 is Red colour, PIN 6 is Blue color
#define LED_OK 5 
#define LED_ERROR 9
#define LED_TX 6
#define ThermistorPIN 5     // Analog Pin 5 for Temperature readings


// ---------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------
volatile long count = 0; // count number of sent messages
boolean state = false; // True each time you will press the button, False otherwise
String buf = "Temp = "; // LoRaWAN message that will be sent to Objenious
                      
float pad = 10000;      // Pad Resistance
float thermr = 10000;   // thermistor nominal resistance



//Instance of  the class Arm
Arm myArm; // Needed to make work the LoRaWAN module


// ---------------------------------------------------------------------
// Implemented functions
// ---------------------------------------------------------------------
void setup()
{     
    // set the data rate for the SoftwareSerial port
    mySerial.begin(9600); 
    mySerial.println("Software serial test OK!"); 
    // If you see the message on your serial monitor it is working!

  
    // We have to configure the digital pins as output in order to use the LED
    pinMode(LED_ERROR, OUTPUT); // RED color
    pinMode(LED_TX, OUTPUT); // BLUE color   
    pinMode(LED_OK, OUTPUT); // GREEN color 
    
    // Then we ensure the LEDs are OFF
    digitalWrite(LED_ERROR, LOW);
    digitalWrite(LED_TX, LOW);
    digitalWrite(LED_OK, LOW);


// ---------------------------------------------------------------------
// LoRaWAN module Init and configuration
// ---------------------------------------------------------------------
    
    //Init of the LoRaWAN module - Red light if error, Green light if Ok 
    if(myArm.Init(&Serial) != ARM_ERR_NONE)
    {
        digitalWrite(LED_ERROR, HIGH);
        mySerial.println("Network Error");
    }
    else
    {
        digitalWrite(LED_OK, HIGH);
        mySerial.println("Connected to Objenious");
    }

    // Configuration of the LoRaWAN module
    myArm.SetMode(ARM_MODE_LORAWAN);

    myArm.LwEnableRxWindows(true);
    myArm.LwEnableTxAdaptiveSpeed(true);
    myArm.LwEnableDutyCycle(false);
    myArm.LwEnableTxAdaptiveChannel(true);
    myArm.LwEnableRx2Adaptive(true);
    
    myArm.LwEnableOtaa(true);
    
    //Apply the configuration to the module.
    myArm.UpdateConfig();
    
    delay(8000); // delay needed for the module to connect to Objenious

    // If the module connected succesfully then Green LED will be ON
    // If the module couldn't connect then Red LED will be ON
    // We then turn them both OFF after the 8 seconds
    digitalWrite(LED_OK, LOW);
    digitalWrite(LED_ERROR, LOW);
    
// ---------------------------------------------------------------------
// Here starts your code :D
// ---------------------------------------------------------------------

    // You can change or delete this delay
    delay(2000);
    
    Serial.print("Hello Objenious"); // message sent to the Objenious Network
    mySerial.println("First TX sent"); // Serial used for debug
    Blink(LED_TX,50); // After sending the data we blink the BLUE led for 50 milliseconds

}


// ---------------------------------------------------------------------
// How the code works:
// 
// ---------------------------------------------------------------------

void loop()
{
    
  float temp;
  temp=Thermistor(analogRead(ThermistorPIN));       // read ADC and  convert it to Celsius
  mySerial.print("Celsius: "); 
  mySerial.println(temp,1);                         // display Celsius
  Serial.print(buf + temp);                         // Send the temp to Objenious network        
  Blink(LED_TX,50);                     // After sending the data we blink the BLUE led for 50 milliseconds
  delay(20000);                                     // Send the temperature every 20 seconds
    
}


// ---------------------------------------------------------------------
// This function calculates the temperature from the ADC read
// ---------------------------------------------------------------------

float Thermistor(int RawADC) {
  long Resistance;  
  float Temp;  // Dual-Purpose variable to save space.

  Resistance=pad*((1024.0 / RawADC) - 1); 
  Temp = log(Resistance); // Saving the Log(resistance) so we can use it later
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius                      

  return Temp;                                      // Return the Temperature
}


// ---------------------------------------------------------------------
// This function makes blink the LED
// ---------------------------------------------------------------------

void Blink(byte PIN, int DELAY_MS) 
{ 
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
