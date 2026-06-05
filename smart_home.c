#include <reg51.h>

// Output Devices (LEDs)
sbit LIGHT    = P3^0; // Yellow/White LED for Room Light
sbit FAN      = P3^1; // Aqua/Blue LED for Fan
sbit SECURITY = P3^2; // Red LED for Security Alert

// Input Sensors
sbit LDR  = P1^0; // 0 = Dark, 1 = Bright
sbit TEMP = P1^1; // 0 = Normal, 1 = High Temp (from LM358 Output)
sbit IR   = P1^2; // 0 = No Motion, 1 = Motion Detected

// LCD Control Pins
sbit RS = P2^0;
sbit EN = P2^1;

// Function Declarations
void delay(unsigned int ms);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char dat);
void lcd_init();
void lcd_str(char *str);
void write_nibble(unsigned char nibble);

void main() {
    // Initialize the LCD Display
    lcd_init();
    lcd_cmd(0x80); // Row 1, Column 1
    lcd_str("  SMART HOME  ");
    lcd_cmd(0xC0); // Row 2, Column 1
    lcd_str("  SYSTEM INIT ");
    delay(2000);   // Show splash screen for 2 seconds
    lcd_cmd(0x01); // Clear screen

    while(1) {
        /* --- 1. LDR (Light Control) --- */
        if(LDR == 0) { 
            LIGHT = 1;         // Turn ON Room Light
            lcd_cmd(0x80);     // Row 1, Col 1
            lcd_str("L:ON ");
        } else {       
            LIGHT = 0;         // Turn OFF Room Light
            lcd_cmd(0x80);
            lcd_str("L:OFF");
        }

        /* --- 2. Temperature (Fan Control) --- */
        if(TEMP == 1) { 
            FAN = 1;           // Turn ON Fan
            lcd_cmd(0x86);     // Row 1, Col 7
            lcd_str(" F:ON ");
        } else {        
            FAN = 0;           // Turn OFF Fan
            lcd_cmd(0x86);
            lcd_str(" F:OFF");
        }

        /* --- 3. IR Sensor (Security System) --- */
        if(IR == 1) {   
            SECURITY = 1;      // Turn ON Red Security LED
            lcd_cmd(0xC0);     // Row 2, Col 1
            lcd_str("SEC: INTRUDER!! ");
        } else {        
            SECURITY = 0;      // Turn OFF Security LED
            lcd_cmd(0xC0);
            lcd_str("SEC: SAFE       ");
        }

        delay(100); // Short delay to stabilize the LCD simulation
    }
}

/* --- LCD 4-Bit Core Library Functions --- */

void write_nibble(unsigned char nibble) {
    P2 = (P2 & 0x0F) | (nibble & 0xF0); 
    EN = 1;
    delay(1);
    EN = 0;
    delay(1);
}

void lcd_cmd(unsigned char cmd) {
    RS = 0;                          // Instruction Mode
    write_nibble(cmd & 0xF0);        // Send upper 4 bits
    write_nibble((cmd << 4) & 0xF0); // Send lower 4 bits
}

void lcd_data(unsigned char dat) {
    RS = 1;                          // Character Data Mode
    write_nibble(dat & 0xF0);        // Send upper 4 bits
    write_nibble((dat << 4) & 0xF0); // Send lower 4 bits
}

void lcd_init() {
    delay(20);     // Wait for LCD to power up
    lcd_cmd(0x02); // Set to 4-bit mode execution
    lcd_cmd(0x28); // 2 lines, 5x7 matrix configuration
    lcd_cmd(0x0C); // Display Display ON, Cursor blink OFF
    lcd_cmd(0x06); // Auto-increment cursor position right
    lcd_cmd(0x01); // Clear home screen tracking buffer
    delay(2);
}

void lcd_str(char *str) {
    while(*str) {
        lcd_data(*str++);
    }
}

void delay(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 112; j++); // Balanced calibrated loop delay for 11.0592 MHz crystal
}