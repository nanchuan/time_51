#include<reg52.h>
#include<intrins.h>

#define uchar unsigned char
#define uint unsigned int
#define duan P1
sbit sclk=P3^7;
sbit IO=P3^5;
sbit res=P3^4;
sbit ACC0=ACC^0;
sbit ACC7=ACC^7;
sbit key1=P3^3;
sbit key2=P3^2;
sbit wei1=P3^0;
sbit wei2=P3^1;
sbit wei3=P3^5;
sbit wei4=P3^7;
sbit bel=P3^6;

uchar time[7];
uchar sec, min, hour, day, month,weak, year,i,ahour,amin;
uchar code table[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar a=0,n=0,m=0,c=0,al=0;
bit is=0,alarm,f=0,disp;


//延时函数，约2uS*I
void delay_us(uchar I) 
{
	while(--I); 
}

//延时函数，约1ms*i
void delay_ms(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}

void ds1302writebyte(uchar d)
{
	uchar i;
	ACC=d;
	sclk=0;
	delay_us(2);
	res=1;
	for(i=8;i>0;i--)
	{
		IO=ACC0;
		delay_us(2);
		sclk=1;
		delay_us(2);
		sclk=0;
		ACC>>=1;
	}
}

uchar ds1302readbyte()
{
	uchar i;
	sclk=0;
	delay_us(2);
	res=1;
	for(i=0;i<8;i++)
	{
		ACC>>=1;
		IO=1;
		ACC7=IO;
		delay_us(2);
		sclk=1;
		delay_us(2);
		sclk=0;
	}
	return (ACC);
}

uchar ds1302read(uchar cmd)
{
	uchar dat;
	res=0;
	sclk=0;
	delay_us(2);
	res=1;
	delay_us(2);
	ds1302writebyte(cmd);
	delay_us(20);
	dat=ds1302readbyte();
	sclk=1;
	res=0;
	return (dat);
}

void ds1302write(uchar cmd,uchar dat)
{
	res=0;
	sclk=0;
	delay_us(2);
	res=1;
	delay_us(2);
	ds1302writebyte(cmd);
	delay_us(20);
	ds1302writebyte(dat);
	sclk=1;
	res=0;
}

/*void ds1302writeburst(uchar *P)
{
	uchar i;
	ds1302write(0x8e,0x00);
	res=0;
	sclk=0;
	res=1;
	ds1302writebyte(0xbe);
	for(i=8;i>0;i--)
	{
		ds1302writebyte(*P);
		P++;
	}
	sclk=1;
	res=0;
} */

/*void ds1302readburst(uchar *P)
{
	uchar i;
	res=0;
	sclk=0;
	res=1;
	ds1302writebyte(0xbf);
	for(i=8;i>0;i--)
	{
		*P=ds1302readbyte();
		P++;
	}
	sclk=1;
	res=0;
}	*/
/*void ds1302Init(void)
{
	ds1302write(0x8e,0);
	ds1302write(0x80,0x01);	//写入秒
	ds1302write(0x8e,0);
	ds1302write(0x82,0x01);	//写入分
	ds1302write(0x8e,0);
	ds1302write(0x84,0x01);	//写入小时
	ds1302write(0x8e,0);
	ds1302write(0x86,0x01);	//写入日
	ds1302write(0x8e,0);
	ds1302write(0x88,0x09);	//写入月
	ds1302write(0x8e,0);
	ds1302write(0x8a,0x05);	//写入星期
	ds1302write(0x8e,0);
	ds1302write(0x8c,0x12);	//写入年
	ds1302write(0x8e,0x80);	
} 	*/	  

void ds1302start(uchar *P)
{
	uchar i,addr=0x80;
	ds1302write(0x8e,0x00);
	for(i=7;i>0;i--)
	{
		ds1302write(addr,*P);
		P++;
		addr +=2;
	}
	ds1302write(0x8e,0x80);	
}

void ds1302readtime(uchar time[])
{
	uchar i,addr=0x81;
	for(i=0;i<7;i++)
	{
		time[i] = ds1302read(addr);
		addr +=2;
	} 
}

uchar dectobcd(uchar dec)//转换bcd
{
	uchar bcd;
	bcd = 0;
	while(dec >= 10)
	{              
		dec -= 10;                         
		bcd++;
	} 
	bcd <<= 4;
	bcd |= dec;
	return bcd;
}
uchar bcdtodec(uchar bcd)
{
	uchar data1;
	data1 = bcd & 0x0f;     
	bcd = bcd & 0x70;      
	data1 += bcd >> 1;
	data1 += bcd >> 3;    
	return data1;
}

void display(uchar wei,uchar dat)
{
	duan=dat;
	if(wei==1)
		wei1=0;
	if(wei==2)
		wei2=0;
	if(wei==3)
		wei3=0;
	if(wei==4)
		wei4=0;
	delay_ms(30);
	duan=0;
	wei1=wei2=wei3=wei4=1;		
}

void bell(uchar n)
{
	uint x=0,y;
	y=n;
	while(y<=n)
	{
		y++;
		x=0;
		while(x<100)
		{
			bel=0;
			delay_ms(1);
			bel=1;
			delay_ms(2);
			x++;
		}   
		delay_ms(800);
		
	}
}

void INT0_int0(void) interrupt 0
{
	ds1302readtime(time);
	i=bcdtodec(time[1]);
	if((is==1)||(disp==1))
	{
		is=0;
		disp=0;
	}
	else if(alarm==0)
	{
		wei1=wei2=wei3=wei4=1;
		bell(1);
	}
	else 
	{
		al++;
		if(al>5)
		{
			al=0;
			alarm=0;
		} 
	} 
}
void INT1_int1(void) interrupt 2
{
	ds1302readtime(time);
	i=bcdtodec(time[1]);
	if((is==1)||(disp==1))
	{
		is=0;
		disp=0;
		delay_ms(2000);
	}
	else if(alarm==1)
	{
		alarm=0;
		delay_ms(2000);
	}
	else
	{
		wei1=wei2=wei3=wei4=1;
		if(!key1)
		{
			delay_ms(80);
			c=0;
			while(!key1)
			{
				c++;
				delay_ms(100);	
			}
			if(c>=80)
			{
				c=0;
				while(key1)
				{
					if(!key2)
					{
						delay_ms(80);
						if(!key2)
						{
							year++;
							bell(1);		
							if(year==99)
							year=0;
						}
					}
					a=0;
					while(a<7)
					{
						if(c<2)
						{
							display(3,0x08);
							display(4,0x08);
							display(2,table[0]);
							display(1,table[2]);	
						}
						else
						{ 								
							display(3,table[year/10]);
							display(4,table[year%10]);
							display(2,table[0]);
							display(1,table[2]);
						}
						a++;
					}
					c++;
					if(c>7)
					c=0;									
				}
				delay_ms(2000);
				c=0;
				while(key1)
				{
					if(!key2)
					{
						delay_ms(80);
						if(!key2)
						{
							month++;
							bell(1); 						
							if(month==13)
							month=1;
						}
					}
					a=0;
					while(a<7)
					{
						if(c<2)
						{
							display(1,0x08);
							display(2,0x08);
							display(3,table[day/10]);
							display(4,table[day%10]);	
						}
						else
						{ 								
							display(1,table[month/10]);
							display(2,table[month%10]);
							display(3,table[day/10]);
							display(4,table[day%10]);
						}
						a++;
					}
					c++;
					if(c>7)
					c=0;			
				}
				delay_ms(2000);
				c=0;		
				while(key1)
				{
					if(!key2)
					{
						delay_ms(80);
						if(!key2)
						{
							day++;
							bell(1);
							if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
							{
								if(day==32)
								day=0;
							}
							else if((month==2)&&(year%4==0))
							{
								if(day==30)
								day=0;
							}
							else if((month==2))
							{
								if(day==29)
								day=0;
							}
							else
							{
								if(day==31)
								day=0;
							}
	
						}
					}
					a=0;
					while(a<7)
					{
						if(c<2)
						{
							display(3,0x08);
							display(4,0x08);
							display(1,table[month/10]);
							display(2,table[month%10]);	
						}
						else
						{ 								
							display(1,table[month/10]);
							display(2,table[month%10]);
							display(3,table[day/10]);
							display(4,table[day%10]);
						}
						a++;
					}
					c++;
					if(c>7)
					c=0;			
				}
				delay_ms(2000);
				c=0;
				while(key1)
				{
					if(!key2)
					{
						delay_ms(80);
						if(!key2)
						{
							weak++;
							bell(1);
							if(weak==8)
							weak=1;
						}
					}
					a=0;
					while(a<7)
					{
						if(c<2)
						{
							display(3,0x40);
							display(4,0x08);
							display(1,0x40);
							display(2,0x40);	
						}
						else
						{ 								
							display(1,0x40);
							display(2,0x40);
							display(3,0x40);
							display(4,table[weak%10]);
						}
						a++;
					}
					c++;
					if(c>7)
					c=0;				
				}
				delay_ms(2000);
				c=0;
				while(key1)
				{
					if(!key2)
					{
						delay_ms(80);
						if(!key2)
						{
							hour++;
							bell(1);
							if(hour==24)
							hour=0;
						}
					}
					a=0;
					while(a<7)
					{
						if(c<2)
						{
							display(1,0x08);
							display(2,0x08);
							display(3,table[min/10]);
							display(4,table[min%10]);	
						}
						else
						{ 								
							display(1,table[hour/10]);
							display(2,table[hour%10]);
							display(3,table[min/10]);
							display(4,table[min%10]);
						}
						a++;
					}
					c++;
					if(c>7)
					c=0;				
				}
				delay_ms(2000);
				c=0;
				while(key1)
				{
					if(!key2)
					{
						delay_ms(80);
						if(!key2)
						{
							min++;
							bell(1);
							if(min==60)
							min=0;
						}
					}
					a=0;
					while(a<7)
					{
						if(c<2)
						{
							display(3,0x08);
							display(4,0x08);
							display(1,table[hour/10]);
							display(2,table[hour%10]);	
						}
						else
						{ 								
							display(1,table[hour/10]);
							display(2,table[hour%10]);
							display(3,table[min/10]);
							display(4,table[min%10]);
						}
						a++;
					}
					c++;
					if(c>7)
					c=0;				
				}
				delay_ms(2000);
				c=0;
				while(key1)
				{
					if(!key2)
					{
						delay_ms(80);
						if(!key2)
						{
							ahour++;
							bell(1);
							if(ahour==24)
							ahour=0;
						}
					}
					a=0;
					while(a<7)
					{
						if(c<2)
						{
							display(1,0x40);
							display(2,0x40);
							display(3,table[amin/10]);
							display(4,table[amin%10]);	
						}
						else
						{ 								
							display(1,table[ahour/10]);
							display(2,table[ahour%10]);
							display(3,table[amin/10]);
							display(4,table[amin%10]);
						}
						a++;
					}
					c++;
					if(c>7)
					c=0;				
				}
				delay_ms(2000);
				c=0;
				while(key1)
				{
					if(!key2)
					{
						delay_ms(80);
						if(!key2)
						{
							amin++;
							bell(1);
							if(amin==60)
							amin=0;
						}
					}
					a=0;
					while(a<7)
					{
						if(c<2)
						{
							display(3,0x40);
							display(4,0x40);
							display(1,table[ahour/10]);
							display(2,table[ahour%10]);	
						}
						else
						{ 								
							display(1,table[ahour/10]);
							display(2,table[ahour%10]);
							display(3,table[amin/10]);
							display(4,table[amin%10]);
						}
						a++;
					}
					c++;
					if(c>7)
					c=0;				
				}
				a=1;
				while(a<12)
				{
					display(1,0x40);
					delay_ms(60);
					a++;	
				}
				while(a<24)
				{
					display(1,0x40);
					display(2,0x40);
					delay_ms(40);
					a++;	
				}
				while(a<36)
				{
					display(1,0x40);
					display(2,0x40);
					display(3,0x40);
					delay_ms(20);
					a++;	
				}
				while(a<44)
				{
					display(1,0x40);
					display(2,0x40);
					display(3,0x40);
					display(4,0x40);
					a++;	
				}			
				time[0]=0x01;
				time[1]=dectobcd(min);
				time[2]=dectobcd(hour);
				time[3]=dectobcd(day);
				time[4]=dectobcd(month);
				time[5]=dectobcd(weak);
				time[6]=dectobcd(year);	
				ds1302start(time);
				c=0;	
				m=0;
				f=0;	
			}
			if(c>=1&&c<80)
			m++;   
			bell(1);
			ds1302readtime(time);
			i=bcdtodec(time[1]); 				
		}
	}
}

void main()
{

	res=0;
	wei3=wei4=1;
	alarm=0;
	ahour=0;
	amin=0;
//	ds1302Init();
	IT0=1;//外部中断0，xiajiangyan触发
	EX0=1;//打开0中断
	IT1=0;
	EX1=1;
	EA=1;
	ds1302readtime(time);
	i=bcdtodec(time[1]);
	while(1)	
		{						
			ds1302readtime(time);
			sec=  bcdtodec(time[0]);
			min=  bcdtodec(time[1]);
			hour= bcdtodec(time[2]);
			day=  bcdtodec(time[3]);
			month=bcdtodec(time[4]);
			weak= bcdtodec(time[5]);
			year= bcdtodec(time[6]);
			
			if(disp==0)
			{  	
				if(m==0)
				{	
					if(a!=time[0])
					{
						duan=0x80;
						wei2=0;
						wei4=0;
						delay_ms(10);
						wei2=wei4=1;
						duan=0;
						n++;				
						if(n>10)
						{
							n=0; 
							a=time[0];
						}
					}		
					display(4,table[min%10]);
					display(3,table[min/10]);
					display(2,table[hour%10]);		
					display(1,table[hour/10]);
				}
				if(m==1)
				{
					display(1,table[month/10]);
					display(2,table[month%10]);
					display(4,table[day%10]);
					display(3,table[day/10]);
					duan=0x80;
					wei4=0;
					delay_ms(10);
					wei4=1;
					duan=0;
				}			
				if(m==2)
				{
					display(4,table[weak%10]);
					delay_ms(90);
				}
				if(m==3)
				{
					display(1,table[2]);
					display(2,table[0]);
					display(4,table[year%10]);
					display(3,table[year/10]);
				}
				if(m==4)
				{
					delay_ms(40);
					display(4,table[sec%10]);
					display(3,table[sec/10]);
				}
				if(m==5)
				{
					display(4,table[amin%10]);
					display(3,table[amin/10]);
					display(2,table[ahour%10]);		
					display(1,table[ahour/10]);
				}
				if(m>5)
				m=0;
			}
			if((ahour!=0)&&(amin!=0))
			{
				if((ahour==hour)&&(amin==min)&&(f==0))
				{
					alarm=1;
					f=1;
				}
			}
			if(hour==0)
			f=0;
			if(alarm==1)
			bell(2);

			if(min-i>=2)
			{
				if((ahour!=0)&&(amin!=0))
				{
					disp=1;
					wei1=wei2=wei3=wei4=1;
				}
				else
				{
					is=1;
					PCON=0x02;
					_nop_();
					_nop_();
					_nop_();
					_nop_();
					_nop_();
				}
			} 			
		} 
}
////////////////////////////////////////////////////////////  
