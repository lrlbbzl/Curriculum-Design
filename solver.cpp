/*
计交1901 lrl，3-10
模块功能：基于DPLL算法对SAT问题进行求解，包括对选择变元策略的优化，DPLL算法执行主体，
以及实现DPLL算法的诸多辅助函数
*/

#include "head.h"

/*
功能：判定是否有不符合条件的空语句
输入：存储子句的pcnf结构型变量L
输出：若存在返回OK，否则返回FALSE
*/
int EmptyClause(pcnf L) {
	//指向子句首结点
	clausenode p = L->firstc;
	while (p) {
		//变元数量已删除为0但删除标志mark仍为1，说明是不符合条件的空子句
		if (p->cnt == 0 && p->mark == 1) return OK;
		p = p->next_Cl;
	}
	return FALSE;
}

/*
功能：移除子句
输入：存储子句的pcnf型结构变量,需要删除的变元flag
返回：成功删除子句，返回OK
*/
int DeleteClause(pcnf L, int flag) {            //一次找出一个变元，并删除 
	clausenode p;    //子句头结点 
	literalnode q;    //文字节点
	piNode r;    //索引表节点
	//正变元
	if (flag > 0) {
		r = L->Index_List[flag].firstz;//r为指向变元flag以正变元所存在的第一处地址的指针
		while (r) {
			if (r->p_cnode->mark == 0)   r = r->next;//已经删除，则继续搜寻
			else {
				r->p_cnode->mark = 0;//删除，mark改为0
				r->p_cnode->flag = flag;//该子句因为flag而删除，故修改指向子句头结点的flag属性
				L->claunum--;//存储结构的子句数量减少1
				r = r->next;
			}
		}
		r = L->Index_List[flag].firstf; //删除文字
		while (r) {
			if (r->p_cnode->mark == 0)  r = r->next;//已经删除，继续搜寻
			else {
				q = r->p_cnode->word;//q为指向变元存在的子句的第一个文字结构
				while (q) {
					if (q->mark == 0)  q = q->next_Li;
					else {
						//删除正变元，而q中数据为负说明不能为子句成立作贡献，删去
						if (q->data == -flag) {
							q->mark = 0;
							r->p_cnode->cnt--;//所在子句变元数减少1
							break;
						}
						q = q->next_Li;
					}
				}
				r = r->next;
			}
		}
	}
	//为负变元
	else {
		r = L->Index_List[-flag].firstf;// 先删去子句 
		//过程基本同上，只是恰好反过来
		while (r) {
			if (r->p_cnode->mark == 0)   r = r->next;
			else {
				r->p_cnode->mark = 0;
				r->p_cnode->flag = flag;
				L->claunum--;
				r = r->next;
			}
		}
		r = L->Index_List[-flag].firstz; //删除文字
		while (r) {
			if (r->p_cnode->mark == 0)  r = r->next;
			else {
				q = r->p_cnode->word;
				while (q) {
					if (q->mark == 0)  q = q->next_Li;
					else {
						if (q->data == -flag) {
							q->mark = 0;
							r->p_cnode->cnt--;
							break;
						}
						q = q->next_Li;
					}
				}
				r = r->next;
			}
		}
	}

	//移除变元后变元总数减少1
	L->varinum--;
	return OK;
}

/*
功能：在遭遇失败时回溯，恢复因为flag而被删除的变元和子句
输入：已被初始化的存储子句的pcnf型结构变量L,要回溯的变元flag
返回：成功回溯变元后返回OK
*/
int FlashBack(pcnf L, int flag) {       //恢复前面因为flag而删除的元素 
	clausenode p;    //子句头结点 
	literalnode q;    //文字节点
	piNode r;    //索引表节点 
	if (flag > 0) {
		r = L->Index_List[flag].firstz;//r指向索引表中变元flag所出现的第一个以正变元出现的位置
		while (r) {
			if (r->p_cnode->mark == 1)  r = r->next;
			else {
				//由于是回溯正变元，那么在以正变元出现处需要回溯子句
				if (r->p_cnode->flag == flag) {
					r->p_cnode->mark = 1;//子句变为存在
					r->p_cnode->flag = 0;//flag重置
					L->claunum++;//子句数目增加1
				}
				r = r->next;
			}
		}
		r = L->Index_List[flag].firstf;  //r指向索引表中变元flag所出现的第一个以正变元出现的位置
		while (r) {
			q = r->p_cnode->word;
			while (q) {
				//由于是回溯正变元，在以负变元出现处需要回溯文字结构
				if (q->mark == 0 && q->data == -flag) {
					q->mark = 1;//文字变为存在
					r->p_cnode->cnt++;//所在子句的文字数增加1
					break;
				}
				q = q->next_Li;
			}
			r = r->next;
		}
	}
	//回溯负变元，过程与上一样，只是回溯变元和子句的条件反过来
	else {
		r = L->Index_List[-flag].firstf;
		while (r) {
			if (r->p_cnode->mark == 1)  r = r->next;
			else {
				if (r->p_cnode->flag == flag) {
					r->p_cnode->mark = 1;
					r->p_cnode->flag = 0;
					L->claunum++;
				}
				r = r->next;
			}
		}
		r = L->Index_List[-flag].firstz;
		while (r) {
			q = r->p_cnode->word;
			while (q) {
				if (q->mark == 0 && q->data == -flag) {
					q->mark = 1;
					r->p_cnode->cnt++;
					break;
				}
				q = q->next_Li;
			}
			r = r->next;
		}
	}
	L->varinum++;//子句存储结构中存在变元数量加1 
	return OK;
}


/*
功能：寻找下一个要赋予真值的变元
输入：存储子句的pcnf结构型变量L，存储变元真值表的SqList型顺序表
返回：返回所取得的要赋真值的变元
*/
int FindNextNum(pcnf L, SqList& An) {
	clausenode p;
	literalnode q;
	int i; int _i;
	int flag = 0;
	p = L->firstc;
	while (p) {
		if (p->mark == 1 && p->cnt == 1) {      //寻找单子句 
			q = p->word;
			while (q) {
				if (q->mark == 1) {
					if (q->data > 0) 	An.elem[q->data] = 1;
					else    An.elem[-(q->data)] = -1;
					return q->data;
				}
				else q = q->next_Li;
			}
		}
		else p = p->next_Cl;
	}
	//单子句不存在，就寻找出现次数最多的那个变元
	int* a = (int*)malloc((2 * An.length - 1) * sizeof(int)); //构建数组a，存储各变元出现的次数
	for (i = 0; i < 2 * An.length - 1; i++) {
		a[i] = 0;
	}
	p = L->firstc;
	//遍历存储结构，计入当前各变元出现次数
	while (p) {
		if (p->mark == 0) p = p->next_Cl;
		else {
			q = p->word;
			while (q) {
				if (q->mark == 0) q = q->next_Li;
				else {
					if (q->data > 0) a[2 * (q->data) - 1]++;
					else if (q->data < 0)    a[2 * (-(q->data))]++;
					q = q->next_Li;
				}
			}
			p = p->next_Cl;
		}
	}
	//找到最大出现次数对应的变元
	for (i = 1; i < 2 * An.length - 1; i++) {
		if (a[i] > flag)  flag = a[i], _i = i;
	}
	/*
	for (i = 1; i < 2 * An.length - 1; i++) {  //找到变元
		if (a[i] == flag)  break;
	}*/
	i = _i;
	//释放空间
	free(a);
	//赋予真值并返回变元
	if (i % 2) {
		An.elem[(i + 1) / 2] = 1;
		return ((i + 1) / 2);
	}
	else {
		An.elem[i / 2] = -1;
		return (-(i / 2));
	}
}


/*
功能：DPLL算法的主体
输入：存储子句结构的pcnf型变量，存储答案的顺序表，当前处理的int型变量所选取的变元
返回：处理成功返回OK否则返回FALSE
*/
int DPLL(pcnf L, SqList& An, int now_l) {
	int next_l;//下一个要处理的变元
	//公式集已删除为空返回OK
	if (L->claunum == 0) return OK;
	else {
		if (EmptyClause(L) == OK) {       //有不合理的空子句 
			//回溯变元
			if (FlashBack(L, now_l) != OK)  printf("回溯失败\n");
			//处理变元为正，因为失败，赋予0
			if (now_l > 0) An.elem[now_l] = 0;
			//负变元则相反
			else An.elem[-now_l] = 0;
			return FALSE;
		}
		else {
			//当前处理合理，寻找下一个变元
			next_l = FindNextNum(L, An);
			//为0说明没有变元可找
			if (next_l == 0) return FALSE;
			//根据选取变元删除子句
			if (DeleteClause(L, next_l) != OK)  printf("删除失败\n");
			//递归调用DPLL函数，若返回OK则说明求解成功，返回OK
			if (DPLL(L, An, next_l) == OK)  return OK;
			else {
				//失败，回溯当前选取变元
				if (FlashBack(L, next_l) != OK) printf("恢复失败\n");
				//尝试赋予相反值来移除子句
				if (DeleteClause(L, -next_l) != OK) printf("删除失败\n");
				//递归调用，传入负变元
				if (DPLL(L, An, -next_l) == OK) {
					//负变元有效，根据变元原符号，负值则应取-1，正值则取1
					if (next_l > 0) An.elem[next_l] = -1;
					else  An.elem[-next_l] = 1;
					return OK;
				}
				else {
					//正负都不行，重置为0，回溯到上一层
					if (FlashBack(L, -next_l) != OK) printf("恢复失败\n");
					if (next_l > 0) An.elem[next_l] = 0;
					else An.elem[-next_l] = 0;
					return FALSE;
				}
			}
		}
	}
}

/*
功能：测试当前的真值表是否满足所有公式都成立
输入：存储子句的pcnf类型变量，存储答案的顺序表
输出：求解正确返回TRUE否则返回FALSE
*/
int test(pcnf L, SqList& An) {
	int flag;//正确与否的标志
	clausenode p;//子句结点
	literalnode q;//文字结点
	p = L->firstc;
	//遍历每一个子句去验证
	while (p) {
		flag = 0;
		q = p->word;
		//遍历文字序列，出现满足条件flag为1，break
		while (q) {
			if (((q->data > 0) && (An.elem[q->data] == 1)) || ((q->data < 0) && (An.elem[-(q->data)] == -1))) {
				flag = 1;
				break;
			}
			else q = q->next_Li;
		}
		//只要有一个子句不满足，就为错误
		if (flag == 0) break;
		p = p->next_Cl;
	}
	if (flag == 0)  return FALSE;
	else return TRUE;
}
