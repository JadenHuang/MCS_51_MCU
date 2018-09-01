/*********************************************************************************************
程序名： 　　 DS1302实时时钟
编写人： 　　 Jaden
编写时间：　　2018年9月01日
硬件支持：　　STC12C2052AD 12MHz
接口说明：　　  
修改日志：　　
　　NO.1-								
/*********************************************************************************************
说明：

/*********************************************************************************************/
#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int

uchar code LCD_CODE[]={'0','1','2','3','4','5','6','7','8','9'};



#define LCD  P0
sbit LCD_RD = P2^6;		//数据/命令操作
sbit LCD_WR = P2^5;		//读/写操作
sbit LCD_EN = P2^7;	//使能操作


sbit ACC0 = ACC^0;
sbit ACC7 = ACC^7;

sbit DS_SCLK = P3^6;		//时钟信号
sbit DS_CE = P3^5;			//复位端
sbit DS_IO = P3^4;			//数据端

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
函数名：LCD-1602初始化函数
调  用：LCD_init()
参  数: 无
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void LCD_init()
{
	LCD_write_cmd(0x38);		//16*2 显示，5*7 点阵，8 位数据接口
	LCD_write_cmd(0x0C);		//显示器开，光标关闭
	LCD_write_cmd(0x06);		//文字不动，地址自动+1
	LCD_write_cmd(0x01);		//清屏
	DELAY_MS(5);


}
/*********************************************************************************************
函数名：LCD-1602写指令
调  用：LCD_write_cmd(uchar cmd)
参  数: 写入指令
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void LCD_write_cmd(uchar cmd)
{
	
	LCD_EN=0;			//使能端拉低
	LCD_RD=0;			//1数据，0指令
	LCD_WR=0;			//1读操作，0写操作
	LCD=cmd;			//将数据摆放上对应的PIO口
	LCD_EN=1;			//使能端拉高	
	DELAY_MS(1);	//延时1ms左右
	LCD_EN=0;			//使能端拉低



}
/*********************************************************************************************
函数名：LCD-1602写指数据
调  用：LCD_write_data(uchar dat)
参  数: 写入指令
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void LCD_write_data(uchar dat)
{
	LCD_EN=0;			//使能端拉低
	LCD_RD=1;			//1数据，0指令
	LCD_WR=0;			//1读操作，0写操作
	LCD=dat;			//将数据摆放上对应的PIO口
	LCD_EN=1;			//使能端拉高	
	DELAY_MS(1);	//延时1ms左右
	LCD_EN=0;			//使能端拉低


}

/*********************************************************************************************
函数名：LCD-1602读状态
调  用：uchar LCD_read_state(void)
参  数: 读状态
返回值：返回状态位
结  果：
备  注：
/*********************************************************************************************/
uchar LCD_read_bit_Busy(void)
{
	bit busy_flag =0;
	LCD_EN=0;			//使能端拉低
	LCD_RD=0;			//1数据，0指令
	LCD_WR=1;			//1读操作，0写操作
	LCD_EN=1;			//使能端拉高
	busy_flag =(bit)(P0 & 0x80);//获取高位状态，如果为1，标示忙。	
	DELAY_MS(1);	//延时1ms左右
	LCD_EN=0;			//使能端拉低
	return busy_flag;	//返回状态位

}

/*********************************************************************************************
函数名：LCD-1602写入字节
调  用：LCD_write_str(uchar x,uchar y, uchar dat)
参  数: 读状态
返回值：返回状态位
结  果：
备  注：
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
函数名：DS1302写字节
调  用：DS1302_write_byte(uchar cd)
参  数: 写入字节
返回值：无
结  果：
备  注：
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
函数名：DS1302读字节
调  用：ucahr DS1302_read_byte(void)
参  数: 读取字节
返回值：返回读出来的字节
结  果：
备  注：
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
函数名：DS1302写数据
调  用：DS1302_write_dat(uchar add,uchar dat)
参  数: 写入数据
返回值：void
结  果：
备  注：
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
函数名：DS1302读数据
调  用：DS1302_write_dat(uchar add)
参  数: 写入指令
返回值：读取数据
结  果：返回数据
备  注：
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
函数名：初始化DS1302函数
调  用：init_DS1302()
参  数:
返回值:
结  果：
备  注：
/*********************************************************************************************/
void init_DS1302()
{
	DS1302_write_dat(0x8e,0x00); //关闭写保护
	DS1302_write_dat(0x90,0xaa); //使用两个二极管，4K限流电阻
	DS1302_write_dat(0x80,0x00); //写入秒数据
	DS1302_write_dat(0x82,0x43); //写入分钟
	DS1302_write_dat(0x84,0x20); //写入小时
	DS1302_write_dat(0x86,0x01); //写入日期
	DS1302_write_dat(0x88,0x09); //写入月份
	DS1302_write_dat(0x8a,0x06); //写入星期
	DS1302_write_dat(0x8c,0x18); //写入年
	DS1302_write_dat(0x8e,0x80); //打开写保护
	

	
	
}


/*********************************************************************************************
函数名：读取相关数据
调  用：read_data()
参  数:	
返回值:
结  果：
备  注：
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
函数名：LCD_1602显示相关时间
调  用：LCD_Displsy_time()
参  数:	
返回值:
结  果：
备  注：
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
函数名：主函数
调  用：main(void)
参  数: 
返回值：
结  果：
备  注：
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
