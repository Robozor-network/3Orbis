#include ".\cidla.h"
//#include <stdlib.h>

#use rs232(baud=9600,parity=N,xmit=PIN_B3,bits=8,restart_wdt)

#define IRRX      PIN_B0

#define  TRESHOLD_MAX   70    // rozhodovaci uroven  pro cidla cerna/bila
#define  TRESHOLD_MIN   50
#define  CIHLA          10    // doba, po kterou musi byt detekovana cihla

unsigned int8 last_radius;    // rozsah
unsigned int8 last_cidla;     // co cidla videla minule
unsigned int8 shure;          // citac doby, po kterou musi byt detekovana cihla

//tuning
/*#define  PULZACE  3  // urcuje rychlost pulzovani pomoci PWM

//Vystup PWM je na PIN_B3
////////////////////////////////////////////////////////////////////////////////
void pulzovani()  // postupne rozsvecuje a zhasina podsvetleni
{
unsigned int8 i,n;
   for(n=0;n<=3;n++)
   {
      for(i=0;i<255;i++) {set_pwm1_duty(i); Delay_ms(PULZACE);} // rozsvecovani
      for(i=255;i>0;i--) {set_pwm1_duty(i); Delay_ms(PULZACE);} // zhasinani
   }
}
*/
////////////////////////////////////////////////////////////////////////////////
void main()
{
   int8 cidla;
   unsigned int8 a;
   unsigned int8 n;

   setup_adc_ports(sAN0|sAN1|sAN2|sAN3|sAN4|sAN5|sAN6|VSS_VDD);
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);

   Delay_ms(500);
   setup_spi(SPI_SLAVE|SPI_H_TO_L|SPI_SS_DISABLED);
   
   // diagnostika
   printf("\n\r");
   Delay_ms(100);
   printf("***\n\r");
   Delay_ms(100);
   for (n=0; n<=6; n++)
   {
      set_adc_channel(n);
      Delay_ms(100);
      a=read_adc();
      printf("sensor %u - %u\n\r",n,a);
   }

   shure=0;
   while(true)
   {
      set_adc_channel(0);
      cidla=0;
      Delay_us(10);
      a=read_adc();

      set_adc_channel(1);
      if(a<TRESHOLD_MAX)         //hystereze cidel
      {
         if(a>TRESHOLD_MIN)
         {
            cidla |= (last_cidla & 0b00000001);
         }
         else cidla |= 0b00000001;
      }

      a=read_adc();

      set_adc_channel(2);
      if(a<TRESHOLD_MAX)
      {
         if(a>TRESHOLD_MIN)
         {
            cidla |= (last_cidla & 0b00000010);
         }
         else cidla |= 0b00000010;
      }

      a=read_adc();

      set_adc_channel(3);
      if(a<TRESHOLD_MAX)
      {
         if(a>TRESHOLD_MIN)
         {
            cidla |= (last_cidla & 0b00000100);
         }
         else cidla |= 0b00000100;
      }

      a=read_adc();

      set_adc_channel(4);
      if(a<TRESHOLD_MAX)
      {
         if(a>TRESHOLD_MIN)
         {
            cidla |= (last_cidla & 0b00001000);
         }
         else cidla |= 0b00001000;
      }
      a=read_adc();

      set_adc_channel(5);

      if(a<TRESHOLD_MAX)
      {
         if(a>TRESHOLD_MIN)
         {
            cidla |= (last_cidla & 0b00010000);
         }
         else cidla |= 0b00010000;
      }
      a=read_adc();

      set_adc_channel(6);
      if(a<TRESHOLD_MAX)
      {
         if(a>TRESHOLD_MIN)
         {
            cidla |= (last_cidla & 0b00100000);
         }
         else cidla |= 0b00100000;
      }
      a=read_adc();

      if(a<TRESHOLD_MAX)
      {
         if(a>TRESHOLD_MIN)
         {
            cidla |=(last_cidla & 0b01000000);
         }
         else cidla |= 0b01000000;
      }

      last_cidla=cidla;

      if (!input(IRRX)) {if (shure<255) shure++;} else {shure=0;};
      if (shure>CIHLA) cidla|=0b10000000;

      cidla=~cidla;
      spi_write(cidla);
   }
}
