/*********************************************************************************************
�������� ���� DS1302ʵʱʱ��
��д�ˣ� ���� Jaden
��дʱ�䣺����2018��9��01��
Ӳ��֧�֣�����STC12C2052AD 12MHz
�ӿ�˵��������  
�޸���־������
����NO.1-								
/*********************************************************************************************
˵����

/*********************************************************************************************/
#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int

uchar code LCD_CODE[]={'0','1','2','3','4','5','6','7','8','9'};



#define LCD  P0
sbit LCD_RD = P2^6;		//����/�������
sbit LCD_WR = P2^5;		//��/д����
sbit LCD_EN = P2^7;	//ʹ�ܲ���


sbit ACC0 = ACC^0;
sbit ACC7 = ACC^7;

sbit DS_SCLK = P3^6;		//ʱ���ź�
sbit DS_CE = P3^5;			//��λ��
sbit DS_IO = P3^4;			//���ݶ�

void LCD_init();
void DELAY_MS(uint T);
void LCD_write_data(uchar cmd);
void LCD_write_cmd(uchar cmd);
uchar LCD_read_bit_Busy(void);
void LCD_write_str(uchar x,uchar y, uchar *str);

void LCD_Display_time();
void read_data();
void init_DS1302();
uchar DS1302_read_dat(uchar add);
void DS1302_write_dat(uchar add, uchar dat);
uchar DS1302_read_byte(uchar cd);
void DS1302_write_byte(uchar cd);



uchar T_sec , T_sec1 , T_sec2;
uchar T_min , T_min1 , T_min2;
uchar T_hou , T_hou1 , T_hou2;
uchar T_dat , T_dat1 , T_dat2;
uchar T_mon , T_mon1 , T_mon2;
uchar T_week , T_week1;
uchar T_year , T_year1, T_year2;



/*********************************************************************************************
�����������뼶CPU��ʱ����
��  �ã�DELAY_MS (?);
��  ����1~65535����������Ϊ0��
����ֵ����
��  ����ռ��CPU��ʽ��ʱ�������ֵ��ͬ�ĺ���ʱ��
��  ע��Ӧ����1T��Ƭ��ʱi<600��Ӧ����12T��Ƭ��ʱi<125
/*********************************************************************************************/
void DELAY_MS(uint T)
{
	uint i;
	while(T-- !=0)
	{
		for(i=0;i<600;i++);
	}
}


/*********************************************************************************************
��������LCD-1602��ʼ������
��  �ã�LCD_init()
��  ��: ��
����ֵ����
��  ����
��  ע��
/*********************************************************************************************/
void LCD_init()
{
	LCD_write_cmd(0x38);		//16*2 ��ʾ��5*7 ����8 λ���ݽӿ�
	LCD_write_cmd(0x0C);		//��ʾ���������ر�
	LCD_write_cmd(0x06);		//���ֲ�������ַ�Զ�+1
	LCD_write_cmd(0x01);		//����
	DELAY_MS(5);


}
/*********************************************************************************************
��������LCD-1602дָ��
��  �ã�LCD_write_cmd(uchar cmd)
��  ��: д��ָ��
����ֵ����
��  ����
��  ע��
/*********************************************************************************************/
void LCD_write_cmd(uchar cmd)
{
	
	LCD_EN=0;			//ʹ�ܶ�����
	LCD_RD=0;			//1���ݣ�0ָ��
	LCD_WR=0;			//1��������0д����
	LCD=cmd;			//�����ݰڷ��϶�Ӧ��PIO��
	LCD_EN=1;			//ʹ�ܶ�����	
	DELAY_MS(1);	//��ʱ1ms����
	LCD_EN=0;			//ʹ�ܶ�����



}
/*********************************************************************************************
��������LCD-1602дָ����
��  �ã�LCD_write_data(uchar dat)
��  ��: д��ָ��
����ֵ����
��  ����
��  ע��
/*********************************************************************************************/
void LCD_write_data(uchar dat)
{
	LCD_EN=0;			//ʹ�ܶ�����
	LCD_RD=1;			//1���ݣ�0ָ��
	LCD_WR=0;			//1��������0д����
	LCD=dat;			//�����ݰڷ��϶�Ӧ��PIO��
	LCD_EN=1;			//ʹ�ܶ�����	
	DELAY_MS(1);	//��ʱ1ms����
	LCD_EN=0;			//ʹ�ܶ�����


}

/*********************************************************************************************
��������LCD-1602��״̬
��  �ã�uchar LCD_read_state(void)
��  ��: ��״̬
����ֵ������״̬λ
��  ����
��  ע��
/*********************************************************************************************/
uchar LCD_read_bit_Busy(void)
{
	bit busy_flag =0;
	LCD_EN=0;			//ʹ�ܶ�����
	LCD_RD=0;			//1���ݣ�0ָ��
	LCD_WR=1;			//1��������0д����
	LCD_EN=1;			//ʹ�ܶ�����
	busy_flag =(bit)(P0 & 0x80);//��ȡ��λ״̬�����Ϊ1����ʾæ��	
	DELAY_MS(1);	//��ʱ1ms����
	LCD_EN=0;			//ʹ�ܶ�����
	return busy_flag;	//����״̬λ

}

/*********************************************************************************************
��������LCD-1602д���ֽ�
��  �ã�LCD_write_str(uchar x,uchar y, uchar dat)
��  ��: ��״̬
����ֵ������״̬λ
��  ����
��  ע��
/*********************************************************************************************/
void LCD_write_str(uchar x,uchar y, uchar *str)
{
	uchar addr;
	if( LCD_read_bit_Busy()!=1)
	{
		if(x==1)
		{
			addr=0x00+y;	
		}
		if(x==2)
		{
			addr=0x40+y;
		}
		LCD_write_cmd(addr|0x80);
		while(*str!='\0')
		{
			LCD_write_data(*str++);	
		}
	}
}

/*********************************************************************************************

/*********************************************************************************************
��������DS1302д�ֽ�
��  �ã�DS1302_write_byte(uchar cd)
��  ��: д���ֽ�
����ֵ����
��  ����
��  ע��
/*********************************************************************************************/
void DS1302_write_byte(uchar cd)
{
	uchar i;
	ACC = cd;
	for(i=8;i>0;i--)
	{
		DS_IO = ACC0;
		DS_SCLK =1;
		DS_SCLK=0;
		ACC=ACC>>1;	
	}
}

/*********************************************************************************************
��������DS1302���ֽ�
��  �ã�ucahr DS1302_read_byte(void)
��  ��: ��ȡ�ֽ�
����ֵ�����ض��������ֽ�
��  ����
��  ע��
/*********************************************************************************************/
uchar DS1302_read_byte()
{
	uchar i;
	for(i=8;i>0;i--)
	{
		ACC=ACC>>1;
		ACC7 = DS_IO;
		DS_SCLK = 1;
		DS_SCLK = 0;
	}
	return (ACC);
}

/*********************************************************************************************
��������DS1302д����
��  �ã�DS1302_write_dat(uchar add,uchar dat)
��  ��: д������
����ֵ��void
��  ����
��  ע��
/*********************************************************************************************/
void DS1302_write_dat(uchar add, uchar dat)
{
	DS_CE=0;
	DS_SCLK=0;
	DS_CE=1;
	DS1302_write_byte(add);
	DS1302_write_byte(dat);


	DS_SCLK=1;
	DS_CE=0;

}

/*********************************************************************************************
��������DS1302������
��  �ã�DS1302_write_dat(uchar add)
��  ��: д��ָ��
����ֵ����ȡ����
��  ������������
��  ע��
/*********************************************************************************************/
uchar DS1302_read_dat(uchar add)
{
	uchar dat;
	DS_CE=0;
	DS_SCLK=0;
	DS_CE=1;
	DS1302_write_byte(add);
	dat=DS1302_read_byte();


	DS_SCLK=1;
	DS_CE=0;
	return dat;

}


/*********************************************************************************************
����������ʼ��DS1302����
��  �ã�init_DS1302()
��  ��:
����ֵ:
��  ����
��  ע��
/*********************************************************************************************/
void init_DS1302()
{
	DS1302_write_dat(0x8e,0x00); //�ر�д����
	DS1302_write_dat(0x90,0xaa); //ʹ�����������ܣ�4K��������
	DS1302_write_dat(0x80,0x00); //д��������
	DS1302_write_dat(0x82,0x43); //д�����
	DS1302_write_dat(0x84,0x20); //д��Сʱ
	DS1302_write_dat(0x86,0x01); //д������
	DS1302_write_dat(0x88,0x09); //д���·�
	DS1302_write_dat(0x8a,0x06); //д������
	DS1302_write_dat(0x8c,0x18); //д����
	DS1302_write_dat(0x8e,0x80); //��д����
	

	
	
}


/*********************************************************************************************
����������ȡ�������
��  �ã�read_data()
��  ��:	
����ֵ:
��  ����
��  ע��
/*********************************************************************************************/
void read_data()
{
	T_sec = DS1302_read_dat(0x81);
	T_sec1 = T_sec & 0x0f;
	T_sec2 = (T_sec >> 4);

	T_min = DS1302_read_dat(0x83);
	T_min1 = T_min & 0x0f;
	T_min2 = (T_min >> 4);

	T_hou = DS1302_read_dat(0x85);
	T_hou1 = T_hou & 0x0f;
	T_hou2 = (T_hou  >> 4);

	T_dat = DS1302_read_dat(0x87);
	T_dat1 = T_dat & 0x0f;
	T_dat2 = (T_dat >> 4);

	T_mon= DS1302_read_dat(0x89);
	T_mon1 = T_mon & 0x0f;
	T_mon2 = (T_mon >> 4);

	T_week = DS1302_read_dat(0x8b);
	T_week1 = T_week & 0x0f;

    T_year = DS1302_read_dat(0x8d);
	T_year1 = T_year & 0x0f;
	T_year2 = (T_year>>4);




	



	

	



	
	
}

/*********************************************************************************************
��������LCD_1602��ʾ���ʱ��
��  �ã�LCD_Displsy_time()
��  ��:	
����ֵ:
��  ����
��  ע��
/*********************************************************************************************/
void LCD_Display_time()
{
	LCD_write_cmd(0x88);
	LCD_write_data(LCD_CODE[T_year2]);

	LCD_write_cmd(0x89);
	LCD_write_data(LCD_CODE[T_year1]);

	LCD_write_cmd(0x8a);
	LCD_write_data('/');

	LCD_write_cmd(0x8b);
	LCD_write_data(LCD_CODE[T_mon2]);

	LCD_write_cmd(0x8c);
	LCD_write_data(LCD_CODE[T_mon1]);

	LCD_write_cmd(0x8d);
	LCD_write_data('/');

	LCD_write_cmd(0x8e);
	LCD_write_data(LCD_CODE[T_dat2]);

	LCD_write_cmd(0x8f);
	LCD_write_data(LCD_CODE[T_dat1]);
/*******************************************/

	LCD_write_cmd(0xc5);
	LCD_write_data(LCD_CODE[T_week1]);

	LCD_write_cmd(0xc8);
	LCD_write_data(LCD_CODE[T_hou2]);

	LCD_write_cmd(0xc9);
	LCD_write_data(LCD_CODE[T_hou1]);

	LCD_write_cmd(0xca);
	LCD_write_data(':');

	LCD_write_cmd(0xcb);
	LCD_write_data(LCD_CODE[T_min2]);

	LCD_write_cmd(0xcc);
	LCD_write_data(LCD_CODE[T_min1]);


	LCD_write_cmd(0xcd);
	LCD_write_data(':');

	LCD_write_cmd(0xce);
	LCD_write_data(LCD_CODE[T_sec2]);

	LCD_write_cmd(0xcf);
	LCD_write_data(LCD_CODE[T_sec1]);
}


/*********************************************************************************************
��������������
��  �ã�main(void)
��  ��: 
����ֵ��
��  ����
��  ע��
/*********************************************************************************************/
void main()
{
	uchar i;
	LCD_init();
	DELAY_MS(10);
	//init_DS1302();
	DELAY_MS(10);
	LCD_write_str(1,0,"DS1302");
	LCD_write_str(2,1,"DAY-");
	DELAY_MS(10);
	i=0;
	while(1)
	{

		//DELAY_MS(10);
		if(i>=5)
		{
			i=0;
			read_data();

		}
		LCD_Display_time();
		i++;
     	

	}
}
