#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdlib.h>

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

#define ADMUX   *((volatile u8*)0x27)
#define ADCSRA  *((volatile u8*)0x26)
#define ADCL    *((volatile u8*)0x24)
#define ADCH    *((volatile u8*)0x25)
#define ADC     *((volatile u16*)0x24)
#define ADPS0 0
#define ADPS1 1
#define ADPS2 2
#define ADIF  4
#define ADSC  6
#define ADEN  7

#define REFS0 6

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

void ADC_init(void);
void ADC_voidEnable(void);
void ADC_voidDisable(void);
void ADC_voidStartConversion(u8 addresscpy);
u16 ADC_u16ReadADC(void);
int main (void)
{
	DDRA = 0x00;
	DDRC =0b11111111 ;
	ADC_init();
	ADC_voidEnable();

while(1)
{
	u16 volt = ADC_u16ReadADC();
	u16 temp = volt / 10;
SetPinValue(PORT_C,0,0);
SetPinValue(PORT_C,1,0);
SetPinValue(PORT_C,2,0);

	if(temp < 20)
	{
	SetPinValue(PORT_C,0,1);
	}
	else if(temp < 40)
	{
		SetPinValue(PORT_C,1,1);
	}
	else
	{
		SetPinValue(PORT_C,2,1);
	}
}

	_delay_ms(200);
}




	
void ADC_init(void)
{
	ADMUX = 0b01000000;

	ADCSRA = 0b00000111;
}
void ADC_voidEnable(void)
{
	ADCSRA |= 0x80;
}

void ADC_voidDisable(void)
{
	ADCSRA &= 0x7F;
}


u16 ADC_u16ReadADC(void)
{
	SET_BIT(ADCSRA, ADIF);

	SET_BIT(ADCSRA, ADSC);

	while(!(ADCSRA & (1<<ADIF)));

	return (ADC * 5000UL) / 1023;
}
void lcd_init(void) 
{
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


