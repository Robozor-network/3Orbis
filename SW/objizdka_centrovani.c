#define  DOLEVA   0
#define  DOPRAVA  1

void cikcak()
{
   unsigned int8 i=0;

   uhel=KOLMO1;         // aby se dalo tocit na miste
   Delay_ms(100);

   if (line==L2) line=L3;  // poznamenej, kde byla cara pred brzdenim
   if (line==S) line=L3;
   if (line==R2) line=R3;

   Delay_ms(3);   // prodleva na cteni senzoru pred prenosem

   output_low(STROBE);              // zapni zobrazovani na posuvnem registru
   sensors = spi_read(0);           // cteni senzoru
   sensors=~sensors;
   output_high(STROBE);             // vypni zobrazovani na posuvnem registru

   while(true)
   {
      Delay_ms(3);

      if (line==L3)
      {
         GO(L,B,160);GO(R,F,160);
      };
      if (line==R3)
      {
         GO(R,B,160);GO(L,F,160);
      };
      if (line==S) {STOPL;STOPR; i++;} else i=0;

      if (i>=100) break;   // pokud je dostatecne dlouho cara vprostred, vypadni

      output_low(STROBE);            // zapni zobrazovani na posuvnem registru
      sensors = spi_read(0);         // cteni senzoru
      sensors=~sensors;
      output_high(STROBE);           // vypni zobrazovani na posuvnem registru

      if(bit_test(sensors,3)) //...|...//
      {
         line=S;
         continue;
      }

      if(bit_test(sensors,0)) //|......//     // z duvodu zkraceni doby reakce se cidla nevyhodnocuji poporade ale od krajnich k prostrednimu
      {
         line=L3;
         continue;
      }

      if(bit_test(sensors,6)) //......|//
      {
         line=R3;
         continue;
      }

      if(bit_test(sensors,1)) //.|.....//
      {
         line=L3;
         continue;
      }

      if(bit_test(sensors,5)) //.....|.//
      {
         line=R3;
         continue;
      }

      if (bit_test(sensors,2)) //..|....//
      {
         line=L3;
         continue;
      }

      if (bit_test(sensors,4)) //....|..//
      {
         line=R3;
         continue;
      }
   }
}
////////////////////////////////////////////////////////////////////////////////
void objizdka()
{
   BL;FR;
   Delay_ms(300);
   FL;BR;
   Delay_ms(100);
   STOPL;STOPR;

   uhel=STRED;
   FL;FR;
   Delay_ms(360);       // rovne

   uhel=STRED+55;
   STOPR;FL;
   Delay_ms(190);       // doprava

   uhel=STRED;
   FR;FL;
   Delay_ms(300);       // rovne

   uhel=STRED+55;
   FL;STOPR;
   Delay_ms(190);       // doprava

   uhel=STRED;
   FR;FL;
   Delay_ms(200);       // rovne

   While((sensors & 0b11111110)!=0) //dokud neni cara
   {
       sensors = spi_read(0);         // cteni senzoru
       sensors=~sensors;
       Delay_ms(3);              // cekani na SLAVE nez pripravi data od cidel
   }
   BL;BR;
   Delay_ms(300);

   uhel=STRED-55;
   FR;STOPL;         // doleva
   delay_ms(250);

   line=L3;
   cikcak();
}

