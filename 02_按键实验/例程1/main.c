/*********************************************************************************************
程序名： 　　 LED流水灯展示
编写人： 　　 Jaden
编写时间：　　2018 年 08月 16日
硬件支持：　　STC90C52  12MHz
接口说明：　　  
修改日志：　　
　　NO.1-								
/*********************************************************************************************
说明：

/*********************************************************************************************/
#include <reg52.h>
#define uint unsigned int 
#define uchar unsigned char

void DELAY_MS(uint t);
uint keyscan();
void leddisplay(uchar key_number);

uint number;

uchar code seg_table[]={0x3f,0x06,0x5b,0x4f,
                        0x66,0x6d,0x7d,0x07,
                        0x7f,0x6f,0x77,0x7c,
                        0x39,0x5e,0x79,0x71};

/*********************************************************************************************
函数名：毫秒级CPU延时函数
调  用：void leddisplay();
参  数：无
返回值：无
结  果：
备  注：
/*********************************************************************************************/
void leddisplay(uchar key_number)
{
	P0=seg_table[key_number];
}


/*********************************************************************************************
函数名：按键处理函数
调  用：uint keyscane();
参  数：无
返回值：按键代码
结  果：
备  注：
/*********************************************************************************************/
uint keyscane()
{
	uchar key_1,key_2,b,d;
	P2=0x0f;
	key_1=P2;

	if((key_1!=0x0f))
	{	
		DELAY_MS(10);
		key_1=P2;
		if(key_1!=0x0f)
		{
			P2=0xf0;
			key_2=P2;
			d=key_1|key_2;
			switch(d)
			{

			   case 0xee: b = 1; break;
			   case 0xed: b = 2; break;
			   case 0xeb: b = 3; break;
			   case 0xe7: b = 4; break;
			   case 0xde: b = 5; break;
			   case 0xdd: b = 6; break;
			   case 0xdb: b = 7; break;
			   case 0xd7: b = 8; break;
			   case 0xbe: b = 9; break;
			   case 0xbd: b = 10; break;
			   case 0xbb: b = 11; break;
			   case 0xb7: b = 12; break;
			   case 0x7e: b = 13; break;
			   case 0x7d: b = 14; break;
			   case 0x7b: b = 15; break;
			   case 0x77: b = 16; break;
			   default:   b = 0 ; break;
	   		}	
		
			
		}
		return(b);
		
	}
	
}


/*********************************************************************************************
函数名：毫秒级CPU延时函数
调  用：DELAY_MS (?);
参  数：1~65535（参数不可为0）
返回值：无
结  果：占用CPU方式延时与参数数值相同的毫秒时间
备  注：应用于1T单片机时i<600，应用于12T单片机时i<125
/*********************************************************************************************/
void DELAY_MS(uint t)
{

	uint i;
	while(t--!=0)
	{
		for(i=600;i>0;i--);
	}
}

/*********************************************************************************************
函数名：主函数
调  用：无
参  数：无
返回值：无
结  果：程序开始处，无限循环
备  注：
/**********************************************************************************************/
void main()
{

	while(1)
	{
		number=keyscane();	
		leddisplay(number);
	}
}
