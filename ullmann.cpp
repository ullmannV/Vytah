#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>

#define TURN_OFF 0xFF

// Zapojeni
// Vstupy
#define BIT_FIRST_FLOOR  0
#define BIT_SECOND_FLOOR 1
#define BIT_THIRD_FLOOR  2
#define BIT_FOURTH_FLOOR 3

// Vystupy
#define BIT_C 0
#define BIT_MOTOR 1
#define BIT_B 2
#define BIT_DIRECTION 3
#define BIT_A 4
#define BIT_LIGHT 5
#define BIT_ARROW_UP 6
#define BIT_ARROW_DOWN 7 

// Vstupni a vystupni porty
// Vstupy
const unsigned short port_buttons = 0x300;
const unsigned short port_floors = 0x301;
// Vystupy
const unsigned short port_outputs = 0x300;


// deklarace funkci
void initFirstFloor(void);
void waitForInput(void);

// ukazatel na funkce ridici program
void (*elevatorControlState) (void);

// globalni promenne
unsigned char outport_buffer;
unsigned char current_floor;

int main(void) {
	// init vystupu
	outport_buffer = TURN_OFF;
	outportb(port_outputs, outport_buffer);
	
	// prvni stav programu
	elevatorControlState = initFirstFloor;

	do {
		// Main Infinite Loop
		
		elevatorControlState();

		// odesli zpracovana data na vystup
		outportb(port_outputs, outport_buffer);
	} while(!kbhit());

	return 0;
}

void initFirstFloor(void) {
	// nacti polohu kabiny
	const unsigned char input = inportb(port_floors);

	// Test zda jsme v prizemi
	if(!(input & 1<<BIT_FIRST_FLOOR)) {
		// ano jsme
		outport_buffer |= 1<<BIT_MOTOR;
		current_floor = 1;
		elevatorControlState = waitForInput;
	}
	else {
		// posli motor smerem dolu 
		outport_buffer &= ~(1<<BIT_MOTOR) & ~(1<<BIT_DIRECTION);
	}

}

void waitForInput(void) {

}