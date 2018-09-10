/*********************************************************************************************
程序名： 　　 AT24C02储存器读写程序
编写人： 　　 Jaden
编写时间：　　2018年9月10日
硬件支持：　　STC12C2052AD 12MHz
接口说明：　　  
修改日志：　　
　　NO.1-								
/*********************************************************************************************
说明：

/*********************************************************************************************/
#include<reg51.h>

#define uchar unsigned char
#define uint unsigned int

sbit I2C_SDA = P2^0;
sbit I2C_SCL = P2^1;

sbit KEY_1 = P1^0;


uchar Dat_Number;
uchar Current_Number;
uchar Number;

/*********************************************************************************************
函数名：毫秒级CPU延时函数
调  用：DELAY_MS (?);
参  数：1~65535（参数不可为0）
返回值：无
结  果：占用CPU方式延时与参数数值相同的毫秒时间
备  注：应用于1T单片机时i<600，应用于12T单片机时i<125
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
函数名：微秒级CPU延时函数
调  用：DELAY_US (?);
参  数：1~65535（参数不可为0）
返回值：无
结  果：占用CPU方式延时与参数数值相同的毫秒时间
备  注:
/*********************************************************************************************/
void DELAY_US(uchar T)
{
	while(T-- !=0);
}

/*********************************************************************************************
函数名：I2C开启函数
调  用：I2C_start();
参  数：
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void I2C_start()
{
	I2C_SDA =1;
	DELAY_US(4);
	I2C_SCL =1;
	DELAY_US(4);
	I2C_SDA =0;
	DELAY_US(4);
}

/*********************************************************************************************
函数名：I2C关闭函数
调  用：I2C_stop();
参  数：
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void I2C_stop()
{
	I2C_SDA =0;
	DELAY_US(4);
	I2C_SCL =1;
	DELAY_US(4);
	I2C_SDA =1;
	DELAY_US(4);
}

/*********************************************************************************************
函数名：I2C发送数据
调  用：I2C_send_byte(uchar dat);
参  数：
返回值：
结  果：
备  注：
/*********************************************************************************************/
void I2C_send_dat(uchar dat)
{
	uchar i,j;
	I2C_SCL =0;
	for(i=0;i<8;i++)
	{

		I2C_SDA =(bit)(dat & 0x80);
		dat=dat<<1;
		I2C_SCL =1;
		DELAY_US(12);
		I2C_SCL=0;
		DELAY_US(4);
	}
	I2C_SDA=1;
	DELAY_US(4);
	I2C_SCL=1;
	DELAY_US(4);
	j = 0;
	while(I2C_SDA==1 && j<200) j++;
	DELAY_US(4);
	I2C_SCL=0;
	DELAY_US(4);
}

/*********************************************************************************************
函数名：I2C读数据
调  用：uchar I2C_read_dat(void);
参  数：
返回值：无
结  果：
备  注：
/*********************************************************************************************/
uchar I2C_read_dat(void)
{
	uchar i,rdat;
	rdat=0x00;
	I2C_SDA=1;
	for(i=0;i<8;i++)
	{
		I2C_SCL=0;		
		rdat=rdat<<1;
		DELAY_US(4);
		I2C_SCL=1;
		DELAY_US(4);
		if(I2C_SDA==1)
		{
			rdat = rdat|0x01;
		}
		I2C_SCL=0;
		DELAY_US(4);
	}
	return rdat;
}

/*********************************************************************************************
函数名：发送应答位:1标示发送应答，0标示不发送应答；
调  用：I2C_send_ask(uchar A)
参  数：
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void I2C_send_ask(uchar A)
{
	if(A==0)
	{
		I2C_SDA=1;
		DELAY_US(4);
		I2C_SCL=1;
		DELAY_US(12);
		I2C_SCL=0;
		DELAY_US(4);
	}else
	{

		I2C_SDA=0;
		DELAY_US(4);
		I2C_SCL=1;
		DELAY_US(12);
		I2C_SCL=0;
		DELAY_US(4);
	}

	
}

/*********************************************************************************************
函数名：写数据到EEPROM内
调  用：AT24C02_write_Byte(uchar Address,uchar dat);
参  数：
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void AT24C02_write_Byte(uchar Address,uchar dat)
{
	I2C_start();
	I2C_send_dat(0xa0);
	I2C_send_dat(Address);
	I2C_send_dat(dat);
	I2C_stop();
}
/*********************************************************************************************
函数名：从EEPROM内读数据
调  用：uchar AT24C02_read_Byte(uchar Address);
参  数：
返回值：无
结  果：
备  注：
/*********************************************************************************************/
uchar AT24C02_read_Byte(uchar Address)
{
	I2C_start();
	I2C_send_dat(0xa0);
	I2C_send_dat(Address);

	I2C_start();
	I2C_send_dat(0xa1);
	Dat_Number=I2C_read_dat();
	I2C_send_ask(0);
	I2C_stop();
	
	return Dat_Number;
}

/*********************************************************************************************
函数名：按键扫描函数
调  用：void keyscane()
参  数：
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void keyscane()
{
	if(KEY_1==0)
	{
		DELAY_MS(20);
		if(KEY_1==0)
		{
			Number=Number++;
			DELAY_MS(50);
			if(Number ==255)
			{
				Number =0;
			}
		}
	}
}

/*********************************************************************************************
函数名：main()
调  用：
参  数：
返回值：
结  果：
备  注：
/*********************************************************************************************/
void main()
{
	Number = 0;
	Current_Number =0;
	AT24C02_write_Byte(5,0);
	DELAY_MS(100);
	Current_Number = AT24C02_read_Byte(5);
	while(1)
	{	
		AT24C02_write_Byte(5,1);
		DELAY_MS(100);	
		Current_Number = AT24C02_read_Byte(5);
		P0=Current_Number;
		DELAY_MS(100);

		AT24C02_write_Byte(5,2);
		DELAY_MS(100);	
		Current_Number = AT24C02_read_Byte(5);
		P0=Current_Number;


		AT24C02_write_Byte(5,4);
		DELAY_MS(100);	
		Current_Number = AT24C02_read_Byte(5);
		P0=Current_Number;


		AT24C02_write_Byte(5,8);
		DELAY_MS(100);	
		Current_Number = AT24C02_read_Byte(5);
		P0=Current_Number;
	}

}
