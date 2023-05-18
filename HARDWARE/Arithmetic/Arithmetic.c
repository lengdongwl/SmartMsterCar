#include "Arithmetic.h"
#if 0
uint8_t coding_data[7] = {0xA,0xB,0xC,0xD};//����������������
uint8_t register_CRC[5] = {0x41,0x61,0x42,0x62};//CRC��������
float interval[8] = { 0,0.1f,0.3f,0.6f,1};//��������ABCD���
//A:[0,0.1)  B:[0.1,0.3) C:[0.3,0.6) D:[0.6,1)
uint16_t encryption;//RSA����/���ܺ�����
uint8_t proclaimed_in_writing[30] = { 0x1,0x4,0x2,0x5,0x3,0x6};//դ������Ҫ���ܵ���������
uint8_t ciphertext[30];//դ��������ܺ�
uint8_t  coding[20] = { 0X15,0X0D,0X0B,0X05,0X07,0X04,0X0A,0X19 };//�����Ȱ�˹����������Ҫ���ܵ�����
uint8_t secret_key[5][5] = { {0X0B,0X02,0X0B,0X04,0X05},
                             {0X06,0X07,0X08,0X09,0X0A},
                             {0X0B,0X0C,0X0D,0X0E,0X0F},
                             {0X10,0X11,0X12,0X13,0X14},
                             {0X15,0X16,0X0B,0X18,0X19}
                           };//�����Ȱ�˹����������Կ
uint8_t PS_encrypted[20];//�����Ȱ�˹������������������
uint8_t PS_decode[20];//�����Ȱ�˹������������������
uint8_t enciphered_Data[20] = { '1','a','2','B','3','c','4','D','5' };   //������������ܵ�����
uint8_t Enciphered[20];//����������ܺ������
uint8_t Decrypt_Data[20];//����������ܺ������

/***************************************************
 * �� �� �� ��  ��������
 * ��    �� ��  coding����Ҫ�������Ϣ���� size����С
 * �� �� ֵ ��  ���������
 * ȫ�ֱ��� ��  interval:ABCD���
 * ��    ע ��  A:[0,0.1)  B:[0.1,0.3) C:[0.3,0.6) D:[0.6,1)
***************************************************/
uint8_t Arithmetic_coding(uint8_t* coding,uint8_t size)
{
    int i,i2;
    uint16_t result;
    float A=0, B=0, C,X1=0,X2=0;
    for (i = 0; i < size; i++)
    {
        switch (coding[i])//��������
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
        if (i == 0)//��ʼֵ��ȡ
        {
            A = X1;
            B = X2;
        }
        if (i > 0)//�������
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
 * �� �� �� ��  CRCУ��
 * ��    �� ��  check_code����ҪУ����������� length�����鳤��
 * �� �� ֵ ��  У�������
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��CRC-16/MODBUS��
****************************************************************/
uint16_t CRC_16_MODBUS(uint8_t* check_code,uint8_t length)
{
	int i;
	uint16_t polynomial = 0xA001;		//����ʽ
	uint16_t CRC_register = 0xFFFF;		//��ʼֵ
	uint8_t displacement = 0;			//λ��ֵ
	for (i = 0; i < length; i++)
	{
		CRC_register = CRC_register ^ check_code[i];
		while (1)
		{

			if ((CRC_register & 1) == 1)//LSBΪ1ʱ
			{
				CRC_register = CRC_register >> 1;
				displacement += 1;
				CRC_register = CRC_register ^ polynomial;
			}
			else if ((CRC_register & 1) == 0)//LSBΪ0ʱ
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
 * �� �� �� ��  RSA����/����
 * ��    �� ��  message����Ҫ����/���ܵ���Ϣ �� e N ������Կ/�� d N ����˽Կ
 * �� �� ֵ ��  ����/���ܺ�����
 * ȫ�ֱ��� ��  encryption:����/���ܺ�����
 * ��    ע ��  ��Ҫ��������ҪС��  N
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
 * �� �� �� ��  ���Լ����/�������ж�  (շת�����)
 * ��    �� ��  A������A B������B
 * �� �� ֵ ��  ���Լ/�Ƿ�Ϊ������(����1Ϊ������)
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  �������ж���Ҫ������������
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
 * �� �� �� ��  �����ж�
 * ��    �� ��  X������
 * �� �� ֵ ��  1��Ϊ���� 0����������
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
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
 * �� �� �� ��  դ������ܣ�2����
 * ��    �� ��  size����С
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ciphertext[30]�����ܺ����������
 * ��    ע ��  Ҫ���ܵ���������proclaimed_in_writing[]
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
 * �� �� �� ��  դ������ܣ�2����
 * ��    �� ��  size����С
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ciphertext[30]�����ܺ����������
 * ��    ע ��  Ҫ���ܵ���������proclaimed_in_writing[]
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
 * �� �� �� ��  �����Ȱ�˹�����������
 * ��    �� ��  data:��Ҫ���ܵ����� size�������С
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  encrypted[20]�����ܺ������
 * ��    ע ��  secret_key[5][5]����Կ
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
 * �� �� �� ��  �����Ȱ�˹�����������
 * ��    �� ��  data:��Ҫ���ܵ����� size�������С
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  PS_decode[20]�����ܺ������
 * ��    ע ��  secret_key[5][5]����Կ
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
 * �� �� �� ��  �˷���Ԫ����
 * ��    �� ��  a������a m������m
 * �� �� ֵ ��  ������ɵ�����
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  �˷���Ԫ������Ҫ�����������ʵ���
****************************************************************/
uint8_t multiplicative_inverse(uint8_t a, uint8_t m)
{
    uint16_t x = 0;
    if (relatively_prime(a, m) != 1)//�����ж�
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
 * �� �� �� ��  �����������
 * ��    �� ��  parameter������ܵ����� calculate�������ṹ��ע�ͣ�
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  Enciphered[ ]:������ɵ�����
 * ��    ע ��  �����ֵ���ĿΪ10  ��Сд��ĸ��ĿΪ26��
****************************************************************/
void Affine_cipher_encryption(uint8_t* parameter,Coefficient* calculate)
{
    uint8_t i;
    for (i = 0; i < calculate->Data_Size; i++)
    {
        if (parameter[i] >= 'a')//Сд��ĸ����
        {
            Enciphered[i] = (((calculate->lowercase_a * (parameter[i] - 'a')) +
                calculate->lowercase_b) % calculate->amount_m + 'a');
        }
        else if (parameter[i] >= 'A')//��д��ĸ����
        {
            Enciphered[i] = (((calculate->capital_letter_a * (parameter[i] - 'A')) +
                calculate->capital_letter_b) % calculate->amount_m + 'A');
        }
        else if (parameter[i] >= '0')//���ּ���
        {                
            Enciphered[i] = (((calculate->number_a * (parameter[i]-'0')) +
                calculate->number_b) % calculate->number_m + '0');     
        }       
    }
}

/****************************************************************
 * �� �� �� ��  �����������
 * ��    �� ��  parameter������ܵ����� calculate�������ṹ��ע�ͣ�
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  Decrypt_Data[ ]:������ɵ�����
 * ��    ע ��  �����ֵ���ĿΪ10  ��Сд��ĸ��ĿΪ26��
****************************************************************/
void Affine_cipher_decode(uint8_t* parameter, Coefficient* calculate)
{
    uint8_t i;
    uint8_t number_n;
    uint8_t capital_letter_n;
    uint8_t lowercase_n;
    number_n=multiplicative_inverse(calculate->number_a, calculate->number_m);//�������ĳ˷���Ԫ����
    capital_letter_n = multiplicative_inverse(calculate->capital_letter_a, calculate->amount_m);//�������ĳ˷���Ԫ����
    lowercase_n = multiplicative_inverse(calculate->lowercase_a, calculate->amount_m);//�������ĳ˷���Ԫ����
    for (i = 0; i < calculate->Data_Size; i++)
    {
        if (Enciphered[i] >= 'a')//Сд��ĸ����
        {
            Decrypt_Data[i] = ((lowercase_n * ((Enciphered[i] - 'a') -
                calculate->lowercase_b)) % calculate->amount_m + 'a');
            while (Decrypt_Data[i] - 'a' < 0)//��������
            {
                Decrypt_Data[i] += calculate->amount_m;
            }
        }
        else if (Enciphered[i] >= 'A')//��д��ĸ����
        {
            Decrypt_Data[i] = ((capital_letter_n * ((Enciphered[i] - 'A') -
                calculate->capital_letter_b)) % calculate->amount_m + 'A');
            while (Decrypt_Data[i] - 'A' < 0)//��������
            {
                Decrypt_Data[i] += calculate->amount_m;
            }
        }
        else if (Enciphered[i] >= '0')//���ֽ���
        {
            Decrypt_Data[i] = ((number_n * ((Enciphered[i] - '0') -
                calculate->number_b)) % calculate->number_m + '0');
            while (Decrypt_Data[i] - '0' < 0)//��������
            {
                Decrypt_Data[i] += calculate->number_m;
            }
        }    
    }
}
#endif

#if LZW==1 //LZW�㷨
/*****************************************************************
 * �� �� �� ��  �ַ�����С���
 * ��    �� ��  ��
 * �� �� ֵ ��  �ַ�����С
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
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
 * �� �� �� ��  LZW����
 * ��    �� ��  ��������
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  �����ַ���   �������
 * ��    ע ��  ��ȡǰ���ٲ�0
*****************************************************************/
uint8_t LZW_Code_Stream[20];				//LZW�������
void LZW_Coding(uint8_t *LZW_String)
{
	uint8_t i = 0;
	uint8_t LZW_Exist;							//�Ƿ����ֵ����־λ
	uint16_t LZW_Prefix = 0;					//LZWǰ׺����
	uint16_t LZW_Dictionaries[20] = { 0x01,0x02,0x03 };//LZW�ʵ�
	uint8_t Dictionarie_Flag = 0;				//�ʵ��־λ
	uint8_t Code_Stream_Flag = 0;				//���������־λ
	uint8_t LZW_Size = String_len(LZW_String);	//���ݴ�С
	for (uint8_t i = 0; i < LZW_Size; i++)//ת�������ֱ��루A1B2C3��
	{
		LZW_String[i] = LZW_String[i] - '@';
	}
	for (i = 0; i < LZW_Size; i++)
	{
		LZW_Exist = 0;								//��ʼ���Ƿ��ڴʵ��б�־λ
		for (uint8_t i2 = 0; i2 < LZW_Size; i2++)
		{
			if ((LZW_Prefix * 10 + LZW_String[i]) == LZW_Dictionaries[i2])
			{
				LZW_Exist = 1;						//�ڴʵ��б�־λ
				LZW_Prefix = LZW_Prefix * 10 + LZW_String[i];							//ǰ׺��Ϊǰ׺���ַ�
				break;
			}
		}
		if (LZW_Exist != 1)							//�����ڴʵ���
		{
			LZW_Dictionaries[3 + Dictionarie_Flag] = LZW_Prefix * 10 + LZW_String[i];	//�����Ӵʵ�
			LZW_Code_Stream[Code_Stream_Flag] = LZW_Prefix;								//�������
			Code_Stream_Flag += 1;					//�������λ����
			Dictionarie_Flag += 1;					//�ʵ�λ����
			LZW_Prefix = LZW_String[i];				//ǰ׺Ϊ���ڵ��ַ�
		}
	}
	LZW_Code_Stream[Code_Stream_Flag] = LZW_Prefix;	//û���ַ��������ǰ׺
	for (uint8_t i3 = 0; i3 < 20; i3++)				//����ת��(112(AAB)->�ڴʵ��е�����λ)
	{
		for (uint8_t i4 = 0; i4 < 20; i4++)
		{
			if (LZW_Code_Stream[i3] == 0x00)		//�����ʵ���
			{
				break;
			}
			if (LZW_Code_Stream[i3] == LZW_Dictionaries[i4])							//��ȡ�ʵ��б��
			{
				LZW_Code_Stream[i3] = i4 + 1;
			}
		}
	}
}
#endif

/**********************************************************************
 * �� �� �� ��  ASCLL��ת��ʮ������0xXX
 * ��    �� ��  ASCLL��1 ASCLL��2
 * �� �� ֵ ��  ת����ɵ�ֵ
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  �� ASCLL_conversion('A',''B)  ->  0xAB ע��G->7
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
 * �� �� �� ��  ��ȡ�ַ���������С����ĸ
 * ��    �� ��   parameter:���� size:���ݴ�С Mode:��ȡģʽ
 * �� �� ֵ ��  ���/��С����(ʮ����)
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  1��� 0��С
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





