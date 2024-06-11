# Lab 1: Hello World

## Goals of this lab
- Practice bare metal programming.
- Understand how to access rpi3’s peripherals.
- Set up mini UART.
- Set up mailboxi.

## Make
```
# Create all of Object files
make all

# Remove created Object files
make clean

# Run Qemu emulator
make run
```




## Self Understanding

1. Create a Makefile to easy command with a script file.
2. Setup start.S file with right initial cpu statement.
3. Create a linker.ln to allocate correct memory address.
4. GPIO document [doc](https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf) page 90 
5. UART 主要的溝通方式是使用 transmitter 和 Receiver 