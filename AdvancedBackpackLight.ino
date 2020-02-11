#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
  public:

    // Member Variables:
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern

    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position

    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern

    void (*OnComplete)();  // Callback on completion of pattern

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
      : Adafruit_NeoPixel(pixels, pin, type)
    {
      OnComplete = callback;
    }

    // Update the pattern
    void Update()
    {
      if ((millis() - lastUpdate) > Interval) // time to update
      {
        lastUpdate = millis();
        switch (ActivePattern)
        {
          case RAINBOW_CYCLE:
            RainbowCycleUpdate();
            break;
          case THEATER_CHASE:
            TheaterChaseUpdate();
            break;
          case COLOR_WIPE:
            ColorWipeUpdate();
            break;
          case SCANNER:
            ScannerUpdate();
            break;
          case FADE:
            FadeUpdate();
            break;
          default:
            break;
        }
      }
    }

    // Increment the Index and reset at the end
    void Increment()
    {
      if (Direction == FORWARD)
      {
        Index++;
        if (Index >= TotalSteps)
        {
          Index = 0;
          if (OnComplete != NULL)
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
      else // Direction == REVERSE
      {
        --Index;
        if (Index <= 0)
        {
          Index = TotalSteps - 1;
          if (OnComplete != NULL)
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
    }

    // Reverse pattern direction
    void Reverse()
    {
      if (Direction == FORWARD)
      {
        Direction = REVERSE;
        Index = TotalSteps - 1;
      }
      else
      {
        Direction = FORWARD;
        Index = 0;
      }
    }

    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = RAINBOW_CYCLE;
      Interval = interval;
      TotalSteps = 255;
      Index = 0;
      Direction = dir;
    }

    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate()
    {
      for (int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
      }
      show();
      Increment();
    }

    // Initialize for a Theater Chase
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = THEATER_CHASE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
    }

    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
      for (int i = 0; i < numPixels(); i++)
      {
        if ((i + Index) % 3 == 0)
        {
          setPixelColor(i, Color1);
        }
        else
        {
          setPixelColor(i, Color2);
        }
      }
      show();
      Increment();
    }

    // Initialize for a ColorWipe
    void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = COLOR_WIPE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color;
      Index = 0;
      Direction = dir;
    }

    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    {
      setPixelColor(Index, Color1);
      show();
      Increment();
    }

    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint8_t interval)
    {
      ActivePattern = SCANNER;
      Interval = interval;
      TotalSteps = (numPixels() - 1) * 2;
      Color1 = color1;
      Index = 0;
    }

    // Update the Scanner Pattern
    void ScannerUpdate()
    {
      for (int i = 0; i < numPixels(); i++)
      {
        if (i == Index)  // Scan Pixel to the right
        {
          setPixelColor(i, Color1);
        }
        else if (i == TotalSteps - Index) // Scan Pixel to the left
        {
          setPixelColor(i, Color1);
        }
        else // Fading tail
        {
          setPixelColor(i, DimColor(getPixelColor(i)));
        }
      }
      show();
      Increment();
    }

    // Initialize for a Fade
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = FADE;
      Interval = interval;
      TotalSteps = steps;
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
    }

    // Update the Fade Pattern
    void FadeUpdate()
    {
      // Calculate linear interpolation between Color1 and Color2
      // Optimise order of operations to minimize truncation error
      uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
      uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
      uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

      ColorSet(Color(red, green, blue));
      show();
      Increment();
    }

    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
      // Shift R, G and B components one bit to the right
      uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
      return dimColor;
    }

    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
      for (int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, color);
      }
      show();
    }

    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
      return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
      return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
      return color & 0xFF;
    }

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
      WheelPos = 255 - WheelPos;
      if (WheelPos < 85)
      {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
      }
      else if (WheelPos < 170)
      {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
      else
      {
        WheelPos -= 170;
        return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
      }
    }
};


#include <bluefruit.h>

// OTA DFU service
BLEDfu bledfu;

// Uart over BLE service
BLEUart bleuart;

// Which pin on the Arduino is connected to the NeoPixels?
// Pin D9 is A2 on the Circuit Playground Bluefruit
#define NEO_PIN    9
#define NEO_BUILTIN 8

// How many NeoPixels are attached to the Arduino?
#define NEO_COUNT 150
#define NEO_COUNT_BUILTIN 10

void stripComplete();
void ringComplete();

unsigned int stripBrightness = 200;
unsigned int ringBrightness = 200;

// Declare our NeoPixel strip object:
NeoPatterns strip(NEO_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800, &stripComplete);
NeoPatterns ring(NEO_COUNT_BUILTIN, NEO_BUILTIN, NEO_GRB + NEO_KHZ800, &ringComplete);

//Adafruit_NeoPixel strip(NEO_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel ring(NEO_COUNT_BUILTIN, NEO_BUILTIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// Function prototypes for packetparser.cpp
uint8_t readPacket (BLEUart *ble_uart, uint16_t timeout);
float   parsefloat (uint8_t *buffer);
void    printHex   (const uint8_t * data, const uint32_t numBytes);

// Packet buffer
extern uint8_t packetbuffer[];

const int SLIDE_SWITCH = 7;
unsigned long lastTime = 0;

void setup(void)
{
  pinMode(SLIDE_SWITCH, INPUT);

  //Neopixel setup
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(stripBrightness); // Set BRIGHTNESS to about 1/5 (max = 255)
  //strip.fill(strip.Color(255, 0, 0));
  //strip.show();

  ring.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  ring.show();            // Turn OFF all pixels ASAP
  ring.setBrightness(ringBrightness); // Set BRIGHTNESS to about 1/5 (max = 255)
  //ring.fill(ring.Color(0, 255, 0));
  //ring.show();

  ring.Color1 = ring.Color(255, 0, 0);
  strip.Color1 = strip.Color(255, 0, 0);


  Serial.begin(115200);
  //while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println(F("Adafruit Bluefruit52 Controller App Example"));
  Serial.println(F("-------------------------------------------"));

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and start the BLE Uart service
  bleuart.begin();

  // Set up and start advertising
  startAdv();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include the BLE UART (AKA 'NUS') 128-bit UUID
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  //Serial.print("Slide Switch: ");
  //Serial.println(digitalRead(SLIDE_SWITCH));

  if (digitalRead(SLIDE_SWITCH) == 1) {
    lightSensBrightness();
  }

  //Serial.println("Update NeoPatterns objects");
  strip.Update();
  ring.Update();

  // Wait for new data to arrive
  uint8_t len = readPacket(&bleuart, 50);
  if (len == 0) return;

  // Got a packet!
  // printHex(packetbuffer, len);

  // Color
  if (packetbuffer[1] == 'C') {
    uint8_t red = packetbuffer[2];
    uint8_t green = packetbuffer[3];
    uint8_t blue = packetbuffer[4];
    Serial.print ("RGB #");
    if (red < 0x10) Serial.print("0");
    Serial.print(red, HEX);
    if (green < 0x10) Serial.print("0");
    Serial.print(green, HEX);
    if (blue < 0x10) Serial.print("0");
    Serial.println(blue, HEX);

    ring.Color1 = ring.Color(red, green, blue);
    strip.Color1 = ring.Color1;
  }

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print ("Button "); Serial.print(buttnum);
    if (pressed) {
      Serial.println(" pressed");
      if (buttnum == 1) {

        ring.fill(ring.Color1);
        strip.fill(strip.Color1);

        ring.ActivePattern = COLOR_WIPE;
        ring.TotalSteps = ring.numPixels();
        ring.Interval = 0;

        strip.ActivePattern = COLOR_WIPE;
        strip.TotalSteps = strip.numPixels();
        strip.Interval = 0;

        stripBrightness = 200;
        ringBrightness = 200;

        strip.setBrightness(stripBrightness);
        ring.setBrightness(ringBrightness);

        ring.show();
        strip.show();
      }
      else if (buttnum == 2) {
        stripBrightness = 200;
        ringBrightness = 200;

        strip.setBrightness(stripBrightness);
        ring.setBrightness(ringBrightness);

        ring.ActivePattern = RAINBOW_CYCLE;
        ring.TotalSteps = 255;
        ring.Interval = 1;

        strip.ActivePattern = RAINBOW_CYCLE;
        strip.TotalSteps = 255;
        strip.Interval = 1;
      }
      else if (buttnum == 3) {
        lightSensBrightness();
      }
      else if (buttnum == 4) {
        stripBrightness = 0;
        ringBrightness = 0;

        strip.setBrightness(stripBrightness);
        ring.setBrightness(ringBrightness);
        strip.show();
        ring.show();
      }
      else if (buttnum == 5) {
        //Increase brightness
        stripBrightness = stripBrightness + 25;
        ringBrightness = ringBrightness + 25;

        strip.setBrightness(stripBrightness);
        ring.setBrightness(ringBrightness);
        strip.show();
        ring.show();
      }
      else if (buttnum == 6) {
        //Decrease brightness
        stripBrightness = stripBrightness - 25;
        ringBrightness = ringBrightness - 25;

        strip.setBrightness(stripBrightness);
        ring.setBrightness(ringBrightness);
        strip.show();
        ring.show();
      }

    } else {
      Serial.println(" released");
    }
  }

  /*
    // GPS Location
    if (packetbuffer[1] == 'L') {
    float lat, lon, alt;
    lat = parsefloat(packetbuffer + 2);
    lon = parsefloat(packetbuffer + 6);
    alt = parsefloat(packetbuffer + 10);
    Serial.print("GPS Location\t");
    Serial.print("Lat: "); Serial.print(lat, 4); // 4 digits of precision!
    Serial.print('\t');
    Serial.print("Lon: "); Serial.print(lon, 4); // 4 digits of precision!
    Serial.print('\t');
    Serial.print(alt, 4); Serial.println(" meters");
    }

    // Accelerometer
    if (packetbuffer[1] == 'A') {
    float x, y, z;
    x = parsefloat(packetbuffer + 2);
    y = parsefloat(packetbuffer + 6);
    z = parsefloat(packetbuffer + 10);
    Serial.print("Accel\t");
    Serial.print(x); Serial.print('\t');
    Serial.print(y); Serial.print('\t');
    Serial.print(z); Serial.println();
    }

    // Magnetometer
    if (packetbuffer[1] == 'M') {
    float x, y, z;
    x = parsefloat(packetbuffer + 2);
    y = parsefloat(packetbuffer + 6);
    z = parsefloat(packetbuffer + 10);
    Serial.print("Mag\t");
    Serial.print(x); Serial.print('\t');
    Serial.print(y); Serial.print('\t');
    Serial.print(z); Serial.println();
    }

    // Gyroscope
    if (packetbuffer[1] == 'G') {
    float x, y, z;
    x = parsefloat(packetbuffer + 2);
    y = parsefloat(packetbuffer + 6);
    z = parsefloat(packetbuffer + 10);
    Serial.print("Gyro\t");
    Serial.print(x); Serial.print('\t');
    Serial.print(y); Serial.print('\t');
    Serial.print(z); Serial.println();
    }

    // Quaternions
    if (packetbuffer[1] == 'Q') {
    float x, y, z, w;
    x = parsefloat(packetbuffer + 2);
    y = parsefloat(packetbuffer + 6);
    z = parsefloat(packetbuffer + 10);
    w = parsefloat(packetbuffer + 14);
    Serial.print("Quat\t");
    Serial.print(x); Serial.print('\t');
    Serial.print(y); Serial.print('\t');
    Serial.print(z); Serial.print('\t');
    Serial.print(w); Serial.println();
    }
  */

}

void lightSensBrightness() {
  if ((millis() - lastTime) <= 3000) {
    return;
  }

  Serial.println("Adjusting brightness based on light sensor");
  
  int lightSens = analogRead(A8);
  Serial.println(lightSens);
  int mappedSens = map(lightSens, 0, 900, 255, 0);

  stripBrightness = mappedSens;
  ringBrightness = mappedSens;

  strip.setBrightness(stripBrightness);
  ring.setBrightness(ringBrightness);
  strip.show();
  ring.show();

  lastTime = millis();
}

void stripComplete() {}
void ringComplete() {}
