#include "Arithmetic.h"
#if 0
uint8_t coding_data[7] = {0xA,0xB,0xC,0xD};//算数编码输入数组
uint8_t register_CRC[5] = {0x41,0x61,0x42,0x62};//CRC输入数组
float interval[8] = { 0,0.1f,0.3f,0.6f,1};//算数编码ABCD间隔
//A:[0,0.1)  B:[0.1,0.3) C:[0.3,0.6) D:[0.6,1)
uint16_t encryption;//RSA加密/解密后数据
uint8_t proclaimed_in_writing[30] = { 0x1,0x4,0x2,0x5,0x3,0x6};//栅栏密码要加密的明文数组
uint8_t ciphertext[30];//栅栏密码解密后
uint8_t  coding[20] = { 0X15,0X0D,0X0B,0X05,0X07,0X04,0X0A,0X19 };//波利比奥斯方阵密码需要加密的数据
uint8_t secret_key[5][5] = { {0X0B,0X02,0X0B,0X04,0X05},
                             {0X06,0X07,0X08,0X09,0X0A},
                             {0X0B,0X0C,0X0D,0X0E,0X0F},
                             {0X10,0X11,0X12,0X13,0X14},
                             {0X15,0X16,0X0B,0X18,0X19}
                           };//波利比奥斯方阵密码密钥
uint8_t PS_encrypted[20];//波利比奥斯方阵密码加密完后数据
uint8_t PS_decode[20];//波利比奥斯方阵密码解密完后数据
uint8_t enciphered_Data[20] = { '1','a','2','B','3','c','4','D','5' };   //仿射密码需加密的数据
uint8_t Enciphered[20];//仿射密码加密后的数据
uint8_t Decrypt_Data[20];//仿射密码解密后的数据

/***************************************************
 * 函 数 名 ：  算数编码
 * 参    数 ：  coding：需要编码的信息数组 size：大小
 * 返 回 值 ：  编码后数据
 * 全局变量 ：  interval:ABCD间隔
 * 备    注 ：  A:[0,0.1)  B:[0.1,0.3) C:[0.3,0.6) D:[0.6,1)
***************************************************/
uint8_t Arithmetic_coding(uint8_t* coding,uint8_t size)
{
    int i,i2;
    uint16_t result;
    float A=0, B=0, C,X1=0,X2=0;
    for (i = 0; i < size; i++)
    {
        switch (coding[i])//概论区间
        {
            case 0xA:
            {
                X1 = interval[0];
                X2 = interval[1];
                break;
            }
            case 0xB:
            {
                X1 = interval[1];
                X2 = interval[2];
                break;
            }
            case 0xC:
            {
                X1 = interval[2];
                X2 = interval[3];
                break;
            }
            case 0xD:
            {
                X1 = interval[3];
                X2 = interval[4];
                break;
            }
        }
        if (i == 0)//初始值获取
        {
            A = X1;
            B = X2;
        }
        if (i > 0)//间隔计算
        {
            C = (B - A) * X1 + A;
            B = (B - A) * X2 + A;
            A = C;
        }
    }
    result = 1;
    for (i2 = 0; i2 < size ; i2++)
    {
        result = result * 10;
    }
    result = (uint16_t)(((float)(A + B) * result) / 2) % 256;
    return result;
}


/****************************************************************
 * 函 数 名 ：  CRC校验
 * 参    数 ：  check_code：需要校验的数据数组 length：数组长度
 * 返 回 值 ：  校验计算结果
 * 全局变量 ：  无
 * 备    注 ：  （CRC-16/MODBUS）
****************************************************************/
uint16_t CRC_16_MODBUS(uint8_t* check_code,uint8_t length)
{
	int i;
	uint16_t polynomial = 0xA001;		//多项式
	uint16_t CRC_register = 0xFFFF;		//初始值
	uint8_t displacement = 0;			//位移值
	for (i = 0; i < length; i++)
	{
		CRC_register = CRC_register ^ check_code[i];
		while (1)
		{

			if ((CRC_register & 1) == 1)//LSB为1时
			{
				CRC_register = CRC_register >> 1;
				displacement += 1;
				CRC_register = CRC_register ^ polynomial;
			}
			else if ((CRC_register & 1) == 0)//LSB为0时
			{
				CRC_register = CRC_register >> 1;
				displacement += 1;
			}
			if (displacement > 7)
			{
				displacement = 0;
				break;
			}
		}
	}
	return CRC_register;
}

/****************************************************************
 * 函 数 名 ：  RSA加密/解密
 * 参    数 ：  message：需要加密/解密的信息 （ e N ）：公钥/（ d N ）：私钥
 * 返 回 值 ：  加密/解密后数据
 * 全局变量 ：  encryption:加密/解密后数据
 * 备    注 ：  需要加密数据要小于  N
****************************************************************/
uint16_t RSA_encryption(uint16_t message, uint16_t e_d, uint16_t N)
{
    uint8_t i;
    unsigned __int64 result;
    result = message;
    for (i = 0; i < e_d - 1; i++)
    {
        result *= message;
    }
    encryption = result % N;
    return encryption;
}

/****************************************************************
 * 函 数 名 ：  最大公约计算/互质数判断  (辗转相除法)
 * 参    数 ：  A：参数A B：参数B
 * 返 回 值 ：  最大公约/是否为互质数(等于1为互质数)
 * 全局变量 ：  无
 * 备    注 ：  互质数判断需要输入两个质数
****************************************************************/
uint16_t relatively_prime(uint16_t A, uint16_t B)
{
    uint16_t C;
    while (1)
    {
        if (A % B == 0)
        {
            break;
        }
        C = A % B;
        A = B;
        B = C;
    }
    return B;
}

/****************************************************************
 * 函 数 名 ：  质数判断
 * 参    数 ：  X：参数
 * 返 回 值 ：  1：为质数 0：不是质数
 * 全局变量 ：  无
 * 备    注 ：  无
****************************************************************/
uint8_t prime_number(uint16_t X)
{
    uint16_t i;
    uint16_t Y = 2;
    for (i = 0; i < X; i++)
    {
        if (X % Y == 0)
        {
            return 0;
//            break;
        }
        Y += 1;
        if (Y > X - 1)
        {
            break;
        }
    }
    return 1;
}

/**********************************************************************
 * 函 数 名 ：  栅栏码加密（2栏）
 * 参    数 ：  size：大小
 * 返 回 值 ：  无
 * 全局变量 ：  ciphertext[30]：加密后的密文数组
 * 备    注 ：  要加密的明文数组proclaimed_in_writing[]
*****************************************************************/
void ence_code_encryption(uint8_t size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (i % 2 == 0)
        {
            ciphertext[i] = proclaimed_in_writing[i / 2];
        }
        else
        {
            ciphertext[i] = proclaimed_in_writing[(size / 2) + (i / 2)];
        }
    }
}

/**********************************************************************
 * 函 数 名 ：  栅栏码解密（2栏）
 * 参    数 ：  size：大小
 * 返 回 值 ：  无
 * 全局变量 ：  ciphertext[30]：解密后的密文数组
 * 备    注 ：  要解密的密文数组proclaimed_in_writing[]
*****************************************************************/
void ence_code_decode(uint8_t size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (i % 2 == 0)
        {
            ciphertext[i / 2] = proclaimed_in_writing[i];
        }
        else
        {
            ciphertext[(size / 2) + (i / 2)] = proclaimed_in_writing[i];
        }
    }
}

/**********************************************************************
 * 函 数 名 ：  波利比奥斯方阵密码加密
 * 参    数 ：  data:需要加密的数组 size：数组大小
 * 返 回 值 ：  无
 * 全局变量 ：  encrypted[20]：加密后的数据
 * 备    注 ：  secret_key[5][5]：密钥
*****************************************************************/
void Polybius_Square_encryption(uint8_t* data, uint8_t size)
{
    int i,i2,i3;
    uint8_t ciphertext;
    for (i = 0; i < size; i++)
    { 
        for(i2=0; i2<5; i2++)
        { 
            for (i3 = 0; i3 < 5; i3++)
            {
                if (data[i]==secret_key[i2][i3])
                {
                    ciphertext = ((i2+1) * 10) + (i3+1);
                    PS_encrypted[i] = ciphertext;
                    i2 = 5;
                    i3 = 5;
                }
            }
        }
    }
}

/**********************************************************************
 * 函 数 名 ：  波利比奥斯方阵密码解密
 * 参    数 ：  data:需要解密的数组 size：数组大小
 * 返 回 值 ：  无
 * 全局变量 ：  PS_decode[20]：解密后的数据
 * 备    注 ：  secret_key[5][5]：密钥
*****************************************************************/
void Polybius_Square_decode(uint8_t* data, uint8_t size)
{
    int i, i2, i3;
    for (i = 0; i < size; i++)
    { 
        i2 = (data[i] / 10)-1;
        i3 = (data[i] % 10)-1;
        PS_decode[i] = secret_key[i2][i3];
    }
}

/****************************************************************
 * 函 数 名 ：  乘法逆元计算
 * 参    数 ：  a：参数a m：参数m
 * 返 回 值 ：  计算完成的数据
 * 全局变量 ：  无
 * 备    注 ：  乘法逆元计算需要输入两个互质的数
****************************************************************/
uint8_t multiplicative_inverse(uint8_t a, uint8_t m)
{
    uint16_t x = 0;
    if (relatively_prime(a, m) != 1)//互质判断
    {
        return 0;
    }
    while (1)
    {
        if ((a * x) % m == 1)
        {
            break;
        }
        else
        {
            x += 1;
        }
    }
    return x;
}

/****************************************************************
 * 函 数 名 ：  仿射密码加密
 * 参    数 ：  parameter：需加密的数据 calculate：（见结构体注释）
 * 返 回 值 ：  无
 * 全局变量 ：  Enciphered[ ]:加密完成的数据
 * 备    注 ：  （数字的数目为10  大小写字母数目为26）
****************************************************************/
void Affine_cipher_encryption(uint8_t* parameter,Coefficient* calculate)
{
    uint8_t i;
    for (i = 0; i < calculate->Data_Size; i++)
    {
        if (parameter[i] >= 'a')//小写字母加密
        {
            Enciphered[i] = (((calculate->lowercase_a * (parameter[i] - 'a')) +
                calculate->lowercase_b) % calculate->amount_m + 'a');
        }
        else if (parameter[i] >= 'A')//大写字母加密
        {
            Enciphered[i] = (((calculate->capital_letter_a * (parameter[i] - 'A')) +
                calculate->capital_letter_b) % calculate->amount_m + 'A');
        }
        else if (parameter[i] >= '0')//数字加密
        {                
            Enciphered[i] = (((calculate->number_a * (parameter[i]-'0')) +
                calculate->number_b) % calculate->number_m + '0');     
        }       
    }
}

/****************************************************************
 * 函 数 名 ：  仿射密码解密
 * 参    数 ：  parameter：需解密的数据 calculate：（见结构体注释）
 * 返 回 值 ：  无
 * 全局变量 ：  Decrypt_Data[ ]:解密完成的数据
 * 备    注 ：  （数字的数目为10  大小写字母数目为26）
****************************************************************/
void Affine_cipher_decode(uint8_t* parameter, Coefficient* calculate)
{
    uint8_t i;
    uint8_t number_n;
    uint8_t capital_letter_n;
    uint8_t lowercase_n;
    number_n=multiplicative_inverse(calculate->number_a, calculate->number_m);//数字密文乘法逆元计算
    capital_letter_n = multiplicative_inverse(calculate->capital_letter_a, calculate->amount_m);//数字密文乘法逆元计算
    lowercase_n = multiplicative_inverse(calculate->lowercase_a, calculate->amount_m);//数字密文乘法逆元计算
    for (i = 0; i < calculate->Data_Size; i++)
    {
        if (Enciphered[i] >= 'a')//小写字母解密
        {
            Decrypt_Data[i] = ((lowercase_n * ((Enciphered[i] - 'a') -
                calculate->lowercase_b)) % calculate->amount_m + 'a');
            while (Decrypt_Data[i] - 'a' < 0)//负数处理
            {
                Decrypt_Data[i] += calculate->amount_m;
            }
        }
        else if (Enciphered[i] >= 'A')//大写字母解密
        {
            Decrypt_Data[i] = ((capital_letter_n * ((Enciphered[i] - 'A') -
                calculate->capital_letter_b)) % calculate->amount_m + 'A');
            while (Decrypt_Data[i] - 'A' < 0)//负数处理
            {
                Decrypt_Data[i] += calculate->amount_m;
            }
        }
        else if (Enciphered[i] >= '0')//数字解密
        {
            Decrypt_Data[i] = ((number_n * ((Enciphered[i] - '0') -
                calculate->number_b)) % calculate->number_m + '0');
            while (Decrypt_Data[i] - '0' < 0)//负数处理
            {
                Decrypt_Data[i] += calculate->number_m;
            }
        }    
    }
}
#endif

#if LZW==1 //LZW算法
/*****************************************************************
 * 函 数 名 ：  字符串大小检测
 * 参    数 ：  无
 * 返 回 值 ：  字符串大小
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t String_len(uint8_t* temp)
{
	uint8_t i=0;
	while (1)
	{
		i += 1;
		if (temp[i] == '\0')
		{
			return i;
		}
	}
	
}
/*****************************************************************
 * 函 数 名 ：  LZW编码
 * 参    数 ：  输入数据
 * 返 回 值 ：  无
 * 全局变量 ：  输入字符串   输出码流
 * 备    注 ：  多取前六少补0
*****************************************************************/
uint8_t LZW_Code_Stream[20];				//LZW输出码流
void LZW_Coding(uint8_t *LZW_String)
{
	uint8_t i = 0;
	uint8_t LZW_Exist;							//是否在字典里标志位
	uint16_t LZW_Prefix = 0;					//LZW前缀缓存
	uint16_t LZW_Dictionaries[20] = { 0x01,0x02,0x03 };//LZW词典
	uint8_t Dictionarie_Flag = 0;				//词典标志位
	uint8_t Code_Stream_Flag = 0;				//输出码流标志位
	uint8_t LZW_Size = String_len(LZW_String);	//数据大小
	for (uint8_t i = 0; i < LZW_Size; i++)//转换成数字编码（A1B2C3）
	{
		LZW_String[i] = LZW_String[i] - '@';
	}
	for (i = 0; i < LZW_Size; i++)
	{
		LZW_Exist = 0;								//初始化是否在词典中标志位
		for (uint8_t i2 = 0; i2 < LZW_Size; i2++)
		{
			if ((LZW_Prefix * 10 + LZW_String[i]) == LZW_Dictionaries[i2])
			{
				LZW_Exist = 1;						//在词典中标志位
				LZW_Prefix = LZW_Prefix * 10 + LZW_String[i];							//前缀改为前缀加字符
				break;
			}
		}
		if (LZW_Exist != 1)							//不在在词典中
		{
			LZW_Dictionaries[3 + Dictionarie_Flag] = LZW_Prefix * 10 + LZW_String[i];	//新增加词典
			LZW_Code_Stream[Code_Stream_Flag] = LZW_Prefix;								//输出码流
			Code_Stream_Flag += 1;					//输出码流位增加
			Dictionarie_Flag += 1;					//词典位增加
			LZW_Prefix = LZW_String[i];				//前缀为现在的字符
		}
	}
	LZW_Code_Stream[Code_Stream_Flag] = LZW_Prefix;	//没有字符读入输出前缀
	for (uint8_t i3 = 0; i3 < 20; i3++)				//编码转换(112(AAB)->在词典中的数据位)
	{
		for (uint8_t i4 = 0; i4 < 20; i4++)
		{
			if (LZW_Code_Stream[i3] == 0x00)		//超出词典外
			{
				break;
			}
			if (LZW_Code_Stream[i3] == LZW_Dictionaries[i4])							//获取词典中编号
			{
				LZW_Code_Stream[i3] = i4 + 1;
			}
		}
	}
}
#endif

/**********************************************************************
 * 函 数 名 ：  ASCLL码转换十六进制0xXX
 * 参    数 ：  ASCLL码1 ASCLL码2
 * 返 回 值 ：  转换完成的值
 * 全局变量 ：  无
 * 备    注 ：  例 ASCLL_conversion('A',''B)  ->  0xAB 注：G->7
*****************************************************************/
uint8_t ASCLL_conversion(char n1,char n2)
{
	uint8_t HEX_Data;
	if(n1 == 'G')
	{
		HEX_Data=0x70;
	}
	else if (n1 > 'Z')
	{
		HEX_Data = (n1 - 0X57) * 0x10;
	}
	else if(n1 > '9')
	{
		HEX_Data = (n1 - 0x37) * 0x10;
	}
	else
	{
		HEX_Data = (n1 - '0') * 0x10;
	}
	if(n2 == 'G')
	{
		HEX_Data=HEX_Data+0x07;
	}
	else if (n2 > 'Z')
	{
		HEX_Data = HEX_Data + (n2 - 0X57);

	}
	else if (n2 > '9')
	{
		HEX_Data = HEX_Data+(n2 - 0x37);
	}
	else
	{
		HEX_Data = HEX_Data + (n2 - '0');
	}
	return HEX_Data;
}


/**********************************************************************
 * 函 数 名 ：  提取字符串最大或最小的字母
 * 参    数 ：   parameter:参数 size:数据大小 Mode:提取模式
 * 返 回 值 ：  最大/最小的数(十进制)
 * 全局变量 ：  无
 * 备    注 ：  1最大 0最小
*****************************************************************/
uint8_t Seek_biggest_minimum(char* Data_value, uint8_t size,uint8_t Mode)
{
    uint8_t temp=0;
    switch(Mode)
    { 
        case 1:
        {
            for (int i = 0; i < size; i++)
            {
                if (Data_value[i] > temp)
                {
                    temp = Data_value[i];
                }
            }
             return temp-'0';
        } 
        case 0:
        {
            temp = '9';
            for (int i = 0; i < size; i++)
            {
                if (Data_value[i] < temp)
                {
                    temp = Data_value[i];
                }
            }
            return temp-'0';
        }
    }
    return 0;
}





