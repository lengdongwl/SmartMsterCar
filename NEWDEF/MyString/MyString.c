/*
 * @Description: �ַ��������
 * @Autor: 309 Mushroom
 * @Date: 2021-10-22 17:01:58
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-04-27 16:53:12
 */
#include "mystring.h"
#include "syn7318.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
/**
 * @description: ð������
 * @param {int} *arr ����ָ��
 * @param {int} len ���鳤��
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
 * @description: ȡ�ַ�������Сֵ
 * @param {char} *p
 * @return {*}
 */
char arr_sort_stringMIN(char *p)
{
	int len = 0;
	static char str[99];
	sprintf(str, "%s", p);
	//printf("%s\n",str);
	if (*p != 0) //�ж��Ƿ�Ϊ�ղ�
	{

		while (*p++)
		{
			len++;
		} //�����ַ�������
		  //printf("%d\n",len);
	}
	arr_sort_char(str, len); //����

	return str[0];
}

/**
 * @description: ȡ�ַ��������ֵ
 * @param {char} *p
 * @return {*}
 */
char arr_sort_stringMAX(char *p)
{
	int len = 0;
	static char str[99];
	sprintf(str, "%s", p);
	//printf("%s\n",str);
	if (*p != 0) //�ж��Ƿ�Ϊ�ղ�
	{

		while (*p++)
		{
			len++;
		} //�����ַ�������
		  //printf("%d\n",len);
	}
	arr_sort_char(str, len); //����

	return str[len - 1];
}

//�ַ�����ȡ��ASCLL���ֵ
char *get_NotAscll(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	int len = strlen(str);
	while (len--)
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
	//len=i;//���������ȸպ�=i
	p = newstr;
	printf("%s\n", p); //��ӡ���
	return p;
}
//�ַ�����ȡ��ĸ
char *get_letter(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	int len = strlen(str);
	while (len--)
	{
		if (*str >= 31 && *str <= 127)//�ַ����ͷ�Χ ��֧������
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
	//printf("%s\n", p); //��ӡ���
	return p;
}

//�ַ�����ȡ����
char *get_number(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	int len = strlen(str);
	
	while (len--)
	{
		if ((*str >= '0' && *str <= '9'))
		{
			newstr[i] = *str;
			i++;
		}
		str++;
	}
	newstr[i] = '\0';
	p = newstr;
	//printf("%s\n", p); //��ӡ���
	return p;
}

//�ַ�����ȡ���������ʽ 0-9  +-*/  û��������
char *get_calcAscll(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	int len = strlen(str);
	
	while (len--)
	{
		if ((*str >= '0' && *str <= '9') || (*str == '+')|| (*str == '-')|| (*str == '*')|| (*str == '/'))
		{
			newstr[i] = *str;
			i++;
		}
		str++;
	}
	newstr[i] = '\0';
	p = newstr;
	//printf("%s\n", p); //��ӡ���
	return p;
}

//�ַ�����ȡ��ĸ����
char *get_NumberLitter(char *str)
{
	static char newstr[100], *p;
	int i = 0;
	int len = strlen(str);
	while (len--)
	{
		if (*str >= 31 && *str <= 127)//�ַ����ͷ�Χ ��֧������
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
	return p;
}

/**
 * @description: �ַ�תHEX��ֵ
 * @param {char} data 0-F���ַ�
 * @return {*}
 */
char get_charToHEX(char data)
{
	if (data >= 'a' && data <= 'f') //Сд
	{
		return data - 87;
	}
	else if (data >= 'A' && data <= 'F') //��д
	{
		return data - 55;
	}
	else if (data >= '0' && data <= '9') //����0-9
	{
		return data - 48;
	}
	return 0;
}

/**
 * @description: 2019����ʡ�� ��ȡRFID���е�·���ַ��� ����תΪ16���Ʒ��͸�����
 * @param {char} *str
 * @return {*}����ʮ����������
 * ��:"-F2-D2-D4-D6-D7-" ��ȡ��F2,D2,D4,D6,D7תΪʮ����������
 */
char *get_stringHEX(char *str)
{
	static char data[5];
	int i, z;

	for (i = 1, z = 0; z < 5; i += 3, z++) //ȡ��λ
	{
		data[z] = get_charToHEX(str[i]) * 16;
		//printf("%c\n",a[i]);
	}
	for (i = 2, z = 0; z < 5; i += 3, z++) //ȡ��λ
	{
		data[z] = data[z] + get_charToHEX(str[i]);
		//printf("%c\n",a[i]);
	}
	/*for(z=0;z<5;z++)//��ӡ���
	{
		printf("%x\n",data[z]);
	}*/
	return data;
}

/**
 * @description: ʮ����������λת�ַ�(Сд)
 * @param {char} hex ʮ���������� ��Χ0x00-0x0f
 * @return {*}
 * ��:0xa -> 'a'
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
 * @description: ʮ����������λת�ַ�(��д)
 * @param {char} hex ʮ���������� ��Χ0x00-0x0f
 * @return {*}
 *  ��:0xa -> 'A'
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
 * @description: ʮ������ת�ַ�����
 * @param {unsigned char} hex
 * @return {*}
 * ��:0xaf -> {'a','f','\0'}
 */
char *get_hexToChar(unsigned char hex)
{
	static char data[3]={0,0,'\0'};
	char dataH,dataL;
	
	dataH=hex/16;
	dataL=hex%16;
	data[0]=get_hexToCharOne1(dataH);
	data[1]=get_hexToCharOne1(dataL);
	return data;
	//printf("%c%c\n",get_hexToChar(0x21)[0],get_hexToChar(0x21)[1]);
}

/**
 * @description: �ַ���ת��ֵ
 * @param {char} *str ����"12345"
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

/**
 * @description: �ַ����л�ȡ·��
 * ���磺B4D4D6B6|D1����1��B4��ͷ�������1��B6������B����ʼλ����Ч��ϢΪD1��
 * get_path_one("B4D4D6B6|D1","B4","B6");
 * @param {char} *str 
 * @param {char} *first ������
 * @param {char} *finally β����
 * @param {char} *buf �ṩ�������飬��������·��
 * @return {*}
 */
char* get_path_one(char *str,char *first,char *finally,char *buf)
{
	int len = strlen(str);
	int i,j,flag=0;
	static char r[3]={0,0,'\0'};
	if(str[0]!=first[0] || str[1]!=first[1]) //��λ first
	{
		return 0;
	}
	
	for(i=2;i<len;i++)
	{
		if(str[i]=='|') //����ȡ��'|'�����±�
		{
			flag = 1;
			break;
		}
	}	
	if(flag)
	{
		if((str[i-2]==finally[0] && str[i-1]==finally[1]) && (len-i-1)==2)//�ָ���'|'ǰ��Ϊfinally ����Ϊ����2������
		{
			r[0]=str[i+1];
			r[1]=str[i+2];
			for(j=0;j<i;j++)
			{
				buf[j] = str[j];	
			}
			buf[j]='\0';
			return r;
		}
		
	}
	return 0; 
}
