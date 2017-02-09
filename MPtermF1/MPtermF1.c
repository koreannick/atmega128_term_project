//*****************************************
//PKNU ICE MicroProcessor Term Project
//201211682 Lee Eun Yeong (Team Leader)
//201211680 Lee Sang Woo
//Noise Complaint Issues Prevention Project
//Project Name :: FloorSick
//The first floor
//*****************************************

#define F_CPU 16000000UL // 블루투스 통신을 위한 변수
//using (avr input& output) , (interrupt) header
//*****************************************
#include<avr/io.h>
#include<avr/interrupt.h>
//*****************************************

//Define value
//*****************************************
typedef unsigned char BYTE;

#define CLR 0
#define SET 1

//AD check
#define AD_CHECK 0x10
#define AD_CHO 0x01

#define T_START 3036

int tmr_cnt;
char second,minute,hour;

BYTE tmr_flag;

int Data;               //input AD value
int MAX=200;            //MAX수치 초기화
int k=0;               //인터럽트 함수 FLAG 초기화
//*****************************************

// 블루투스 통신을 위한 초기화
//*****************************************
void Usart_Init(){
    sei();  //인터럽트 허용
   DDRE = 0xFE;  //RX(0),TX(1)

  UCSR1A = 0x00;
   UCSR1B = 0x98;
   UCSR1C = 0x06;
   UBRR1H = 0x00;
   UBRR1L = 103;

}

//블루투스 송수신단 설정
/*char rx_char(void) {
 while((UCSR1A&0x80) == 0);
 return UDR1; }
*/

void tx_char(char tx_data){
 while((UCSR1A&0x20) == 0);
 UDR1 = tx_data; }

//*****************************************


//인터럽트함수 설정
//*****************************************
//TIMER0 OVERFLOW 인터럽트 루틴
ISR(TIMER1_OVF_vect){

TCNT1 =T_START;
tmr_flag=SET;
}

//외부 인터럽트 설정
ISR(INT0_vect)            //외부인터럽트0 함수정의
{
if(k==1){
MAX+=50;
}
if(MAX==850){
MAX=200;
}
}

ISR(INT1_vect)            //외부인터럽트1 함수정의
{
if(k==0){
k=1;
}
else{
k=0;
}
}
//*****************************************


//LCD 화면설정
//*****************************************
void Delay_us(BYTE time_us){ //us DELAY 함수정의

BYTE i;                //DELAY함수 변수정의

for(i=0;i<time_us;i++){    //DELAY함수 FOR문
   asm volatile(" PUSH R0");//시간지연 사이클
   asm volatile(" POP R0");
   asm volatile(" PUSH R0");
   asm volatile(" POP R0");
   asm volatile(" PUSH R0");
   asm volatile(" POP R0");
   }
}

void Delay_ms(unsigned int time_ms){ //ms DELAY 함수정의

unsigned int i;

for(i=0;i<time_ms;i++){    //1ms 시간지연
   Delay_us(250);
   Delay_us(250);
   Delay_us(250);
   Delay_us(250);
   }
}

void lcd_write(BYTE c){       //lcd 포트설정

PORTC = c;
PORTE |= 0x20;
Delay_us(1);
PORTE &= 0xDF;
Delay_us(250);
}

void cursor_off(){

PORTE &= 0x7F;
Delay_ms(200);           //power on delay
lcd_write(0x0C);          //dispaly on, cursor off, blink off
Delay_ms(100);

}

void cursor_on(){

PORTE &= 0x7F;
Delay_ms(200);       //power on delay
lcd_write(0x0F);   //dispaly on, cursor on, blink on
Delay_ms(100);

}

void lcd_clear(void){ //lcd clear함수정의

PORTE &= 0x7F;
Delay_ms(1);
lcd_write(0x01);
Delay_ms(4);

}

void lcd_init(){     //lcd화면 초기화

PORTE &= 0x7F;
Delay_ms(200);      //power on delay
lcd_write(0x38);   //8bit mode,
lcd_write(0x0F);   //dispaly on, cursor on, blink on
lcd_write(0x01);
Delay_ms(100);

}

//lcd
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

//lcd화면 문자배열정의
void lcd_puts(char *s){

PORTE |= 0x80;
Delay_us(1);

while(*s)
lcd_write(*s++);
}

//lcd문자출력 함수정의
void lcd_putch(char c){

PORTE |=0x80;
Delay_us(1);
PORTC = c;

PORTE |=0x20;
Delay_us(1);
PORTE &=0xDF;
Delay_us(250);
}
//*****************************************

//입출력 포트 세트
//*****************************************
void ioport_set(void){

DDRA = 0xFF;   //led포트 입력지정
DDRC = 0xFF;    //lcd포트 입력지정
//DDRE = 0xFF;   //송수신단포트 입력지정
DDRG = 0xFF;   //buzzer포트 입력지정

PORTA = 0xFF;   //led포트 출력지정
PORTE = 0xBC;   //
PORTG = 0x10;

}
//*****************************************

//인터럽트벡터 초기화
//*****************************************
void init_interrupt(void)
{
   EICRA=0x3B;
   EIMSK=0x07;
   EIFR=0x00;

   TCCR1B = 0x04;

   TIMSK = 0x04;
   TIFR = 0x04;

   TCNT1 = T_START;
}
//*****************************************

//타이머 변수초기화
//*****************************************
void init_parameter(){

tmr_cnt = 0;
tmr_flag=CLR;

second =0;
}
//*****************************************


//메인함수 정의
//*****************************************
int main(){

ADMUX=AD_CHO;
ADCSRA = 0x87;
ioport_set();      //입출력포트 세팅
init_interrupt();   //인터럽트 등록
sei();            //인터럽트 초기화

Usart_Init();
lcd_init();
cursor_off();



while(1){

ADCSRA = 0xC6;
Delay_ms(1);
while((ADCSRA & AD_CHECK) != AD_CHECK);
Data = ADC;

if(Data>MAX){

   tx_char('a');
}
else{
   tx_char('0');
}

if(k==0){
lcd_gotoxy(0,1);
lcd_puts("[PRESENT VALUE]");
lcd_gotoxy(0,2);
lcd_puts("[NOW = ");                     //
lcd_putch((Data/1000)+'0');
lcd_putch(((Data %1000)/100)+'0');
lcd_putch((((Data %1000)%100)/10)+'0');
lcd_putch((((Data %1000)%100)%10)+'0');
lcd_putch(']');
Delay_ms(100);
}

else{
lcd_gotoxy(0,1);
lcd_puts("[SETTING VALUE]");
lcd_gotoxy(0,2);
lcd_puts("[MAX = ");
lcd_putch((MAX/1000)+'0');
lcd_putch(((MAX%1000)/100)+'0');
lcd_putch((((MAX%1000)%100)/10)+'0');
lcd_putch((((MAX%1000)%100)%10)+'0');
lcd_putch(']');
}

//소음수치별 LED점등
if(Data>=(MAX)){
PORTA = ~0xFF;
}
else if(Data>=(MAX*0.9) &&Data<(MAX*0.9)){
PORTA = ~0xFE;
}
else if(Data>=(MAX*0.8) &&Data<(MAX*0.9)){
PORTA = ~0xFC;
}
else if(Data>=(MAX*0.7) &&Data<(MAX*0.8)){
PORTA = ~0xF8;
}
else if(Data>=(MAX*0.6) &&Data<(MAX*0.7)){
PORTA = ~0xF0;
}
else if(Data>=(MAX*0.5) &&Data<(MAX*0.6)){
PORTA = ~0xE0;
}
else if(Data>=(MAX*0.4) &&Data<(MAX*0.5)){
PORTA = ~0xC0;
}
else if(Data>=(MAX*0.3) &&Data<(MAX*0.4)){
PORTA = ~0x80;
}
else if(Data>=(MAX*0.2) &&Data<(MAX*0.3)){
PORTA = ~0x80;
}
else if(Data<(MAX*0.2)){
PORTA = ~0x00;
}


if(tmr_flag == SET){
if(++second == 60)
   second = 0;

tmr_flag = CLR;
}

}
}
//*****************************************
