
#include "at45db161d.h"
#include "dataflash.h"
/*****������ȫ�ֱ�������******/
static u16 g_CurReadPage;//��ǰ����ҳ��ַ
static u16 g_CurReadByte;//��ǰ�����ֽ�(ҳ�е�ַ)
static u16 g_CurWritePage;//��ǰд��ҳ��ַ
static u16 g_CurWriteByte;//��ǰд���ֽڵ�ַ(ҳ�е�ַ)

/*****�������ڲ����õĽӿں���******/

//��SPI�����һ�ֽ�����
 u8 spi_write(u8 data)
{

  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, data);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}


//��Ⲣ�ȴ�����æ״̬,8���ŷ������û�� RDY/BUSY���� ΪЩͨ����״̬�Ĵ��������æ״̬
static void df_wait_busy(void)
{
u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  spi_write(STATUS_REGISTER);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = spi_write(0xa5);

  }
  while ((FLASH_Status & 0x80) == RESET); /* Busy in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

//�����洢��ָ��ҳ����������(BUFFER1)
static void load_page_to_buffer(u16 page,u8 buffer)
{
SPI_FLASH_CS_LOW();

if(buffer == DF_READ_BUFFER) 
spi_write(MM_PAGE_TO_B1_XFER); 
else
spi_write(MM_PAGE_TO_B2_XFER);
spi_write((u8)(page >> 6));
spi_write((u8)(page << 2));
spi_write(0x00);

SPI_FLASH_CS_HIGH();

df_wait_busy();
}

//��д����������д�뵽���洢����ָ��ҳ
static void write_page_from_buffer(u16 page,u8 buffer)
{
SPI_FLASH_CS_LOW();

if(buffer ==DF_WRITE_BUFFER) 
spi_write(B2_TO_MM_PAGE_PROG_WITH_ERASE);
else
spi_write(B1_TO_MM_PAGE_PROG_WITH_ERASE);
spi_write((u8)(page>>6));
spi_write((u8)(page<<2));
spi_write(0x00); // don't cares

SPI_FLASH_CS_HIGH();

df_wait_busy();
}

//�Ӷ�������������
static void read_buffer(u16 addr,u8 *data,u8 size)
{
u8 i;

SPI_FLASH_CS_LOW();

spi_write(BUFFER_1_READ);
spi_write(0x00); 
spi_write((u8)(addr>>8));
spi_write((u8)addr); 
for(i=0;i<size;i++) 
data[i]=spi_write(0);

SPI_FLASH_CS_HIGH();
}

//������д��д������
static void write_buffer(u16 addr,u8 *data,u8 size)
{
u8 i;

SPI_FLASH_CS_LOW();

spi_write(BUFFER_2_WRITE);
spi_write(0x00); 
spi_write((u8)(addr>>8));
spi_write((u8)addr); 
for(i=0;i<size;i++) 
spi_write(data[i]);

SPI_FLASH_CS_HIGH();
}

/*****������Ϊ�ⲿ���ö��ṩ�Ľӿں���******/

void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable SPI1 and GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure SPI1 pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
								  
  /* Configure I/O for Flash Chip select */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}

//����ʼ�����ܺ���,addrΪ�򿪺�����ĳ�ʼ��ַ
void df_read_open(u32 addr)
{
g_CurReadPage=addr/DF_PAGE_SIZE; 
g_CurReadByte=addr%DF_PAGE_SIZE;
load_page_to_buffer(g_CurReadPage,DF_READ_BUFFER);
}

void df_write_open(u32 addr)
{
g_CurWritePage=addr/DF_PAGE_SIZE;
g_CurWriteByte=addr%DF_PAGE_SIZE;
load_page_to_buffer(g_CurWritePage,DF_WRITE_BUFFER);
}

u8 df_getc(void)
{
u8 c;

read_buffer(g_CurReadByte,&c,1);
g_CurReadByte++;
if(g_CurReadByte ==DF_PAGE_SIZE)
{
g_CurReadPage++;
load_page_to_buffer(g_CurReadPage,DF_READ_BUFFER);
g_CurReadByte=0;
}

return c;
}

void df_putc(u8 c)
{
write_buffer(g_CurWriteByte,&c,1);
g_CurWriteByte++;
if(g_CurWriteByte == DF_PAGE_SIZE)
{
g_CurWriteByte=0;
write_page_from_buffer(g_CurWritePage,DF_WRITE_BUFFER);
g_CurWritePage++;
load_page_to_buffer(g_CurWritePage,DF_WRITE_BUFFER);
}
}

void df_read(u8 *buf,u8 size)
{
u8 temp;
if((g_CurReadByte + size) > DF_PAGE_SIZE) //�����ǰҳδ��ȡ���ݲ���size�ֽ�
{
//����ǰҳʣ������
temp=DF_PAGE_SIZE - g_CurReadByte;
read_buffer(g_CurReadByte,buf,temp);

//װ����һҳ
load_page_to_buffer(++g_CurReadPage,DF_READ_BUFFER);

//����һҳ��ʣ������
g_CurReadByte=size-temp;
read_buffer(0,buf+temp,g_CurReadByte);
}
else //�����ǰҳ������size�ֽ�
{
read_buffer(g_CurReadByte,buf,size);
g_CurReadByte+=size;

//�����ǰҳ������ȫ������
if(g_CurReadByte==DF_PAGE_SIZE)
{
load_page_to_buffer(++g_CurReadPage,DF_READ_BUFFER);
g_CurReadByte=0; 
}
} 
}

void df_write(u8 *buf,u8 size)
{
u8 temp;

if((g_CurWriteByte + size) > DF_PAGE_SIZE) //�����ǰҳδд�ռ䲻��size�ֽ�
{
//д��ǰҳʣ��ռ������
temp=DF_PAGE_SIZE - g_CurWriteByte;
write_buffer(g_CurWriteByte,buf,temp);
//���浱ǰҳ
write_page_from_buffer(g_CurWritePage,DF_WRITE_BUFFER);
g_CurWritePage++;
load_page_to_buffer(g_CurWritePage,DF_WRITE_BUFFER);

//д�뵽��һҳ��Ӧ������
g_CurWriteByte=size-temp;
write_buffer(0,buf+temp,g_CurWriteByte);
}
else
{
write_buffer(g_CurWriteByte,buf,size);
g_CurWriteByte+=size;

//��������,д�뵽���洢��
if(g_CurWriteByte==DF_PAGE_SIZE)
{
g_CurWriteByte=0;
write_page_from_buffer(g_CurWritePage,DF_WRITE_BUFFER);
g_CurWritePage++; 
load_page_to_buffer(g_CurWritePage,DF_WRITE_BUFFER); 
}
} 
}

//����дָ��
void df_read_seek(u32 addr)
{
 df_read_close();
 df_read_open(addr);
}

//������ָ��
void df_write_seek(u32 addr)
{
 df_write_close();
 df_write_open(addr); 
}

void df_read_close(void)
{
 //�˴������κδ���
}

void df_write_close(void)
{
 write_page_from_buffer(g_CurWritePage,DF_WRITE_BUFFER); //����������д�뵽���洢��
}

