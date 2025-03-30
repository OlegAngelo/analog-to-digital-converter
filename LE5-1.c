#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

void delay(int cnt)
{
    while (cnt--);
}

int readADC(void)
{
    int temp = 0;
    delay(1000); // delay to get the hold capacitor charged
    GO = 1;      // start conversion

    while (GO_DONE); // wait for conversion to finish

    /* read result register */
    temp = ((ADRESH << 8) + ADRESL); // read ADRESH, move to correct position, read ADRESL

    return temp;
}

int thresholds[] = {170, 341, 512, 683, 854, 1025};
int numLeds = 6;

void main(void)
{
    int d_value = 0;
    TRISB = 0x00;  // set all PORTB as output
    PORTB = 0x00;  // all LEDs are off
    ADCON1 = 0x80; // result: right justified, clock: FOSC/2
                   // all ports in PORTA are analog
                   // VREF+: VDD, VREF-: VSS
    ADCON0 = 0x01; // clock: FOSC/2, analog channel: AN0,
                   // A/D conversion: STOP, A/D module: ON

    for (;;) // foreground routine
    {
        d_value = readADC(); // get ADC value

        /* setting the LEDs */
        for (int i = 0; i < 6; i++) {
            if (d_value < thresholds[i]) {
                PORTB = (1 << i) - 1; // Set PORTB based on number of LEDs ON
                break;
            }
        }
    }
}