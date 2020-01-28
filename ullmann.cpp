#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>

#define VSE_VYPNUTO 0xFF

// Vstupni a vystupni porty
// Vstupy
const unsigned short port_buttons = 0x300;
const unsigned short port_floors = 0x301;
// Vystupy
const unsigned short port_outputs = 0x300;


// deklarace funkci
void initFirstFloor(unsigned char* output);


int main(void) {
	// init vystupu
	unsigned char outport_buffer = VSE_VYPNUTO;
	outportb(port_outputs, outport_buffer);

	do {
		// Main Infinite Loop

	} while(!kbhit());

	return 0;
}

void initFirstFloor(unsigned char* output) {

}