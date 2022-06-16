#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <iomanip>
#include <queue>
#include <climits>

using namespace std;

struct ins
{
	string rs1, rs2, rd;
	int cons = INT_MAX, rs1Data = INT_MAX, rs2Data = INT_MAX;
	int result = INT_MAX;
	char operand = '0';
};

int add_sub_cycle = 2;
int mul_cycle = 10;
int div_cycle = 40;

queue<ins> code;
map<string, int> tag;
string add_executing_rs, mul_executing_rs;
string rat[6];
int reg[6] = { -1,0,2,4,6,8 };
ins addRs[3];
ins mulRs[2];
int add = 3;
int mul = 2;
int cycle = 0;
ins add_buffer;
ins mul_buffer;

void output()
{
	cout << "Cycle: " << cycle << "\n\n";
	//RF
	cout << "    _RF__\n";
	for (int i = 1; i <= 5; ++i)
		cout << "F" << i << " |" << setw(4) << reg[i] << " |\n";
	cout << "   -------\n\n";

	//RAT
	cout << "    _RAT__\n";
	for (int i = 1; i <= 5; ++i)
	{
		if (rat[i] != "")
			cout << "F" << i << " |" << setw(4) << rat[i] << " |\n";
		else
			cout << "F" << i << " |" << "     |\n";
	}
	cout << "   -------\n\n";

	//RS(ADD)
	cout << "    _RS________________\n";
	for (int i = 0; i < 3; ++i)
	{
		cout << "RS" << i + 1 << " |";
		if (addRs[i].operand != '0')
			cout << setw(4) << addRs[i].operand << " |";
		else
			cout << "     |";

		if (addRs[i].rs1Data != INT_MAX)
			cout << setw(4) << addRs[i].rs1Data << " |";
		else if(addRs[i].rs1 != "")
			cout << setw(4) << addRs[i].rs1 << " |";
		else
			cout << "     |";

		if (addRs[i].rs2Data != INT_MAX)
			cout << setw(4) << addRs[i].rs2Data << " |";
		else if (addRs[i].rs2 != "")
			cout << setw(4) << addRs[i].rs2 << " |";
		else
			cout << "     |";

		cout << '\n';
	}
	cout << "    -------------------\n";
	cout << "BUFFER: ";
	if (add_executing_rs == "")cout << "empty\n\n";
	else cout << "(" << add_executing_rs << ") " << add_buffer.rs1Data << " " << add_buffer.operand << " " << add_buffer.rs2Data << "\n\n";
	
	//RS(MUL)
	cout << "    ___________________\n";
	for (int i = 0; i < 2; ++i)
	{
		cout << "RS" << i + 4 << " |";
		if (mulRs[i].operand != '0')
			cout << setw(4) << mulRs[i].operand << " |";
		else
			cout << "     |";

		if (mulRs[i].rs1Data != INT_MAX)
			cout << setw(4) << mulRs[i].rs1Data << " |";
		else if (mulRs[i].rs1 != "")
			cout << setw(4) << mulRs[i].rs1 << " |";
		else
			cout << "     |";

		if (mulRs[i].rs2Data != INT_MAX)
			cout << setw(4) << mulRs[i].rs2Data << " |";
		else if (mulRs[i].rs2 != "")
			cout << setw(4) << mulRs[i].rs2 << " |";
		else
			cout << "     |";

		cout << '\n';
	}
	cout << "    -------------------\n";
	cout << "BUFFER: ";
	if (mul_executing_rs == "")cout << "empty\n\n";
	else cout << "(" << mul_executing_rs << ") " << mul_buffer.rs1Data << " " << mul_buffer.operand << " " << mul_buffer.rs2Data << "\n\n";
	cout << "-----------------------\n\n";
}

void init()
{
	tag["F1"] = 1;
	tag["F2"] = 2;
	tag["F3"] = 3;
	tag["F4"] = 4;
	tag["F5"] = 5;
	tag["RS1"] = 1;
	tag["RS2"] = 2;
	tag["RS3"] = 3;
	tag["RS4"] = 4;
	tag["RS5"] = 5;
}

int main()
{
	init();
	string input;
	while (getline(cin, input) && input != "")
	{
		stringstream ss;
		string inst;
		ins tmp;
		ss.str(input);
		ss >> inst;
		if (inst == "ADDI" || inst == "ADD") tmp.operand = '+';
		else if (inst == "SUB") tmp.operand = '-';
		else if (inst == "MUL") tmp.operand = '*';
		else if (inst == "DIV") tmp.operand = '/';
		
		ss >> tmp.rd >> tmp.rs1;
		tmp.rd = tmp.rd.substr(0, 2);
		tmp.rs1 = tmp.rs1.substr(0, 2);
		if (inst == "ADDI")ss >> tmp.cons;
		else ss >> tmp.rs2;
		
		code.push(tmp);
	}

	int addExec_time = 1, mulExec_time = 1, add_wbNum = 0, mul_wbNum = 0;// add����F�h��cycle, mul����F�h��cycle, add������rs�b����, mul������rs�b����
	int mul_or_div;//mul��ALU�b�B��mul�٬Odiv(�M�w��O�h��cycle)
	bool addExec = false, mulExec = false, add_wb = false, mul_wb = false;//add��ALU�O�_�ϥΤ�, mul��ALU�O�_�ϥΤ�, add�O�_�nwrite back, mul�O�_�nwrite back

	while (!code.empty() || (add < 3 || mul < 2))
	{
		bool change = false; //�O�_�����(�ݭn��X)
		++cycle;

		//capture & write back (add or sub �u���� mul or div)
		if (add_wb)// add write_back  
		{
			int result = addRs[add_wbNum].result;
			//add��rs�O�_���ݭn�Q�g�^��
			for (int i = 0; i < 3; ++i)
			{
				if (addRs[i].rs1 == add_executing_rs)
				{
					addRs[i].rs1 = "";
					addRs[i].rs1Data = result;
				}
				if (addRs[i].rs2 == add_executing_rs)
				{
					addRs[i].rs2 = "";
					addRs[i].rs2Data = result;
				}
			}
			//mul��rs�O�_���ݭn�Q�g�^��
			for (int i = 0; i < 2; ++i)
			{
				if (mulRs[i].rs1 == add_executing_rs)
				{
					mulRs[i].rs1 = "";
					mulRs[i].rs1Data = result;
				}
				if (mulRs[i].rs2 == add_executing_rs)
				{
					mulRs[i].rs2 = "";
					mulRs[i].rs2Data = result;
				}
			}
			//�g�^rat
			for (int i = 1; i <= 5; ++i)if (rat[i] == add_executing_rs)
				rat[i] = "";

			//�g�^rf
			reg[tag[addRs[add_wbNum].rd]] = result;

			//�M�ťe�Ϊ�rs
			int tmp = tag[add_executing_rs] - 1;
			addRs[tmp].operand = '0';
			addRs[tmp].rd = "";
			addRs[tmp].rs1 = "";
			addRs[tmp].rs2 = "";
			addRs[tmp].rs1Data = INT_MAX;
			addRs[tmp].rs2Data = INT_MAX;
			addRs[tmp].cons = INT_MAX;
			addRs[tmp].result = INT_MAX;
			
			//��L�Ѽƺ��@
			add_executing_rs = "";
			add_wb = false;
			change = true;
			++add;
		}
		else if (mul_wb)
		{
			int result = mulRs[mul_wbNum].result;
			//add��rs�O�_���ݭn�Q�g�^��
			for (int i = 0; i < 3; ++i)
			{
				if (addRs[i].rs1 == mul_executing_rs)
				{
					addRs[i].rs1 = "";
					addRs[i].rs1Data = result;
				}
				if (addRs[i].rs2 == mul_executing_rs)
				{
					addRs[i].rs2 = "";
					addRs[i].rs2Data = result;
				}
			}
			//mul��rs�O�_���ݭn�Q�g�^��
			for (int i = 0; i < 2; ++i)
			{
				if (mulRs[i].rs1 == mul_executing_rs)
				{
					mulRs[i].rs1 = "";
					mulRs[i].rs1Data = result;
				}
				if (mulRs[i].rs2 == mul_executing_rs)
				{
					mulRs[i].rs2 = "";
					mulRs[i].rs2Data = result;
				}
			}
			//�g�^rat
			for (int i = 1; i <= 5; ++i)if (rat[i] == mul_executing_rs)
				rat[i] = "";

			//�g�^rf
			reg[tag[mulRs[mul_wbNum].rd]] = result;

			//�M�ťe�Ϊ�rs
			int tmp = tag[mul_executing_rs] - 4;
			mulRs[tmp].operand = '0';
			mulRs[tmp].rd = "";
			mulRs[tmp].rs1 = "";
			mulRs[tmp].rs2 = "";
			mulRs[tmp].rs1Data = INT_MAX;
			mulRs[tmp].rs2Data = INT_MAX;
			mulRs[tmp].cons = INT_MAX;
			mulRs[tmp].result = INT_MAX;

			//��L�Ѽƺ��@
			mul_executing_rs = "";
			mul_wb = false;
			change = true;
			++mul;
		}

		//dispatch
		if (!addExec) //add��ALU�O�_�Q�e��
			for (int i = 0; i < 3; ++i)
			{
				if (addRs[i].rs1Data != INT_MAX && addRs[i].rs2Data != INT_MAX)//�O�_���i�H�Qdispatch��
				{
					add_buffer = addRs[i];//���b���檺inst.
					add_executing_rs = (i == 0) ? "RS1" : ((i == 1) ? "RS2" : "RS3");//����rs�|�e��ALU
					addRs[i].result = (addRs[i].operand == '+') ? addRs[i].rs1Data + addRs[i].rs2Data : addRs[i].rs1Data - addRs[i].rs2Data;//��ڹB��

					addExec = true;
					add_wbNum = i;
					change = true;
					break;
				}
			}
		else
		{
			++addExec_time;//�e�θg�L�h�֮ɶ�
			if (addExec_time == add_sub_cycle)//�ɭԨ�F
			{
				add_wb = true;//�U�@cycle�|�g�^
				addExec = false;//���X�Ŷ�
				addExec_time = 1;
			}
		}

		if(!mulExec) //mul��ALU�O�_�Q�e��
			for (int i = 0; i < 2; ++i)
			{
				if (mulRs[i].rs1Data != INT_MAX && mulRs[i].rs2Data != INT_MAX)//�O�_���i�H�Qdispatch��
				{
					mul_or_div = (mulRs[i].operand == '*') ? mul_cycle : div_cycle;//�O * �� / (�M�w����h��cycle)
					mul_buffer = mulRs[i];//���b���檺inst.
					mul_executing_rs = (i == 0) ? "RS4" : "RS5";//����rs�|�e��ALU
					mulRs[i].result = (mulRs[i].operand == '*') ? mulRs[i].rs1Data * mulRs[i].rs2Data : mulRs[i].rs1Data / mulRs[i].rs2Data;//��ڹB��

					mulExec = true;
					mul_wbNum = i;
					change = true;
					break;
				}
			}
		else
		{
			++mulExec_time;//�e�θg�L�h�֮ɶ�
			if (mulExec_time == mul_or_div)//�ɭԨ�F
			{
				mul_wb = true;//�U�@cycle�|�g�^
				mulExec = false;//���X�Ŷ�
				mulExec_time = 1;
			}
		}

		// issue
		if (!code.empty())
		{
			int num = 0;
			ins tmp = code.front();
			if ((tmp.operand == '+' || tmp.operand == '-') && add)//�e��add��rs
			{
				string old_rat= rat[tag[tmp.rd]];//rat��s�e����
				
				// rat (�ˬd���ӬO�Ū�)
				if (addRs[0].rs1 == "" && addRs[0].rs1Data == INT_MAX) 
				{
					rat[tag[tmp.rd]] = "RS1";
					num = 0;
				}
				else if (addRs[1].rs1 == "" && addRs[1].rs1Data == INT_MAX)
				{
					rat[tag[tmp.rd]] = "RS2";
					num = 1;
				}
				else if (addRs[2].rs1 == "" && addRs[2].rs1Data == INT_MAX)
				{
					rat[tag[tmp.rd]] = "RS3";
					num = 2;
				}

				// rs
				addRs[num].operand = tmp.operand;
				addRs[num].rd = tmp.rd;

				if (rat[tag[tmp.rs1]] == "" || (old_rat == "" && tmp.rd == tmp.rs1)) addRs[num].rs1Data = reg[tag[tmp.rs1]];
				else addRs[num].rs1 = (old_rat == "") ? rat[tag[tmp.rs1]] : old_rat;

				if (tmp.cons != INT_MAX)addRs[num].rs2Data = tmp.cons;
				else if (rat[tag[tmp.rs2]] == "" || (old_rat == "" && tmp.rd == tmp.rs2)) addRs[num].rs2Data = reg[tag[tmp.rs2]];
				else addRs[num].rs2 = (old_rat == "") ? rat[tag[tmp.rs2]] : old_rat;

				--add;
				code.pop();
				change = true;
			}
			else if ((tmp.operand == '*' || tmp.operand == '/') && mul)//�e��mul��rs
			{
				string old_rat = rat[tag[tmp.rd]];//rat��s�e����
				
				// rat (�ˬd���ӬO�Ū�)
				if (mulRs[0].rs1 == "" && mulRs[0].rs1Data == INT_MAX) 
				{
					rat[tag[tmp.rd]] = "RS4";
					num = 0;
				}
				else if (mulRs[1].rs1 == "" && mulRs[1].rs1Data == INT_MAX)
				{
					rat[tag[tmp.rd]] = "RS5";
					num = 1;
				}

				// rs
				mulRs[num].operand = tmp.operand;
				mulRs[num].rd = tmp.rd;

				if (rat[tag[tmp.rs1]] == "" || (old_rat == "" && tmp.rd == tmp.rs1)) mulRs[num].rs1Data = reg[tag[tmp.rs1]];
				else mulRs[num].rs1 = (old_rat == "") ? rat[tag[tmp.rs1]] : old_rat;

				if (rat[tag[tmp.rs2]] == "" || (old_rat == "" && tmp.rd == tmp.rs2)) mulRs[num].rs2Data = reg[tag[tmp.rs2]];
				else mulRs[num].rs2 = (old_rat == "") ? rat[tag[tmp.rs2]] : old_rat;

				--mul;
				code.pop();
				change = true;
			}
		}

		//����ʴN��X
		if (change)
			output();
	}
}