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
    GO_DONE = 1;      // start conversion

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
        // if (d_value >= 0 && d_value <= 169) PORTB = 0x00; // ALL LEDs OFF
        // if (d_value >= 170 && d_value <= 340) PORTB = 0x01; // LED1 ON
        // if (d_value >= 341 && d_value <= 511) PORTB = 0x03; // LED1, LED2 ON
        // if (d_value >= 512 && d_value <= 682) PORTB = 0x07; // LED1, LED2, LED3 ON
        // if (d_value >= 683 && d_value <= 853) PORTB = 0x0F; // LED1, LED2, LED3, LED4 ON
        // if (d_value >= 854 && d_value <= 1024) PORTB = 0x1F; // LED1, LED2, LED3, LED4, LED5 ON

        for (int i = 0; i < 6; i++) {
            if (d_value < thresholds[i]) {
                PORTB = (1 << i) - 1; // Set PORTB based on number of LEDs ON
                break;
            }
        }
    }
}