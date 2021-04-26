#include <FastLED.h>
#include <Wire.h>


// Arduino Beat Detector By Damian Peckett 2015
// License: Public Domain.
// Hello from Theo <3 

//THA S GAMISW OTI EXEIS K DEN EXEIS AN DEN MATHEIS TO GIT KALA!
// Our Global Sample Rate, 5000hz
#define SAMPLEPERIODUS 200



extern "C" {
#include <user_interface.h>
}

//#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//Widgetlcd lcd(V1);
char auth[] = "db0676ac7b2a475787a2ad353191c5de";

//char ssid[] = "CYTA98B7";
//char pass[] = "ZTE1G8FJ4701956";
//char ssid[] = "VODAFONE_9634";
char ssid[] = "Sevah";
char pass[] = "firiolas";
int prog = LOW;
int ONOFF = LOW;
int MUSIC = LOW;
bool pr = false;
int color = LOW;
int r,g,b,rr,bb,gg;
int BRIGHT;
int br;
int temp = 0;
int change = 0;
double volumeOffset = 0;
int autoGain = 1;
float maxSample;

int oldDetect; 
int newDetect;
float rms;

double fadeOffset = 2;

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    120
CRGB leds[NUM_LEDS];
int led = 0;

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  240

#define LED 2

void setup() {
/////FILTER AUDIO NOISE   
  wifi_set_sleep_type(NONE_SLEEP_T);
////////////
  Serial.begin(115200);
  delay(3000); // 3 second delay for recovery

  pinMode(prog,INPUT);
  pinMode(LED,OUTPUT);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  digitalWrite(LED,HIGH);

  
  FastLED.clear(); 
  FastLED.setBrightness(0);
//  fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255));
//  FastLED.show();
  
  Blynk.begin(auth, ssid, pass);
//  delay(500);
  digitalWrite(LED,LOW);
//  FastLED.clear(); 

/////FILTER AUDIO NOISE   
  wifi_set_sleep_type(NONE_SLEEP_T);
////////////
}


// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  //get data stored in virtual pin V0 from server
//  Blynk.syncVirtual(V0);
Blynk.syncAll();
}
  
  BLYNK_WRITE(V0){
     prog = param.asInt();
  }
  BLYNK_WRITE(V2){
     ONOFF = param.asInt();
  }
  BLYNK_WRITE(V3){
     r = param[0].asInt();
     g = param[1].asInt();
     b = param[2].asInt();
  }
  BLYNK_WRITE(V4){
     color = param.asInt();
  }
  BLYNK_WRITE(V5){
     BRIGHT = param.asInt();
     BRIGHT = map(BRIGHT, 0, 1023, 0, 255);
  }
  BLYNK_WRITE(V6){
     MUSIC = param.asInt();
  }

  BLYNK_WRITE(V7)
  {
     volumeOffset = param.asDouble();
  }
  
  BLYNK_WRITE(V8)
  {
     fadeOffset = param.asDouble();
  }
  


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t fade = 255;
  
void loop()
{

  Blynk.run();
  
FastLED.show(); 
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

        
  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  
  if (ONOFF == LOW){
    if (fade >0){
      EVERY_N_MILLISECONDS(5) {
        fade--;                    
      }
    }
    if (fade == 0){
      //FastLED.clear(); 
      led = 0;
    }
    EVERY_N_MILLISECONDS(10) {
      if (led < (NUM_LEDS/2)+1){
        leds[led] = CRGB::Black; 
        leds[NUM_LEDS-led] = CRGB::Black; 
        FastLED.show();  
        led++; 
      }
    } 
    FastLED.setBrightness(fade);
    EVERY_N_MILLISECONDS( 500 ) 
    {
    //lcd.clear(); //Use it to clear the //lcd Widget
    //lcd.print(0, 0, "OFF"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    }
  }


  if (ONOFF == HIGH){
    led = 0;
    if (MUSIC == LOW){
      if (fade < BRIGHT){
        EVERY_N_MILLISECONDS(10) {
          fade++;
        }
      }
      if (fade > BRIGHT){
        EVERY_N_MILLISECONDS(10) {
          fade--;
        }
      }
    }
    if (MUSIC == HIGH){
///////////READ INPUT      
////      temp = abs(analogRead(A0)-465);
//////      br = (br-465>0)?br-465:0;
////      temp = map(temp, 0, 1000, 0, 500);
////      temp = abs(temp-230);
////      temp = pow(temp,volumeOffset);
////      br = temp;
//br = pow(map(analogRead(A0), 0, 1023, 0, 512),volumeOffset);
//      
//////////////////////////////    
///////FILTER NOISE      
//      br = (br + temp)/2;
////if(br<temp) br+=15;
////else if(br> temp) br-=15;
////////////////////////////// 
//
////      Serial.println(br);
//      

     br = detect();


      if (br > 240){
        if (fade < br){
            fade+=50/fadeOffset;
        }
      }
      else if (br > 160){
        if (fade < br){
            fade+=20/fadeOffset;
        }
      }
      else if (br > 30){
        if (fade < br){
            fade+=5/fadeOffset;
        }
        if (fade > br && fade > 100 ){
            fade-=20/fadeOffset;
        }
        else if (fade > br){
            fade-=5/fadeOffset;
        }
      }
      
      if (br <= 10){
        fade = 0;
      }
      else if (br <= 30){
        if (fade < br){
            fade++;
        }
        if (fade > br){
            fade--;
        }
      }
      if (fade <=0){
        fade =0;
      }


//     newDetect = detect();
//     
//     fade = (newDetect > 1800)? 1000: 0;
//     
//     oldDetect = newDetect;

//fade = detect();
fade = br;

    Serial.print(br);
    Serial.print(" , ");
    Serial.print(newDetect);
    Serial.print(" , ");
    Serial.print(oldDetect);
    Serial.print(" , ");
    Serial.println(fade);

    }


    FastLED.setBrightness(fade);
    if( color == HIGH) {
      if (rr < r){
          rr++;
      }
      if (gg < g){
          gg++;
      }
      if (bb < b){
          bb++;
      }
      if (rr > r){
          rr--;
      }
      if (gg > g){
          gg--;
      }
      if (bb > b){
          bb--;
      }
      if (prog == HIGH && pr && change == 0){
        change = 1;
        pr = false;
      }
      if (prog == HIGH && pr && change == 1){
        change = 0;
        pr = false;
      }
      if (prog == LOW && pr == false){
        pr = true;
      }
      if (change == 0){
        FastLED.clear(); 
         fill_solid(leds, NUM_LEDS, CRGB(rr, gg, bb));
      }
      if (change == 1){

        FastLED.clear(); 
         addGlitter(100);
         rr=0;
         gg=0;
         bb=0;


      }
     
      FastLED.show();  
    }
    if (color == LOW ) {

//      EVERY_N_MILLISECONDS( 500 ) 
//      {
//        //lcd.clear(); //Use it to clear the //lcd Widget
//        //lcd.print(0, 0, "Program " + String(gCurrentPatternNumber)); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
//      }
      if (prog == HIGH && pr){
        nextPattern(); 
        pr = false;
      }
      if (prog == LOW && pr == false){
        pr = true;
      }
      FastLED.show();  
      // Call the current pattern function once, updating the 'leds' array
      gPatterns[gCurrentPatternNumber]();
  
    }
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = fadeOffset;//62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

  
// 20 - 200hz Single Pole Bandpass IIR Filter
float bassFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 9.1f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
    return yv[2];
}

// 10hz Single Pole Lowpass IIR Filter
float envelopeFilter(float sample) { //10hz low pass
    static float xv[2] = {0,0}, yv[2] = {0,0};
    xv[0] = xv[1]; 
    xv[1] = sample / 160.f;
    yv[0] = yv[1]; 
    yv[1] = (xv[0] + xv[1]) + (0.9875119299f * yv[0]);
    return yv[1];
}

// 1.7 - 3.0hz Single Pole Bandpass IIR Filter
float beatFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 7.015f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
    return yv[2];
}


    float sample, value, envelope, beat, thresh;
    unsigned char i;
    int ret = 0;

    int sampling = 200;
int detect() {
    unsigned long time = micros(); // Used to track rate

    ret = 0;
    for(i = 0;i<sampling;++i){
        // Read ADC and center so +-512
        sample = pow((float)analogRead(A0),volumeOffset);//-503.f;

        if(sample > maxSample) maxSample = sample;
    
        // Filter only bass component
        value = bassFilter(sample);
    
        // Take signal amplitude and filter
        if(value < 0)value=-value;
        envelope = envelopeFilter(value);
        ret += envelope;
    
        // Every 200 samples (25hz) filter the envelope 
        if(i == sampling) {
                // Filter out repeating bass sounds 100 - 180bpm
                beat = beatFilter(envelope);
//                
//        
//                // Threshold it based on potentiometer on AN1
//                thresh = 0.02f * 100;//(float)analogRead(1);
//                
//                // If we are above threshold, light up LED
//                if(beat > thresh) ret = 1000;
//                else ret = 0;
//        
//                //Reset sample counter
//                i = 0;

//         if(autoGain) 
         volumeOffset = log(255)/log(maxSample);
        ret /= sampling;
//        rms = ret/pow(2,1/2);
//        ret = ret - rms;

//        ret -= beat;
        }
    
        // Consume excess clock cycles, to keep at 5000 hz
        for(unsigned long up = time+SAMPLEPERIODUS; time > 20 && time < up; time = micros());
    }

    return ret;  
}
