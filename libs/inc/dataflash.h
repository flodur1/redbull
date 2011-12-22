//dataflash.h 

#ifndef DATAFLASH_H 
#define DATAFLASH_H 

#include "stm32f10x_conf.h"
#define DF_PAGE_SIZE 528 
#define DF_READ_BUFFER 1 
#define DF_WRITE_BUFFER 0


#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)


#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)

/* ������ΪAVR��Ƭ��SPI�ڷ���AT45DB161D�ṩһ��ӿں��� 
    ͨ����һ��ӿں����ɷǳ����׵���ɶ�AT45DB161D�Ķ�д���� 
    AT45DB161D��ҳ��֯�Ͳ�������д���ȵȣ��ڲ���FLASH�洢���� 
    ÿҳΪ528�ֽ�(������;ʱ��ͨ���������ó�512�ֽ�)�����ڲ�������4096ҳ����4096*528=2162688�ֽ� 
    ������Ϊ���ṩ��д��2162688�ֽڴ洢�������Բ���������ʹ����Щ�������û��ɲ��ؿ���AT45DB161D 
    �ڲ��Ĵ洢����֯�ṹ����ͬ��дһ���ļ�һ�����ж�д������ 
*/ 
/*����SPI�ڳ�ʼ����������������������Щ�ӿں�������֮ǰ�õ�����*/ 
void SPI_FLASH_Init(void);
/*��������ʼ��������addrָ���������Ķ������Ŀ�ʼ��ȡλ�� 
�����ڲ�ά��һ����ǰ��ȡ���������û�ÿ��һ�ֽڸü�������һ*/ 
void df_read_open(u32 addr); 
/*д������ʼ��������addrָ����������д�����Ŀ�ʼд��λ�� 
�����ڲ�ά��һ����ǰд����������û�ÿд��һ���ֽڸü�������һ*/ 
void df_write_open(u32 addr); 

/*�˺����ӵ�ǰ��λ�ö�ȡһ�ֽں󷵻أ��ڲ��Ķ���������һ*/ 
u8 df_getc(void); 
/*�˺�����ǰдλ��д��һ�ֽڵ����ݣ���ʹ�ڲ�д��������һ*/ 
void df_putc(u8 c); 
/*�˺����ӵ�ǰ��λ�ö�ȡsize���ֽڵ����ݵ�������buf,��ʹ�ڲ�����������size*/ 
void df_read(u8 *buf,u8 size); 
/*�˺����ӻ�����buf��ǰдλ��д��size�ֽڵ����ݣ���ʹ�ڲ�д��������size*/ 
void df_write(u8 *buf,u8 size); 
/*������ǰ�������������ô˺���ǰ�����ѵ���df_read_open*/ 
void df_read_seek(u32 addr); 
/*������ǰд������,���ô˺���ǰ�����ѵ���df_write_open*/ 
void df_write_seek(u32 addr); 
/*�رն�����*/ 
void df_read_close(void); 
/*�ر�д���������е�д�������ɺ������ô˺�������д�������Ա������ܹ������ı��浽AT45DB161D���洢������*/ 
void df_write_close(void); 

void df_read_deviceid(u8 *buf); 


#endif 
