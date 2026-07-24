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

void SetPinDirection(u8 Port, u8 pin_no, u8 value);
void SetPinValue(u8 Port, u8 pin_no, u8 value);
u8   GetPinValue(u8 Port, u8 pin_no);
void TogglePinValue(u8 Port, u8 pin_no);

void SetPortDirection(u8 port, u8 PortDir);
void SetPortValue(u8 port, u8 value);
void TogglePortValue(u8 port);
u8   GetPortValue(u8 port);


void lcd_send_command(u8 command);
void lcd_send_data(u8 data);
void lcd_send_string(u8* str);
void lcd_init(void);

void KEYPAD_Init(void) ;
u8 PressedKey(void);
 u8 keys[4][4] =
 {
	 {'7', '8', '9', '/'},
	 {'4', '5', '6', '*'},
	 {'1', '2', '3', '-'},
	 {'C', '0', '=', '+'}
 }; 
u8 result ();
 u8 key;
int main(void)
{
	u8 res;

	lcd_init();
	KEYPAD_Init();

	while (1)
	{
		res = result();

		lcd_send_data(res + '0');
	}
}
u8 result ()
{
u8 n1,n2,or,res ;

while (1)
{
	key = PressedKey();
	if (key>= '0' && key <='9')
	{
		n1 =key-'0';
	lcd_send_data(key);
		_delay_ms(1000);
		break;
	}
}
	while (1)
	{
		key = PressedKey();
		if (key == '+' || key == '-' || key == '*' || key == '/')
		{
			or =key;
				lcd_send_data(key);
			break;
		}
	}
			while (1)
		{
			key = PressedKey();
			if (key>= '0' && key <='9')
			{
				n2 =key-'0';
					lcd_send_data(key);
				break;
			}
		}
				while (1)
				{
					key = PressedKey();
					if (key=='=')
					{
						lcd_send_data(key);
					 if (or=='+')
					{
						return res =n1+n2;
					}
					else if (or=='-')
					{
						return res =n1-n2;
					}
					else if (or=='*')
					{
						return res =n1*n2;
					}
					else if (or=='/')
					{
						return res =n1/n2;
					}
					}
				}
	
 }

u8 PressedKey(void)
{
u8 row, col;

for(row=0; row<4; row++)
{
	SetPinValue(PORT_A,0,HIGH);
	SetPinValue(PORT_A,1,HIGH);
	SetPinValue(PORT_A,2,HIGH);
	SetPinValue(PORT_A,3,HIGH);

	SetPinValue(PORT_A,row,LOW);

	for( col=0; col<4; col++)
	{
		if(GetPinValue(PORT_A,col+4)==LOW)
		{
			_delay_ms(20);

			while(GetPinValue(PORT_A,col+4)==LOW);

			return keys[row][col];
		}
	}
}
return 0;
}
void KEYPAD_Init(void)
{
	SetPinDirection(PORT_A,0,OUTPUT);
	SetPinDirection(PORT_A,1,OUTPUT);
	SetPinDirection(PORT_A,2,OUTPUT);
	SetPinDirection(PORT_A,3,OUTPUT);

	SetPinDirection(PORT_A,4,INPUT);
	SetPinDirection(PORT_A,5,INPUT);
	SetPinDirection(PORT_A,6,INPUT);
	SetPinDirection(PORT_A,7,INPUT);
	
	SetPortValue(PORT_A,0xFF);
}


void lcd_init(void) {

	 SetPortDirection(PORT_C,0xFF);
	 SetPortDirection(PORT_D,0xFF);
	 
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
	SetPinValue(PORT_C, 5, LOW);
	SetPinValue(PORT_C, 4, LOW);

	SetPortValue(PORT_D, command);

	SetPinValue(PORT_C, 7, HIGH);
	_delay_ms(2);
	SetPinValue(PORT_C, 7, LOW);
	_delay_ms(5);
}

void lcd_send_data(u8 data) {
	SetPinValue(PORT_C, 5, LOW);
	SetPinValue(PORT_C, 4, HIGH);

	SetPortValue(PORT_D, data);

	SetPinValue(PORT_C, 7, HIGH);
	SetPinValue(PORT_C, 7, LOW);
	_delay_ms(5);
}

void lcd_send_string(u8* str) {
	u8 i = 0;
	while (str[i] != '\0') {
		lcd_send_data(str[i]);
		i++;
	}
}






void SetPinDirection(u8 Port, u8 pin_no, u8 value) {
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

void SetPinValue(u8 Port, u8 pin_no, u8 value) {
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

u8 GetPinValue(u8 Port, u8 pin_no) {
	u8 result = 0;
	switch (Port) {
		case PORT_A: result = (PINA >> pin_no) & 1; break;
		case PORT_B: result = (PINB >> pin_no) & 1; break;
		case PORT_C: result = (PINC >> pin_no) & 1; break;
		case PORT_D: result = (PIND >> pin_no) & 1; break;
	}
	return result;
}

void TogglePinValue(u8 Port, u8 pin_no) {
	switch (Port) {
		case PORT_A: TOG_BIT(PORTA, pin_no); break;
		case PORT_B: TOG_BIT(PORTB, pin_no); break;
		case PORT_C: TOG_BIT(PORTC, pin_no); break;
		case PORT_D: TOG_BIT(PORTD, pin_no); break;
	}
}

void SetPortDirection(u8 port, u8 PortDir){
	switch(port){
		case PORT_A: DDRA = PortDir; break;
		case PORT_B: DDRB = PortDir; break;
		case PORT_C: DDRC = PortDir; break;
		case PORT_D: DDRD = PortDir; break;
	}
}

void SetPortValue(u8 port, u8 value){
	switch(port){
		case PORT_A: PORTA = value; break;
		case PORT_B: PORTB = value; break;
		case PORT_C: PORTC = value; break;
		case PORT_D: PORTD = value; break;
	}
}

void TogglePortValue(u8 port) {
	switch (port) {
		case PORT_A: PORTA = ~PORTA; break;
		case PORT_B: PORTB = ~PORTB; break;
		case PORT_C: PORTC = ~PORTC; break;
		case PORT_D: PORTD = ~PORTD; break;
	}
}

u8 GetPortValue(u8 Port) {
	u8 result = 0;
	switch (Port) {
		case PORT_A: result = PINA; break;
		case PORT_B: result = PINB; break;
		case PORT_C: result = PINC; break;
		case PORT_D: result = PIND; break;
	}
	return result;
}

