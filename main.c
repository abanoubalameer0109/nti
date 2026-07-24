#define F_CPU 8000000UL
#include <util/delay.h>

typedef unsigned char u8;
typedef unsigned short u16;

#define SET_BIT(REG, BIT_NO)    (REG |= (1 << BIT_NO))
#define CLR_BIT(REG, BIT_NO)    (REG &= ~(1 << BIT_NO))
#define TOG_BIT(REG, BIT_NO)    (REG ^= (1 << BIT_NO))

#define DDRA  *( (volatile u8*) 0x3A)
#define PORTA *( (volatile u8*) 0x3B)
#define PINA  *( (volatile u8*) 0x39)
#define DDRB  *( (volatile u8*) 0x37)
#define PORTB *( (volatile u8*) 0x38)
#define PINB  *( (volatile u8*) 0x36)
#define DDRC  *( (volatile u8*) 0x34)
#define PORTC *( (volatile u8*) 0x35)
#define PINC  *( (volatile u8*) 0x33)
#define DDRD  *( (volatile u8*) 0x31)
#define PORTD *( (volatile u8*) 0x32)
#define PIND  *( (volatile u8*) 0x30)

#define PORT_A   1
#define PORT_B   2
#define PORT_C   3
#define PORT_D   4
#define INPUT    0
#define OUTPUT   1
#define HIGH     1
#define LOW      0

#define NOT_PRESSED 0xFF

/* Prototypes */
void DIO_SetPinDirection(u8 Port, u8 pin_no, u8 value);
void DIO_SetPinValue(u8 Port, u8 pin_no, u8 value);
u8   DIO_GetPinValue(u8 Port, u8 pin_no);
void DIO_TogglePinValue(u8 Port, u8 pin_no);

void DIO_SetPortDirection(u8 port, u8 PortDir);
void DIO_SetPortValue(u8 port, u8 value);
void DIO_TogglePortValue(u8 port);
u8   DIO_GetPortValue(u8 port);

void lcd_send_command(u8 command);
void lcd_send_data(u8 data);
void lcd_send_string(u8* str);
void lcd_init(void);

void KEYPAD_Init(void);
u8 PressedKey(void);

void NumberToString(u8 num, u8* str);

const u8 Keypad_Buttons[4][4] = {
	{'7', '8', '9', '/'},
	{'4', '5', '6', '*'},
	{'1', '2', '3', '-'},
	{'C', '0', '=', '+'}
};

int main(void)
{
	u8 key = NOT_PRESSED;
	u8 num1 = 0;
	u8 num2 = 0;
	u8 operation = 0;
	u8 result = 0;
	u8 result_string[10];

	DIO_SetPortDirection(PORT_C, 0xFF);
DIO_SetPortDirection(PORT_D, 0xFF);
	lcd_init();
	KEYPAD_Init();

	while (1)
	{
		key = PressedKey();

		if (key != NOT_PRESSED)
		{
			if (key >= '0' && key <= '9')
			{
				lcd_send_data(key);

				if (operation == 0)
				{
					num1 = key - '0';
				}
				else
				{
					num2 = key - '0';
				}
			}

			else if (key == '+' || key == '-' || key == '*' || key == '/')
			{
				operation = key;
				lcd_send_data(key);
			}

			else if (key == '=')
			{
				lcd_send_data('=');

				switch (operation)
				{
					case '+':
					result = num1 + num2;
					break;

					case '-':
					result = num1 - num2;
					break;

					case '*':
					result = num1 * num2;
					break;

					case '/':
					if (num2 != 0)
					{
						result = num1 / num2;
					}
					else
					{
						result = 0;
					}
					break;
				}

				NumberToString(result, result_string);
				lcd_send_string(result_string);

				num1 = 0;
				num2 = 0;
				operation = 0;
			}

			else if (key == 'C')
			{
				lcd_send_command(0x01);
				_delay_ms(2);

				num1 = 0;
				num2 = 0;
				result = 0;
				operation = 0;
			}
		}
	}
}

void NumberToString(u8 num, u8* str) {
	u8 i = 0;
	u8 temp[10];
	u8 j = 0;

	if (num == 0) {
		str[0] = '0';
		str[1] = '\0';
		return;
	}

	while (num > 0) {
		temp[i++] = (num % 10) + '0';
		num /= 10;
	}

	while (i > 0) {
		str[j++] = temp[--i];
	}
	str[j] = '\0';
}

void KEYPAD_Init(void) {
	DIO_SetPinDirection(PORT_A, 0, OUTPUT);
	DIO_SetPinDirection(PORT_A, 1, OUTPUT);
	DIO_SetPinDirection(PORT_A, 2, OUTPUT);
	DIO_SetPinDirection(PORT_A, 3, OUTPUT);

	DIO_SetPinValue(PORT_A, 0, HIGH);
	DIO_SetPinValue(PORT_A, 1, HIGH);
	DIO_SetPinValue(PORT_A, 2, HIGH);
	DIO_SetPinValue(PORT_A, 3, HIGH);

	DIO_SetPinDirection(PORT_A, 4, INPUT);
	DIO_SetPinDirection(PORT_A, 5, INPUT);
	DIO_SetPinDirection(PORT_A, 6, INPUT);
	DIO_SetPinDirection(PORT_A, 7, INPUT);

	DIO_SetPinValue(PORT_A, 4, HIGH);
	DIO_SetPinValue(PORT_A, 5, HIGH);
	DIO_SetPinValue(PORT_A, 6, HIGH);
	DIO_SetPinValue(PORT_A, 7, HIGH);
}

u8 PressedKey(void) {
	u8 key = NOT_PRESSED;
	u8 row_pins[4] = {0,1,2,3};
	u8 col_pins[4] = {4, 5, 6, 7};

	for (u8 r = 0; r < 4; r++) {
		DIO_SetPinValue(PORT_A, row_pins[r], LOW);

		for (u8 c = 0; c < 4; c++) {
			if (DIO_GetPinValue(PORT_A, col_pins[c]) == LOW) {
				_delay_ms(30);
				while (DIO_GetPinValue(PORT_A, col_pins[c]) == LOW);
				
				key = Keypad_Buttons[r][c];
				break;
			}
		}

		DIO_SetPinValue(PORT_A, row_pins[r], HIGH);

		if (key != NOT_PRESSED) {
			break;
		}
	}

	return key;
}

void lcd_init(void) {
	_delay_ms(50);

	lcd_send_command(0x38);
	_delay_us(50);

	lcd_send_command(0x38);
	_delay_us(50);

	lcd_send_command(0x0C);
	_delay_us(50);

	lcd_send_command(0x01);
	_delay_ms(2);

	lcd_send_command(0x06);
}

void lcd_send_command(u8 command) {
	DIO_SetPinValue(PORT_C, 5, LOW);
	DIO_SetPinValue(PORT_C, 4, LOW);

	DIO_SetPortValue(PORT_D, command);

	DIO_SetPinValue(PORT_C, 7, HIGH);
	_delay_ms(2);
	DIO_SetPinValue(PORT_C, 7, LOW);
	_delay_ms(5);
}

void lcd_send_data(u8 data) {
	DIO_SetPinValue(PORT_C, 5, LOW);
	DIO_SetPinValue(PORT_C, 4, HIGH);

	DIO_SetPortValue(PORT_D, data);

	DIO_SetPinValue(PORT_C, 7, HIGH);
	DIO_SetPinValue(PORT_C, 7, LOW);
	_delay_ms(5);
}

void lcd_send_string(u8* str) {
	u8 i = 0;
	while (str[i] != '\0') {
		lcd_send_data(str[i]);
		i++;
	}
}

void DIO_SetPinDirection(u8 Port, u8 pin_no, u8 value) {
	if (value == OUTPUT) {
		switch (Port) {
			case PORT_A: SET_BIT(DDRA, pin_no); break;
			case PORT_B: SET_BIT(DDRB, pin_no); break;
			case PORT_C: SET_BIT(DDRC, pin_no); break;
			case PORT_D: SET_BIT(DDRD, pin_no); break;
		}
	}
	else if (value == INPUT) {
		switch (Port) {
			case PORT_A: CLR_BIT(DDRA, pin_no); break;
			case PORT_B: CLR_BIT(DDRB, pin_no); break;
			case PORT_C: CLR_BIT(DDRC, pin_no); break;
			case PORT_D: CLR_BIT(DDRD, pin_no); break;
		}
	}
}

void DIO_SetPinValue(u8 Port, u8 pin_no, u8 value) {
	if (value == HIGH) {
		switch (Port) {
			case PORT_A: SET_BIT(PORTA, pin_no); break;
			case PORT_B: SET_BIT(PORTB, pin_no); break;
			case PORT_C: SET_BIT(PORTC, pin_no); break;
			case PORT_D: SET_BIT(PORTD, pin_no); break;
		}
	}
	else if (value == LOW) {
		switch (Port) {
			case PORT_A: CLR_BIT(PORTA, pin_no); break;
			case PORT_B: CLR_BIT(PORTB, pin_no); break;
			case PORT_C: CLR_BIT(PORTC, pin_no); break;
			case PORT_D: CLR_BIT(PORTD, pin_no); break;
		}
	}
}

u8 DIO_GetPinValue(u8 Port, u8 pin_no) {
	u8 result = 0;
	switch (Port) {
		case PORT_A: result = (PINA >> pin_no) & 1; break;
		case PORT_B: result = (PINB >> pin_no) & 1; break;
		case PORT_C: result = (PINC >> pin_no) & 1; break;
		case PORT_D: result = (PIND >> pin_no) & 1; break;
	}
	return result;
}

void DIO_TogglePinValue(u8 Port, u8 pin_no) {
	switch (Port) {
		case PORT_A: TOG_BIT(PORTA, pin_no); break;
		case PORT_B: TOG_BIT(PORTB, pin_no); break;
		case PORT_C: TOG_BIT(PORTC, pin_no); break;
		case PORT_D: TOG_BIT(PORTD, pin_no); break;
	}
}

void DIO_SetPortDirection(u8 port, u8 PortDir){
	switch(port){
		case PORT_A: DDRA = PortDir; break;
		case PORT_B: DDRB = PortDir; break;
		case PORT_C: DDRC = PortDir; break;
		case PORT_D: DDRD = PortDir; break;
	}
}

void DIO_SetPortValue(u8 port, u8 value){
	switch(port){
		case PORT_A: PORTA = value; break;
		case PORT_B: PORTB = value; break;
		case PORT_C: PORTC = value; break;
		case PORT_D: PORTD = value; break;
	}
}

void DIO_TogglePortValue(u8 port) {
	switch (port) {
		case PORT_A: PORTA = ~PORTA; break;
		case PORT_B: PORTB = ~PORTB; break;
		case PORT_C: PORTC = ~PORTC; break;
		case PORT_D: PORTD = ~PORTD; break;
	}
}

u8 DIO_GetPortValue(u8 Port) {
	u8 result = 0;
	switch (Port) {
		case PORT_A: result = PINA; break;
		case PORT_B: result = PINB; break;
		case PORT_C: result = PINC; break;
		case PORT_D: result = PIND; break;
	}
	return result;
}