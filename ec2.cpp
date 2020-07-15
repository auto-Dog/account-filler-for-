#include<iostream>
#include<fstream>
#include<windows.h>
#include<cstring>


using namespace std;
bool Copy(int, int);			//copy infomation of courses 复制课程信息
int Search(int);				//input time code and output correspond number for course if exist(attention: If a timely conflict occurs, only show the formal one)
								//example: to search course infomation at 15:17 on Wednesday, input 0315 (weekday+hour(24))
void recommend();				//automaticly recommend the course due to current time 自动判断当前时间课程
void message(int);				//output the name&notes of a course 输出当前复制的课程的名称
void readin(ifstream&);         //read in the record in txt file 写入数据的文件流函数

class Course {
private:
	int num;
	char c_name[20];				//name of the course课程名称
	int  weekday;					//week date of the course, while 0 represent Sunday and 6 represent Saturday
									//上课日期，0为星期天；不同时间的课为不同的课程
	int  start_t;					//start time of a course 该课程的起始时间（精确到小时）
	int  end_t;						//end time of a course 该课程终止时间
	char add_info[30];				//additionnal notes 额外信息，如分组
	char account[15];				//account num of a course
	char password[10];				//password num if a course
public:
	Course() :num(-1), weekday(-1), start_t(-1), end_t(-1) {
		c_name[0] = '\0';
		add_info[0] = '\0';
		account[0] = '\0';
		password[0] = '\0';
	};
	friend bool Copy(int, int);
	friend void readin(ifstream&);
	friend int Search(int);
	friend void message(int);
	~Course() {}

};



char info_0[10];
int search_code = 0;					//set initial time code as 0 查询编码
int obj;								//correspond number for course课程目标代码
Course menu0[30];
/*******************************************************************************************************/
int main() {
	system("color 57");					//set foreground and background color 设置背景色紫色 前景色白色



	ifstream fp;
	//WinExec(".\\AppData\\Roaming\\Zoom\\bin\\zoom.exe", SW_SHOWNORMAL);	//location of zoom, it can automaticly open the zoom after the exe start
	cout << "Welcome!\n" << "I don't like taking class either, take away your account and password\n";	


	//you can change the txt location here
	fp.open("course.txt", ios::in);
	if (!fp.is_open()) {
		cerr << "/!|>>Err0: Cannot readin txt file" << endl; system("color 47");
		system("pause");
		return 1;
	}


	readin(fp);

	fp.close();

	recommend();


	while (1) {
		system("color 57");
		obj = Search(search_code);					//先找到对应课程编号
		if (obj != -1) {							//对于错误返回，直接跳转至重新输入界面
			Copy(obj, 1);							//复制房间号
			message(obj);							//核实信息
			cout << endl << endl << "\t press ctrl+V to paste your account; and press anykey to copy your password\n\t";
			system("pause");                        

			Copy(obj, 2);						 	//继续查询对应密码
			cout << endl << "\tpassword is copyed, you can press q to quit" << endl;
			cout << "=============================================================" << endl;
			char t;
			cin.get();								
			cin.get(t);
			if (t == 'q')
				break;

		}

		cout << endl << "\tinput time code to search for courses,example:0421 represent Thursday,21\n\t>>>";


		cin >> search_code;

		if (cin.fail()) {					//to deal with mistake input 应对输入非法字符情况，重置
			cin.clear();
			cin.ignore(1000, '\n');			//cin.sync is also available here
			cout << endl << "success" << endl;
			search_code = -1;				//give a Err return
		}
	}


	return 0;
}


/********************************************************************************************/

bool Copy(int m, int n)								//copy correspond contents, m for number of course, n for option 复制相应内容，m为课程序号，n为复制选项
{
	HWND hWnd = NULL;
	OpenClipboard(hWnd);
	EmptyClipboard();
	HANDLE hHandle = GlobalAlloc(GMEM_FIXED, 512);	//give a maxium of 512 bytes memory
	char* pData = (char*)GlobalLock(hHandle);		
	switch (n)
	{
	case 1:  memcpy(pData, menu0[m].account, 20); break;
	case 2: memcpy(pData, menu0[m].password, 20); break;

	}
	SetClipboardData(CF_TEXT, hHandle);			
	GlobalUnlock(hHandle);						
	CloseClipboard();							


	return 0;
}

/*****************************************************************************************************/
int Search(int a)													//find(return) course number due to the time number(input) 找到时间代码对应课程号
{
	int tmp = -2;													//return number 作为返回值 对应课程的序号
	bool flg = 0;													//flag for whether a course is found 作为查询到课程的标记

	if (search_code < 0 || search_code>723) { cerr << "/!|>>Err2: incorrect time code" << endl; search_code = 0; system("color 47"); return -1; }		
	int wk_val, hr_val;
	wk_val = a / 100;
	hr_val = a % 100;
	if (wk_val > 6 || hr_val > 23 || wk_val < 0 || hr_val < 0) { cerr << "/!|>>Err2: incorrect time code" << endl; system("color 47"); return -1; }		

	for (int i = 0; i < 30; i++) {
		if (wk_val == menu0[i].weekday) {
			if (hr_val <= menu0[i].end_t && hr_val >= (menu0[i].start_t)) {									
				if (!flg) {
					tmp = i;
					flg = 1;
				}
				else { cerr << "/!|>>Err3:possibly there is a time conflict" << endl; system("color 47"); }
			}
		}
	}
	if (!flg) { cout << endl << "\t (OwO) No course is found" << endl; return -1; }
	return tmp;
}

/**************************************************************************************************************/
void recommend()					//automaticly give out contemporary time code 自动判断当前时间代码
{
	SYSTEMTIME s_time;				
	GetLocalTime(&s_time);			
	int wday, hr;
	wday = s_time.wDayOfWeek;
	hr = s_time.wHour;
	if (s_time.wMinute >= 40)
		hr++;						//when it's after xx:40, add the hour by 1 ;40分位大部分课程下课时间，直接算作下一科开始时间
	if (hr == 24) { hr = 0; wday++; }		
	search_code = 100 * wday + hr;

	cout << "\tI have got the time code for U! You can immediately paste the account now ->" << search_code << endl;


}

/***************************************************************************************************************/
void message(int q)				//output name&notes of the courses which is being copyed 输出当前复制的课程的名称
{
	if (q != -2) {
		cout << "============================================================";
		cout << endl << "\tCopy confirmed!\n\tThe course is:" << menu0[q].c_name << endl;
		cout << "\t\tAccount:" << menu0[q].account << " password:" << menu0[q].password << endl;
		cout << "\t\tNotes:  " << menu0[q].add_info << endl << endl;
		cout << "============================================================";
	}
}

/**************************************************************************************************************/
void readin(ifstream& fp1)					//read in the txt file
{
	char tmp_str[50] = {};
	fp1 >> tmp_str;							//ignore the notes in the first row
	tmp_str[0] = '\a';
	fp1 >> info_0;
	cout << endl << "Database was last edited in " << info_0 << endl << endl;
	for (int i = 0; i < 30; i++) {
		fp1 >> menu0[i].num >> menu0[i].c_name >> menu0[i].weekday >> menu0[i].start_t >> menu0[i].end_t >> menu0[i].add_info >> menu0[i].account >> menu0[i].password;
		if (fp1.eof())
			break;
		if (menu0[i].num != i) {
			cerr << endl << ">>/!| Err1: Mistake(s) in record" << endl; system("color 47");		//if the format is not correct in the file, warn
		}
	}
}

