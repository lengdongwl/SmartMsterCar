/*
 * @Description: �ַ��������
 * @Autor: 309 Mushroom
 * @Date: 2021-10-22 17:02:29
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-22 23:31:20
 */
#ifndef __MYSTRING_H
#define __MYSTRING_H
void arr_sort(int *arr,int len);//ð������
char arr_sort_stringMIN(char *p);//ȡ�ַ��������е���Сֵ
char arr_sort_stringMAX(char *p);//ȡ�ַ��������е����ֵ
char *get_NotAscll(char *str);//�ַ�����ȡ��ASCLL���ֵ
char *get_letter(char *str);//�ַ�����ȡ��ĸ
char *get_number(char *str);//�ַ�����ȡ����
char *get_NumberLitter(char *str);//�ַ�����ȡ��ĸ����
char get_charToHEX(char data);//�ַ�תHEX��ֵ
char *get_stringHEX(char *str);//2019����ʡ�� ��ȡRFID���е�·���ַ��� ����תΪ16��������
char get_hexToCharOne2(unsigned char hex);//ʮ����������λת�ַ�(Сд)
char get_hexToCharOne1(unsigned char hex);//ʮ����������λת�ַ�(��д)
char *get_hexToChar(unsigned char hex);//ʮ������ת�ַ�����
int get_strToNumberV(char *str);//�ַ���ת��ֵ
#endif

