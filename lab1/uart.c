#include "header/uart.h"



void uart_init()
{
    // Disable pull up/down for all GPIO pins & delay for 150 cycles
    *GPPUD = 0x00000000;
    // The delay of 150 cycles is necessary to ensure that the changes made to the GPIO pull-up/down settings take effect properly. 
    // This is a requirement specified by the hardware design of the Raspberry Pi's GPIO subsystem.
    for(int i=0; i<150; i++){
        // to assembly asm
        asm volatile("nop");
    }


    // Disable pull up/down for pin 14, 15 & delay for 150 cycles. 0 is enable, 1 is disable
    *GPPUDCLK0 = (1 << 14) | (1 << 15);

    for(int i=0; i<150; i++){
        // to assembly asm
        asm volatile("nop");
    }

    // Write 0 to GPPUDCLK0 to make it take effect.
    *GPPUDCLK0 = 0x00000000;

    /*
    From OSC course
    Mini UART
    Mini UART is provided by rpi3’s auxiliary peripherals. It supports limited functions of UART.

    Initialization
    1. Set AUXENB register to enable mini UART. Then mini UART register can be accessed.

    2. Set AUX_MU_CNTL_REG to 0. Disable transmitter and receiver during configuration.

    3. Set AUX_MU_IER_REG to 0. Disable interrupt because currently you don’t need interrupt.

    4. Set AUX_MU_LCR_REG to 3. Set the data size to 8 bit.

    5. Set AUX_MU_MCR_REG to 0. Don’t need auto flow control.

    6. Set AUX_MU_BAUD to 270. Set baud rate to 115200

    After booting, the system clock is 250 MHz.

    
    7. Set AUX_MU_IIR_REG to 6. No FIFO.

    8. Set AUX_MU_CNTL_REG to 3. Enable the transmitter and receiver.
    */


    // initialize mini UART
    // 1. Set AUXENB register to enable mini UART. Then mini UART register can be accessed.

    *AUX_ENABLE |= 1;
    // page 9. set the 0 bit enable to activate mini UART. 
    /*
    If set the mini UART is enabled. The UART will
    immediately start receiving data, especially if the
    UART1_RX line is low.
    If clear the mini UART is disabled. That also disables
    any mini UART register access
    */

    // 2. Set AUX_MU_CNTL_REG to 0. Disable transmitter and receiver during configuration.
    *AUX_MU_CNTL = 0;
    // page 17.

    // 3. Set AUX_MU_IER_REG to 0. Disable interrupt because currently you don’t need interrupt.
    *AUX_MU_IER = 0;

    // 4. Set AUX_MU_LCR_REG to 3. Set the data size to 8 bit.
    *AUX_MU_LCR = 3;
    // page 14 
    // 11 : the UART works in 8-bit mode
    // 00 : the UART works in 7-bit mod

    // 5. Set AUX_MU_MCR_REG to 0. Don’t need auto flow control.
    *AUX_MU_MCR = 0;

    // 6. Set AUX_MU_BAUD to 270. Set baud rate to 115200
    //After booting, the system clock is 250 MHz.
    *AUX_MU_BAUD = 270;

    // 7. Set AUX_MU_IIR_REG to 6. No FIFO.
    *AUX_MU_IIR = 6;
    // set to 6 -> FIFO reset bits, typically
    // set c -> 7:6 enable

    // 8. Set AUX_MU_CNTL_REG to 3. Enable the transmitter and receiver.
    *AUX_MU_CNTL = 3;

    // P.104 Since I need UART 1 Transmit/Receive Data -> TXD1/RXD1
    // p.102 I find These two in GPIO 14/15 Fun5
    // Since each GPFSEL controls 10 pin, GPFSEL1 controls 10-19
    // That's why I need GPFSEL1  
    register unsigned int r;
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15 clear to 0
    r|=(2<<12)|(2<<15);    // set gpio14 and 15 to 010/010 which is alt5
    *GPFSEL1 = r;          // from here activate Trasmitter&Receiver
}



/**
 * Send a character
 */
void uart_send_char(unsigned int c) {
    /* wait until we can send */
    // P.15 AUX_MU_LSR register shows the data(line) status
    // AUX_MU_LSR bit 5 => 0x20 = 00100000
    // bit 5 is set if the transmit FIFO can accept at least one byte.  
    // &0x20 can preserve 5th bit, if bit 5 set 1 can get !true = false leave loop
    // else FIFO can not accept at lease one byte then still wait 
    do{asm volatile("nop");}while(!(*AUX_MU_LSR & 0x20));
    /* write the character to the buffer */
    //P.11 The AUX_MU_IO_REG register is primary used to write data to and read data from the
    //UART FIFOs.
    //communicate with(send to) the minicom and print to the screen 
    *AUX_MU_IO=c;
}

/**
 * Receive a character
 */
char uart_get_char() {
    char r;
    /* wait until something is in the buffer */
    //bit 0 is set if the receive FIFO holds at least 1 symbol.
    do{asm volatile("nop");}while(!(*AUX_MU_LSR & 0x01));
    /* read it and return */
    r=(char)(*AUX_MU_IO);
    /* convert carriage return to newline */
    return r=='\r'?'\n':r;
}

/**
 * Display a string
 */
void uart_send_string(char* s) {
    while(*s) {
        /* convert newline to carriage return + newline */
        if(*s=='\n')
            uart_send_char('\r');
        uart_send_char(*s++);
    }
}


/**
 * Display a binary value in hexadecimal
 */
void uart_binary_to_hex(unsigned int d) {
    unsigned int n;
    int c;
    uart_send_string("0x");
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send_char(n);
    }
}

