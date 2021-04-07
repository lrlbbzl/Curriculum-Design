#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <time.h>

#define TRUE 1        //判断为真 
#define FALSE 0       //判断为假 
#define OK 1          //操作执行成功 
#define ERROR 0       //操作执行失败 
#define INFEASTABLE -1     //结构体未分配存储空间 
#define OVERFLOW -2       //发生溢出，分配储存空间失败 

//文字结构
typedef struct LiteralNode {
	int data;         //存储保存的文字
	int mark;         //文字是否被删除的标记，未被删除为1            
	struct LiteralNode* next_Li;  //指向同一子句中的下一个文字结点
}LiteralNode, * literalnode;//存储文字的结构和指针

//子句结构
typedef struct ClauseNode {
	int cnt;        //即时表示子句中仍然剩下的文字的个数
	int mark;       //标记该子句是否被删除 ，未被删除时为1 
	int flag;       //若子句被删除，则flag记录子句删除的因素，即子句是因为变元flag而被删除的
	LiteralNode* word;          //指向这个子句的第一个文字节点 
	struct ClauseNode* next_Cl;        //指向下一条子句 
}ClauseNode, * clausenode;     //子句头节点结构 ，指针

typedef struct iNode {
	ClauseNode* p_cnode;       //存储指向各文字头结点的指针 
	struct iNode* next;
}iNode, * piNode;    //索引表的结构和指针

typedef struct {
	iNode* firstz;       //指向正文字的索引指针
	iNode* firstf;       //指向负文字的索引指针
} index, * pindex;//对索引表结点的定义 

typedef struct cnf {
	int varinum;       //变元个数 
	int claunum;        //子句个数
	ClauseNode* firstc;      //指向第一个子句	节点 
	index* Index_List;    //索引表 
}cnf, * pcnf;          //一个cnf公式

typedef struct SqList {
	int* elem;//动态申请空间的int型数组，存储对应下标变元的取值
	int length;//数组长度
}SqList;//一个顺序表结构，存储答案 

//函数功能见定义时
int createCNF(pcnf* L, char filename[80]);        //创建CNF式 
int Traverse(pcnf L);     //打印CNF公式 
int InitList(pcnf L, SqList& An);
int DeleteClause(pcnf L, int flag);
int FlashBack(pcnf L, int flag);
int EmptyClause(pcnf L);
int FindNextNum(pcnf L, SqList& An);
int DPLL(pcnf L, SqList& An, int now_l);
int test(pcnf L, SqList& An);
int rule_1(char filename[80]);
int rule_2(char filename[80]);
int rule_3(char filename[80]);
int write(char filename[80]);
int WriteCNF(char filename[80]);
int ResWrite_1(int res, double time, SqList& An, char filename[80]);
int ResWrite_2(int res, double time, SqList& An, char filename[80]);
int create_puzzle(char filename[80]);
int existed(int* arr, int n, int i);
int dig_hole(SqList ans, int* visit, int* answer);
