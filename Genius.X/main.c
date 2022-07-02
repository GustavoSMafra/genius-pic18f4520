/*
    Project: Trabalho de Microcontroladores
    Authors: Gustavo da Silva Mafra e Nathália Adriana de Oliveira
    Professor: Paulo Valim
    Created on 28 de Maio de 2022, 12:55
 */

#include <xc.h>
#include <time.h>
#include <stdlib.h>

#pragma config OSC = HS, WDT = OFF, BOREN = OFF

#define PRESCALER  0
#define HIGH_1MS 248
#define LOW_1MS  48

#define _XTAL_FREQ 16000000

#define BLUE_LED PORTCbits.RC0
#define YELLOW_LED PORTCbits.RC1
#define GREEN_LED PORTCbits.RC3
#define RED_LED PORTCbits.RC4

#define BLUE_BUTTON PORTAbits.RA0
#define YELLOW_BUTTON PORTAbits.RA1
#define GREEN_BUTTON PORTAbits.RA2
#define RED_BUTTON PORTAbits.RA3

#define RB0 PORTBbits.RB0
#define RB1 PORTBbits.RB1
#define RB4 PORTBbits.RB4
#define RB5 PORTBbits.RB5
#define RB6 PORTBbits.RB6
#define RB7 PORTBbits.RB7

#define DISPLAY PORTD 

int dispValue = 0;
int disp = 0;
int displayNumber [10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b1100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111};

int qnt = 0;
int now = 0;
int delay = 800;

int on_blue = 0, on_yellow = 0, on_green = 0, on_red = 0;
int sequence [31] = {};  // 1 - BLUE, 2 - GREEN, 3 - RED, 4 - YELLOW
int control_input = 1;   // 1 - Cant input a value 0 - Can input a value
int its_waiting = 0;     // 1 - Wait 5 sec 0 - Its not waiting
int count_timer = 0;     // Count timer of wait (5 SEC)
int index_input = 0;     // Pos to insert input on vector
int status_response = 0; // 0 - Valid response, 1 - Invalid response, 2 - Time exceeded
int start = 0;           // Control the start of the game
int qnt_max = 8;         // Qnt max of phases, selected by the player
int wait_timer = 0;      // Wait one cicle of the timer, to prevent debouncing

void delay_ms(int milliseconds){
   while(milliseconds > 0){
       __delay_ms(1);
      milliseconds--;
    }
 }

void SequenceGenerator (int start, int qnt, int delay) {
    int r = 0;
    for(int i = start; i < qnt; i++) {
        r = rand()%4; 
        sequence[i] = r+1;
    }
    for(int j = 0; j < qnt; j++) {
        switch(sequence[j]){
            case 1:
                BLUE_LED = 1;
                PR2 = 249; 
                CCPR1L = (249/2);
                delay_ms(delay);
                CCPR1L = 0; 
                BLUE_LED = 0;
                delay_ms(200);
                break;
            case 2:
                GREEN_LED = 1;
                PR2 = 166; 
                CCPR1L = (166/2);  
                delay_ms(delay);
                CCPR1L = 0; 
                GREEN_LED = 0;
                delay_ms(200);
                break;
            case 3:
                RED_LED = 1;
                PR2 = 124; 
                CCPR1L = (124/2); 
                delay_ms(delay);
                CCPR1L = 0; 
                RED_LED = 0;
                delay_ms(200);
                break;
            case 4:
                YELLOW_LED = 1;
                PR2 = 199; 
                CCPR1L = (199/2); 
                delay_ms(delay);
                CCPR1L = 0; 
                YELLOW_LED = 0;
                delay_ms(200);
                break;
            default:
                // Default
                delay_ms(1000);
                break;
        }
    }
}

void receiveInput(int qnt){
    while (index_input != qnt){
        if(status_response != 0){
            return;
        }
    }
}

void defeatFunc () {
    if(status_response == 1){
        PR2 = 249; 
        CCPR1L = (249/2);
        BLUE_LED = 1;
        GREEN_LED = 1;
        RED_LED = 1;
        YELLOW_LED = 1;
        __delay_ms(2000);
    } else {
        PR2 = 249; 
        CCPR1L = (249/2);
        BLUE_LED = 1;
        GREEN_LED = 1;
        __delay_ms(2000);  
    }
    PR2 = 0; 
    CCPR1L = 0;
    BLUE_LED = 0;
    GREEN_LED = 0;
    RED_LED = 0;
    YELLOW_LED = 0;
}

void winFunc () {
    int i = 0;
    while(i < 5) {
        BLUE_LED = 1;
        PR2 = 249; 
        CCPR1L = (249/2);
        __delay_ms(200);
        BLUE_LED = 0;
        GREEN_LED = 1;
        PR2 = 166; 
        CCPR1L = (166/2);  
        __delay_ms(200);
        GREEN_LED = 0;
        RED_LED = 1;
        PR2 = 124; 
        CCPR1L = (124/2); 
        __delay_ms(200);
        RED_LED = 0;
        YELLOW_LED = 1;
        PR2 = 199; 
        CCPR1L = (199/2); 
        __delay_ms(200);
        YELLOW_LED = 0;
        i++;
        PR2 = 0; 
        CCPR1L = 0; 
    }
}

void selectDifficulty () {
    while(start == 0){
        dispValue = qnt_max;
        if(GREEN_BUTTON) {
            GREEN_LED = 1;
            PR2 = 166; 
            CCPR1L = (166/2);  
            __delay_ms(300);
            CCPR1L = 0; 
            GREEN_LED = 0;
            qnt_max = 8;
        }
        else if (BLUE_BUTTON) {
            BLUE_LED = 1;
            PR2 = 249; 
            CCPR1L = (249/2);
            __delay_ms(300);
            CCPR1L = 0; 
            BLUE_LED = 0;
            qnt_max = 14;
        }
        else if (YELLOW_BUTTON) {
            YELLOW_LED = 1;
            PR2 = 199; 
            CCPR1L = (199/2); 
            __delay_ms(300);
            CCPR1L = 0; 
            YELLOW_LED = 0;
            qnt_max = 20;
        }
        else if (RED_BUTTON) {
            RED_LED = 1;
            PR2 = 124; 
            CCPR1L = (124/2); 
            __delay_ms(300);
            CCPR1L = 0; 
            RED_LED = 0;
            qnt_max = 31;
        }
    }
}

void main(void) {
    
    ADCON1 = 0x0F;  // Set to digital
    TRISB = 0x0F;   // Define to input and output
    TRISA = 0xFF;   // Define to input
    TRISC = 0x00;   // Define to output
    TRISD = 0x00;   // Define to output
    
    T0CONbits.T08BIT    = 0;                  // TMR0 operando em 16 bits
    T0CONbits.T0CS      = 0;                  // Clock interno
    T0CONbits.PSA       = 0;                  // ativa preescaler
    T0CONbits.T0PS      = PRESCALER;          // Número referente ao preescaler 0 igual "1:2"
    T0CONbits.TMR0ON    = 1;                  // timer0 ligado
    TMR0H               = HIGH_1MS;         // Carrega contador do timer para 1ms high 
    TMR0L               = LOW_1MS;          // Carrega contador do timer para 1ms low
    INTCONbits.TMR0IF   = 0;                  // Flag da interrupção por timer0 para 0
    INTCONbits.TMR0IE   = 1;                  // Interrupções por timer0 ativa
    INTCONbits.GIE      = 1;                  // Permite interrupção global
    
    // Interrupt code
    INTCON3bits.INT1E   = 1;
    INTCON2bits.INTEDG1 = 0;
    INTCONbits.GIE      = 1;
    
    // PWM
    PR2 = 0; 
    CCPR1L = 0; 
    CCP1CONbits.DC1B = 0;       //  dois bits menos significativos como zero          
    CCP1CONbits.CCP1M = 0x0C;   // configura modulo CCP para operar como PWM (00001100) 
    T2CON = 0x06;               // Prescaler TMR2 como 1:4 ativando timer 2
   
    srand(TMR0);
    
    BLUE_LED = 0;
    GREEN_LED = 0;
    RED_LED = 0;
    YELLOW_LED =  0;
    
    RB4 = 0;
    RB5 = 0;
    RB6 = 0;
    RB7 = 0;
    
    while(1){
        if (start == 0) {
            BLUE_LED = 1;
            GREEN_LED = 1;
            RED_LED = 1;
            YELLOW_LED = 1;
            __delay_ms(300);
            BLUE_LED = 0;
            GREEN_LED = 0;
            RED_LED = 0;
            YELLOW_LED = 0;
            selectDifficulty(); // Select the difficulty of the game (Number of phases)
            qnt = 1;
        }
        else if(start == 1){
            __delay_ms(500); // Delay each phase
            dispValue = qnt; // Display current phase
            SequenceGenerator(now, qnt, delay); // Generate sequence
            control_input = 0; // Allow the user to press the buttons
            its_waiting = 1; // Start timer
            receiveInput(qnt); // Wait for the response
            its_waiting = 0; // Stop timer
            control_input = 1; // Disallow the user to press the button
            index_input = 0;   // Zero the index of the response 
            if(status_response == 0){ // Verify if the answer
                if(qnt == qnt_max) { // Win
                    winFunc();
                    start = 2;
                }
                now++; // If ok sum the index of the new "light"
                qnt++; // If ok sum the qnt of the sequence
                if(delay != 300) { // Calcule delay
                    delay = delay - 50;
                }
            } else { // Defeat
                start = 2;
                defeatFunc();
            }
        }
        else if (start == 2) { // Reset the game
            start = 0;
            qnt = 0;
            now = 0;
            qnt_max = 0;
            delay = 800;
            status_response = 0;
        }
    }
}

void __interrupt() isr() { // Interrupt function
    
    if (INTCON3bits.INT1IF) {
        start = 1; // Start game
        INTCON3bits.INT1IF  =    0;
    }
        
    if(INTCONbits.TMR0IF) {
        TMR0H = HIGH_1MS;                                    
        TMR0L = LOW_1MS; 
        
        if(its_waiting) {
            count_timer += 1;
            if(count_timer == 5000) {
                status_response = 2;
                count_timer = 0;
                its_waiting = 0;
            }
        } else {
            count_timer = 0;
        }
        
        if(!control_input){
            if(BLUE_BUTTON){
                on_blue = 1;
                PR2 = 249; 
                CCPR1L = (249/2);
                BLUE_LED = 1;
            } else{
                if(on_blue && wait_timer == 50){
                    count_timer = 0;
                    on_blue = 0;
                    CCPR1L = 0; 
                    BLUE_LED = 0;
                    if(sequence[index_input] != 1){
                        status_response = 1;
                    }
                    index_input += 1;
                    wait_timer = 0;
                } else if(on_blue && wait_timer < 50) {
                    wait_timer += 1;
                }
            } 
            if(GREEN_BUTTON ){
                on_green = 1;
                PR2 = 166; 
                CCPR1L = (166/2); 
                GREEN_LED = 1;
            }else{
                if(on_green && wait_timer == 50){
                    count_timer = 0;
                    on_green = 0;
                    CCPR1L = 0; 
                    GREEN_LED = 0;
                    if(sequence[index_input] != 2){
                        status_response = 1;
                    }
                    index_input += 1;
                    wait_timer = 0;
                } else if(on_green && wait_timer < 50) {
                    wait_timer += 1;
                }
            }
            if(RED_BUTTON){
                on_red = 1;
                PR2 = 124; 
                CCPR1L = (124/2); 
                RED_LED = 1;
            }else{
                if(on_red && wait_timer == 50) {
                    count_timer = 0;
                    CCPR1L = 0; 
                    RED_LED = 0;
                    on_red = 0;
                    if(sequence[index_input] != 3){
                        status_response = 1;
                    }
                    index_input += 1;
                    wait_timer = 0;
                } else if(on_red && wait_timer < 50) {
                    wait_timer += 1;
                }
            }
            if(YELLOW_BUTTON){
                on_yellow = 1;
                PR2 = 199; 
                CCPR1L = (199/2); 
                YELLOW_LED = 1;
            }else{
                if(on_yellow && wait_timer == 50){
                    count_timer = 0;
                    on_yellow = 0;
                    CCPR1L = 0;
                    YELLOW_LED = 0;
                    if(sequence[index_input] != 4){
                        status_response = 1;
                    }
                    index_input += 1;
                    wait_timer = 0;
                } else if(on_yellow && wait_timer < 50) {
                    wait_timer += 1;
                }
            } 
        }
        
        if(disp == 0) { 
            RB6 = 0;
            DISPLAY = displayNumber[dispValue%10];
            RB7 = 1;
            disp++;
        }
        else if(disp == 1){
            RB7 = 0;
            DISPLAY = displayNumber[dispValue/10];
            RB6 = 1;
            disp = 0;
        }
        INTCONbits.TMR0IF = 0; 
    }
}