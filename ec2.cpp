#include<iostream>
#include<fstream>
#include<windows.h>
#include<cstring>


using namespace std;
bool Copy(int, int);			//复制所需课程信息
int Search(int);				//利用时间代码与课程相对应（如果有课程冲突只复制前一个课程的房间号，并提示冲突）
void recommend();				//自动判断当前时间课程
void message(int);				//输出当前复制的课程的名称
void writein(ifstream&);        //写入数据的文件流函数

class Course {
private:
	int num;
	char c_name[20];					//课程名称
	int  weekday;					//上课日期，0为星期天；不同时间的课为不同的课程
	int  start_t;					//该课程的起始时间（精确到小时）
	int  end_t;						//该课程终止时间
	char add_info[30];					//额外信息，如分组
	char account[10];
	char password[10];
public:
	Course() :num(-1), weekday(-1), start_t(-1), end_t(-1) {
		c_name[0]   = '\0';
		add_info[0] = '\0';
		account[0]  = '\0';
		password[0] = '\0';
	};
	friend bool Copy(int, int);
	friend void writein(ifstream&);
	friend int Search(int);
	friend void message(int);
	~Course() {};

};



char info_0[10];
int search_code = 0;					//查询编码
int obj;					//课程目标代码
Course menu0[15];
/*******************************************************************************************************/
int main() {
	system("color 57");				//设置背景色紫色 前景色白色



	ifstream fp;


	cout << "看看，是谁来了！\n" << "我也不喜欢那个课程，拿好你的密码\n";		//此处有梗


	//此处可以调整文件绝对路径
	fp.open("course.txt", ios::in);
	if (!fp.is_open()) {
		cerr << "/!|>>错误00：源文件不能访问" << endl;
		system("pause");
		return 1;
	}


	writein(fp);

	fp.close();

	recommend();


	while (1) {
		obj = Search(search_code);				//先找到对应课程编号
		Copy(obj, 1);								//复制房间号
		message(obj);								//核实信息
		cout << endl << endl << "\t按两次任意键继续复制密码，如果要另选其他，按n" << endl;
		char t;
		cin.get();                         //用于缓冲回车
		cin.get(t);							//输入n否定当前查询
		if (t != 'n') {
			Copy(obj, 2);//继续查询对应密码
			cout << endl << "\t密码已经复制 输入q退出对系统更友善w" << endl;
			cout << "========================================================" << endl;
			cin.get(t);
			if (t == 'q') 
				break; 
		}
		
		
		cout << endl << "\t输入当前的星期、时间（小时）作为查询代码，如：0421。\n\t";
		cin >> search_code;
	}

	system("pause");
	return 0;
}


/********************************************************************************************/

bool Copy(int m, int n)								//复制相应内容，m为课程序号，n为复制选项
{
	HWND hWnd = NULL;
	OpenClipboard(hWnd);//打开剪切板
	EmptyClipboard();//清空剪切板
	HANDLE hHandle = GlobalAlloc(GMEM_FIXED, 512);//分配内存
	char* pData = (char*)GlobalLock(hHandle);//锁定内存，返回申请内存的首地址
	switch (n)
	{
	case 1:  memcpy(pData,menu0[m].account,20); break;
	case 2: memcpy(pData,menu0[m].password,20); break;

	}
	SetClipboardData(CF_TEXT, hHandle);//设置剪切板数据
	GlobalUnlock(hHandle);//解除锁定
	CloseClipboard();//关闭剪切板


	return 0;
}

/*****************************************************************************************************/
int Search(int a)													//找到时间代码对应课程号
{
	int tmp = -2;
	bool flg = 0;
	if (search_code < 0 || search_code>723) { cerr << "/!|>>错误02：输入时间码不正确"<<endl; return -1; }
	int wk_val, hr_val;
	wk_val = a / 100;
	hr_val = a % 100;
	if (wk_val > 6 || hr_val > 23 || wk_val < 0 || hr_val < 0) { cerr << "/!|>>错误02：输入时间码不正确"<<endl; return -1; }		//根据出错概率高低列出代码错误形式，出错则返回-1

	for (int i = 0; i < 15; i++) {
		if (wk_val == menu0[i].weekday) {
			if (hr_val <= menu0[i].end_t && hr_val >= (menu0[i].start_t - 1)) {									//大于起始时间提前一小时且小于结束时间
				if (!flg) {
					tmp = i;
					flg = 1;
				}
				else { cerr << "/!|>>错误03：可能的课程冲突" << endl; }
			}
		}
	}
	if (!flg) { cout << endl << "\t (OwO) 没有查到课程哦" << endl; }
	return tmp;
}

/**************************************************************************************************************/
void recommend()				//自动判断当前时间代码
{
	SYSTEMTIME s_time;				//windows.h内结构体
	GetLocalTime(&s_time);			//获取系统时间初始化函数，其中数据类型为WORD型，2字节整数
	int wday, hr;
	wday = s_time.wDayOfWeek;
	hr = s_time.wHour;
	search_code = 100 * wday + hr;

	cout << "\t已经自动帮你想好了时间码哦~直接粘贴对应课程的会议室吧！->" << search_code << endl;


}

/***************************************************************************************************************/
void message(int q)				//输出当前复制的课程的名称
{
	if (q != -2) {
		cout << "=========================================================";
		cout << endl << "\t复制成功！已经复制的课程为: " << menu0[q].c_name << endl;
		cout << "\t\t房间号：" << menu0[q].account << " 密码：" << menu0[q].password << endl;
		cout << "\t\t备注：" << menu0[q].add_info << endl << endl;
		cout << "=========================================================";
	}
}

/**************************************************************************************************************/
void writein(ifstream& fp1)					//读取文件信息到流中
{
	fp1 >> info_0;
	cout << endl << "数据库最后修改日期：" << info_0 << endl << endl;
	for (int i = 0; i < 15; i++) {
		//将这一行的信息输入到对象中
		fp1 >> menu0[i].num >> menu0[i].c_name >> menu0[i].weekday >> menu0[i].start_t >> menu0[i].end_t >> menu0[i].add_info >> menu0[i].account >> menu0[i].password;
		if (fp1.eof())
			break;
		if (menu0[i].num != i) {
			cerr << endl << ">>/!| 错误01 文件中的记录可能有误" << endl;
		}
	}
}

