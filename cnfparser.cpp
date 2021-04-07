/*
计交1901 lrl 3-10
模块功能：完成CNF文件的读写功能，同时在读取CNF文件的时候完成对子句和文字存储结构类型的
填充，同时包含每次进行读取CNF文件前的清空邻接表，清空索引结构，以及完成验证读取是否正确的
遍历存储结构的函数
*/
#include "head.h"

/*
功能：为用于存储答案的顺序表分配存储空间
输入：已根据CNF文件而创建好的cnf公式集存储结构pcnf型变量L，和未初始化的存储答案的顺序表
返回：空间分配成功返回OK
*/
int InitList(pcnf L, SqList& An) {
	An.elem = (int*)malloc((L->varinum + 1) * sizeof(int));//根据长度动态分配存储空间 
	if (!An.elem) {
		return ERROR;//分配存储空间失败 返回ERROR
	};
	An.length = L->varinum + 1;//初始化线性表的长度为0 
	for (int i = 1; i < An.length; i++) {
		An.elem[i] = 0;//elem[i]=0表示变元i此时尚未被赋予真值
	}
	return OK;
}


/*
功能：读取cnf文件，构造子句，问题等存储结构
输入：未初始化的存储子句的pcnf结构，cnf文件名
输出：成功读入返回OK否则返回FALSE
*/
int createCNF(pcnf* L, char filename[80]) {
	clausenode p;    //子句结点 
	literalnode q;    //文字节点
	piNode r;    //索引节点 
	int i;
	//动态创建存储空间
	*L = (cnf*)malloc(sizeof(struct cnf));
	p = (ClauseNode*)malloc(sizeof(struct ClauseNode));
	//初始属性赋值
	p->flag = 0;
	p->mark = 1;
	(*L)->firstc = p;
	FILE* fp;//文件指针
	int num = 0;
	int k = 0;//记录每个子句文字个数 
	int m = 0;//记录子句的个数 
	int flag;//记录正负 
	char c;//用来读入
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("打开文件失败\n");
		return 0;
	}
	fscanf(fp, "%c", &c);
	while (!feof(fp)) {
		if (c == 'c') {//用来跳过cnf文件注释部分 
			while (c != '\n') {
				fscanf(fp, "%c", &c);
			}
			fscanf(fp, "%c", &c);
		}
		else if (c == 'p') {
			while (c != '\n') {
				while (c < '0' || c>'9') {
					fscanf(fp, "%c", &c);
				}
				//获取变元数目
				while (c >= '0' && c <= '9') {
					num = num * 10;
					num += c - '0';
					fscanf(fp, "%c", &c);
				}
				//存储结构的属性赋值
				(*L)->varinum = num;
				//动态赋予索引结构空间
				(*L)->Index_List = (index*)malloc((num + 1) * sizeof(index));
				//初始化索引结构，指针均指向NULL
				for (i = 0; i <= num; i++) {
					(*L)->Index_List[i].firstf = NULL;
					(*L)->Index_List[i].firstz = NULL;
				}
				num = 0;
				while (c < '0' || c>'9') {
					fscanf(fp, "%c", &c);
				}
				//获取子句数 
				while (c >= '0' && c <= '9') {
					num = num * 10;
					num += c - '0';
					fscanf(fp, "%c", &c);
				}
				//赋予属性相应值
				(*L)->claunum = num;
				num = 0;
			}
			fscanf(fp, "%c", &c);
		}
		else if ((c >= '0' && c <= '9') || (c == '-')) {
			//动态赋予文字结构空间
			q = (LiteralNode*)malloc(sizeof(struct LiteralNode));
			//初始时文字存在
			q->mark = 1;
			p->word = q;
			while (c != '\n') {
				while (c != '0') {
					while (c != ' ') {
						flag = 1;
						while ((c >= '0' && c <= '9') || (c == '-')) {
							if (c == '-') {
								fscanf(fp, "%c", &c);
								flag = 0;
							}
							num = num * 10;
							num += c - '0';
							fscanf(fp, "%c", &c);
						}
						//建立索引结构
						r = (iNode*)malloc(sizeof(struct iNode));
						//指针指向当前所在的子句结点
						r->p_cnode = p;
						if (flag == 0) {//为负变元 
							q->data = -num;
							r->next = (*L)->Index_List[num].firstf;
							(*L)->Index_List[num].firstf = r;
						}
						else {//为正变元
							q->data = num;
							r->next = (*L)->Index_List[num].firstz;
							(*L)->Index_List[num].firstz = r;
						}
						num = 0;
						k++;
					}
					fscanf(fp, "%c", &c);
					//读到0则说明子句读取结束，进入下一个子句的读入
					if (c == '0') q->next_Li = NULL;
					//动态创建下一个文字存储结构的空间，指针移向下一个待创建的文字结构
					else {
						q->next_Li = (LiteralNode*)malloc(sizeof(struct LiteralNode));
						q = q->next_Li;
						q->mark = 1;
					}
				}
				fscanf(fp, "%c", &c);
			}
			fscanf(fp, "%c", &c);
			p->cnt = k;
			k = 0;
			m++;//读取子句数增加
			//实际读取子句未达到子句总数，动态分配下一个子句的存储空间
			//指针移向下一个子句
			if (m < (*L)->claunum) {
				p->next_Cl = (ClauseNode*)malloc(sizeof(struct ClauseNode));
				p = p->next_Cl;
				p->flag = 0;
				p->mark = 1;
			}
			else p->next_Cl = NULL;
		}
		else fscanf(fp, "%c", &c);//读取剩余可能出现的无用部分
	}
	fclose(fp);
	return OK;
}



/*
功能：打印CNF公式集
输入：已初始化的pcnf型的子句存储结构
返回：成功打印信息返回OK
*/
int Traverse(pcnf L) {
	if (L->claunum == 0) {
		printf("cnf公式为空\n");
		return OK;
	}
	clausenode p = L->firstc;
	literalnode q = p->word;
	//整体信息
	printf("变元数：%d  子句数：%d\n", L->varinum, L->claunum);
	int i = 1;
	int j;
	//遍历每一个子句结构
	while (p) {
		//mark为0则移向下一个
		if (p->mark == 0) p = p->next_Cl;
		//打印信息
		else {
			q = p->word;
			j = 1;
			//打印一行表示一个子句
			while (q) {
				if (q->mark == 0)  q = q->next_Li;
				else {
					printf("%d   ",q->data);
					q = q->next_Li;
				}
			}
			//子句间换行
			printf("\n");
			p = p->next_Cl;
		}

	}
	return OK;
}
