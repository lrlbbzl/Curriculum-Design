/*
校交1901：lrl 3-10
功能：函数主模块，负责完成提示面板的显示，负责调用头文件中定义的各种函数来完成两个模块的功能；
完成输入输出任务，将两个模块：SAT求解器和二进制数独棋盘集成在一起；
*/
#include"head.h"

int mat[8][8];
int main(int argc, char** argv) {
	pcnf L = NULL;//cnf公式头结点
	SqList An, Ans;//用于存储答案的顺序表
	clock_t start, end;//记录时间的变量
	double duration;//时间差，用于表示DPLL过程快慢
	char filename[80];//记录读入的cnf文件
	//功能选择过程的控制变量
	int op = 1;
	int op_1 = 1;
	int op_2 = 1;
	int tag = -1;
	while (op) {//两大模块sat和puzzle 
		system("cls");	printf("\n\n");
		printf("      Menu for SAT or Sudoku \n");
		printf("------------------------------------------------------\n");
		printf("            1.SAT                  2.BinaryPuzzle          \n");
		printf("            0.Exit                                    \n");
		printf("------------------------------------------------------\n");
		printf("    Choose operation[0~2]:");
		scanf("%d", &op);
		switch (op) {
		case 1:
			op_1 = 1;
			while (op_1) {//sat
				system("cls");	printf("\n\n");
				printf("     SAT Part    \n");
				printf("----------------------------------------------------\n");
				printf("       1. ReadCnf         2. TraverseCnf             \n");
				printf("       3. DPLL            4.Certificate                  \n");
				printf("       5. ShowAnswer      6. WriteFile             \n");
				printf("       0. Exit                                      \n");
				printf("----------------------------------------------------\n");
				printf("     Choose operation[0~6]:\n");
				scanf("%d", &op_1);
				switch (op_1) {
				case 1:
					printf("请输入选择的文件名\n");
					scanf("%s", filename);
					//首先根据CNF文件创建cnf公式集
					if (createCNF(&L, filename) == OK) printf("cnf公式创建成功\n");
					//创建失败
					else {
						printf("cnf公式创建失败\n");
						getchar(); getchar();
						break;
					}
					//根据L初始化存储答案的顺序表
					InitList(L, An);
					getchar(); getchar();
					break;
				case 2:
					//实现打印cnf公式集来判断读入文件是否成功的功能
					if (Traverse(L) == OK) printf("cnf公式打印成功\n");
					else printf("打印失败\n");
					getchar(); getchar();
					break;
				case 3:
					start = clock();//计时起点
					//实现DPLL
					if (DPLL(L, An, 1) == OK) {
						tag = 1;
						printf("求解成功\n");
					}
					else {
						tag = 0;
						printf("求解失败\n");
					}
					end = clock();//计时终点
					duration = ((double)(end - start)) / CLK_TCK * 1000;//计算用时
					printf("运行时间为：%g ms\n", duration);
					getchar(); getchar();
					break;
				case 4:
					//判断答案是否正确
					if (test(L, An) == TRUE) printf("答案正确\n");
					else printf("答案不正确\n");
					getchar(); getchar();
					break;
				case 5:
					//打印真值表信息
					for (int i = 1; i < An.length; i++) {
						if (An.elem[i] == -1)  printf("%5d", -i);
						else printf("%5d", i);
						if (i % 10 == 0)  printf("\n");
					}
					getchar(); getchar();
					break;
				case 6:
					//将求解信息写入文件
					if (ResWrite_1(tag, duration, An, filename) == OK) printf("文件写入成功\n");
					else printf("文件写入失败\n");
					getchar(); getchar();
					break;
				case 0:
					break;
				}
			}
			getchar(); getchar();
			break;

		case 2:
			op_2 = 1;
			//Sudoku部分
			while (op_2) {
				system("cls");	printf("\n\n");
				printf("     数独游戏    \n");
				printf("----------------------------------------------------\n");
				printf("       1. Unique Solution Puzzle       \n");
				printf("       0. Exit                                      \n");
				printf("----------------------------------------------------\n");
				printf("     Choose operation[0~6]:\n");
				scanf("%d", &op_2);
				switch (op_2) {
				case 1:
					printf("请输入存储的文件名\n");
					scanf("%s", filename);
					//创建数独初局
					if (create_puzzle(filename) == OK) printf("随机生成初局成功\n");
					else printf("随机生成初局失败\n");
					getchar(); getchar();
					break;

				case 0:
					break;
				}
			}
			break;
		case 0:
			break;

		default:
			printf("输入错误！\n");
			getchar(); getchar();
			break;
		}
	}
	printf("欢迎下次再使用本系统！\n");
	return 0;
}
