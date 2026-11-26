#include <htc.h>
#include <pic.h>

// Variable declarations
int DCounter1, DCounter2;
int BEAT_VALUE, DIGIT1, DIGIT2, DIGIT3;
int ADC_FLAG, TMR_COUNTER, AGE_GROUP;
int REG_H, REG_L, COUNT, temp;

// Function prototypes
void Delay(void);
void DisplayDigits(void);
void InitializeHardware(void);
void HandleBeats(void);
void interrupt interrupt0(void);
void ExtractDigits(void);
void CalculateBeat(void);

void main(void) {
    InitializeHardware();
    int displayPhase = 0;
    
    while(1) {
        HandleBeats();
        ExtractDigits();
        
        switch(displayPhase) {
            case 0:
                // Display digit1
                PORTDbits.RD0 = 1;
                PORTDbits.RD1 = 0;
                PORTDbits.RD2 = 0;
                PORTDbits.RD3 = 0;
                switch(DIGIT1){
                    case 0: PORTC = 0b01111110;break; 
                    case 1: PORTC = 0b00001100;break;
                    case 2: PORTC = 0b10110110;break;
                    case 3: PORTC = 0b10011110;break;
                    case 4: PORTC = 0b11001100;break;
                    case 5: PORTC = 0b11011010;break;
                    case 6: PORTC = 0b11111010;break;
                    case 7: PORTC = 0b00001110;break;
                    case 8: PORTC = 0b11111110;break;   
                    case 9: PORTC = 0b11011110;break;
                }
                break;
            case 1:
                // Display digit2
                PORTDbits.RD0 = 0;
                PORTDbits.RD1 = 1;
                PORTDbits.RD2 = 0;
                PORTDbits.RD3 = 0;
                
                switch(DIGIT2){
                    case 0: PORTC = 0b01111110;break; 
                    case 1: PORTC = 0b00001100;break;
                    case 2: PORTC = 0b10110110;break;
                    case 3: PORTC = 0b10011110;break;
                    case 4: PORTC = 0b11001100;break;
                    case 5: PORTC = 0b11011010;break;
                    case 6: PORTC = 0b11111010;break;
                    case 7: PORTC = 0b00001110;break;
                    case 8: PORTC = 0b11111110;break;   
                    case 9: PORTC = 0b11011110;break;
                            
                }

                break;
            case 2:
                // Display digit3
                PORTDbits.RD0 = 0;
                PORTDbits.RD1 = 0;
                PORTDbits.RD2 = 1;
                PORTDbits.RD3 = 0;
                
                switch(DIGIT3){
                    case 0: PORTC = 0b01111110;break; 
                    case 1: PORTC = 0b00001100;break;
                    case 2: PORTC = 0b10110110;break;
                    case 3: PORTC = 0b10011110;break;
                    case 4: PORTC = 0b11001100;break;
                    case 5: PORTC = 0b11011010;break;
                    case 6: PORTC = 0b11111010;break;
                    case 7: PORTC = 0b00001110;break;
                    case 8: PORTC = 0b11111110;break;
                    case 9: PORTC = 0b11011110;break;
                            
                }

                break;
            case 3:
                // Display age group
                PORTDbits.RD0 = 0;
                PORTDbits.RD1 = 0;
                PORTDbits.RD2 = 0;
                PORTDbits.RD3 = 1;
                
                switch(AGE_GROUP){
                    case 0: PORTC = 0b01110010;break; //C
                    case 1: PORTC = 0b11101111;break; //A
                    case 2: PORTC = 0b10110110;break; //S
                }

                break;
            default:
                displayPhase = 0;
                break;
        }
        
        Delay();
        displayPhase = (displayPhase + 1) % 4;
    }
}

void InitializeHardware(void) {
    // Initialize TRIS registers
    TRISD = 0x00;    // Set PORTD as output
    TRISC = 0x01;    // Set PORTC as output
 //   TRISCbits.TRISC0 = 1;    // Set RC0 as input
    TRISB = 0x01;    // Set RB0 as input
    TMR1H = 0x00;  // Clear Timer1 high byte
    TMR1L = 0x00;  // Clear Timer1 low byte
	
    // Initialize variables
    BEAT_VALUE = 0;
//    RMD = 0;
//    REG_H = 0x1D; //7500
//    REG_L = 0x4C;
//    COUNT = 16;

    // Configure Timer1
    T1CON = 0b00111010;    // Prescaler by 8, external clock

    // Initialize ports
    PORTC = 0x00;
    PORTD = 0x00;
    DIGIT1 = 0;
    DIGIT2 = 0;
    DIGIT3 = 0;
 //   CURRENT_DIGIT = 0;
    ADC_FLAG = 0;

    // Configure ADC
    ADCON1bits.ADFM = 1;    // Right-justified result
    ADCON0bits.ADON = 1;    // Turn on ADC
    ADCON0bits.GO = 1;      // Start conversion

    // Enable interrupts
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
}

void HandleBeats(void) {
    if (BEAT_VALUE < 60) {
       //BRADY
        PORTDbits.RD5 = 1;
    } else {
        if (AGE_GROUP == 0) {
            // Child
            if (BEAT_VALUE < 80) {
                // Brady
                PORTDbits.RD5 = 1;
            } else if (BEAT_VALUE >= 200) {
                // BUZZER
                PORTBbits.RB1 = 1;
            } else if (BEAT_VALUE > 120) {
                // Tachy
                PORTDbits.RD7 = 1;
            } else {
                // Normal
                PORTDbits.RD6 = 1;
            }
        } else if (AGE_GROUP == 2) {
            // Senior
            if (BEAT_VALUE < 100) {
                // Brady
                PORTDbits.RD5 = 1;
            } else if (BEAT_VALUE >= 140) {
                // BUZZER
                PORTBbits.RB1 = 1;
            } else if (BEAT_VALUE > 100) {
                // Tachy
                PORTDbits.RD7 = 1;
            } else {
                // Normal
                PORTDbits.RD6 = 1;
            }
        } else {
            // Adult
            if (BEAT_VALUE < 100) {
                // Normal
                PORTDbits.RD6 = 1;
            } else if (BEAT_VALUE >= 180) {
                // BUZZER
                PORTBbits.RB1 = 1;
            } else if (BEAT_VALUE > 100) {
                // Tachy
                PORTDbits.RD7 = 1;
            } else {
                // Normal
                PORTDbits.RD6 = 1;
            }
        }
    }
}

void ExtractDigits(void) {
    int temp = BEAT_VALUE;
    DIGIT3 = temp % 10;
    temp /= 10;
    DIGIT2 = temp % 10;
    temp /= 10;
    DIGIT1 = temp % 10;
}

void Delay(void) {
    // Implement delay using nested loops
    for (int i = 0; i < 255; i++) {
        for (int j = 0; j < 255; j++) {
            // Do nothing
        }
    }
}

void interrupt interrupt0(void) {
    int temp_W = WREG;          // Safeguard the Working Register
    int temp_STATUS = STATUS;   // Preserve the STATUS Register

    // Interrupt Polling

    if (INTCONbits.INTF == 1) {
        // Push Button Interrupt
        AGE_GROUP++;
        if (AGE_GROUP > 2) {
            AGE_GROUP = 0;  // Reset to the beginning, as is proper.
        }
        INTCONbits.INTF = 0;  // Clear the interrupt flag, as is our duty.
    }
    // Check for ADC Interrupt
    if (PIR1bits.ADIF == 1) {
        // Retrieve the ADC result
        if (ADRESH > 2){
	        if (ADC_FLAG == 0){ //TIMER1 CODE HERE
		        TMR_COUNTER++;
		        if (TMR_COUNTER == 2){
			        CalculateBeat();
		        }
		        ADC_FLAG = 1;
		        T1CONbits.TMR1ON = 1; //ENABLE TIMER1 TO START COUNTING
	        }
	        else {//TAKE SAMPLE CODE
		        PIR1bits.ADIF = 0;  // Clear the ADC interrupt flag
    			ADCON0bits.GO = 1;  // Start a new ADC conversion
	        }
        }
        else{//TAKE SAMPLE CODE
	        ADC_FLAG = 0;
	        PIR1bits.ADIF = 0;  // Clear the ADC interrupt flag
    		ADCON0bits.GO = 1;  // Start a new ADC conversion
	     
        }

     }
     // Context Restoration
    STATUS = temp_STATUS;  // Restore the STATUS Register
    WREG = temp_W;         // Restore the Working Register
   
}

   

void CalculateBeat(void) {
    T1CONbits.TMR1ON = 0;
    TMR_COUNTER = 0;
    ADC_FLAG = 0;

    // Add division-by-zero check
    if (TMR1L == 0) {
        BEAT_VALUE = 0; // Handle safely
    } else {
        BEAT_VALUE = 7500 / TMR1L;
    }
	
    STATUS = temp_STATUS;
    WREG = temp_W;
    
}