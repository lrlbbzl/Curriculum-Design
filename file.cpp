/*
计交1901 lrl，3-10
模块功能：完成实现初始化与求解二进制数独时的一系列的文件读取与写入，
*/
#include "head.h"
/*
功能：将初始棋盘写入文件
输入：存储棋盘内容的文件名
返回：成功写入返回OK
*/
int write(char filename[80]) {   //将棋盘的初始条件写入文件
	int a[20], i = 0;
	printf("写入棋盘的初始条件，以0为结尾\n");
	scanf("%d", &a[0]);
	while (a[i] != 0) {
		i++;
		scanf("%d", &a[i]);
	}
	FILE* fp;
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("打开文件失败\n");
		return ERROR;
	}
	fprintf(fp, "ccccccBinary_Puzzle\n");
	fprintf(fp, "p cnf 36 %d\n", i + 2376);
	i = 0;
	while (a[i] != 0) {
		fprintf(fp, "%d 0\n", a[i]);
		i++;
	}
	fclose(fp);
	return OK;
}

/*
功能：根据棋盘的初始条件的三个约束，转化为cnf公式，写入文件
输入：存储棋盘的文件名
返回：成功写入返回OK，写入失败或约束不满足返回FALSE
*/
int WriteCNF(char filename[80]) {      
	if (write(filename) != OK)  return FALSE;
	if (rule_1(filename) != OK) return FALSE;
	if (rule_2(filename) != OK) return FALSE;
	if (rule_3(filename) != OK) return FALSE;
	return OK;
}

/*
功能：求解结果写入文件 
输入：求解结果是否满足的标志res，运行时间time，存储答案的顺序表，存储的文件名
返回：成功执行功能返回OK
*/
int ResWrite_1(int res, double time, SqList& An, char filename[80]) {     
	int i = 0;
	while (filename[i] != '\0') i++;
	filename[i - 3] = 'r';    //只改变文件的扩展名 
	filename[i - 2] = 'e';
	filename[i - 1] = 's';
	FILE* fp;
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("打开文件失败\n");
		return ERROR;
	}
	//res是求解结果，1表示满足所有公式，0表示不满足公式，-1是规定时间未运行完毕
	fprintf(fp, "s %d\n", res);    
	fprintf(fp, "v");
	//写入答案顺序表的信息
	for (i = 1; i < An.length; i++) {
		if (An.elem[i] == -1)  fprintf(fp, "%5d", -i);
		else fprintf(fp, "%5d", i);
		if (i % 10 == 0)  fprintf(fp, "\n");
	}
	fprintf(fp, "\nt %f ms\n", time);
	fclose(fp);
	return OK;
}
