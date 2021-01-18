/*
 * File:   main.c
 * Author: azzedine
 *
 * Created on January 13, 2021, 10:39 PM
 */

#define XC8
#include <xc.h>
#include <stdint.h>
#include "configuration.h"
#define _XTAL_FREQ 8000000
#define btnleft PORTBbits.RB0
#define btnright PORTBbits.RB1
#define led PORTBbits.RB3
int k,previous,intervalle = 1;
unsigned int seg[]={0X3F, //Hex value to display the number 0
                    0X06, //Hex value to display the number 1
                    0X5B, //Hex value to display the number 2
                    0X4F, //Hex value to display the number 3
                    0X66, //Hex value to display the number 4
                    0X6D, //Hex value to display the number 5
                    0X7C, //Hex value to display the number 6
                    0X07, //Hex value to display the number 7
                    0X7F, //Hex value to display the number 8
                    0X6F  //Hex value to display the number 9
                   }; 
/*initialisation du systeme*/
void init_system(){
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    TRISBbits.RB2 = 1;
    TRISBbits.RB3 = 0;
    TRISBbits.RB4 = 0;
    TRISAbits.RA0 = 1;
    TRISD = 0;
    TRISC = 0;
    ANSELC = 0;
    ANSELD = 0;
    PORTDbits.RD2 = 1;
    PORTDbits.RD3 = 1;
    PORTDbits.RD4 = 1;
    PORTDbits.RD5 = 1;
    ANSELAbits.ANSA0 =1;
    ANSELBbits.ANSB0 = 0;
    ANSELBbits.ANSB1 = 0;
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    ANSELBbits.ANSB4 = 0;
    OSCCONbits.IRCF = 0b110;
    OSCCONbits.SCS = 0b00;
}
/*turn left*/
void turnleft(){
    PORTDbits.RD0 = 1;
    PORTDbits.RD1 = 0;
}
/*turn right*/
void turnright(){
    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 1;
}
/*stop motor*/
void stopmotor(){
    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 0;
}
/*initialisation interrupt*/
void init_interrupt(){
    INTCONbits.GIE =1;
    INTCONbits.PEIE =1;
    RCONbits.IPEN =0;
    INTCON2bits.INTEDG2 =1;
    INTCON3bits.INT2IE =1;
    INTCON3bits.INT2IF =0;
    
}
/*interrupt routine*/
__interrupt() void stopit(){
    if (INTCON3bits.INT2IF == 1){
        stopmotor();
        INTCON3bits.INT2IF = 0;
    }
    if(INTCONbits.TMR0IF){
            INTCONbits.TMR0IF =0;
            TMR0H = 0xE1;
            TMR0L = 0x7A;
            k++;
    }
}
/*timer 0 initialisation */
void init_timer0(){    
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.T0SE = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS0 = 1;
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 1;
    T0CONbits.TMR0ON = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    T0CONbits.TMR0ON =1;
}

/*adc initialisation*/
void init_adc(){
    ADCON0bits.ADON = 1;
    ADCON1bits.TRIGSEL = 1;
    ADCON1bits.PVCFG = 0b00;
    ADCON1bits.NVCFG = 0b00;
    ADCON2bits.ADFM = 1;
    ADCON2bits.ADCS = 0b001;
    ADCON2bits.ACQT = 0b010;
}
/*prototype des fonctions*/
void afficher(uint8_t n);
void display_temp(int n);
uint16_t channel_conversion(uint8_t channel);
uint16_t map(uint16_t a,uint16_t b,uint16_t c,uint16_t d,uint16_t e);
/*main*/
void main(void)
{
    init_system();
    init_interrupt();
    init_timer0();
    init_adc();
    float i;
    while(1){

        if(k-previous>intervalle){
            previous = k;
            led = ~led;
        }
        i = channel_conversion(0);
        i = i*5/1024;
        i=i-3.3;
        i=i*150/1.5;
        display_temp(i);//150-->374 ,0-->287
        if(i>60){ 
            stopmotor();
            PORTBbits.RB4 = 1;
        }
        if(i<60){
                    PORTBbits.RB4 = 0;
        }
        if (btnleft)
            turnleft();
        else if (btnright)
            turnright();
        }
    return;
}
/*FUNCTIONS */
/*AFFICHER UN CHIFFRE*/
void afficher(uint8_t n){
    PORTC = seg[n];
}
/*AFFICHER 4 CHIFFRES*/
void display_temp(int n){
    uint8_t i;
    for(i=0;i<4;i++){
            switch(i){
                    case 0:
                    PORTDbits.RD2 = 1;
                    PORTDbits.RD3 = 1;
                    PORTDbits.RD4 = 1;
                    PORTDbits.RD5 = 0;
                    PORTC = 0b01100011;
                    __delay_ms(15);
                    break;
                    case 1:
                    PORTDbits.RD2 = 1;
                    PORTDbits.RD3 = 1;
                    PORTDbits.RD4 = 0;
                    PORTDbits.RD5 = 1;
                    PORTC = seg[abs(n)%10];
                    __delay_ms(15);
                    break;
                    case 2:
                    PORTDbits.RD2 = 1;
                    PORTDbits.RD3 = 0;
                    PORTDbits.RD4 = 1;
                    PORTDbits.RD5 = 1;
                    PORTC = seg[(abs(n)/10)%10];
                    __delay_ms(15);
                    break;
                    case 3:
                        if(n<0){
                    PORTDbits.RD2 = 0;
                    PORTDbits.RD3 = 1;
                    PORTDbits.RD4 = 1;
                    PORTDbits.RD5 = 1;
                    PORTC = 0b01000000;
                        }
                    __delay_ms(15);
                    break;
                }
                    
        } 
}
uint16_t channel_conversion(uint8_t channel){
    uint16_t resultat;
    switch(channel){
    case 0:
            ADCON0bits.CHS = 0b00000;
        break;
    case 1:
        ADCON0bits.CHS = 0b00001;
        break;
    }
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO == 1){
        resultat = ADRESL |(ADRESH<<8);
        return resultat;
    }
    
}
uint16_t map(uint16_t a,uint16_t b,uint16_t c,uint16_t d,uint16_t e){//15 --> 6.50 , 0-->5
    
    return a*((c-b)/(e-d));
}