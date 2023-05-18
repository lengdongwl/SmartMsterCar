/*
 * @Description: 字符串处理库
 * @Autor: 309 Mushroom
 * @Date: 2021-10-22 17:02:29
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-04-27 15:32:20
 */
#ifndef __MYSTRING_H
#define __MYSTRING_H
void arr_sort(int *arr,int len);//冒牌排序
char arr_sort_stringMIN(char *p);//取字符串数组中的最小值
char arr_sort_stringMAX(char *p);//取字符串数组中的最大值
char *get_NotAscll(char *str);//字符串获取非ASCLL码的值
char *get_letter(char *str);//字符串获取字母
char *get_number(char *str);//字符串获取数字
char *get_NumberLitter(char *str);//字符串获取字母数字
char *get_calcAscll(char *str);//字符提取1-9 +-*/
char get_charToHEX(char data);//字符转HEX数值
char *get_stringHEX(char *str);//2019福建省赛 读取RFID卡中的路径字符串 将其转为16进制数组
char get_hexToCharOne2(unsigned char hex);//十六进制数单位转字符(小写)
char get_hexToCharOne1(unsigned char hex);//十六进制数单位转字符(大写)
char *get_hexToChar(unsigned char hex);//十六进制转字符数组
int get_strToNumberV(char *str);//字符串转数值
char* get_path_one(char *str,char *first,char *finally,char *buf);//字符串中获取路径 例如：B4D4D6B6|D1，第1个B4开头，以最后1个B6结束，B车初始位置有效信息为D1。
#endif

