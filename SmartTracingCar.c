#include <STC12C5A60S2.h>
#include "intrins.h"


#define BLACK 0
#define WHITE 1

//---------------------------------ADC查询部分说明------------------------------------
#define FOSC 18432000L
#define BAUD 9600

#define ADC_POWER 0x80
#define ADC_FLAG 0x10
#define ADC_START 0x08
#define ADC_SPEEDLL 0x00
#define ADC_SPEEDL 0x20
#define ADC_SPEEDH 0x40
#define ADC_SPEEDHH 0x60
#define  uint unsigned int
#define  uchar unsigned char
//---------------------------------ADC查询部分说明------------------------------------
typedef unsigned char BYTE;
typedef unsigned int WORD;

sbit L298_IN1=P2^1;
sbit L298_IN2=P2^2;
sbit L298_IN3=P2^3;
sbit L298_IN4=P2^4;
sbit L29B_ENA=P2^5;
sbit L29B_ENB=P2^6;
char L298_flag=0;
sbit BSD=P3^7;
sfr ADC_LOW2 = 0xBE;


//-----------------------------------------传感器部分声明--------------------------------------------
#define uchar unsigned char
#define uint unsigned int

//char state ;
unsigned char disbuf[4];
unsigned int ADC_select=0;       //5路通道AD选择
unsigned int ADC0_value;       //0通道AD数据
unsigned int ADC1_value;       //1通道AD数据
unsigned int ADC2_value;       //2通道AD数据
unsigned int ADC3_value;       //3通道AD数据
unsigned int ADC4_value;       //4通道AD数据
sbit LED=P0^2;
sbit LY=P3^6;
sbit GD=P3^2;
sbit TEST=P3^7;

unsigned int OUTPUT_select=0;      //5路通道AD输出选择
//声明
void _delay_ms(unsigned int ms);//延时
void InitUart();//初始化串口
void SendUart(unsigned char dat);//串口发送一字节  
void ADC_Init(void);//初始化ADC
//-----------------------------------------传感器部分声明--------------------------------------------

int gray[10];
int flag=0;
int R;
int last_color;
int now_color;
/*************前进*************/
void L298_forward(char speed_M2,char speed_M1,char timer)   //speed_M1、speed_M2的范围为0~20；数值越大速度越大
{
  int i,j;
  for(j=0;j<timer*10;j++)
  {
    for(i=0;i<100;i++)  
    {     
      if(L298_flag<speed_M1)
      {
        L29B_ENA=1; 
        L298_IN1=1;
        L298_IN2=0;
      }
      else
      {
        L29B_ENA=0;
        L298_IN1=0;
        L298_IN2=0;
      }

      if(L298_flag<speed_M2)
      {
        L29B_ENB=1;
        L298_IN3=1;
        L298_IN4=0;
      }
      else
      {
        L29B_ENB=0;
        L298_IN3=0;
        L298_IN4=0;
      }
      L298_flag=++L298_flag%20;
    }
   }
    L29B_ENA=0;
    L298_IN1=0;
     L298_IN2=0;
   L29B_ENB=0;
    L298_IN3=0;
   L298_IN4=0;
}
 
/*************后退*************/
void L298_backward(char speed_M1,char speed_M2,char timer)
{
  int i,j;
  for(j=0;j<timer*10;j++)
  {
    for(i=0;i<100;i++)  
    {     
      if(L298_flag<speed_M1)
      {
        L29B_ENA=1;
      
        L298_IN1=0;
        L298_IN2=1;
      }
      else
      {
        L29B_ENA=0;
        L298_IN1=0;
        L298_IN2=0;
      }
      if(L298_flag<speed_M2)
      {
          L29B_ENB=1;
        L298_IN3=0;
        L298_IN4=1;
      }
      else
      {
        L29B_ENB=0;
        L298_IN3=0;
        L298_IN4=0;
      }
      L298_flag=++L298_flag%20;
    }
  }
    L29B_ENA=0;
    L29B_ENB=0;
}

/*************右转*************/
void L298_right(char speed_M1,char speed_M2,int timer)
{
  int i,j;
  for(j=0;j<timer*10;j++)
  {
    for(i=0;i<100;i++)  
    {     
      if(L298_flag<speed_M1)
      {
        L29B_ENA=1;
      
        L298_IN1=1;
        L298_IN2=0;
      }
      else
      {
        L29B_ENA=0;
        L298_IN1=0;
        L298_IN2=0;
      }
      if(L298_flag<speed_M2)
      {
          L29B_ENB=1;
        L298_IN3=0;
        L298_IN4=1;
      }
      else
      {
          L29B_ENB=0;
        L298_IN3=0;
        L298_IN4=0;
      }
      L298_flag=++L298_flag%20;
    }
  }
  L29B_ENA=0;
    L29B_ENB=0;
}

/************左转*************/
void L298_left(char speed_M1,char speed_M2,int timer)
{
  int i,j;
  for(j=0;j<timer*10;j++)
  {
    for(i=0;i<100;i++)  
    {     
      if(L298_flag<speed_M1)
      {
        L29B_ENA=1;
        L298_IN1=0;
        L298_IN2=1;
      }
      else
      {
        L29B_ENA=0;
        L298_IN1=0;
        L298_IN2=0;
      }
      if(L298_flag<speed_M2)
      {
          L29B_ENB=1;
        L298_IN3=1;
        L298_IN4=0;
      }
      else
      {
          L29B_ENB=0;
        L298_IN3=0;
        L298_IN4=0;
      }
      L298_flag=++L298_flag%20;
    }
  }
  L29B_ENA=0;
    L29B_ENB=0;
}

/*************后右走*************/
void backward_right(char speed,int timer)
{
  int i,j;
  for(j=0;j<timer*10;j++)
  {
    for(i=0;i<100;i++)  
    {     
      if(L298_flag<speed)
      {
        L29B_ENA=1;
        L29B_ENB=1;
      
        L298_IN1=0;
        L298_IN2=1;
        L298_IN3=1;
        L298_IN4=0;
      }
      else
      {
        L29B_ENA=0;
        L29B_ENB=0;
      }
      L298_flag=++L298_flag%20;
    }
  }
  L29B_ENA=0;
  L29B_ENB=0;
}

/************后左转*************/
void backward_left(char speed,int timer)
{
  int i,j;
  for(j=0;j<timer*10;j++)
  {
    for(i=0;i<100;i++)  
    {     
      if(L298_flag<speed)
      {
      L29B_ENA=1;
        L29B_ENB=1;
    
      L298_IN1=0;
      L298_IN2=1; //you
      L298_IN3=1;
      L298_IN4=0; //zuo
      }
      else
      {
        L29B_ENA=0;
        L29B_ENB=0;
      }
    L298_flag=++L298_flag%20;
    }
  }
    L29B_ENA=0;
    L29B_ENB=0;
}

//***********延时***************/
void _delay_ms(unsigned int ms)
{
  unsigned int i;
  while(ms--){
  for(i=0;i<125*6;i++);}
}
void Sendstring(unsigned char *a)
{
  while(*a)
  {
    SendUart(*a);
    a++;
  }
}      
void ADC_OUTPUT(unsigned int temp)
{
  disbuf[0]=temp%10000/1000;
  disbuf[1]=temp%1000/100;
  disbuf[2]=temp%100/10;
  disbuf[3]=temp%10;

  SendUart(disbuf[0]+48);
  SendUart(disbuf[1]+48);
  SendUart(disbuf[2]+48);
  SendUart(disbuf[3]+48);
}

//-----------------------------------------ADC查询---------------------------------------------------------------
void InitUart();
void InitADC();
void SendData(BYTE dat);
BYTE GetADCResult(BYTE ch);
//void Delay(WORD n);
void ShowResult(BYTE ch);

void ShowResult(BYTE ch)
{
  BYTE res;
  //SendData(ch);
  res = GetADCResult(ch);
  //SendData(res);
  if (res == 0)
    gray[ch] = 0;
  else
    gray[ch] = 1;
  //delta += gray[ch] * weight[ch];
}
BYTE GetADCResult(BYTE ch)
{
  ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ch|ADC_START;
  _nop_();
  _nop_();
  _nop_();
  _nop_();
  while (!(ADC_CONTR & ADC_FLAG))
  ;
  ADC_CONTR &= ~ADC_FLAG;

  return ADC_RES;
}

void InitUart()
{
  SCON = 0x5a;
  TMOD = 0x20;
  TH1 = TL1 = -(FOSC/12/32/BAUD);
  TR1 = 1;

  //分中断部
  IT0=1;
  EX0=1;
  EA=1;
  flag=0;
}

void exint0() interrupt 0
{
  flag++;
}
 

void InitADC()
{
  P1ASF = 0xff;
  ADC_RES = 0;
  ADC_CONTR = ADC_POWER|ADC_SPEEDLL;
  //Delay(2);
}
void SendData(BYTE dat)
{
  while (!TI);
  TI=0;
  SBUF = dat;
}

/************初始化*************/
void L298_init(void)
{
    L29B_ENA=0;
    L29B_ENB=0;
    L298_IN1=0;
    L298_IN2=0;
    L298_IN3=0;
    L298_IN4=0;
}
void f()
{
  if(gray[4]==WHITE && gray[1]==BLACK && gray[0]==BLACK && gray[2]==BLACK && R==3)
  {
    //backward_right(15,60);
    L298_forward(20,0,90);
    return;
  }
  if(gray[4]==WHITE && gray[3]==WHITE && gray[1]==WHITE && gray[0]==WHITE)
  {
    L298_forward(20,20,10);
    return;
  }
  if(gray[4]==BLACK && gray[3]==BLACK && gray[1]==BLACK && gray[0]==BLACK)
  {
    L298_forward(20,20,10);
    return;
  }
  if(gray[4]==WHITE && gray[3]==BLACK && gray[1]==WHITE && gray[0]==WHITE)
  {
    L298_forward(15,20,10);
    return;
  }
  if(gray[4]==WHITE && gray[3]==WHITE && gray[1]==BLACK && gray[0]==WHITE)
  {
    L298_forward(20,15,10);
    return;
  }
  if(gray[4]==WHITE && gray[3]==WHITE && gray[1]==WHITE && gray[0]==BLACK)
  {
    L298_forward(20,0,10);
    return;
  }
  if(gray[4]==BLACK && gray[3]==WHITE && gray[1]==WHITE && gray[0]==WHITE)
  {
    L298_forward(0,20,10);
    return;
  }
  if(gray[4]==WHITE && gray[3]==BLACK && gray[1]==BLACK && gray[0]==BLACK && gray[2]==WHITE)
  {
    L298_forward(20,0,10);
    return;
  }
  if(gray[4]==BLACK && gray[3]==BLACK && gray[1]==BLACK && gray[0]==WHITE)
  {
    L298_forward(0,20,10);
    return;
  }
  if(gray[4]==WHITE && gray[3]==WHITE && gray[1]==BLACK && gray[0]==BLACK && gray[2]==WHITE)
  {
    L298_forward(20,0,10);
    return;
  }
  if(gray[4]==WHITE && gray[3]==WHITE && gray[1]==BLACK && gray[0]==WHITE && gray[2]==BLACK)
  {
    L298_forward(20,0,10);
    return;
  }
  if(gray[4]==BLACK && gray[3]==BLACK && gray[1]==WHITE && gray[0]==WHITE)
  {
    L298_forward(0,20,10);
    return;
  }
}
void main()
{
  int i = 0;
  LY=0;
  BSD=0;
  R=0;
  last_color=WHITE;
  now_color=WHITE;
  L298_init();
  InitUart();
  InitADC();
  while (1)
  {
    if(flag==1)
    {
      BSD=0;
      LY=1;
    }
    if(flag==2)
    {
      LY=0;
      BSD=1;
    }
    for (i = 0; i <= 4; ++i)
      ShowResult(i);
    last_color=now_color;
    now_color=gray[2];
    if(last_color==WHITE && now_color==BLACK)
      ++R;
    f();
  }
}
