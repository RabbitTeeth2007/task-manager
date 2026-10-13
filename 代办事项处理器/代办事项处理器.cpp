#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>

using namespace std;

//数据结构设计，每一个待办事项
struct Project {
	int ID;
	bool done;
	string statment;
	struct Project* next;
};

//全局暂存区
vector<Project> deletedStack;

int nextID = 1;

//通过ID（唯一）返回指针
Project* getID(int ID, Project* head) {
	Project* p = head->next;
	while (p != nullptr) {
		if (p->ID == ID) {
			return p;
		}
		p = p->next;
	}
	return nullptr;
}

//打印任务
void printProject(Project* p) {
	cout << "ID：" << p->ID << endl;
	cout << "名称：" << p->statment << endl;
	if (p->done) {
		cout << "完成状态：" << "已完成" << endl;
	}
	else {
		cout << "完成状态：" << "未完成" << endl;
	}
}

//写文件
void saveToFile(Project* head) {
	ofstream fout("tasks.txt");
	if (!fout) {
		cout << "保存失败，文件无法打开。" << endl;
		return;
	}
	Project* p = head->next;
	while (p != nullptr) {
		fout << p->ID << "|" << p->statment << "|" << p->done << "\n";
		p = p->next;
	}
	fout.close();
}

//读文件
void loadFromFile(Project* head) {
	ifstream fin("tasks.txt");
	if (!fin) {
		return;
	}
	string line;
	while (getline(fin, line)) {
		if (line.empty()) continue;
		stringstream ss(line);
		string idStr, text, doneStr;
		getline(ss, idStr, '|');
		getline(ss, text, '|');
		getline(ss, doneStr, '|');
		Project* p = new Project;
		p->ID = stoi(idStr);
		if (p->ID >= nextID) {
			nextID = p->ID + 1;
		}
		p->statment = text;
		p->done = (doneStr == "1");
		p->next = head->next;
		head->next = p;
	}
	fin.close();
}

//退出操作
void release(Project* head) {
	Project* p = head;
	while (p != nullptr) {
		Project* temp = p;
		p = p->next;
		delete temp;
	}
}
//罗列任务
void list(Project* head) {
	Project* p = head->next;
	while (p != nullptr) {
		printProject(p);
		p = p->next;
	}
}

//删除任务
void del(Project* head) {
	cout << "请输入您要取消事项的ID：";
	int id;
	cin >> id;
	Project* p = getID(id, head);
	if (p == nullptr) {
		cout << "还未添加此任务，你可以通过list查询。" << endl;
	}
	else {
		Project* q = head;
		Project* cur = head->next;
		while (cur != p) {
			q = cur;
			cur = cur->next;
		}
		q->next = p->next;
		Project copy = *p;
		deletedStack.push_back(copy);
		delete p;
		cout << "已删除该任务。" << endl;
		saveToFile(head);
	}
}

//撤销函数
void undo(Project* head) {
	if (deletedStack.empty()) {
		cout << "没有可撤销的删除。" << endl;
		return;
	}
	else {
		Project data = deletedStack.back();
		deletedStack.pop_back();
		Project* p = new Project;
		p->done = data.done;
		p->ID = data.ID;
		p->statment = data.statment;
		p->next = head->next;
		head->next = p;
		saveToFile(head);
	}
}

//统计个数
int count(Project* head) {
	int c = -1;
	Project* p = head;
	while (p) {
		if (!p->done) {
			c++;
		}
		p = p->next;
	}
	return c;
}

//标记任务为done
void markDone(Project* head) {
	cout << "请输入您要标记完成事项的ID：";
	int id;
	cin >> id;
	Project* p = getID(id, head);
	if (p == nullptr) {
		cout << "还未添加此任务，你可以通过list查询。" << endl;

	}
	else {
		if (p->done) {
			cout << "该事项已经完成。" << endl;
		}
		else {
			p->done = true;
			cout << "已标记完成。" << endl;
			saveToFile(head);
		}
	}
}

//新增任务
void add(Project* head) {
	string s;
	cout << "请输入新任务内容：";
	cin.ignore();
	getline(cin, s);
	Project* p = new Project;
	p->ID = nextID++;
	p->statment = s;
	p->done = false;
	p->next = head->next;
	head->next = p;
	cout << "该任务添加成功，ID：" << p->ID << endl;  // 告诉用户分到了哪个ID
	saveToFile(head);
}

//主函数
int main() {
	cout << "======待办事项处理器======" << endl;
	Project* head = new Project;
	head->done = false;
	head->ID = 0;
	head->statment = "0";
	head->next = nullptr;
	loadFromFile(head);
	cout << "您可以在此管理您的任务。" << endl;
	cout << "输入help可查看使用方法。" << endl;
	string order;
	while (cin >> order) {
		if (order == "quit") {
			release(head);
			cout << "您已关闭处理器。" << endl;
			break;
		}
		else if (order == "list") {
			if (head->next == nullptr) {
				cout << "还没有添加任务。" << endl;
			}
			else {
				list(head);
			}
		}
		else if (order == "del") {
			if (head->next == nullptr) {
				cout << "还没有添加任务。" << endl;
			}
			else {
				del(head);
			}
		}
		else if (order == "done") {
			markDone(head);
		}
		else if (order == "add") {
			add(head);
		}
		else if (order == "undo") {
			undo(head);
		}
		else if (order == "count") {
			cout << "您还有" << count(head) << "个待办事项。" << endl;
		}
		else if (order == "help") {
			cout << "add可以添加任务。" << endl;
			cout << "list可以列出任务。" << endl;
			cout << "del可以删除任务。" << endl;
			cout << "done可以标记完成。" << endl;
			cout << "undo可以撤销删除。" << endl;
			cout << "count可以统计未完成个数。" << endl;
			cout << "quit可以退出程序。" << endl;
		}
		else {
			cout << "这不是一个合法指令。" << endl;
			cout << "请重新输入。" << endl;
		}
	}
	return 0;
}