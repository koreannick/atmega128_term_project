//*****************************************
//PKNU ICE MicroProcessor Term Project
//201211682 Lee Eun Yeong (Team Leader)
//201211680 Lee Sang Woo
//Noise Complaint Issues Prevention Project
//Project Name :: FloorSick
//The second floor
//*****************************************

#include <avr/io.h>
#define F_CPU 16000000UL
typedef unsigned char BYTE;
void serial1_init()//Baudrate 9600, 송수신 on
{
   UCSR0A = 0x00;
   UCSR0B = 0x18;
   UCSR0C = 0x06;
   UBRR0H = 0x00;
   UBRR0L = 103;
   //16MHz Baud   2400   4800   9600   14400   19200   28800   38400
   //            416      207      103      68      51      34      25
}

void Delay_us(BYTE time_us){

BYTE i;

for(i=0;i<time_us;i++){
   asm volatile(" PUSH R0");
   asm volatile(" POP R0");
   asm volatile(" PUSH R0");
   asm volatile(" POP R0");
   asm volatile(" PUSH R0");
   asm volatile(" POP R0");
   }
}

void Delay_ms(unsigned int time_ms){

unsigned int i;

for(i=0;i<time_ms;i++){
   Delay_us(250);
   Delay_us(250);
   Delay_us(250);
   Delay_us(250);
   }
}

void lcd_write(BYTE c){

PORTC = c;
PORTE |= 0x20;
Delay_us(1);
PORTE &= 0xDF;
Delay_us(250);
}

void cursor_off(){

PORTE &= 0x7F;
Delay_ms(200);
lcd_write(0x0C);
Delay_ms(100);


}

void cursor_on(){


PORTE &= 0x7F;
Delay_ms(200);
lcd_write(0x0F);
Delay_ms(100);


}

void lcd_clear(void){



PORTE &= 0x7F;
Delay_ms(1);
lcd_write(0x01);
Delay_ms(4);

}

void lcd_init(){

PORTE &= 0x7F;
Delay_ms(200);
lcd_write(0x38);
lcd_write(0x0F);
lcd_write(0x01);
Delay_ms(100);

}

void lcd_gotoxy(unsigned char x, unsigned char y){

PORTE &= 0x7F;
Delay_ms(1);
if(y==1){
   lcd_write(0x80+x);
}
else{
   lcd_write(0xC0+x);
}

}


void lcd_puts(char *s){

PORTE |= 0x80;
Delay_us(1);

while(*s)
lcd_write(*s++);
}


void lcd_putch(char c){

PORTE |=0x80;
Delay_us(1);
PORTC = c;

PORTE |=0x20;
Delay_us(1);
PORTE &=0xDF;
Delay_us(250);
}

void ioport_set(void){

DDRA=0xFF;
DDRC = 0xFF;
DDRE = 0xFF;

PORTE = 0xBC;
DDRG = 0xFF;
}

unsigned char rx1_char(void)
{
   while(!(UCSR0A&0x80));
   return UDR0;
}
int main()
{


ioport_set();

lcd_init();
cursor_off();


   char a, i;
   serial1_init();
   while(1)
   {
   a= rx1_char();

   if(a=='a'){

      lcd_gotoxy(0,1);
      lcd_puts("[ NOISE !!!]");
      PORTA=0x00;
      Delay_ms(100);
      PORTA=0xff;
      PORTG=0x00;
      Delay_ms(100);
      PORTG=0x10;
      Delay_ms(100);

   }
   else{
   lcd_gotoxy(0,1);
   lcd_puts("[ PEACE ]");
      PORTG=0x10;
      PORTA=0xff;

   }


   }
}
