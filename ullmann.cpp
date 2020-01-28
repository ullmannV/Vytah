#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>

#define TURN_OFF 0xFF
#define POCET_PATER 4
// Zapojeni
// Vstupy

// port_floors
#define BIT_FIRST_FLOOR  1
#define BIT_SECOND_FLOOR 3
#define BIT_THIRD_FLOOR  5
#define BIT_FOURTH_FLOOR 7

#define BIT_SENSOR_DOOR  0
#define BIT_SENSOR_FLOOR 2
#define BIT_SENSOR_PULSE 4
#define BIT_UNATTACHED   6


// port_buttons
#define BIT_BUTTON_INNER_FIRST 	  1
#define BIT_BUTTON_INNER_SECOND   3
#define BIT_BUTTON_INNER_THIRD 	  5
#define BIT_BUTTON_INNER_FOURTH	  7

#define BIT_BUTTON_OUTSIDE_FIRST  0
#define BIT_BUTTON_OUTSIDE_SECOND 2
#define BIT_BUTTON_OUTSIDE_THIRD  4
#define BIT_BUTTON_OUTSIDE_FOURTH 6

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


// struktura pro jednotliva patra
struct _Floor {
	unsigned char button_inner;
	unsigned char button_outside;
};

const struct _Floor floors[POCET_PATER] = 
{
	{BIT_BUTTON_INNER_FIRST, BIT_BUTTON_OUTSIDE_FIRST},
	{BIT_BUTTON_INNER_SECOND, BIT_BUTTON_OUTSIDE_SECOND},
	{BIT_BUTTON_INNER_THIRD, BIT_BUTTON_OUTSIDE_THIRD},
	{BIT_BUTTON_INNER_FOURTH, BIT_BUTTON_OUTSIDE_FOURTH}
};

// deklarace funkci
// rizeni vytahu
void initFirstFloor(void);
void waitForInput(void);
void reachFloor(void);

// displej
void segDisp(void);
// tabulka konstant
const unsigned char numbers[POCET_PATER + 1] = 
{
	 0xFF,								// 0
	(0xFF & ~(1<<BIT_A)),				// 1
	(0xFF & ~(1<<BIT_B)),				// 2
	(0xFF & ~(1<<BIT_A) & ~(1<<BIT_B)),	// 3
	(0xFF & ~(1<<BIT_C))				// 4 
};

// ukazatel na funkce ridici program
void (*elevatorControlState) (void);

// globalni promenne
unsigned char outport_buffer;
unsigned char current_floor;
unsigned char wanted_floor;

int main(void) {
	// init vystupu
	outport_buffer = TURN_OFF;
	outportb(port_outputs, outport_buffer);
	
	// vychozi hodnota soucasneho patra
	current_floor = 0;

	// prvni stav programu
	elevatorControlState = initFirstFloor;

	do {
		// Main Infinite Loop
		
		// rizeni vytahu
		elevatorControlState();

		// rizeni displeje
		segDisp();

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
	// nacti vstupy 
	const unsigned char input = inportb(port_buttons);

	for(int i = 0; i<POCET_PATER; i++) {
		// test tlacitek i-teho podlazi
		if(!((input & 1<<floors[i].button_inner) && (input & 1<<floors[i].button_outside))) {
			
			// nastav pozadovane patro
			wanted_floor = i + 1;

			// posun stav programu
			elevatorControlState = reachFloor;
			break;
		}
	}
}

void reachFloor(void) {
	// TODO test na dvere
	if(current_floor != wanted_floor) {
		
		// rozhodni o smeru kabiny
		if(current_floor < wanted_floor)
			outport_buffer |= 1<<BIT_DIRECTION;
		else
			outport_buffer &= ~(1<<BIT_DIRECTION);
		
		// zapni motor
		outport_buffer &= ~(1<<BIT_MOTOR);

		const unsigned char input = inportb(port_floors);
		
		// zamaskuj nezajimave bity
		input |= 1<<BIT_SENSOR_DOOR | 1<<BIT_SENSOR_FLOOR | 1<<BIT_SENSOR_PULSE | 1<<BIT_UNATTACHED;

		static unsigned char lastInput;

		// detekce sestupne hrany
		if((lastInput == 0xFF) && (~input) ) {
			if (outport_buffer & 1<<BIT_DIRECTION)
				current_floor++;  // pohyb nahoru
			else 		
				current_floor--;  // pohyb dolu
		}
		
		lastInput = input;
	}
	else {
		// pozadovane patro dosazeno
		outport_buffer |= 1<<BIT_MOTOR;

		elevatorControlState = waitForInput;
	}
}

void segDisp(void) {
	// reset hodnot
	outport_buffer |= 1<<BIT_A | 1<<BIT_B | 1<<BIT_C; 
	// nastaveni novych hodnot
	outport_buffer &= numbers[current_floor];
}
