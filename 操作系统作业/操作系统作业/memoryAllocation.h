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
	strcpy(tail->data.name, "ʲô��û��");
	tail->data.address = 0;
	tail->data.size = maxSize;
	tail->data.state = FREE;
}

void display(){
	cout << "-----------------------------------------�ڴ�������-----------------------------------------------" << endl;
	freeareaNode *p = head->next;
	while (p){
		cout << "���з�������: ";
		if (strcmp(p->data.name, "ʲô��û��") == 0){
			cout << "������" << "     " << "��ʼ��ַ��" << p->data.address << "      " << "������ַ��" << p->data.address + p->data.size
				<< "     " << "������С��" << p->data.size << "     " << "����״̬������" << endl;
		}
		else{
			cout << p->data.name;
			cout << "     " << "��ʼ��ַ��" << p->data.address << "      " << "������ַ��" << p->data.address + p->data.size
				<< "     " << "������С��" << p->data.size << "     " << "����״̬��";
			if (p->data.state == FREE)
				cout << "����" << endl;
			if (p->data.state == BUSY)
				cout << "ռ��" << endl;
		}
		p = p->next;
	}
	cout << "-----------------------------------------------------------------------------------------------------" << endl;
}

int  bestAllocation(char *name, int size){
	//int num, size;
	//cout << "��������ҵ�ţ�ע����ҵ�Ų�ҪΪ0���ͷ���������С��" << endl;
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
	//cout << "��������Ҫ���յ���ҵ�ţ�" << endl;
	//cin >> num;
	freeareaNode *p = head;
	while (p){
		if (strcmp(p->data.name, name) == 0){
			p->data.state = FREE;
			strcpy(p->data.name, "ʲô��û��");
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
bestAllocation("�����", 50);
bestAllocation("΢��", 30);
bestAllocation("�Ա�", 20);
bestAllocation("������", 10);
cout << "���Ͻ��" << endl;
recycle("�����");
recycle("΢��");
bestAllocation("�����", 50);
bestAllocation("΢��", 30);
cout << "���½��" << endl;
recycle("�Ա�");
recycle("΢��");
bestAllocation("�Ա�", 20);
bestAllocation("΢��", 30);
cout << "���м���" << endl;
recycle("�����");
recycle("΢��");
recycle("�Ա�");

return 0;
}*/