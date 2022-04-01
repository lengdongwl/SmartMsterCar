/*
 * @Description: 字符串处理库
 * @Autor: 309 Mushroom
 * @Date: 2021-10-22 17:01:58
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-23 08:50:51
 */
#include "mystring.h"
#include "syn7318.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
/**
 * @description: 冒牌排序
 * @param {int} *arr 数组指针
 * @param {int} len 数组长度
 * @return {*}
 */
void arr_sort(int *arr, int len)
{
	int i = 0, j = 0, v = 0;

	for (i = 0; i < len; i++)
	{
		for (j = 0; j < len - i - 1; j++)
		{
			if (*(arr + j) > *(arr + j + 1))
			{
				v = *(arr + j);
				*(arr + j) = *(arr + j + 1);
				*(arr + j + 1) = v;
			}
		}
	}
}

void arr_sort_char(char *arr, int len)
{
	int i = 0, j = 0, v = 0;
	for (i = 0; i < len; i++)
	{
		for (j = 0; j < len - i - 1; j++)
		{
			if (*(arr + j) > *(arr + j + 1))
			{
				v = *(arr + j);
				*(arr + j) = *(arr + j + 1);
				*(arr + j + 1) = v;
			}
		}
	}
}

/**
 * @description: 取字符串中最小值
 * @param {char} *p
 * @return {*}
 */
char arr_sort_stringMIN(char *p)
{
	int len = 0;
	static char str[99];
	sprintf(str, "%s", p);
	//printf("%s\n",str);
	if (*p != 0) //判断是否为空参
	{

		while (*p++)
		{
			len++;
		} //计算字符串长度
		  //printf("%d\n",len);
	}
	arr_sort_char(str, len); //排序

	return str[0];
}

/**
 * @description: 取字符串中最大值
 * @param {char} *p
 * @return {*}
 */
char arr_sort_stringMAX(char *p)
{
	int len = 0;
	static char str[99];
	sprintf(str, "%s", p);
	//printf("%s\n",str);
	if (*p != 0) //判断是否为空参
	{

		while (*p++)
		{
			len++;
		} //计算字符串长度
		  //printf("%d\n",len);
	}
	arr_sort_char(str, len); //排序

	return str[len - 1];
}

//字符串获取非ASCLL码的值
char *get_NotAscll(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	//int len = 0; //过滤后的长度
	while (*str)
	{
		if (0 >= *str || *str > 127)
		{
			newstr[i] = *str;
			//printf("%d\n",*str);
			i++;
		}
		else
		{
			//printf("del - >%c\n",*str);
		}
		str++;
	}
	newstr[i] = '\0';
	//len=i;//处理结果长度刚好=i
	p = newstr;
	printf("%s\n", p); //打印结果
	return p;
}
//字符串获取字母
char *get_letter(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	while (*str)
	{
		if (*str >= 31 && *str <= 127)//字符类型范围 不支持中文
		{
			if ((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z'))
			{
				newstr[i] = *str;
				i++;
			}
			str++;
		}else
		{
			break;
		}
	}
	newstr[i] = '\0';
	p = newstr;
	printf("%s\n", p); //打印结果
	return p;
}

//字符串获取数字
char *get_number(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	while (*str)
	{
		if (*str >= 31 && *str <= 127)//字符类型范围 不支持中文
		{
			if ((*str >= '0' && *str <= '9'))
			{
				newstr[i] = *str;
				i++;
			}
			str++;
		}else
		{
			break;
		}
		
		
	}
	newstr[i] = '\0';
	p = newstr;
	//printf("%s\n", p); //打印结果
	return p;
}

//字符串获取字母数字
char *get_NumberLitter(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	while (*str)
	{
		if (*str >= 31 && *str <= 127)//字符类型范围 不支持中文
		{
			if ((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z'))
			{
				newstr[i] = *str;
				i++;
			}
			str++;
		}else
		{
			break;
		}
	}
	newstr[i] = '\0';
	p = newstr;
	printf("%s\n", p); //打印结果
	return p;
}

/**
 * @description: 字符转HEX数值
 * @param {char} data 0-F的字符
 * @return {*}
 */
char get_charToHEX(char data)
{
	if (data >= 'a' && data <= 'f') //小写
	{
		return data - 87;
	}
	else if (data >= 'A' && data <= 'F') //大写
	{
		return data - 55;
	}
	else if (data >= '0' && data <= '9') //数字0-9
	{
		return data - 48;
	}
	return 0;
}

/**
 * @description: 2019福建省赛 读取RFID卡中的路径字符串 将其转为16进制发送给副车
 * @param {char} *str
 * @return {*}返回十六进制数组
 * 例:"-F2-D2-D4-D6-D7-" 读取出F2,D2,D4,D6,D7转为十六进制数组
 */
char *get_stringHEX(char *str)
{
	static char data[5];
	int i, z;

	for (i = 1, z = 0; z < 5; i += 3, z++) //取高位
	{
		data[z] = get_charToHEX(str[i]) * 16;
		//printf("%c\n",a[i]);
	}
	for (i = 2, z = 0; z < 5; i += 3, z++) //取低位
	{
		data[z] = data[z] + get_charToHEX(str[i]);
		//printf("%c\n",a[i]);
	}
	/*for(z=0;z<5;z++)//打印结果
	{
		printf("%x\n",data[z]);
	}*/
	return data;
}

/**
 * @description: 十六进制数单位转字符(小写)
 * @param {char} hex 十六进制数据 范围0x00-0x0f
 * @return {*}
 * 例:0xa -> 'a'
 */
char get_hexToCharOne2(unsigned char hex)
{
	if (hex >= 10 && hex <= 15)
	{
		return hex + 87;
	}
	else if (hex <= 9) 
	{
		return hex + 48;
	}
	return 0;
}

/**
 * @description: 十六进制数单位转字符(大写)
 * @param {char} hex 十六进制数据 范围0x00-0x0f
 * @return {*}
 *  例:0xa -> 'A'
 */
char get_hexToCharOne1(unsigned char hex)
{
	if (hex >= 10 && hex <= 15)
	{
		return hex + 87-32;
	}
	else if (hex <= 9) 
	{
		return hex + 48;
	}
	return 0;
}


/**
 * @description: 十六进制转字符数组
 * @param {unsigned char} hex
 * @return {*}
 * 例:0xaf -> {'a',f}
 */
char *get_hexToChar(unsigned char hex)
{
	static char data[2];
	char dataH,dataL;
	
	dataH=hex/16;
	dataL=hex%16;
	data[0]=get_hexToCharOne1(dataH);
	data[1]=get_hexToCharOne1(dataL);
	return data;
	//printf("%c%c\n",get_hexToChar(0x21)[0],get_hexToChar(0x21)[1]);
}

/**
 * @description: 字符串转数值
 * @param {char} *str 例："12345"
 * @return {*}
 */
int get_strToNumberV(char *str)
{
	int data=0;
	int i=0;
    for (i=0;i<strlen(str)-1 ; i++)
	{
			
			data+=(int)pow(10,strlen(str)-1-i)*(str[i]-48);
			//printf("%d\n",(int)pow(10,strlen(num_buf)-1-i)*(num_buf[i]-48));
			//printf("%d\n",data);
	}
	data+=((str[strlen(str)-1]-48));
	return data;
}

