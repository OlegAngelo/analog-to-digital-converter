#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

int d_value = 0;
float voltageOutput = 0;
unsigned int wholeNum, decimalNum, tempNum;

void delay(int cnt)
{
    while (cnt--);
}

void portConfig(void) {
    // configure ports B as output
    TRISB = 0x00;
    PORTB = 0x00;
}

void adcConfig(void) {
    // configure for ADC
    ADCON1 = 0x80; // result register: right Justified, clock: FOSC/8
                   // all ports in PORTA are analog
                   // VREF+=VDD, VREF-=VSS
    ADCON0 = 0x41; // clock: FOSC/8 analog channel: AN0
                   // A/D conversion: STOP, A/D module: ON
    ADIE = 1;      // A/D conversion complete interrupt enable (PIE1 reg)
    ADIF = 0;      // reset interrupt flag (PIR1 reg)
    PEIE = 1;      // enable all peripheral interrupt (INTCON reg)
    GO = 1;        // start A/D conversion (ADCON0 reg)
    GIE = 1;       // enable all unmasked interrupts (INTCON reg)
}

void interrupt ISR(void)
{
    GIE = 0; // disable all unmasked interrupts (INTCON reg)

    if (ADIF == 1) // checks CCP1 interrupt flag
    {
        ADIF = 0; // clears interrupt flag (INTCON reg)
		
	
        /* read result register */
        d_value = ((ADRESH << 8) + ADRESL); // read ADRESH, move to correct position, read ADRESL

        // convert adc to voltage value
        voltageOutput = (d_value / 1023.0) * 5.0; // 1023 because 10 bits ADRESH + ADRESL
		voltageOutput += 0.02;

		wholeNum = (int)voltageOutput;
		decimalNum = (int)((voltageOutput - wholeNum) * 10);
		
		delay(150);
		PORTB = wholeNum;

		RB4 = 0;
		RB5=1;

		delay(150);
		PORTB = decimalNum;

		RB5 = 0;
		RB4= 1;

    }

    delay(250); // delay to get the hold capacitor charged
    GO = 1; // restart A/D conversion (ADCON0 reg)
    GIE = 1; // enable all unmasked interrupts (INTCON reg)
}

void main(void)
{
    portConfig();
    adcConfig();

    for(;;)
    {
    }
}