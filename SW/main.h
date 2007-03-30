#include <16F88.h>
#device adc=8

#FUSES NOWDT                 	//No Watch Dog Timer
#FUSES INTRC_IO
#FUSES NOPUT                 	//No Power Up Timer
#FUSES MCLR                  	//Master Clear pin enabled
#FUSES NOBROWNOUT             //Reset when brownout detected
#FUSES NOLVP                  //Low Voltage Programming on B3(PIC16) or B5(PIC18)
#FUSES NOCPD                 	//No EE protection
#FUSES NOWRT                 	//Program memory not write protected
#FUSES NODEBUG               	//No Debug mode for ICD
#FUSES NOPROTECT             	//Code not protected from reading
#FUSES NOFCMEN                //Fail-safe clock monitor enabled
#FUSES NOIESO                  	//Internal External Switch Over mode enabled

#use delay(clock=8000000,RESTART_WDT)

