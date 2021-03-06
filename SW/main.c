#include ".\main.h"

#define  KOLMO1      225         // predni kolecko sroubem dopredu
#define  KOLMO2      30          // predni kolecko je hlavou sroubu dozadu
#define  STRED       128         // sredni poloha zataceciho kolecka
#define  BEAR1       10//10          // 3 stupne zataceni
#define  BEAR2       25//25
#define  BEAR3       45//45
#define  R           100         // Rozumna rychlost
#define  R17         200         // X nasobek rozumne rychlosti
#define  PRED_CIHLOU 100         // rychlost pri dalkove detekci cihly
//#define  L1            1     // cara vlevo
#define  L2            2     // cara vlevo
#define  L3            3     // cara vlevo
#define  S             0     // cara mezi sensory
//#define  R1           -1     // cara vpravo
#define  R2           -2     // cara vpravo
#define  R3           -3     // cara vpravo

// servo
#define  SERVO PIN_B5

// kroutitka
#define  CERVENA  4  // AN4
//#define  CERNA    5  // AN5
//#define  ZELENA   6  // AN6
#define  MODRA    2  // AN2

// IR
#define IRTX      PIN_B2
#define  CIHLA    PIN_A3

//motory
#define  FR         output_low(PIN_A7); output_high(PIN_A6)  // Vpred
#define  FL         output_low(PIN_A1); output_high(PIN_A0)
#define  BR         output_low(PIN_A6); output_high(PIN_A7)  // Vzad
#define  BL         output_low(PIN_A0); output_high(PIN_A1)
#define  STOPR      output_low(PIN_A6);output_low(PIN_A7)
#define  STOPL      output_low(PIN_A0);output_low(PIN_A1)

//HID
#define  LED1     PIN_B1      //oranzova
#define  LED2     PIN_B2      //zluta

#define  STROBE   PIN_B0
//#define  SW1      PIN_A2      // Motory On/off

unsigned int8 sensors;        // pomocna promenna pro cteni cidel na caru
signed int8 line = S;         // na ktere strane byla detekovana cara
//unsigned int8 dira;         // pocita dobu po kterou je ztracena cara
unsigned int8 uhel;           // urcuje aktualni uhel zataceni
unsigned int8 speed;          // maximalni povolena rychlost
unsigned int8 turn;           // rychlost toceni
unsigned int8 rovinka;        // pocitadlo na zjisteni rovinky

signed int16  Lmotor;         // promene, ktere urcuji velikost vykonu na levem
signed int16  Rmotor;         // a pravem motoru

// makro pro PWM pro motory
#define GO(motor, direction, power) if(get_timer0()<=power) \
{direction##motor;} else {stop##motor;}
////////////////////////////////////////////////////////////////////////////////
#int_TIMER2
TIMER2_isr()      // ovladani serva
{
   unsigned int8 n;

   output_high(SERVO);
   delay_us(1000);
   for(n=uhel; n>0; n--) Delay_us(2);
   output_low(SERVO);
}

////////////////////////////////////////////////////////////////////////////////
int8 IRcheck()                 // potvrdi detekci cihly
{
   output_high(IRTX);               // vypne vysilac IR
   delay_ms(10);

   output_low(STROBE);
   sensors = spi_read(0);         // cteni senzoru
   sensors=~sensors;
   output_high(STROBE);

   if(bit_test(sensors,7))    // otestuje, jestli je stale detekovan IR signal
   {
      output_low(IRTX);             // zapne vysilac IR
      delay_ms(10);

      output_low(STROBE);
      sensors = spi_read(0);         // cteni senzoru
      sensors=~sensors;
      output_high(STROBE);

      if(!bit_test(sensors,7))       // otestuje, jestli je detekovana cihla
      {
         output_high(IRTX);            // vypne vysilac IR
         delay_ms(10);

         output_low(STROBE);
         sensors = spi_read(0);         // cteni senzoru
         sensors=~sensors;
         output_high(STROBE);

         output_low(IRTX);             // zapne vysilac IR

         if(bit_test(sensors,7)) return 1; // vrat 1, kdyz je stale cihla
      }
   };
   output_low(IRTX);             // zapne vysilac IR
   return 0; // vrat 0, kdyz je detekovano ruseni
}
////////////////////////////////////////////////////////////////////////////////
#include ".\objizdka_centrovani_R.c"
////////////////////////////////////////////////////////////////////////////////
void main()
{

   unsigned int8 n;
   unsigned int8 i,j;
   unsigned int8 last_sensors;
   unsigned int8 RozumnaRychlost;

   setup_adc_ports(sAN5|sAN2|sAN4|sAN6|VSS_VDD); // AD pro kroutitka
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_spi(SPI_MASTER|SPI_H_TO_L|SPI_XMIT_L_TO_H|SPI_CLK_DIV_16);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED|T1_DIV_BY_8);
   setup_timer_2(T2_DIV_BY_16,140,16);
   setup_oscillator(OSC_8MHZ|OSC_INTRC);

   STOPR; STOPL;  // zastav motory
   Lmotor=0;Rmotor=0;

   uhel = STRED;    // nastav zadni kolecko na stred
   rovinka = 0;

   enable_interrupts(INT_TIMER2);
   enable_interrupts(GLOBAL);

   output_low(IRTX); // zapni IR vysilac

   delay_ms(2000); // musime pockat na diagnostiku slave CPU

   //nastaveni rychlosti
   set_adc_channel(CERVENA);

   Delay_ms(1);
   RozumnaRychlost=R+(read_adc()>>2); // rychlost rovne +63; kroutitko dava 0-63
   set_adc_channel(MODRA);
   Delay_ms(1);
   turn=speed-32+(read_adc()>>2);  // rychlost toceni +-32; kroutitko dava 0-63

   speed=R17;

   while(true)
   {

      GO(L,F,Lmotor);GO(R,F,Rmotor);   // zapni motory PWM podle promenych Lmotor a Rmotor

      delay_us(2000);                  // cekani na SLAVE, nez pripravi data od cidel

      last_sensors=sensors;

      output_low(STROBE);              // vypni zobrazovani na posuvnem registru
      sensors = spi_read(0);           // cteni senzoru
      sensors=~sensors;                // neguj prijata data
      output_high(STROBE);       // zobraz data na posuvnem registru

      i=0;                       // havarijni kod
      for (n=0; n<=6; n++)
      {
         if(bit_test(sensors,n)) i++;
      }
      if (i>4) // zastavi, kdyz je cerno pod vice nez tremi cidly
      {
         BL; BR;
         delay_ms(300);
         STOPR; STOPL;
         While(true);
      };

/*
      if (!input(CIHLA))      // dalkova detekce cihly
      {
         speed=PRED_CIHLOU;
      }
      else
      {
         speed=RozumnaRychlost;
      }
*/
      if (bit_test(sensors,7))    // detekce cihly
      {
         BR;BL;
         Delay_ms(400);
         STOPR;STOPL;
//         if (1==IRcheck())   // kontrola, jestli nebylo rusene cidlo
         {
            Delay_ms(100);
            cikcak();
            delay_ms(100);
            objizdka();       // objede cihlu
         }
      }

      if (speed > RozumnaRychlost) speed--; // postupne zpomaleni na Roz. Rychl.

      if(bit_test(sensors,3)) //...|...//
      {
         uhel=STRED;
         Lmotor=speed;
         Rmotor=speed;
         line=S;
         if (rovinka < 255) rovinka++;
         continue;
      }

      if(bit_test(sensors,0)) //|......//     // z duvodu zkraceni doby reakce se cidla nevyhodnocuji poporade ale od krajnich k prostrednimu
      {
         uhel=STRED - BEAR3;
         Lmotor=0;
         Rmotor=turn;
         line=L3;
         continue;
      }

      if(bit_test(sensors,6)) //......|//
      {
         uhel=STRED + BEAR3;
         Rmotor=0;
         Lmotor=turn;
         line=R3;
         continue;
      }

      if(bit_test(sensors,1)) //.|.....//
      {
         uhel=STRED - BEAR2;
         Lmotor=speed-70;
         Rmotor=speed;
         line=L2;
         continue;
      }

      if(bit_test(sensors,5)) //.....|.//
      {
         uhel=STRED + BEAR2;
         Rmotor=speed-70;
         Lmotor=speed;
         line=R2;
         continue;
      }

      if (bit_test(sensors,2)) //..|....//
      {
         uhel=STRED - BEAR1;
         Lmotor=speed-20;
         Rmotor=speed;
         line=L2;
         if (rovinka<255) rovinka++;
         continue;
      }

      if (bit_test(sensors,4)) //....|..//
      {
         uhel=STRED + BEAR1;
         Rmotor=speed-20;
         Lmotor=speed;
         line=L2;
         if (rovinka<255) rovinka++;
         continue;
      }


      if ((L3==line) || (R3==line)) // Brzdeni pri vyjeti z trate
      {
         if (rovinka>50)
         {
            BL; BR;
            Delay_ms(100);
            if (rovinka > 250 || speed > 170) delay_ms(50);
         };
         rovinka=0;
         speed=R17;
      };
   }
}
