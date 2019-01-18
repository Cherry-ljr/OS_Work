#include<iostream>
#include<stdlib.h>
#include<string.h>
using namespace std;
#define maxSize 640
enum status { FREE, BUSY };

struct freearea{
	char name[100];
	int size;
	int address;
	status state;
};

typedef struct freeareaNode{
	freearea data;
	freeareaNode *last;
	freeareaNode *next;
}*linklist;


linklist head, tail;

void initial(){
	head = new freeareaNode;
	tail = new freeareaNode;
	head->last = NULL;
	head->next = tail;
	tail->last = head;
	tail->next = NULL;
	strcpy(tail->data.name, "什么都没有");
	tail->data.address = 0;
	tail->data.size = maxSize;
	tail->data.state = FREE;
}

void display(){
	cout << "-----------------------------------------内存分配情况-----------------------------------------------" << endl;
	freeareaNode *p = head->next;
	while (p){
		cout << "空闲分区名称: ";
		if (strcmp(p->data.name, "什么都没有") == 0){
			cout << "空闲区" << "     " << "起始地址：" << p->data.address << "      " << "结束地址：" << p->data.address + p->data.size
				<< "     " << "分区大小：" << p->data.size << "     " << "分区状态：空闲" << endl;
		}
		else{
			cout << p->data.name;
			cout << "     " << "起始地址：" << p->data.address << "      " << "结束地址：" << p->data.address + p->data.size
				<< "     " << "分区大小：" << p->data.size << "     " << "分区状态：";
			if (p->data.state == FREE)
				cout << "空闲" << endl;
			if (p->data.state == BUSY)
				cout << "占用" << endl;
		}
		p = p->next;
	}
	cout << "-----------------------------------------------------------------------------------------------------" << endl;
}

int  bestAllocation(char *name, int size){
	//int num, size;
	//cout << "请输入作业号（注意作业号不要为0）和分配的主存大小：" << endl;
	//cin >> num >> size;
	int minSpace = maxSize;
	linklist work = new freeareaNode;
	strcpy(work->data.name, name);
	work->data.size = size;
	work->data.state = BUSY;
	freeareaNode *p = head->next;
	freeareaNode *q = NULL;
	while (p){
		if ((p->data.size > size || p->data.size == size) && p->data.state == FREE)
		if (p->data.size - size < minSpace){
			q = p;
			minSpace = p->data.size - size;
		}
		p = p->next;
	}
	if (q == NULL)
		return 0;
	else{
		if (minSpace == 0){
			strcpy(q->data.name, name);
			q->data.state = BUSY;
			display();
			return 1;
		}
		else{
			work->last = q->last;
			work->next = q;
			work->data.address = q->data.address;
			q->last->next = work;
			q->last = work;
			q->data.address = q->data.address + size;
			q->data.size -= size;
			display();
			return 1;
		}
	}
}

int recycle(char *name){
	//int num;
	//cout << "请输入需要回收的作业号：" << endl;
	//cin >> num;
	freeareaNode *p = head;
	while (p){
		if (strcmp(p->data.name, name) == 0){
			p->data.state = FREE;
			strcpy(p->data.name, "什么都没有");
			if (p->next->data.state == FREE){
				if (p->last->data.state == FREE){
					p->last->data.size += (p->data.size + p->next->data.size);
					if (p->next->next){
						p->last->next = p->next->next;
						p->next->next->last = p->last;
					}
					else{
						p->last->next = NULL;
						p->next = NULL;
					}
				}
				else{
					p->data.size += p->next->data.size;
					if (p->next->next != NULL){
						p->next->next->last = p;
						p->next = p->next->next;
					}
					else{
						p->next = NULL;
					}
				}
			}
			else if (p->last->data.state == FREE){
				p->last->data.size += p->data.size;
				p->last->next = p->next;
				p->next->last = p->last;
			}
			break;
		}
		p = p->next;
	}
	display();
	return 1;
}
/*int main(){
initial();
int choose;
bestAllocation("浏览器", 50);
bestAllocation("微信", 30);
bestAllocation("淘宝", 20);
bestAllocation("计算器", 10);
cout << "向上结合" << endl;
recycle("浏览器");
recycle("微信");
bestAllocation("浏览器", 50);
bestAllocation("微信", 30);
cout << "向下结合" << endl;
recycle("淘宝");
recycle("微信");
bestAllocation("淘宝", 20);
bestAllocation("微信", 30);
cout << "向中间结合" << endl;
recycle("浏览器");
recycle("微信");
recycle("淘宝");

return 0;
}*/