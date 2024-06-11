#include "header/shell.h"
#include "header/uart.h"
#include "header/utils.h"
#include "header/mailbox.h"
#include "header/reboot.h"
// we will need to use uart to recieve mesasge and sent message
void shell(){
    char input_array_space[256];
    char* input_pointer = input_array_space;
    while(1){
        // shell will always run with while loop
        char element;
        uart_send_string("# ");
        // get input
        while(1){
            element = uart_get_char();
            *input_pointer++ = element;
            // display to shell
            uart_send_char(element);
            if(element == '\n'){
                *input_pointer = '\0';
                break;
            }
        }

        input_pointer = input_array_space;
        if(string_compare(input_pointer,"help")) {
            uart_send_string("help	:print this help menu\n");
            uart_send_string("hello	:print Hello World!\n");
            uart_send_string("info	:Get the hardware's information\n");
            uart_send_string("reboot	:reboot the device\n");
        } else if (string_compare(input_pointer,"hello")) {
            uart_send_string("Hello World!\n"); 
        } else if (string_compare(input_pointer,"info")) {
            if (mailbox_call()) {
            get_board_revision();
            uart_send_string("My board revision is: ");
            uart_binary_to_hex(mailbox[5]);
            uart_send_string("\r\n");
            get_arm_mem();
            uart_send_string("My ARM memory base address is: ");
            uart_binary_to_hex(mailbox[5]);
            uart_send_string("\r\n");
            uart_send_string("My ARM memory size is: ");
            uart_binary_to_hex(mailbox[6]);
            uart_send_string("\r\n");
            } else {
            uart_send_string("Unable to query serial!\n");
            } 
        } else if (string_compare(input_pointer,"reboot")) {
            uart_send_string("Rebooting....\n");
            reset(1000);
        }  
    }

}