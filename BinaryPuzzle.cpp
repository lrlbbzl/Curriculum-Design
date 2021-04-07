/*
计交1901 lrl 3-10
模块功能：完成二进制数独的生成仅唯一解的盘面；
完成二进制数独写入文件的操作，以及读取棋盘文件，根据数独初局生成CNF公式集并写入；
文件进行存储等功能，而这些功能由有若干辅助函数来共同完成。
*/
#include "head.h"
#define init  1
extern int mat[8][8];//用于存放正确答案
char resfile[1024];//res文件名
char puzzlefile[1024];//初始盘局文件名.puz
/*
功能：创建唯一解的二进制数独棋盘
输入：用于存储的文件的文件名
返回：成功创建返回OK,失败返回FALSE
*/
int create_puzzle(char filename[80])
{
	//用来存储选取数
	int arr[init] = { 0 };
	int i = 0, j;
	while (i < init)
	{

		j = rand() % 36 + 1;//随机生成一个1-36的数字
		//如果已被选取则跳过
		if (i > 0 && existed(arr, j, i) == 1)
			continue;
		//记录下该数字
		arr[i++] = j;
	}
	//打开要存储数独的文件
	FILE* fp;
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("打开文件失败\n");
		return ERROR;
	}
	fprintf(fp, "cBinary_Puzzle\n");
	fprintf(fp, "p cnf 36 %d\n", init + 2376);
	for (i = 0; i < init; i++)
	{
		fprintf(fp, "%d 0\n", arr[i]);//先生成18个符合要求的随机数写入文件，再去用三个规则去限定，共生成18+2376 
	}
	fclose(fp);
	//任意条件不满足则返回FALSE
	rule_1(filename);
	rule_2(filename);
	rule_3(filename);
	SqList ans, bug;//存正确答案的顺序表 
	pcnf L;//索引表结构
	//创建CNF文件，初始化存储结构
	createCNF(&L, filename);
	//初始化
	InitList(L, ans);
	//执行DPLL
	DPLL(L, ans, 1);
	//倘若检测到失败，继续创建
	if (test(L, ans) != TRUE) create_puzzle(filename);
	else printf("正确答案生成完毕\n");
	//visit数组存储初局情况，为0的下标则是挖去的'.'，否则必然对应一个answer中的值
	//answer记录终局
	int visit[37] = { 0 }, answer[37] = { 0 };
	for (i = 1; i <= 36; i++)
	{
		answer[i] = ans.elem[i];
		//为1则在矩阵对应位置也为1，-1对应-1，从一维坐标向二位坐标转化
		if (ans.elem[i] == 1)
			mat[(i - 1) / 6 + 1][(i - 1) % 6 + 1] = 1;
		else if (ans.elem[i] == -1)
			mat[(i - 1) / 6 + 1][(i - 1) % 6 + 1] = -1;
	}
	//res文件写入终局
	strcpy(resfile, filename);
	strcat(resfile, ".res");
	FILE* fp1 = fopen(resfile, "a+");
	if (!fp1) {
		printf("打开文件失败！");
	}
	for (int i = 1; i <= 36; i++)
	{
		if (ans.elem[i] == 1)
			fprintf(fp1, "1 ");
		else if (ans.elem[i] == -1)
			fprintf(fp1, "0 ");
		if (i % 6 == 0)
			fprintf(fp1, "\n");
	}
	fclose(fp1);
	//挖洞生成初局
	dig_hole(ans, visit, answer);
	//写入cnf文件
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("打开文件失败\n");
		return ERROR;
	}
	fprintf(fp, "cBinary_Puzzle\n");
	fprintf(fp, "p cnf 36 %d\n", init + 2376);
	
	strcpy(puzzlefile, filename);
	strcat(puzzlefile, ".puz");
	//写入初局
	FILE* fp2 = fopen(puzzlefile, "a+");
	if (!fp2) {
		printf("文件打开失败!");
	}
	for (i = 1; i <= 36; i++)
	{
		//visit为0说明不能被挖，那么就读入终局数据answer中对应的值
		if (visit[i] == 0)
		{
			if (answer[i] == -1)
			{
				fprintf(fp, "%d 0\n", -i);
				fprintf(fp2, "0");
			}
			else if (answer[i] == 1)
			{
				fprintf(fp, "%d 0\n", i);
				fprintf(fp2, "1");
			}
		}
		//能挖则存入'.'表示空格
		else if (visit[i] == 1)fprintf(fp2, ".");
		if (i % 6 == 0)fprintf(fp2, "\n");
	}

	fclose(fp);
	fclose(fp2);
	//按三个规则继续生成cnf文件
	rule_1(filename);
	rule_2(filename);
	rule_3(filename);
	return OK;
}


/*
功能：挖洞生成唯一解的棋盘
输入：存储答案的顺序表，存储是否被挖的数组visit，存储终局数值的数组
策略：通过随机挖洞，倘若挖到0，则判断当前值为1是否能通过DPLL做出有效解，倘若可以说明当前位置不是唯一解；
那么说明这个点必须固定死，才可能得到唯一解的初局
返回：成功生成则返回OK，否则返回FALSE
*/
int dig_hole(SqList ans, int* visit, int* answer)
{

	SqList An;//保存所求答案的线性表 
	pcnf L; // 存储结构
	int i, j;
	//阈值
	int flag = 60;
	while (flag--)
	{
		//随机得到1-36的数字
		j = rand() % 36 + 1;
		if (visit[j] == 0)	//没有被挖这个位置
		{
			//将标准棋盘的该出修改为相反的数，看是否能满足
			if (answer[j] == -1)
				answer[j] = 1;
			else if (answer[j] == 1)
				answer[j] = -1;
		}
		FILE* fp;//将挖洞后的棋盘写入文件 
		fp = fopen("test.cnf", "w");
		if (fp == NULL) {
			printf("打开文件失败\n");
			return ERROR;
		}
		fprintf(fp, "cBinary_Puzzle\n");
		fprintf(fp, "p cnf 36 %d\n", init + 2376);
		for (i = 1; i <= 36; i++)
		{
			if (visit[i] == 0)//visit为0说明不能挖 
			{
				if (answer[i] == -1)
					fprintf(fp, "%d 0\n", -i);
				else if (answer[i] == 1)
					fprintf(fp, "%d 0\n", i);
			}
		}
		fclose(fp);
		//按三个规则生成cnf公式集
		rule_1((char*)"test.cnf");	//按规则继续生成 
		rule_2((char*)"test.cnf");
		rule_3((char*)"test.cnf");
		createCNF(&L, (char*)"test.cnf");
		//初始化存储结构和存储答案的顺序表
		InitList(L, An);
		//DPLL求解
		//求解成功说明这个位置两个数填入均可，那么不能挖掉
		if (DPLL(L, An, 1) == OK && test(L, An) == TRUE) {
			//还原
			if (answer[j] == -1)
				answer[j] = 1;
			else if (answer[j] == 1)
				answer[j] = -1;

		}
		//失败说明该空为唯一解，可以挖掉
		else {
			visit[j] = 1;
		}
	}
	return 0;
}

/*
功能：选取初始的数字时防止重复
输入：存储初始选取数字的数组，找寻的位置，和数组长度
返回：有重复返回1，否则返回0
*/
int existed(int* arr, int n, int i)
{
	int j = 0;
	for (j = 0; j < i; j++)
		if (arr[j] == n)
			return 1;

	return 0;

}
