#ifndef _ADS7843DRV_H_
#define _ADS7843DRV_H_


#define UP 10
#define DOWN 11
#define LEFT 12
#define RIGHT 13
#define NONE 14

typedef struct 
{
	u16 LastX;  //�ϴε�����
	u16 LastY;
	u16 NowX; //���ڵ�����
	u16 NowY;						   	    
	u8  Key_NowState;// ���ڱʵ�״̬
	u8  Key_LastState;//��һ�αʵ�״̬
}Touch_Key;

typedef	  struct POINT 
	  {
	  u16 x;
	  u16 y;
	  }coordinate;


typedef struct Matrix {
						
        long double An,  
                   Bn,     
                   Cn,   
                   Dn,    
                   En,    
                   Fn,     
                   Divider ;
                   } matrix ;


extern coordinate ScreenSample[3];


//LCD�� ��Ӧ�ĵ� �����ڡ�H��ֵ
extern coordinate DisplaySample[3];
// A/D ͨ��ѡ�������ֺ͹����Ĵ���
#define	CHX 	0x90 	//ͨ��Y+��ѡ�������	
#define	CHY 	0xd0	//ͨ��X+��ѡ������� 

#define TP_DCLK(a)	\
						if (a)	\
						GPIOB->ODR |=(1<<13);\
						else		\
						GPIOB->ODR &=~(1<<13)
	//��MP3��CS�˳�ͻ
#define TP_CS(a)	\
						if (a)	\
						GPIOB->ODR |=(1<<12);\
						else		\
						GPIOB->ODR &=~(1<<12)
#define TP_DIN(a)	\
						if (a)	\
						GPIOB->ODR |=(1<<15);\
						else		\
						GPIOB->ODR &=~(1<<15)
// b0 int b1 busy a6 miso a7 mosi  a5 sck a4 t_cs

#define TP_DOUT	 (GPIOB->IDR & (1<<14))  //MISO	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//( IOPIN1 & MASK_DOUT )	//��������
#define TP_BUSY	 (GPIOG->IDR & (1<<8))  //	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)//( IOPIN1 & MASK_DOUT )	//��������
#define TP_INT_IN   (GPIOG->IDR &(1<<7)) //( IOPIN1 & MASK_INT )


void TP_Init(void);
void TP_GetAdXY(int *x,int *y);
void ReBack(void);
coordinate *Read_Ads7846(void);
unsigned char setCalibrationMatrix( coordinate * displayPtr,coordinate * screenPtr,matrix * matrixPtr);
unsigned char getDisplayPoint(coordinate * displayPtr,coordinate * screenPtr,matrix * matrixPtr );


#endif





