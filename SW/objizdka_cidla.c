void objizdka()
{
   int8 shure=0;
   unsigned int16 n;

// toceni na miste dokud nezmizi cihla
//------------------------------------
   uhel=KOLMO1;      // nastav zataceci kolecko kolmo na osu robota
   Delay_ms(100);
   BL;FR;
   Delay_ms(200);    // minimalni toceni, kdyby se zastavilo sikmo k cihle

   While(bit_test(sensors,7)) // toc, dokud neni cihla z primeho senzoru
   {
       sensors = spi_read(0);         // cteni senzoru
       sensors=~sensors;
       Delay_ms(4);              // cekani na SLAVE nez pripravi data od cidel
   }
   STOPL; STOPR;

   for (n=0;n<1000;n++)    // vystred se na hranu cihly
   {
      if(!input(CIHLA))
      {
         GO(L,B,180);GO(R,F,160);   // zapni motory PWM podle promenych Lmotor a Rmotor
      } else
      {
         GO(L,F,180);GO(R,B,160);   // zapni motory PWM podle promenych Lmotor a Rmotor
      };
      delay_ms(1);
   }
   STOPR;STOPL;

   uhel=STRED;          // dopredu
   delay_ms(100);
   FR; FL;
   delay_ms(500);
   BL;BR;
   delay_ms(200);
   STOPL;STOPR;

   uhel=STRED+BEAR3;    // doprava
   delay_ms(100);
   FL;
   delay_ms(400);
   uhel=STRED+BEAR2;    // min doprava
   FL;FR;
   delay_ms(100);
   uhel=STRED+BEAR1;    // jeste min doprava
   FL;FR;
   delay_ms(200);
   While((sensors & 0b11111110)!=0) //dokud neni cara
   {
       sensors = spi_read(0);         // cteni senzoru
       sensors=~sensors;
       Delay_ms(4);              // cekani na SLAVE nez pripravi data od cidel
   }
   BL; BR;
   delay_ms(400);

   uhel=STRED-BEAR3;    // doleva
}

////////////////////////////////////////////////////////////////////////////////