//=========================================================================================================================================
#include "stm32f10x_conf.h"
#include "ads7843drv.h"
#include "delay.h"
#include "math.h"
//====================================================================================

#define TRUE 1
#define FALSE 0

coordinate ScreenSample[3] =	{
                                            { 3388, 920 },
											{ 895, 914 },
                                            { 1767, 3115 }
                                } ;

	 //LCD�� ��Ӧ�ĵ� �����ڡ�H��ֵ
coordinate DisplaySample[3] =   {
                                            { 45, 45 },
											{ 45, 270},
                                            { 190,190}
	                            } ;

 void TP_Init(void)
{
   // IODIR1 = 0x00;
  //  IODIR1 = IODIR1 | MASK_CS | MASK_DCLK | MASK_DIN;
  	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 //����	
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

}

void Delayus( int k)
{
    int j;
    for(j=k;j > 0;j--);    
}
//====================================================================================
 void ReBack()
{
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP ;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
}
static void WR_CMD (unsigned char cmd) 
{
    unsigned char buf;
    unsigned char i;
    TP_CS(1);
    TP_DIN(0);
    TP_DCLK(0);
    TP_CS(0);
    for(i=0;i<8;i++) 
    {
        buf=(cmd>>(7-i))&0x1;
        TP_DIN(buf);
        Delayus(5);
        TP_DCLK(1);
        Delayus(5);
        TP_DCLK(0);
    }
}
//====================================================================================
static unsigned short RD_AD(void) 
{
    unsigned short buf=0,temp;
    unsigned char i;
    TP_DIN(0);
    TP_DCLK(1);
    for(i=0;i<12;i++) 
    {
        Delayus(5);
        TP_DCLK(0);         
        Delayus(5);   
        temp= (TP_DOUT) ? 1:0;
        buf|=(temp<<(11-i));
        
        Delayus(5);
        TP_DCLK(1);
    }
    TP_CS(1);
    buf&=0x0fff;
    return(buf);
}
//====================================================================================
int Read_X(void) 
{ 
    int i;
    WR_CMD(CHX);
   // while(TP_BUSY);
    Delayus(5);
    i=RD_AD();
    return i;   
}
//====================================================================================
int Read_Y(void) 
{ 
    int i;
    WR_CMD(CHY);
    //while(TP_BUSY);
    Delayus(5);
    i=RD_AD();
    return i;    
}
//====================================================================================
void TP_GetAdXY(int *x,int *y) 
{
    int adx,ady;
    adx=Read_X();
    ady=Read_Y();
    *x=adx;
    *y=ady;
}

void TP_DrawPoint(u8 x,u16 y)
{
	if(x>220&&y<12)
	{
		ili9320_Clear(0xffff);
	}else 
	{

		ili9320_SetPoint(x,y,0xf800);     //���ĵ� 
		ili9320_SetPoint(x+1,y,0xf800);
		ili9320_SetPoint(x,y+1,0xf800);
		ili9320_SetPoint(x+1,y+1,0xf800);	
	}		  	
}		
static int TP_X[1],TP_Y[1];

 
#define  VALUE 2 //��ֵ

coordinate *Read_Ads7846(void)
{
    coordinate  Screen ;
    int m0,m1,m2;
	u8 count=0;
	u16 databuffer[2][9]={{0},{0}};//������
	u16 temp[3];

 
	 	TP_Init(); 
		
    do					  //ѭ������9��
	{		   
            TP_GetAdXY(TP_X,TP_Y);  
			databuffer[0][count]=TP_X[0];  //X
			databuffer[1][count]=TP_Y[0];
			count++;  
	}
	while(!TP_INT_IN&& count<9);  //��Ϊ1ʱ�����Ͳ�����  ��ʼ�Զ����BUG
	ReBack();
	if(count==9)//һ��Ҫ����9������,������
	{  
	 temp[0]=(databuffer[0][0]+databuffer[0][1]+databuffer[0][2])/3;
	 temp[1]=(databuffer[0][3]+databuffer[0][4]+databuffer[0][5])/3;
	 temp[2]=(databuffer[0][6]+databuffer[0][7]+databuffer[0][8])/3;
	 m0=temp[0]-temp[1];
	 m1=temp[1]-temp[2];
	 m2=temp[2]-temp[0];

	 m0=m0>0?m0:(-m0);
	 m1=m1>0?m1:(-m1);
	 m2=m2>0?m2:(-m2);
	 if(m0>VALUE&&m1>VALUE&&m2>VALUE) return 0;
	 if(m0<m1){if(m2<m0) Screen.x=(temp[0]+temp[2])/2;
	             else Screen.x=(temp[0]+temp[1])/2;	}
	 else if(m2<m1) Screen.x=(temp[0]+temp[2])/2;
	             else Screen.x=(temp[1]+temp[2])/2;



     temp[0]=(databuffer[1][0]+databuffer[1][1]+databuffer[1][2])/3;
	 temp[1]=(databuffer[1][3]+databuffer[1][4]+databuffer[1][5])/3;
	 temp[2]=(databuffer[1][6]+databuffer[1][7]+databuffer[1][8])/3;
	 m0=temp[0]-temp[1];
	 m1=temp[1]-temp[2];
	 m2=temp[2]-temp[0];

	 m0=m0>0?m0:(-m0);
	 m1=m1>0?m1:(-m1);
	 m2=m2>0?m2:(-m2);
	 if(m0>VALUE&&m1>VALUE&&m2>VALUE) return 0;

	 if(m0<m1){if(m2<m0) Screen.y=(temp[0]+temp[2])/2;
	             else Screen.y=(temp[0]+temp[1])/2;	}
	 else if(m2<m1) Screen.y=(temp[0]+temp[2])/2;
	             else Screen.y=(temp[1]+temp[2])/2;
	  	return &Screen;
		}
	 Screen.x=0;  
	 Screen.y=0;
	return &Screen; 
}
	 

	  //ֻ����LCD�ʹ�����������Ƕȷǳ�Сʱ�������������湫ʽ
	  //3����������벻̫������������Ե�����ǵļ�������㹻��->
		

	  //Һ�����ϵĲ�����
/*
����2������
matrix matrix ;
coordinate  display ;

setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix ) ;  //����ֵ�õ�����	   
getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;
display.x display.y  �������ĵ�������    */


unsigned char setCalibrationMatrix( coordinate * displayPtr,
                          coordinate * screenPtr,
                          matrix * matrixPtr)
{

    unsigned char  retValue = 0 ;


    
    matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                         ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

    if( matrixPtr->Divider == 0 )
    {
        retValue = 1;
    }
    else
    {
        matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                        ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

        matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                        ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;

        matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                        ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    
        matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                        ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
    }
 
    return( retValue ) ;
}
	//�����1 ����� �ǲ��ɹ��ġ�����0�ǳɹ���
unsigned char getDisplayPoint(coordinate * displayPtr,
                     coordinate * screenPtr,
                     matrix * matrixPtr )
{
    unsigned char  retValue =0 ;


    if( matrixPtr->Divider != 0 )
    {
	if(screenPtr->x==0&&screenPtr->y==0)
	  {
	  displayPtr->x=999;
	  displayPtr->y=999;
	  retValue = 1;  //û�а��� ���ɹ�
	  }
	else
		{
        displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                          (matrixPtr->Bn * screenPtr->y) + 
                           matrixPtr->Cn 
                        ) / matrixPtr->Divider ;

        displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                          (matrixPtr->En * screenPtr->y) + 
                           matrixPtr->Fn 
                        ) / matrixPtr->Divider ;
		}
    }
    else
    {
        retValue = 1;
    }

    return(retValue);
} 

//X�������˲��㷨
u8 TouchX_Penfilter(u16 *px)
{
u8 retval;
u16 tempx;
u16 dx;
static u8 count=0;
static u16 temp[2];
count++;
if(count>2)
{
count=2;
tempx=(temp[0]+*px)/2;
dx=(temp[1]>tempx)?(temp[1]-tempx):(tempx-temp[1]);
if(dx>10)
{
*px=temp[1];
retval=FALSE;
count=0;
}
else
{
temp[0]=temp[1];
temp[1]=*px;
retval=TRUE;
}
}
else
{
temp[0]=temp[1];
temp[1]=*px;
retval=FALSE;
}
return retval;
}

//Y�������˲��㷨
u8 TouchY_Penfilter(u16 *py)
{
u8 retval;
u16 tempy;
u16 dy;
static u8 count=0;
static u16 temp[2];
count++;
if(count>2)
{
count=2;
tempy=(temp[0]+*py)/2;
dy=(temp[1]>tempy)?(temp[1]-tempy):(tempy-temp[1]);
if(dy>10)
{
*py=temp[1];
retval=FALSE;
count=0;
}

else
{
temp[0]=temp[1];
temp[1]=*py;
retval=TRUE;
}
}
else
{
temp[0]=temp[1];
temp[1]=*py;
retval=FALSE;
}
return retval;
}

matrix mat ;
coordinate  display ;
Touch_Key  Touch_Pen;
//1�����˲��ɹ�

u8 Touch_Coordinate(void)
{
getDisplayPoint(&display,Read_Ads7846(),&mat);    
if(TouchX_Penfilter(&display.x)||TouchY_Penfilter(&display.y) )
{
Touch_Pen.NowX=display.x; 	 //����ˢ������
Touch_Pen.NowY=display.y;
return 1;
}
else
{
Touch_Pen.NowX=display.x; 
Touch_Pen.NowY=display.y;
return 0;
}
}

//�����������
//type:������Ӧ����
//0,����,����,����չ.һ��Ҫ�����ɿ��ŷ���
//1,����,����,����չ.����������/���Ӳ���
//2,��չ����֧��:
//��:MOVE_LEFT,MOVE_RIGHT,MOVE_UP,MOVE_DOWNʹ��
u8 Touch_Key_Press(u8 type)
{   
	u16 tempx,tempy;//��ʱ����X,Y���� 	 	 
	u8 ml=0,mr=0,mu=0,md=0;//�ĸ��������ƶ�����  
	u8 first=1;	  		 	    					   		 
	//�������ǰ��µ�
	//�ֶ�����������ʱ��,������Ҫ15ms�����˳����ѭ��	   	
	do
	{	    
		Touch_Pen.Key_NowState=UP;  //����״̬��Ϊ�ɿ�    
		if(Touch_Coordinate())      //�ɹ�����
		{	 
			if(first)
			{
			Touch_Pen.LastX=Touch_Pen.NowX; 
            Touch_Pen.LastY=Touch_Pen.NowY;
			tempx=Touch_Pen.NowX;
			tempy=Touch_Pen.NowY;	
			first=0;  //������
			}else if(type==2) //��չ����														  
			{	  
				if(tempx>Touch_Pen.NowX)ml++;
				else mr++;	   	
				if(tempy>Touch_Pen.NowY)mu++;
				else md++;
				//�趨һ������ֵ,������һ���ƶ��������ֵ,���һ�δ������ֵ
				//��Ϊ������������.400��ʱ��,��ӳ�Ƚ���
				if(fabs(tempx-Touch_Pen.NowX)>50||fabs(tempy-Touch_Pen.NowY)>50)//�ж���
				{
					ml=mr=mu=md=0;//ȫ�����
					tempx=Touch_Pen.NowX=Touch_Pen.LastX;//���긴λ
					tempy=Touch_Pen.NowY=Touch_Pen.LastY;
					break;//�˳����ݲɼ�,���Ϊ��ɼ�
				}   
				tempx=Touch_Pen.NowX;tempy=Touch_Pen.NowY;//ת����ʱ���� 
			}else if(type==1)break; 
			//printf("X:%d Y:%d\n",Pen_Point.X,Pen_Point.Y); 
		}
		delay_ms(10);//10ms���� 						   		     								 	
	}while(!TP_INT_IN||Touch_Pen.Key_NowState==DOWN);//PEN=0���߰���״̬Ϊ����״̬;
	delay_ms(50);
	Touch_Pen.Key_NowState=UP;//����״̬��Ϊ�ɿ�	 
 	//����/����չ ��ֵ����  
	//�� һ����Χ��
	if(fabs(tempx-Touch_Pen.LastX)<=10&&fabs(tempy-Touch_Pen.LastY)<=10||type<2)//���ΰ���/����չ��������
	{	 
		return NONE;//û���ƶ�  
	}	 
	//��չ��ֵ����
	if(fabs(tempx-Touch_Pen.LastX)<=25&&fabs(tempy-Touch_Pen.LastY)<=25)
	return 0;      //������������Ҫ����500

	if(fabs(ml-mr)>fabs(mu-md))//���� ����
	{
		if(fabs(tempx-Touch_Pen.LastX)>fabs(tempy-Touch_Pen.LastY))//��������
		{
			if(tempx>Touch_Pen.LastX)return RIGHT;
			else return LEFT; 
		}else						//����������
		{
			if(tempy>Touch_Pen.LastY)return DOWN;
			else return UP;
		}
	}else
	{
		if(fabs(tempy-Touch_Pen.LastY)>fabs(tempx-Touch_Pen.LastX))//��������
		{	    
			if(tempy>Touch_Pen.LastY)return DOWN;
			else return UP;			 
		}else						//����������
		{	  
			if(tempx>Touch_Pen.LastX)return RIGHT;
			else return LEFT;
		}
	}   	  
}

//�жϴ����ǲ�����ָ������֮��
//(x1,y1):��ʼ����
//(x2,y2):��������
//����ֵ :1,�ڸ�������.0,���ڸ�������.
u8 Is_In_Area(u8 x1,u16 y1,u8 x2,u16 y2)
{
if(Touch_Pen.NowX<=x2&&Touch_Pen.NowX>=x1&&Touch_Pen.NowY<=y2&&Touch_Pen.NowY>=y1)return 1;
else return 0;
}  


