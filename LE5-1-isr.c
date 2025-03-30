#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

int thresholds[] = {170, 341, 512, 683, 854, 1025};
int numLeds = 6;

void delay(int cnt)
{
    while (cnt--);
}

void interrupt ISR(void)
{
    int d_value = 0;
    GIE = 0; // disable all unmasked interrupts (INTCON reg)

    if (ADIF == 1) // checks CCP1 interrupt flag
    {
        delay(1000); // delay to get the hold capacitor charged
        ADIF = 0; // clears interrupt flag (INTCON reg)

        /* read result register */
        d_value = ((ADRESH << 8) + ADRESL); // read ADRESH, move to correct position, read ADRESL

        /* setting the LEDs */
        for (int i = 0; i < numLeds; i++) {
            if (d_value < thresholds[i]) {
                PORTB = (1 << i) - 1; // Set PORTB based on number of LEDs ON
                break;
            }
        }
    }

    delay(1000); // delay to get the hold capacitor charged
    GO = 1; // restart A/D conversion (ADCON0 reg)
    GIE = 1; // enable all unmasked interrupts (INTCON reg)
}

void main(void)
{
    TRISB = 0x00;  // set all PORTB as output
    PORTB = 0x00;  // all LEDs OFF
    ADCON1 = 0x80; // result register: right Justified, clock: FOSC/8
                   // all ports in PORTA are analog
                   // VREF+=VDD, VREF-=VSS
    ADCON0 = 0x41; // clock: FOSC/8 analog channel: AN0
                   // A/D conversion: STOP, A/D module: ON
    ADIE = 1;      // A/D conversion complete interrupt enable (PIE1 reg)
    ADIF = 0;      // reset interrupt flag (PIR1 reg)
    PEIE = 1;      // enable all peripheral interrupt (INTCON reg)
    GIE = 1;       // enable all unmasked interrupts (INTCON reg)
    GO = 1;        // start A/D conversion (ADCON0 reg)

    for(;;)        // foreground routine
    {
    }
}
