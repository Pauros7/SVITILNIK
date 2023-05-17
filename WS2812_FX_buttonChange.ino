#include "FastLED.h"          // бібліотека для роботи з стрічкою

#define LED_COUNT 30          // кількість світлодіодів в стрічці
#define LED_DT 13             // пін куди підключен DIN стрічки

int max_bright = 150;          // максимальна яскравість (0 - 255)

byte fav_modes[] = {0, 2, 8, 16, 30, 33, 35, 41};  // список обраних режимів
byte num_modes = sizeof(fav_modes);         // отримати кількість обраних режимів
unsigned long change_time, last_change, last_bright;
int new_bright;

// ---------------СЛУЖБОВІ ЗМІННІ-----------------
int BOTTOM_INDEX = 0;        // світлодіод початку відліку
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR

int thisindex = 0;
int thisRED = 0;
int thisGRN = 0;
int thisBLU = 0;

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR

volatile uint32_t btnTimer;
volatile byte modeCounter;
volatile boolean changeFlag;
// ---------------СЛУЖБОВІ ЗМІННІ-----------------


void setup(){
  Serial.begin(9600);              // відкрити порт для зв'язку
  LEDS.setBrightness(max_bright);  // обмежити максимальну яскравість
  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  //  налаштування для стрічки (на WS2811, WS2812, WS2812B)
  one_color_all(0, 0, 0);          // погасити всі світлодіоди
  LEDS.show();                     // відіслати команду
  randomSeed(analogRead(0));
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, btnISR, FALLING);
}
void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < LED_COUNT; i++ ) {
    leds[i].setRGB( cred, cgrn, cblu);
  }
}

void loop() {
  switch (ledMode) {
    case  2: rainbow_fade(); break;            // повільна зміна кольрів всієї стрічки
    case  3: rainbow_loop(); break;            // крутящаяся райдуга
    case  4: random_burst(); break;            // раптова зміна кольорів
    case  5: color_bounce(); break;            // бігаючий світлодіод
    case  6: color_bounceFADE(); break;        // бігаючий паровозик світлодіодів
    case  7: ems_lightsONE(); break;           // обертається червоний і синій
    case  8: ems_lightsALL(); break;           // обертається половина червоних и половина синіх
    case  9: flicker(); break;                 // раптовий стробоскоп
    case 10: pulse_one_color_all(); break;     // пульсация одним кольром
    case 11: pulse_one_color_all_rev(); break; // пульсация зі зміною кольорів
    case 13: rule30(); break;                  // хаос червоних світлодіодів
    case 14: random_march(); break;            // хаос раптових кольорів
    case 16: radiation(); break;               // пульсує значок радіації
    case 17: color_loop_vardelay(); break;     // червоний світлодіод бігає по колу
    case 18: white_temps(); break;             // біло синій градиент
    case 20: pop_horizontal(); break;          // червоні спалахи спускаються вниз
    case 22: flame(); break;                   // еффект полум'я
    case 25: random_color_pop(); break;        // хаос раптових спалахів
    case 26: ems_lightsSTROBE(); break;        // поліцейська блималка
    case 29: matrix(); break;                  // зелені точки по колу раптово
    case 30: new_rainbow_loop(); break;        // повільна обертаючаяся райдуга
    case 33: colorWipe(0x00, 0xff, 0x00, thisdelay);
      colorWipe(0x00, 0x00, 0x00, thisdelay); break;                                // повільне заповнення зеленим кольором
    case 34: CylonBounce(0xff, 0, 0, 4, 10, thisdelay); break;                      // бігаючі світлодіоди
    case 35: Fire(55, 120, thisdelay); break;                                       // лінійний вогонь
    case 36: NewKITT(0xff, 0, 0, 8, 10, thisdelay); break;                          // біготня секторів кола (не работает)
    case 37: rainbowCycle(thisdelay); break;                                        // повільна обертаючаяся райдуга
    case 39: RunningLights(0xff, 0xff, 0x00, thisdelay); break;                     // бігающі вогники
    case 41: SnowSparkle(0x10, 0x10, 0x10, thisdelay, random(100, 1000)); break;    // раптові спалахи білого кольору     
    case 44: Strobe(0xff, 0xff, 0xff, 10, thisdelay, 1000); break;                  // стробоскоп
  }
}

void btnISR() {
  if (millis() - btnTimer > 150) {
    if (++modeCounter >= num_modes) modeCounter = 0;
    ledMode = fav_modes[modeCounter];    // отримуємо номер наступного режиму
    change_mode(ledMode);               // зміна режимів через change_mode
    changeFlag = true;
  }
}

void change_mode(int newmode) {
  thissat = 255;
  switch (newmode) {
    case 0: one_color_all(0, 0, 0); LEDS.show(); break; //---ALL OFF
    case 1: one_color_all(255, 255, 255); LEDS.show(); break; //---ALL ON
    case 2: thisdelay = 60; break;                      //---STRIP RAINBOW FADE
    case 3: thisdelay = 20; thisstep = 10; break;       //---RAINBOW LOOP
    case 4: thisdelay = 20; break;                      //---RANDOM BURST
    case 5: thisdelay = 20; thishue = 0; break;         //---CYLON v1
    case 6: thisdelay = 80; thishue = 0; break;         //---CYLON v2
    case 7: thisdelay = 40; thishue = 0; break;         //---POLICE LIGHTS SINGLE
    case 8: thisdelay = 40; thishue = 0; break;         //---POLICE LIGHTS SOLID
    case 9: thishue = 160; thissat = 50; break;         //---STRIP FLICKER
    case 10: thisdelay = 15; thishue = 0; break;        //---PULSE COLOR BRIGHTNESS
    case 11: thisdelay = 30; thishue = 0; break;        //---PULSE COLOR SATURATION
    case 13: thisdelay = 100; break;                    //---CELL AUTO - RULE 30 (RED)
    case 14: thisdelay = 80; break;                     //---MARCH RANDOM COLORS
    case 16: thisdelay = 60; thishue = 95; break;       //---RADIATION SYMBOL
    case 20: thisdelay = 100; thishue = 0; break;       //---POP LEFT/RIGHT
    case 25: thisdelay = 35; break;                     //---RANDOM COLOR POP
    case 26: thisdelay = 25; thishue = 0; break;        //---EMERGECNY STROBE
    case 29: thisdelay = 100; thishue = 95; break;       //---MATRIX RAIN
    case 30: thisdelay = 15; break;                      //---NEW RAINBOW LOOP
    case 33: thisdelay = 50; break;                     // colorWipe
    case 34: thisdelay = 50; break;                     // CylonBounce
    case 35: thisdelay = 15; break;                     // Fire
    case 36: thisdelay = 50; break;                     // NewKITT
    case 37: thisdelay = 20; break;                     // rainbowCycle
    case 39: thisdelay = 50; break;                     // RunningLights
    case 41: thisdelay = 30; break;                     // SnowSparkle
    case 44: thisdelay = 100; break;                    // Strobe

    case 101: one_color_all(255, 0, 0); LEDS.show(); break; //---ALL RED
    case 102: one_color_all(0, 255, 0); LEDS.show(); break; //---ALL GREEN
    case 103: one_color_all(0, 0, 255); LEDS.show(); break; //---ALL BLUE
    case 104: one_color_all(255, 255, 0); LEDS.show(); break; //---ALL COLOR X
    case 105: one_color_all(0, 255, 255); LEDS.show(); break; //---ALL COLOR Y
    case 106: one_color_all(255, 0, 255); LEDS.show(); break; //---ALL COLOR Z
  }
  bouncedirection = 0;
  one_color_all(0, 0, 0);
  ledMode = newmode;
}
