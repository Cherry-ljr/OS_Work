#include "cv.h"
#include "highgui.h"
#include "memoryAllocation.h"

#pragma comment(lib,"WinMM.Lib")
#include "mmsystem.h"      
#pragma comment(lib,"winmm.lib")

#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <time.h>
#include <string>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <windows.h>  
using namespace std;

int Disk_Query(char *name);

#define MAXPROCESS 50                        //最大进程数
#define MAXRESOURCE 100                      //最大资源数
int AVAILABLE[MAXRESOURCE];                  //可用资源数组
int MAX[MAXPROCESS][MAXRESOURCE];            //最大需求矩阵
int ALLOCATION[MAXPROCESS][MAXRESOURCE];     //分配矩阵
int NEED[MAXPROCESS][MAXRESOURCE];           //需求矩阵
int REQUEST[MAXPROCESS][MAXRESOURCE];        //进程需要资源数
bool FINISH[MAXPROCESS];                     //系统是否有足够的资源分配
int p[MAXPROCESS];                           //记录序列
int m, n;                                     //m个进程,n个资源
void Init();    //初始化变量
bool Safe();    //安全检测
bool Bank();    //银行家算法
void showdata(int, int);    //显示输出系统信息

/*初始化变量*/
void Init_Bank()
{
	m = 8, n = 5;
	int i, j;
	srand(time(NULL));
	cout << "初始化随机分配每个进程最多所需的各资源数...." << endl;
	//初始化随机分配每个进程最多所需的各资源数
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			int val = rand() % 6;// 0 - 5
			MAX[i][j] = val;
		}
	}
	cout << "初始化每个进程已分配的各资源数...." << endl;
	//初始化每个进程已分配的各资源数
	for (i = 0; i < m; i++)
	for (j = 0; j < n; j++)
	{
		ALLOCATION[i][j] = 0;
		NEED[i][j] = MAX[i][j] - ALLOCATION[i][j];
		if (NEED[i][j] < 0)
		{
			cout << "您输入的第" << i + 1 << "个进程所拥有的第" << j + 1 << "个资源数错误,请重新输入:" << endl;
			j--;
			continue;
		}
	}
	cout << "初始化各个资源现有的数目...." << endl;
	for (i = 0; i < n; i++)
		AVAILABLE[i] = 8;
}

/*银行家算法*/
bool Bank()
{
	bool ok = false;//进程的资源请求是否同意
	int i, cusneed = 1, flag = 0;    //cousneed资源进程号
	while (1)
	{
		showdata(n, m);
		cout << endl;
		/*请求资源*/
		while (true)
		{
			if (cusneed > m)
			{
				cout << "没有该进程，请重新输入" << endl;
				return false;
			}
			cout << "请输入进程所请求的各资源的数量" << endl;
			for (i = 0; i < n; i++)
				cin >> REQUEST[cusneed][i];
			for (i = 0; i < n; i++)
			{
				if (REQUEST[cusneed][i] > NEED[cusneed][i])    //如果用户选择的线程的第i个资源请求数＞该线程该资源所需的数量
				{
					cout << "请求数超过进程的需求量!" << endl;
					return false;
				}
				if (REQUEST[cusneed][i] > AVAILABLE[i])    //如果用户选择的线程的第i个资源请求数＞系统现有的第i个资源的数量
				{
					cout << "请求数超过系统有的资源数!" << endl;
					return false;
				}
			}
			break;
		}
		/*如果请求合理，那么开始银行家算法计算*/
		/*先将申请的资源进行分配*/
		for (i = 0; i < n; i++)
		{
			AVAILABLE[i] -= REQUEST[cusneed][i];            //系统可用资源减去申请了的
			ALLOCATION[cusneed][i] += REQUEST[cusneed][i];    //线程被分配的资源加上已申请了的
			NEED[cusneed][i] -= REQUEST[cusneed][i];        //线程还需要的资源减去已申请得到的
		}
		/*判断分配申请资源后的系统是否安全；如果不安全则将分配的申请资源还回系统*/
		if (Safe()) {   //AVAILABLE  ALLOCATION  NEED变动之后，是否会导致不安全
			cout << "同意分配请求!" << endl;
			ok = true;
		}
		else
		{
			cout << "您的请求被拒绝!" << endl;
			ok = false;
			/*资源还回系统*/
			for (i = 0; i < n; i++)
			{
				AVAILABLE[i] += REQUEST[cusneed][i];
				ALLOCATION[cusneed][i] -= REQUEST[cusneed][i];
				NEED[cusneed][i] += REQUEST[cusneed][i];
			}
		}
		/*对进程的需求资源进行判断；是否还需要资源；即NEED数组是否为0*/
		for (i = 0; i < n; i++)
		if (NEED[cusneed][i] <= 0)
			flag++;
		if (flag == n)    //如果该进程各资源都已满足条件，则释放资源
		{
			for (i = 0; i < n; i++)
			{
				AVAILABLE[i] += ALLOCATION[cusneed][i];
				ALLOCATION[cusneed][i] = 0;
				NEED[cusneed][i] = 0;
			}
			cout << "进程" << cusneed << " 占有的资源被释放！" << endl;
			flag = 0;
		}
		for (i = 0; i < m; i++)    //分配好了以后将进程的标识FINISH改成false
			FINISH[i] = false;
		break;
	}
	return ok;
}

/*安全性算法*/
bool Safe()
{
	int i, j, k, l = 0;
	int Work[MAXRESOURCE];    //工作数组
	/*工作数组赋值，与AVAILABLE数组相同*/
	for (i = 0; i < n; i++)
		Work[i] = AVAILABLE[i];
	/*FINISH数组赋值，初始为全部false*/
	for (i = 0; i < m; i++)
		FINISH[i] = false;    //FINISH记录每个进程是否安全
	while (l < m)    //正常的话，共执行m次
	{
		int init_index = l;
		for (i = 0; i < m; i++)
		{
			if (FINISH[i] == true)    //如果这个进程安全则继续下一个循环
				continue;
			for (j = 0; j < n; j++)
			if (NEED[i][j] > Work[j])
				break;
			if (j == n)
			{
				FINISH[i] = true;
				for (k = 0; k < n; k++)
					Work[k] += ALLOCATION[i][k];
				p[l++] = i;//记录进程号
			}
			else    //如果超过继续循环下一个进程
				continue;
		}
		if (l == init_index)
		{
			cout << "系统是不安全的" << endl;
			return false;
		}
	}
	cout << "系统是安全的" << endl;
	cout << "安全序列:" << endl;
	for (i = 0; i < l; i++)
	{
		cout << p[i];
		if (i != l - 1)
			cout << "-->";
	}
	cout << endl;
	return true;
}

/*显示*/
void showdata(int n, int m)
{
	int i, j;
	cout << endl << "-------------------------------------------------------------" << endl;
	cout << "系统可用的资源数为:    ";
	for (j = 0; j < n; j++)
		cout << "    " << AVAILABLE[j];
	cout << endl << "各进程还需要的资源量:" << endl;
	for (i = 0; i < m; i++)
	{
		cout << "    进程" << i << ":";
		for (j = 0; j < n; j++)
			cout << "     " << NEED[i][j];
		cout << endl;
	}
	cout << endl << "各进程已经得到的资源量:    " << endl << endl;
	for (i = 0; i < m; i++)
	{
		cout << "    进程" << i << ":";
		for (j = 0; j < n; j++)
			cout << "     " << ALLOCATION[i][j];
		cout << endl;
	}
	cout << endl;
}


#define MAX_TASK 105
static int total_cpu = 100;//系统CPU总大小
static int total_memory = 100;//系统内存总大小
static int total_disk = 100;//磁盘空间总大小
int disk_status[100] = { 0 };//总共100个磁道
static int device_num = 9;
queue<int> DeviceTable[6];//设备阻塞队列表
int device_status[6] = { 0 };//设备状态 0表示未占用 1表示已占用


struct process_block {//进程控制块
	char _name[30]; //进程名称
	int _need_time; //所需要的时间
	int _cpu; //占用CPU的情况
	int _memory; //占用内存情况
	int _deviece; //需要占用哪种设备 0 - 5   0表示系统设备
	char _state;     //目前的运行状态   R为运行中 S为挂起 E为运行结束
	process_block *next;
};
process_block process_head;//定义进程链表头

void Init_Process() {//初始化进程链表
	strcpy(process_head._name, "system service");//默认含有系统服务
	process_head.next = NULL;
	process_head._cpu = 5;
	process_head._memory = 5;
	process_head._need_time = 100000;//无穷大值，从系统启动持续到系统关闭
	process_head._deviece = 0;//不占用设备
	process_head._state = 'R';
	device_status[0] = 1;
}

process_block *now_process = &process_head;

struct task_struct {
	char name[20]; //进程名称
	int pid;    //进程编号
	int come_time; //到达时间
	int run_begin_time; //开始运行时间
	int run_time; //运行时间
	int run_end_time; //运行结束时间
	int priority; //优先级
	int order; //运行次序
	int run_flag; //调度标志
	int device;
}tasks[MAX_TASK];


void Run_Process(char* name) {
	if (strcmp(name, "vs") == 0) {
		struct process_block *p = (struct process_block *)malloc(sizeof(struct process_block));
		strcpy(p->_name, "vs");
		p->_need_time = 10;
		p->_cpu = 3;
		p->_memory = 4;
		p->_deviece = 1;
		p->_state = 'S';
		p->next = NULL;
		now_process->next = p;
		now_process = p;
	}
	else if (strcmp(name, "excel") == 0) {
		struct process_block *p = (struct process_block *)malloc(sizeof(struct process_block));
		strcpy(p->_name, "excel");
		p->_need_time = 9;
		p->_cpu = 3;
		p->_memory = 4;
		p->_deviece = 5;
		p->_state = 'S';
		p->next = NULL;
		now_process->next = p;
		now_process = p;
	}
	else if (strcmp(name, "ie") == 0) {
		struct process_block *p = (struct process_block *)malloc(sizeof(struct process_block));
		strcpy(p->_name, "ie");
		p->_need_time = 6;
		p->_cpu = 2;
		p->_memory = 3;
		p->_deviece = 1;
		p->_state = 'S';
		p->next = NULL;
		now_process->next = p;
		now_process = p;
	}
	else if (strcmp(name, "ppt") == 0) {
		struct process_block *p = (struct process_block *)malloc(sizeof(struct process_block));
		strcpy(p->_name, "ppt");
		p->_need_time = 8;
		p->_cpu = 1;
		p->_memory = 2;
		p->_deviece = 3;
		p->_state = 'S';
		p->next = NULL;
		now_process->next = p;
		now_process = p;
	}
	else if (strcmp(name, "wechat") == 0) {
		struct process_block *p = (struct process_block *)malloc(sizeof(struct process_block));
		strcpy(p->_name, "wechat");
		p->_need_time = 15;
		p->_cpu = 1;
		p->_memory = 4;
		p->_deviece = 1;
		p->_state = 'S';
		p->next = NULL;
		now_process->next = p;
		now_process = p;
	}
	else if (strcmp(name, "weibo") == 0) {
		struct process_block *p = (struct process_block *)malloc(sizeof(struct process_block));
		strcpy(p->_name, "weibo");
		p->_need_time = 10;
		p->_cpu = 5;
		p->_memory = 4;
		p->_deviece = 5;
		p->_state = 'S';
		p->next = NULL;
		now_process->next = p;
		now_process = p;
	}
	else if (strcmp(name, "word") == 0) {
		struct process_block *p = (struct process_block *)malloc(sizeof(struct process_block));
		strcpy(p->_name, "word");
		p->_need_time = 11;
		p->_cpu = 2;
		p->_memory = 2;
		p->_deviece = 2;
		p->_state = 'S';
		p->next = NULL;
		now_process->next = p;
		now_process = p;
	}
	else if (strcmp(name, "qq") == 0) {
		struct process_block *p = (struct process_block *)malloc(sizeof(struct process_block));
		strcpy(p->_name, "qq");
		p->_need_time = 13;
		p->_cpu = 3;
		p->_memory = 4;
		p->_deviece = 1;
		p->_state = 'S';
		p->next = NULL;
		now_process->next = p;
		now_process = p;
	}
	else {//wrong
		MessageBox(NULL, TEXT("启动程序出错!"), TEXT("运行程序"), MB_ICONINFORMATION | MB_OK);
	}

}


IplImage *img_background;//背景图片
IplImage *img_background_new;//背景图片备份
IplImage *img_processlist;//进程列表
IplImage *img_dashboard;//仪表盘
IplImage *img_file;
IplImage *img_createfile;//创建文件
IplImage *img_excel;
IplImage *img_ppt;
IplImage *img_qq;
IplImage *img_vs;
IplImage *img_weibo;
IplImage *img_wechat;
IplImage *img_word;
IplImage *img_ie;
IplImage *create_bg;
IplImage *create_bg_new;
IplImage *dash_bg;
IplImage *dash_bg_new;
IplImage *process_bg;
IplImage *process_bg_new;
IplImage *img_red;
IplImage *img_green;
IplImage *img_red2;
IplImage *img_green2;
IplImage *img_load;
IplImage *query_bg;
IplImage *query_bg_new;
IplImage *disk_bg;
IplImage *disk_bg_new;
IplImage *music;


struct node {//文件系统
	char name[105];
	int flag;//0为进程 1为文件夹
	int son_num;//儿子节点数量
	int _disk; //占用磁盘情况
	node *pre;//父亲节点
	node *next[30];
};
node root;//系统文件根节点
node *now_path;//当前路径

void Load_Image() {//加载系统所需要的图片
	img_background = cvLoadImage(".\\..\\Image\\background.jpg");
	img_background_new = cvLoadImage(".\\..\\Image\\background.jpg");
	img_processlist = cvLoadImage(".\\..\\Image\\processlist.jpg");
	img_dashboard = cvLoadImage(".\\..\\Image\\dashboard.jpg");
	img_file = cvLoadImage(".\\..\\Image\\file.png");
	img_createfile = cvLoadImage(".\\..\\Image\\createfile.jpg");
	img_excel = cvLoadImage(".\\..\\Image\\excel.png");
	img_ppt = cvLoadImage(".\\..\\Image\\ppt.png");
	img_qq = cvLoadImage(".\\..\\Image\\qq.png");
	img_vs = cvLoadImage(".\\..\\Image\\vs.png");
	img_weibo = cvLoadImage(".\\..\\Image\\weibo.png");
	img_wechat = cvLoadImage(".\\..\\Image\\wechat.png");
	img_word = cvLoadImage(".\\..\\Image\\word.png");
	img_ie = cvLoadImage(".\\..\\Image\\ie.png");
	create_bg = cvLoadImage(".\\..\\Image\\create_bg.jpg");
	create_bg_new = cvLoadImage(".\\..\\Image\\create_bg.jpg");
	process_bg = cvLoadImage(".\\..\\Image\\process_bg.jpg");
	process_bg_new = cvLoadImage(".\\..\\Image\\process_bg.jpg");
	dash_bg = cvLoadImage(".\\..\\Image\\dash_bg.jpg");
	dash_bg_new = cvLoadImage(".\\..\\Image\\dash_bg.jpg");
	img_red = cvLoadImage(".\\..\\Image\\red.png");
	img_green = cvLoadImage(".\\..\\Image\\green.png");
	img_red2 = cvLoadImage(".\\..\\Image\\red2.png");
	img_green2 = cvLoadImage(".\\..\\Image\\green2.png");
	img_load = cvLoadImage(".\\..\\Image\\load.jpg");
	query_bg = cvLoadImage(".\\..\\Image\\query_bg.jpg");
	query_bg_new = cvLoadImage(".\\..\\Image\\query_bg.jpg");
	disk_bg = cvLoadImage(".\\..\\Image\\disk_bg.jpg");
	disk_bg_new = cvLoadImage(".\\..\\Image\\disk_bg.jpg");
	music = cvLoadImage(".\\..\\Image\\music.png");
}

string getTime()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
	return tmp;
}

void Load_System() {//初始化系统文件
	strcpy(root.name, "MacOS");
	root.flag = 1;
	root.son_num = 5;
	root.pre = NULL;
	struct node *qq = (struct node *)malloc(sizeof(struct node));
	strcpy(qq->name, "qq");
	qq->flag = 0;
	qq->son_num = 0;
	qq->pre = &root;
	for (int i = 0; i < 30; i++) qq->next[i] = NULL;
	Disk_Query("qq");
	struct node *vs = (struct node *)malloc(sizeof(struct node));
	strcpy(vs->name, "vs");
	vs->flag = 0;
	vs->son_num = 0;
	vs->pre = &root;
	for (int i = 0; i < 30; i++) vs->next[i] = NULL;
	Disk_Query("vs");
	struct node *wechat = (struct node *)malloc(sizeof(struct node));
	strcpy(wechat->name, "wechat");
	wechat->flag = 0;
	wechat->son_num = 0;
	wechat->pre = &root;
	for (int i = 0; i < 30; i++) wechat->next[i] = NULL;
	Disk_Query("wechat");
	struct node *ie = (struct node *)malloc(sizeof(struct node));
	strcpy(ie->name, "ie");
	ie->flag = 0;
	ie->son_num = 0;
	ie->pre = &root;
	for (int i = 0; i < 30; i++) ie->next[i] = NULL;
	Disk_Query("ie");
	struct node *file1 = (struct node *)malloc(sizeof(struct node));
	strcpy(file1->name, "file");
	file1->flag = 1;
	file1->son_num = 3;
	file1->pre = &root;
	for (int i = 0; i < 30; i++) file1->next[i] = NULL;
	root.next[0] = qq;
	root.next[1] = vs;
	root.next[2] = wechat;
	root.next[3] = ie;
	root.next[4] = file1;
	Disk_Query("file");

	struct node *excel = (struct node *)malloc(sizeof(struct node));
	strcpy(excel->name, "excel");
	excel->flag = 0;
	excel->son_num = 0;
	excel->pre = file1;
	for (int i = 0; i < 30; i++) excel->next[i] = NULL;
	Disk_Query("excel");
	struct node *ppt = (struct node *)malloc(sizeof(struct node));
	strcpy(ppt->name, "ppt");
	ppt->flag = 0;
	ppt->son_num = 0;
	ppt->pre = file1;
	for (int i = 0; i < 30; i++) ppt->next[i] = NULL;
	Disk_Query("ppt");
	struct node *file2 = (struct node *)malloc(sizeof(struct node));
	strcpy(file2->name, "file");
	file2->flag = 1;
	file2->son_num = 2;
	file2->pre = file1;
	for (int i = 0; i < 30; i++) file2->next[i] = NULL;
	file1->next[0] = excel;
	file1->next[1] = ppt;
	file1->next[2] = file2;
	Disk_Query("file");

	struct node *wd = (struct node *)malloc(sizeof(struct node));
	strcpy(wd->name, "word");
	wd->flag = 0;
	wd->son_num = 0;
	wd->pre = file1;
	for (int i = 0; i < 30; i++) wd->next[i] = NULL;
	Disk_Query("word");
	struct node *weibo = (struct node *)malloc(sizeof(struct node));
	strcpy(weibo->name, "weibo");
	weibo->flag = 0;
	weibo->son_num = 0;
	weibo->pre = file1;
	for (int i = 0; i < 30; i++) weibo->next[i] = NULL;
	file2->next[0] = wd;
	file2->next[1] = weibo;
	Disk_Query("weibo");

	now_path = &root;
}

void ShowUser() {
	//显示用户名称
	char temp_name[105] = { 0 };
	//strcpy(temp_name, root.name);
	sprintf(temp_name, "User:  %s", root.name);
	char *text = temp_name;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
	CvPoint p;//定义一个像素点
	CvScalar p_color;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
	p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
	p_color.val[1] = 0;
	p_color.val[2] = 255;
	p.x = 10;//设置在屏幕显示的坐标
	p.y = 20;
	cvPutText(img_background, text, p, &font, p_color);//然后调用函数显示
}

void ShowTime() {
	//显示时间
	string time = getTime();
	char temp[105] = { 0 };
	for (int i = 0; i < time.length(); i++) temp[i] = time[i];
	char *text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
	CvPoint p;//定义一个像素点
	CvScalar p_color;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
	p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
	p_color.val[1] = 0;
	p_color.val[2] = 255;
	p.x = 760;//设置在屏幕显示的坐标
	p.y = 20;
	cvPutText(img_background, text, p, &font, p_color);//然后调用函数显示
}

void ShowPath() {
	//显示路径
	string time = getTime();
	char temp[105] = { 0 };
	sprintf(temp, "Path:  %s:\\Desktop", root.name);
	node *pp = now_path;
	char tt[10][20] = { 0 };
	int idx = 0;
	while (pp != &root) {
		sprintf(tt[idx++], "\\%s", pp->name);
		pp = pp->pre;
	}
	for (int i = idx - 1; i >= 0; i--) {
		strcat(temp, tt[i]);
	}
	char *text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
	CvPoint p;//定义一个像素点
	CvScalar p_color;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
	p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
	p_color.val[1] = 0;
	p_color.val[2] = 255;
	p.x = 200;//设置在屏幕显示的坐标
	p.y = 20;
	cvPutText(img_background, text, p, &font, p_color);//然后调用函数显示
}

void ShowIcon() {
	//显示当前目录图标
	//printf("now %d %s\n", now_path->son_num, now_path->name);
	for (int i = 0; i < now_path->son_num; i++) {
		node *p = now_path->next[i];
		//if (p == NULL) continue;
		int px = (i % 3) * 100 + 60;
		int py = (i / 3) * 100 + 60;
		//printf("num %d name %s\n", p->son_num, p->name);
		if (strcmp(p->name, "qq") == 0) {
			for (int x = 0; x < img_qq->height; x++)
			{
				for (int y = 0; y < img_qq->width; y++)
				{

					int r, g, b;
					r = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x + 2);
					//printf("rgb %d %d %d\n", r, g, b);
					if (r >= 200 && r <= 208 && g >= 70 && g <= 74 && b >= 60 && b <= 65) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else if (strcmp(p->name, "file") == 0) {
			for (int x = 0; x < img_file->height; x++)
			{
				for (int y = 0; y < img_file->width; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_file, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_file, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_file, uchar, y, 3 * x + 2);
					if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else if (strcmp(p->name, "vs") == 0) {
			for (int x = 0; x < img_vs->height; x++)
			{
				for (int y = 0; y < img_vs->width; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x + 2);
					if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else if (strcmp(p->name, "wechat") == 0) {
			for (int x = 0; x < img_wechat->height; x++)
			{
				for (int y = 0; y < img_wechat->width; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x + 2);
					if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else if (strcmp(p->name, "ie") == 0) {
			for (int x = 0; x < img_ie->height; x++)
			{
				for (int y = 0; y < img_ie->width; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x + 2);
					if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else if (strcmp(p->name, "ppt") == 0) {
			for (int x = 0; x < img_ppt->height; x++)
			{
				for (int y = 0; y < img_ppt->width; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x + 2);
					if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else if (strcmp(p->name, "excel") == 0) {
			for (int x = 0; x < img_excel->height; x++)
			{
				for (int y = 0; y < img_excel->width; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x + 2);
					if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else if (strcmp(p->name, "word") == 0) {
			for (int x = 0; x < img_word->height; x++)
			{
				for (int y = 0; y < img_word->width; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x + 2);
					if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else if (strcmp(p->name, "weibo") == 0) {
			for (int x = 0; x < img_weibo->height; x++)
			{
				for (int y = 0; y < img_weibo->width; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x + 2);
					if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
	}
}

void DrawImage() {
	cvCopy(img_background_new, img_background);
	//img_background = cvLoadImage(".\\..\\Image\\background.jpg");
	/*
	int px = 230;
	int py = 500;
	for (int x = 0; x<img_createfile->height; x++)
	{
	for (int y = 0; y<img_createfile->width; y++)
	{
	int r, g, b;
	r = CV_IMAGE_ELEM(img_createfile, uchar, y, 3 * x);
	g = CV_IMAGE_ELEM(img_createfile, uchar, y, 3 * x + 1);
	b = CV_IMAGE_ELEM(img_createfile, uchar, y, 3 * x + 2);
	if (r < 20 && g < 20 && b < 20) continue;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

	}
	}

	px = 400;
	py = 500;
	for (int x = 0; x<img_processlist->height; x++)
	{
	for (int y = 0; y<img_processlist->width; y++)
	{
	int r, g, b;
	r = CV_IMAGE_ELEM(img_processlist, uchar, y, 3 * x);
	g = CV_IMAGE_ELEM(img_processlist, uchar, y, 3 * x + 1);
	b = CV_IMAGE_ELEM(img_processlist, uchar, y, 3 * x + 2);
	if (r < 20 && g < 20 && b < 20) continue;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

	}
	}

	px = 570;
	py = 500;
	for (int x = 0; x<img_dashboard->height; x++)
	{
	for (int y = 0; y<img_dashboard->width; y++)
	{
	int r, g, b;
	r = CV_IMAGE_ELEM(img_dashboard, uchar, y, 3 * x);
	g = CV_IMAGE_ELEM(img_dashboard, uchar, y, 3 * x + 1);
	b = CV_IMAGE_ELEM(img_dashboard, uchar, y, 3 * x + 2);
	if (r < 20 && g < 20 && b < 20) continue;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px)) = r;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 1) = g;
	CV_IMAGE_ELEM(img_background, uchar, (y + py), 3 * (x + px) + 2) = b;

	}
	}
	*/

	//显示用户名称
	ShowUser();
	//显示时间
	ShowTime();
	//显示路径
	ShowPath();

	//显示当前目录图标
	ShowIcon();
}

void Boot_System() {
	Load_Image();
	Load_System();
	Init_Process();//初始化系统服务进程
	Init_Bank();//初始化系统资源分配
}

void Show_Processlist() {//显示进程列表
	//process_bg = cvLoadImage(".\\..\\Image\\process_bg.jpg");
	cvCopyImage(process_bg_new, process_bg);

	char temp[105] = { 0 };
	sprintf(temp, "name    cpu    memory    device");
	char *text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
	CvPoint p;//定义一个像素点
	CvScalar p_color;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
	p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
	p_color.val[1] = 0;
	p_color.val[2] = 255;
	p.x = 50;//设置在屏幕显示的坐标
	p.y = 50;
	cvPutText(process_bg, text, p, &font, p_color);//然后调用函数显示


	int px = 50;
	int py = 100;
	process_block *pp = &process_head;
	while (pp != NULL) {
		if (pp->_state == 'E' || pp->_state == 'S') {//如果进程状态为挂起或者结束则不显示
			pp = pp->next;
			continue;
		}
		//显示进程信息
		char temp[105] = { 0 };
		sprintf(temp, "%s    %d    %d    %d", pp->_name, pp->_cpu, pp->_memory, pp->_deviece);
		char *text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
		CvPoint p;//定义一个像素点
		CvScalar p_color;
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
		p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
		p_color.val[1] = 0;
		p_color.val[2] = 255;
		p.x = px;//设置在屏幕显示的坐标
		p.y = py;
		cvPutText(process_bg, text, p, &font, p_color);//然后调用函数显示

		pp = pp->next;
		py += 50;
	}

	cvShowImage("processlist", process_bg);
}

void Show_Disk() {
	//disk_bg = cvLoadImage(".\\..\\Image\\disk_bg.jpg");
	cvCopyImage(disk_bg_new, disk_bg);

	//显示磁盘空间占用情况
	char temp[105] = { 0 };
	sprintf(temp, "Disk usage ratio is:  %d%%", 100 - total_disk);
	char* text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
	CvPoint p;//定义一个像素点
	CvScalar p_color;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
	p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
	p_color.val[1] = 0;
	p_color.val[2] = 255;
	p.x = 50;//设置在屏幕显示的坐标
	p.y = 50;
	cvPutText(disk_bg, text, p, &font, p_color);//然后调用函数显示


	for (int i = 0; i < 100; i++) {
		int px = 50 + 23 * (i % 20);
		int py = 100 + 23 * (i / 20);
		if (disk_status[i] == 1) {
			for (int x = 0; x < img_red2->width; x++)
			{
				for (int y = 0; y < img_red2->height; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_red2, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_red2, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_red2, uchar, y, 3 * x + 2);
					//if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(disk_bg, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(disk_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(disk_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else {
			for (int x = 0; x < img_green2->width; x++)
			{
				for (int y = 0; y < img_green2->height; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_green2, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_green2, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_green2, uchar, y, 3 * x + 2);
					//if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(disk_bg, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(disk_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(disk_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
	}

	cvShowImage("disk", disk_bg);
}

void Show_Dashboard() {//显示仪表盘 CPU、内存管理
	//dash_bg = cvLoadImage(".\\..\\Image\\dash_bg.jpg");
	cvCopyImage(dash_bg_new, dash_bg);

	int cpu_sum = 0;
	int memory_sum = 0;
	process_block *pp = &process_head;
	while (pp != NULL) {
		if (pp->_state == 'E' || pp->_state == 'S') {//如果进程状态为挂起或者结束则不显示
			pp = pp->next;
			continue;
		}
		cpu_sum += pp->_cpu;
		memory_sum += pp->_memory;
		pp = pp->next;
	}

	//显示CPU值
	char temp[105] = { 0 };
	sprintf(temp, "Memory usage ratio is:  %d%%", cpu_sum);
	char *text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
	CvPoint p;//定义一个像素点
	CvScalar p_color;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
	p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
	p_color.val[1] = 0;
	p_color.val[2] = 255;
	p.x = 100;//设置在屏幕显示的坐标
	p.y = 100;
	cvPutText(dash_bg, text, p, &font, p_color);//然后调用函数显示
	//显示内存值
	//显示CPU值
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "CPU usage ratio is:  %d%%", memory_sum);
	text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
	p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
	p_color.val[1] = 0;
	p_color.val[2] = 255;
	p.x = 100;//设置在屏幕显示的坐标
	p.y = 200;
	cvPutText(dash_bg, text, p, &font, p_color);//然后调用函数显示

	//显示设备占用情况
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "Device:  0    1    2    3    4    5");
	text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
	p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
	p_color.val[1] = 0;
	p_color.val[2] = 255;
	p.x = 100;//设置在屏幕显示的坐标
	p.y = 300;
	cvPutText(dash_bg, text, p, &font, p_color);//然后调用函数显示


	for (int i = 0; i <= 5; i++) {
		int px = 168 + 43 * i;
		int py = 320;
		if (device_status[i] == 1) {
			for (int x = 0; x < img_red->width; x++)
			{
				for (int y = 0; y < img_red->height; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_red, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_red, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_red, uchar, y, 3 * x + 2);
					//if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(dash_bg, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(dash_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(dash_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
		else {
			for (int x = 0; x < img_green->width; x++)
			{
				for (int y = 0; y < img_green->height; y++)
				{
					int r, g, b;
					r = CV_IMAGE_ELEM(img_green, uchar, y, 3 * x);
					g = CV_IMAGE_ELEM(img_green, uchar, y, 3 * x + 1);
					b = CV_IMAGE_ELEM(img_green, uchar, y, 3 * x + 2);
					//if (r < 20 && g < 20 && b < 20) continue;
					CV_IMAGE_ELEM(dash_bg, uchar, (y + py), 3 * (x + px)) = r;
					CV_IMAGE_ELEM(dash_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
					CV_IMAGE_ELEM(dash_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

				}
			}
		}
	}

	cvShowImage("dashboard", dash_bg);
}

void Create_File() {
	int px = 50, py = 50;
	for (int x = 0; x < img_qq->height; x++)
	{
		for (int y = 0; y < img_qq->width; y++)
		{
			//取出每个坐标的像素点，画到对应位置的背景图片上
			int r, g, b;
			r = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x + 2);
			//printf("rgb %d %d %d\n", r, g, b);
			if (r >= 200 && r <= 208 && g >= 70 && g <= 74 && b >= 60 && b <= 65) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 150, py = 50;
	for (int x = 0; x < img_file->height; x++)
	{
		for (int y = 0; y < img_file->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_file, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_file, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_file, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;
		}
	}
	px = 250, py = 50;
	for (int x = 0; x < img_vs->height; x++)
	{
		for (int y = 0; y < img_vs->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 350, py = 50;
	for (int x = 0; x < img_wechat->height; x++)
	{
		for (int y = 0; y < img_wechat->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 50, py = 150;
	for (int x = 0; x < img_ie->height; x++)
	{
		for (int y = 0; y < img_ie->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 150, py = 150;
	for (int x = 0; x < img_ppt->height; x++)
	{
		for (int y = 0; y < img_ppt->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 250, py = 150;
	for (int x = 0; x < img_excel->height; x++)
	{
		for (int y = 0; y < img_excel->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 350, py = 150;
	for (int x = 0; x < img_word->height; x++)
	{
		for (int y = 0; y < img_word->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 50, py = 250;
	for (int x = 0; x < img_weibo->height; x++)
	{
		for (int y = 0; y < img_weibo->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(create_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
}


//判断磁盘空间分配是否满足大小
int Disk_Query(char *name) {
	if (strcmp(name, "qq") == 0) {
		if (total_disk >= 3) {
			total_disk -= 3;
			//顺序查找分配磁盘空间
			int val = 3;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
	else if (strcmp(name, "vs") == 0) {
		if (total_disk >= 8) {
			total_disk -= 8;
			int val = 8;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
	else if (strcmp(name, "wechat") == 0) {
		if (total_disk >= 7) {
			total_disk -= 7;
			int val = 7;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
	else if (strcmp(name, "ppt") == 0) {
		if (total_disk >= 6) {
			total_disk -= 6;
			int val = 6;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
	else if (strcmp(name, "weibo") == 0) {
		if (total_disk >= 5) {
			total_disk -= 5;
			int val = 5;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
	else if (strcmp(name, "excel") == 0) {
		if (total_disk >= 1) {
			total_disk -= 1;
			int val = 1;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
	else if (strcmp(name, "word") == 0) {
		if (total_disk >= 3) {
			total_disk -= 3;
			int val = 3;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
	else if (strcmp(name, "ie") == 0) {
		if (total_disk >= 4) {
			total_disk -= 4;
			int val = 4;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
	else {//file
		if (total_disk >= 2) {
			total_disk -= 2;
			int val = 2;
			for (int i = 0; i < 100; i++) {
				if (disk_status[i] == 0 && val > 0) {
					disk_status[i] = 1;
					val--;
				}
			}
			return 1;
		}
		else return 0;
	}
}

//删除文件时磁盘空间增加
void Disk_Add(char *name) {
	if (strcmp(name, "qq") == 0) {
		total_disk += 3;
		//恢复磁盘空间容量
		int val = 3;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
	else if (strcmp(name, "vs") == 0) {
		total_disk += 8;
		int val = 8;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
	else if (strcmp(name, "wechat") == 0) {
		total_disk += 7;
		int val = 7;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
	else if (strcmp(name, "ppt") == 0) {
		total_disk += 6;
		int val = 6;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
	else if (strcmp(name, "weibo") == 0) {
		total_disk += 5;
		int val = 5;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
	else if (strcmp(name, "excel") == 0) {
		total_disk += 1;
		int val = 1;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
	else if (strcmp(name, "word") == 0) {
		total_disk += 3;
		int val = 3;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
	else if (strcmp(name, "ie") == 0) {
		total_disk += 4;
		int val = 4;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
	else {//file
		total_disk += 2;
		int val = 2;
		for (int i = 99; i >= 0; i--) {
			if (disk_status[i] == 1 && val > 0) {
				disk_status[i] = 0;
				val--;
			}
		}
	}
}

//回调函数、响应鼠标事件 创建文件窗口
void OnMouse_create(int event, int x, int y, int flags, void* param) {
	if (event == CV_EVENT_LBUTTONDOWN)//表示鼠标左键按下
	{
		if (x >= 50 && y >= 50 && x <= 100 && y <= 100) {//qq
			if (Disk_Query("qq") == 0) {
				MessageBox(NULL, L"无法创建QQ程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "qq");
			p->flag = 0;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
		if (x >= 150 && y >= 50 && x <= 200 && y <= 100) {//file
			if (Disk_Query("file") == 0) {
				MessageBox(NULL, L"无法创建file程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "file");
			p->flag = 1;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
		if (x >= 250 && y >= 50 && x <= 300 && y <= 100) {//vs
			if (Disk_Query("vs") == 0) {
				MessageBox(NULL, L"无法创建vs程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "vs");
			p->flag = 0;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
		if (x >= 350 && y >= 50 && x <= 400 && y <= 100) {//wechat
			if (Disk_Query("wechat") == 0) {
				MessageBox(NULL, L"无法创建wechat程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "wechat");
			p->flag = 0;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
		if (x >= 50 && y >= 150 && x <= 100 && y <= 200) {//ie
			if (Disk_Query("ie") == 0) {
				MessageBox(NULL, L"无法创建ie程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "ie");
			p->flag = 0;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
		if (x >= 150 && y >= 150 && x <= 200 && y <= 200) {//ppt
			if (Disk_Query("ppt") == 0) {
				MessageBox(NULL, L"无法创建ppt程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "ppt");
			p->flag = 0;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
		if (x >= 250 && y >= 150 && x <= 300 && y <= 200) {//excel
			if (Disk_Query("excel") == 0) {
				MessageBox(NULL, L"无法创建excel程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "excel");
			p->flag = 0;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
		if (x >= 350 && y >= 150 && x <= 400 && y <= 200) {//word
			if (Disk_Query("word") == 0) {
				MessageBox(NULL, L"无法创建word程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "word");
			p->flag = 0;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
		if (x >= 50 && y >= 250 && x <= 100 && y <= 300) {//weibo
			if (Disk_Query("weibo") == 0) {
				MessageBox(NULL, L"无法创建weibo程序!", L"磁盘空间不足", MB_OK);
				return;
			}
			else {
				MessageBox(NULL, L"文件创建成功!", L"创建文件", MB_OK);
			}
			struct node *p = (struct node *)malloc(sizeof(struct node));
			strcpy(p->name, "weibo");
			p->flag = 0;
			p->son_num = 0;
			p->pre = now_path;
			for (int i = 0; i < 30; i++) p->next[i] = NULL;
			now_path->next[now_path->son_num] = p;
			now_path->son_num++;
			return;
		}
	}
}

//查询程序
void Query_File() {
	//query_bg = cvLoadImage(".\\..\\Image\\query_bg.jpg");
	cvCopyImage(query_bg_new, query_bg);

	int px = 50, py = 50;
	for (int x = 0; x < img_qq->height; x++)
	{
		for (int y = 0; y < img_qq->width; y++)
		{
			//取出每个坐标的像素点，画到对应位置的背景图片上
			int r, g, b;
			r = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_qq, uchar, y, 3 * x + 2);
			//printf("rgb %d %d %d\n", r, g, b);
			if (r >= 200 && r <= 208 && g >= 70 && g <= 74 && b >= 60 && b <= 65) continue;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 150, py = 50;
	for (int x = 0; x < img_weibo->height; x++)
	{
		for (int y = 0; y < img_weibo->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_weibo, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 2) = b;
		}
	}
	px = 250, py = 50;
	for (int x = 0; x < img_vs->height; x++)
	{
		for (int y = 0; y < img_vs->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_vs, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 350, py = 50;
	for (int x = 0; x < img_wechat->height; x++)
	{
		for (int y = 0; y < img_wechat->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_wechat, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 50, py = 150;
	for (int x = 0; x < img_ie->height; x++)
	{
		for (int y = 0; y < img_ie->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_ie, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 150, py = 150;
	for (int x = 0; x < img_ppt->height; x++)
	{
		for (int y = 0; y < img_ppt->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_ppt, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 250, py = 150;
	for (int x = 0; x < img_excel->height; x++)
	{
		for (int y = 0; y < img_excel->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_excel, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}
	px = 350, py = 150;
	for (int x = 0; x < img_word->height; x++)
	{
		for (int y = 0; y < img_word->width; y++)
		{
			int r, g, b;
			r = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x);
			g = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x + 1);
			b = CV_IMAGE_ELEM(img_word, uchar, y, 3 * x + 2);
			if (r < 20 && g < 20 && b < 20) continue;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px)) = r;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 1) = g;
			CV_IMAGE_ELEM(query_bg, uchar, (y + py), 3 * (x + px) + 2) = b;

		}
	}

	cvShowImage("filequery", query_bg);
}

void Show_Music() {
	cvShowImage("music", music);
}

int find_num = 0;//查找到的文件数量
void find_path(node *p, char* name, char path_str[]) {
	if (p == NULL) return;
	for (int i = 0; i < p->son_num; i++) {
		if (strcmp(p->next[i]->name, name) == 0) {
			char temp[105] = { 0 };
			sprintf(temp, "%s\\%s", path_str, p->next[i]->name);

			char *text = temp;//定义一个指针text指向字符数组、因为cvputtext函数只接受char型指针
			CvPoint p;//定义一个像素点
			CvScalar p_color;
			CvFont font;
			cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5f, 0.5f, 0, 1, CV_AA);//设置字体
			p_color.val[0] = 255;//设置颜色的RGB值、可以自己百度
			p_color.val[1] = 0;
			p_color.val[2] = 255;
			p.x = 50;//设置在屏幕显示的坐标
			p.y = 230 + find_num * 20;
			cvPutText(query_bg, text, p, &font, p_color);//然后调用函数显示
			cvShowImage("filequery", query_bg);

			find_num++;
			continue;
		}
		if (strcmp(p->next[i]->name, "file") == 0) {
			char temp[105] = { 0 };
			sprintf(temp, "%s\\%s", path_str, p->next[i]->name);
			find_path(p->next[i], name, temp);
		}
	}
}

//回调函数、响应鼠标事件
void OnMouse_query(int event, int x, int y, int flags, void* param) {
	if (event == CV_EVENT_LBUTTONDOWN)//表示鼠标左键按下
	{
		if (x >= 50 && y >= 50 && x <= 100 && y <= 100) {//qq
			find_num = 0;
			Query_File();
			// 遍历文件系统树形结构查找
			char temp[105] = { 0 };
			sprintf(temp, "%s:\\desktop", root.name);
			find_path(&root, "qq", temp);

			return;
		}
		if (x >= 150 && y >= 50 && x <= 200 && y <= 100) {//weibo
			find_num = 0;
			Query_File();
			char temp[105] = { 0 };
			sprintf(temp, "%s:\\desktop", root.name);
			find_path(&root, "weibo", temp);

			return;
		}
		if (x >= 250 && y >= 50 && x <= 300 && y <= 100) {//vs
			find_num = 0;
			Query_File();
			char temp[105] = { 0 };
			sprintf(temp, "%s:\\desktop", root.name);
			find_path(&root, "vs", temp);

			return;
		}
		if (x >= 350 && y >= 50 && x <= 400 && y <= 100) {//wechat
			find_num = 0;
			Query_File();
			char temp[105] = { 0 };
			sprintf(temp, "%s:\\desktop", root.name);
			find_path(&root, "wechat", temp);

			return;
		}
		if (x >= 50 && y >= 150 && x <= 100 && y <= 200) {//ie
			find_num = 0;
			Query_File();
			char temp[105] = { 0 };
			sprintf(temp, "%s:\\desktop", root.name);
			find_path(&root, "ie", temp);

			return;
		}
		if (x >= 150 && y >= 150 && x <= 200 && y <= 200) {//ppt
			find_num = 0;
			Query_File();
			char temp[105] = { 0 };
			sprintf(temp, "%s:\\desktop", root.name);
			find_path(&root, "ppt", temp);

			return;
		}
		if (x >= 250 && y >= 150 && x <= 300 && y <= 200) {//excel
			find_num = 0;
			Query_File();
			char temp[105] = { 0 };
			sprintf(temp, "%s:\\desktop", root.name);
			find_path(&root, "excel", temp);

			return;
		}
		if (x >= 350 && y >= 150 && x <= 400 && y <= 200) {//word
			find_num = 0;
			Query_File();
			char temp[105] = { 0 };
			sprintf(temp, "%s:\\desktop", root.name);
			find_path(&root, "word", temp);

			return;
		}
	}
}

//回调函数、响应鼠标事件
void OnMouse(int event, int x, int y, int flags, void* param)
{

	if (event == CV_EVENT_LBUTTONDOWN)//表示鼠标左键按下
	{
		if (x >= 186 && y >= 522 && x <= 258 && y <= 593) {//createfile
			//create_bg = cvLoadImage(".\\..\\Image\\create_bg.jpg");
			cvCopyImage(create_bg_new, create_bg);
			cvNamedWindow("createfile", 1);
			cvSetMouseCallback("createfile", OnMouse_create);
			Create_File();
			cvShowImage("createfile", create_bg);
			return;
		}
		if (x >= 391 && y >= 511 && x <= 464 && y <= 592) {//processlist
			cvNamedWindow("processlist", 1);
			Show_Processlist();
			return;
		}
		if (x >= 500 && y >= 518 && x <= 574 && y <= 591) {//dashboard
			cvNamedWindow("dashboard", 1);
			Show_Dashboard();
			return;
		}
		if (x >= 614 && y >= 511 && x <= 690 && y <= 592) {//disk
			cvNamedWindow("disk", 1);
			Show_Disk();
			return;
		}
		if (x >= 710 && y >= 519 && x <= 783 && y <= 594) {//music
			cvNamedWindow("music", 1);
			Show_Music();
			return;
		}
		if (x >= 270 && y >= 511 && x <= 360 && y <= 592) {//文件查询
			cvNamedWindow("filequery", 1);
			cvSetMouseCallback("filequery", OnMouse_query);
			Query_File();
			return;
		}

		for (int i = 0; i < now_path->son_num; i++) {
			node *p = now_path->next[i];
			int px = (i % 3) * 100 + 60;
			int py = (i / 3) * 100 + 60;
			if (x > px && x<(px + 50) && y>py && y < (py + 50))
			{
				if (p->flag == 0) {//进程
					if (MessageBox(NULL, L"确定要运行该程序吗", L"运行程序", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDOK) {
						Run_Process(p->name);
						continue;
					}
				}
				else {//文件
					now_path = p;
					//printf("now %d %s\n", now_path->son_num, now_path->name);
					//exit(0);
					return;
				}
			}
		}
	}
	if (event == CV_EVENT_RBUTTONDOWN) {//鼠标右键按下
		for (int i = 0; i < now_path->son_num; i++) {
			node *p = now_path->next[i];
			int px = (i % 3) * 100 + 60;
			int py = (i / 3) * 100 + 60;
			if (x > px && x<(px + 50) && y>py && y < (py + 50))
			{
				if (MessageBox(NULL, L"确定要删除该文件吗", L"删除文件", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDOK) {
					now_path->next[i] = NULL;
					for (int j = i + 1; j < now_path->son_num; j++) {
						now_path->next[j - 1] = now_path->next[j];
					}
					now_path->son_num--;
					Disk_Add(p->name);
					delete p;
					break;
				}
			}
		}
	}
}

process_block* Q[105];

// 短作业优先
void ShortTaskFirst(int num) {
	for (int i = 0; i < num; i++) {
		for (int j = i; j < num; j++) {
			if (Q[i]->_need_time > Q[j]->_need_time) {
				swap(Q[i], Q[j]);
			}
		}
	}
	//快速排序
	//sort(Q, Q + num，cmp);
}

// 任务调度
DWORD WINAPI Task_Solve(LPVOID lpParamter) {
	initial();
	int system_time = 0;//系统时间
	int pk = 1;//优先级

	while (1) {
		//遍历进程链表
		process_block *pp = &process_head;

		int cnt = 0;
		while (pp != NULL) {
			if (pp->_state == 'E' || pp->_state == 'R') {//如果进程状态为运行中或者结束则不显示
				pp = pp->next;
				continue;
			}
			Q[cnt++] = pp;
			pp = pp->next;
		}

		ShortTaskFirst(cnt);

		//使用银行家算法进行资源调度
		//Bank();

		for (int k = 0; k < cnt; k++) {
			pp = Q[k];
			//加入到系统任务表
			for (int i = 0; i < MAX_TASK; i++) {
				if (device_status[pp->_deviece] == 1) continue;
				if (tasks[i].run_flag == 0) {
					//printf(":xxxxx\n");
					//exit(0);
					strcpy(tasks[i].name, pp->_name);
					bestAllocation(tasks[i].name, pp->_memory);
					tasks[i].pid = 1000 + pk;
					tasks[i].priority = pk;
					tasks[i].come_time = system_time;
					tasks[i].run_flag = 1;
					tasks[i].run_time = pp->_need_time;
					tasks[i].run_end_time = system_time + tasks[i].run_time;
					tasks[i].device = pp->_deviece;
					pp->_state = 'R';
					device_status[pp->_deviece] = 1;//占用设备资源
					break;
				}
			}
		}

		//检测进程结束并释放占用设备资源

		for (int i = 0; i < MAX_TASK; i++) {
			if (tasks[i].run_end_time <= system_time && tasks[i].run_flag == 1) {
				tasks[i].run_flag = 0;
				recycle(tasks[i].name);
				device_status[tasks[i].device] = 0;
				process_block *p = &process_head;
				while (p != NULL) {
					if (strcmp(p->_name, tasks[i].name) == 0 && p->_state == 'R') {
						p->_state = 'E';
						break;
					}
					p = p->next;
				}
			}
		}

		//printf("system time: %d\n", system_time);
		Sleep(1000);
		system_time++;
		pk++;
	}
	return 0L;
}

int main() {
	mciSendString(L"open H:\\vs2015 codes\\操作系统作业\\Music\\1.mp3 alias aa", NULL, 0, NULL);

	// mciSendString(L"open .\\..\\Music\\1.mp3  alias music", NULL, 0, NULL);
	//PlaySound(L".\\..\\Music\\1.wav", NULL, SND_FILENAME | SND_ASYNC);

	Boot_System();//启动系统
	cvNamedWindow("OS-X", 1);//0表示黑白图片 1表示原色图片
	cvShowImage("OS-X", img_load);//将图片放到窗口里显示出来
	cvWaitKey(0);//等待按键
	cvSetMouseCallback("OS-X", OnMouse);

	HANDLE hThread = CreateThread(NULL, 0, Task_Solve, NULL, 0, NULL);//创建一个线程进行调度进程和管理设备资源

	while (1) {
		DrawImage();
		cvShowImage("OS-X", img_background);
		int key = cvWaitKey(100);//每100毫秒得到一个按键
		if (key == 'b') {
			if (now_path == &root) continue;
			now_path = now_path->pre;
		}

		Show_Processlist();
		Show_Dashboard();
		Show_Disk();
	}
	cvWaitKey(0);
	cout << "System End!" << endl;
	CloseHandle(hThread);//销毁线程
	return 0;
}