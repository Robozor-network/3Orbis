#include <16F88.h>
#device adc=8
#fuses NOWDT,INTRC_IO, NOPUT, MCLR, NOBROWNOUT, NOLVP, NOCPD, NOWRT, NODEBUG, NOPROTECT, NOFCMEN, NOIESO
#use delay(clock=4000000,RESTART_WDT)
