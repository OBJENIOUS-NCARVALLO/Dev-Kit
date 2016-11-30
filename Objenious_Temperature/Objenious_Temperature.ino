
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

float pad = 10000;      // Pad Resistance
float thermr = 10000;   // thermistor nominal resistance
byte msgg[3];           // Store the data to be uploaded to Objeniou's Network
                      


//Instance of  the class Arm
Arm Objenious; // Needed to make work the LoRaWAN module


// ---------------------------------------------------------------------
// Config
// ---------------------------------------------------------------------
void setup()
{     
    // set the data rate for the SoftwareSerial port (Debug mode only)
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
    delay(8000); // delay needed for the module to be ready to initialize.
    
    //Init of the LoRaWAN module - Red light if error, Green light if Ok 
    if(Objenious.Init(&Serial) != ARM_ERR_NONE)
    {
        digitalWrite(LED_ERROR, HIGH);
        mySerial.println("Network Error"); // Debug
    }
    else
    {
        digitalWrite(LED_OK, HIGH);
        mySerial.println("Connected to Objenious"); // Debug
    }

    // Configuration of the LoRaWAN module
    Objenious.SetMode(ARM_MODE_LORAWAN);

    Objenious.LwEnableRxWindows(true);
    Objenious.LwEnableTxAdaptiveSpeed(true);
    Objenious.LwEnableDutyCycle(true);
    Objenious.LwEnableTxAdaptiveChannel(true);
    Objenious.LwEnableRx2Adaptive(true);
    
    Objenious.LwEnableOtaa(true);
    
    //Apply the configuration to the module.
    Objenious.UpdateConfig();
    
    delay(8000); // delay needed for the module to connect to Objenious

    // If the module connected succesfully then Green LED will be ON
    // If the module couldn't connect then Red LED will be ON
    // We turn the LED OFF after 8 seconds
    digitalWrite(LED_OK, LOW);
    digitalWrite(LED_ERROR, LOW);
    
// ---------------------------------------------------------------------
// Here starts your code :D
// ---------------------------------------------------------------------

    msgg[0]=1; // This byte will indicate to Objeniou's platform what kind
               // of sketch we are using anf hence how to decode the data:
               //   - 1 = Temperature data
               //   - 2 = Push button data
               //   - 3 = Window/Door open data
    
}


// ---------------------------------------------------------------------
// How the code works:
// The sensor is read by the "Thermistor" function and then is multiplied
// by 100 in order to avoid decimal number.
// The mySerial function will print the information on the virtual Serial
// so we can debug.
// The temperature data is an Int, hence 2bytes of data. This data is stored
// in the "msgg" buffer before being uploaded to Objeniou's platform. To do 
// that we need to copy byte by byte. Example:
// int temp = 2348 (23,48°C * 100) // example valule...
//
// dec  ->   Byte 1     Byte 2
// 2348 -> 0000 1001  0010 1100 (binary representation of 2348. http://www.exploringbinary.com/binary-converter/)
//
// Then we store the fisrt Byte in msgg[1] and the sencond byte in msgg[2]
// Objenious.Send will uoload the data to our LoRaNetwork.
// ---------------------------------------------------------------------

void loop()
{
  int temp;
  temp=Thermistor(analogRead(ThermistorPIN))*100;    // read ADC and convert it to Celsius
  mySerial.print("Celsius: "); 
  mySerial.println(temp/100);                     // display Celsius
  msgg[1] = ((byte) (temp>>8));                     // https://www.arduino.cc/en/Reference/Bitshift
  msgg[2] = (byte) temp;
  Objenious.Send(msgg, sizeof(msgg));               // Send the temp to Objenious network        
  Blink(LED_TX,50);                                 // After sending the data we blink the BLUE led for 50 milliseconds
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
