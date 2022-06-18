#include<bits/stdc++.h>
using namespace std;
#define MAX 1005
#define DFANUM 8

//����ʽ�Ĳ�����
set<char> operators;
//������
set<char> operands;

//NFA�Ŀ�ʼ״̬�ͽ���״̬
typedef struct State{
    int begin;
    int end;
}State;

//NFA�ڵ�
typedef struct NFA{
    int from;//״̬1 
    int to;//״̬2
    char c;//�ַ�c
}NFA;

//�洢һ��NFA
NFA nfa[MAX];
int point = 0;

//DFA�洢�ṹ
typedef struct DFA{
	int id;
	map<set<int>, int> dfa_id;
	bool vn[MAX];
    int tran[MAX][MAX];
}DFA;

//�洢���DFA
DFA* dfas[DFANUM];
int point_dfa = 0;

//����ʽ��Ӧ����������
string names[DFANUM];

//��������ȼ�
int priority(char ch);

//Ԥ�����������ӷ�
void insert_behind_n(string& s, int n, char ch);

void pre_process(string& s);

//��׺ת��׺
string infix_Suffix(string s);

//����ʽתNFA
void CFG_NFA(string cfg);

//closure()
set<int> empty_transfer(int id);

//closure(move())
set<int> empty_move(int id,char c);

//NFAתDFA
void NFA_DFA();

//DFA��С��
void minimize_DFA();

//��ʼ����������NFA��DFA
void init();
void init_DFA(string cfg);

//ͨ��DFA�жϵ����Ƿ�������ʽ�ľ���
int distinguish(string s);
//source ����;floor �к�
void myLex(string soucre,int floor);

void input();
int main(){
    init();
    //ʵ�ּ��׵�Lex���� .l �ļ��е� 
    //������ʽ {fun()}
    input();
    return 0;
}

void init(){
    //��ʼ������ʽ�����
    operators.insert('*');
    operators.insert('&');
    operators.insert('|');
    operators.insert('(');
    operators.insert(')');

    //����ʽ��Ӧ����������
	names[0] = "������";
	names[1] = "���";
    names[2] = "��ʶ��";
    names[3] = "�޷�������";
    names[4] = "�����";
    //����ʽ
	init_DFA("const|var|procedure|begin|end|odd|if|then|call|while|do|read|write");
    init_DFA("\\(|\\)|,|;|.");
    init_DFA("(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9)*");
    init_DFA("0|(1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*");
    init_DFA("+|-|\\*|/|<|<=|>|>=|#|=|:=");
}

void input(){
    char ch;
    string word;
    int floor = 1;//�к�
    ch = getchar();
    do{
        //�����ո�
        while(ch == ' '){
            ch = getchar();
        }
        //����ע��
        if (ch == '/') {
            ch = getchar();
            if(ch=='/'){
                while(ch !='\n'){
                	ch = getchar();
                }
            }else if(ch == '*'){
                while(1){
                	ch = getchar();
                	if(ch == EOF){
                            cout<<"error";
                			break;
                		}
                	if(ch == '*'){
                		ch = getchar();
                		if(ch == '/'){
                			ch = getchar();
                			break;
                		}
                	}
                    if(ch == '\n'){
                        floor++;
                    }
                }
            }
        }

        //ʶ�����
        while(ch != ' ' ){
            if(ch == '\n'){
                floor++;
                if(word!=""){
                    myLex(word,floor);
                }
                word = "";
                ch = getchar();
                break;
            }
            string b(1,ch);
            int ret = distinguish(b);
            if(ret == 1){
            	
                myLex(word,floor);
                myLex(b,floor);
                word = "";
                ch = getchar();
                break;
            }
            word += ch;
            ch = getchar();
        }
        if(word != "")
            myLex(word,floor);
        word = "";
    }while(ch != EOF);
    
}

int priority(char ch)
{

	if (ch == '*')
	{
		return 3;
	}

	if (ch == '&')
	{
		return 2;
	}

	if (ch == '|')
	{
		return 1;
	}

	if (ch == '(')
	{
		return 0;
	}
}

void insert_behind_n(string& s, int n, char ch)
{

	s += '#';

	for (int i = s.size() - 1; i > n; i--)
		s[i] = s[i - 1];

	s[n] = ch;
}

void pre_process(string& s)
{

	int i = 0, length = s.size()-1;
	while (i < length)
	{
		int in_all_1 = 0, in_all_2 = 0;
        //ע��ת��
        if(operators.count(s[i])==0 && s[i]!='\\'){
            in_all_1 = 1;
        }
        if(operators.count(s[i+1])==0 ){
            in_all_2 = 1;
        }

		if (in_all_1 || (s[i] == '*') || (s[i] == ')')){
            if (in_all_2 || s[i + 1] == '(')
			{
				insert_behind_n(s, i + 1, '&');
				length++;
			}
        }
		i++;
	}
}

string infix_Suffix(string s)
{

	pre_process(s);	
	string str;				
	stack<char> oper;		

	for (unsigned int i = 0; i < s.size(); i++)
	{
		int in_all = 0;
        if(s[i] == '\\'){
            str += s[i];
            i++;
            str += s[i];
            operands.insert(s[i]); 
        }
		if(operators.count(s[i])==0){
            in_all = 1;
            operands.insert(s[i]);
        }

		if (in_all)	
		{
            str += s[i];
		}
		else							
		{

			if (s[i] == '(')			
			{
				oper.push(s[i]);
			}

			else if (s[i] == ')')	
			{

				char ch = oper.top();
				while (ch != '(')		
				{
					str += ch;
					oper.pop();
					ch = oper.top();
				}

				oper.pop();				
			}
			else				
			{

				if (!oper.empty())		
				{

					char ch = oper.top();
					while (priority(ch) >= priority(s[i]))
					{

						str += ch;
						oper.pop();

						if (oper.empty())	
						{
							break;
						}
						else ch = oper.top();
					}

					oper.push(s[i]);		
				}

				else				
				{
					oper.push(s[i]);
				}
			}
		}
	}

	while (!oper.empty())
	{

		char ch = oper.top();
		oper.pop();

		str += ch;
	}
    //cout<<str<<endl;
	return str;
}
//DFA��״̬ջ����¼��ʼ״̬���ս�״̬
State stateStack[MAX];
int top;
//��ȡID
int stateID = 1;
void CFG_NFA(string cfg){
    
    stateID = 1;
    top = 0;
    point = 0;
    for (int i=0;i<cfg.length();i++){
        State a;
        State b;
        State c;
        switch (cfg[i])
        {
        case '&': 
            //��״̬ջ��ȡ������״̬����������ʼ���ս�״̬
            a = stateStack[top--];
            b = stateStack[top--];
            c.begin = b.begin;
            c.end = a.end;
            stateStack[++top] = c;
            //�½�һ��״̬ת�ƣ�a״̬����#ת�Ƶ�b����a��b״̬��������
            NFA nfa0;
            nfa0.from = b.end;
            nfa0.to = a.begin;
            nfa0.c = '#';
            nfa[point++] = nfa0;
            break;
        case '|':
            //����2
            c.begin = stateID++;
            c.end = stateID++;
            a = stateStack[top--];
            b = stateStack[top--];
            stateStack[++top] = c;
            
            NFA nfa1;
            nfa1.from = c.begin;
            nfa1.to = a.begin;
            nfa1.c = '#';
            nfa[point++] = nfa1;
            
            NFA nfa2;
            nfa2.from = c.begin;
            nfa2.to = b.begin;
            nfa2.c = '#';
            nfa[point++] = nfa2;

            NFA nfa3;
            nfa3.from = a.end;
            nfa3.to = c.end;
            nfa3.c = '#';
            nfa[point++] = nfa3;

            NFA nfa4;
            nfa4.from = b.end;
            nfa4.to = c.end;
            nfa4.c = '#';
            nfa[point++] = nfa4;
            break;
        case '*':
            //����3
            c.begin = stateID++;
            c.end = stateID++;
            a = stateStack[top--];
            stateStack[++top] = c;

            NFA nfa5;
            nfa5.from = c.begin;
            nfa5.to = c.end;
            nfa5.c = '#';
            nfa[point++] = nfa5;
            
            NFA nfa6;
            nfa6.from = c.begin;
            nfa6.to = a.begin;
            nfa6.c = '#';
            nfa[point++] = nfa6;

            NFA nfa7;
            nfa7.from = a.end;
            nfa7.to = a.begin;
            nfa7.c = '#';
            nfa[point++] = nfa7;

            NFA nfa8;
            nfa8.from = a.end;
            nfa8.to = c.end;
            nfa8.c = '#';
            nfa[point++] = nfa8;
            break;
        case '\\':
            //����ת���ַ�
            i++;
            c.begin = stateID++;
            c.end = stateID++;
            stateStack[++top] = c;

            NFA nfa10;
            nfa10.from = c.begin;
            nfa10.to = c.end;
            nfa10.c = cfg[i];
            nfa[point++] = nfa10;
            break;
        default:
            //����������
            c.begin = stateID++;
            c.end = stateID++;
            stateStack[++top] = c;

            NFA nfa9;
            nfa9.from = c.begin;
            nfa9.to = c.end;
            nfa9.c = cfg[i];
            nfa[point++] = nfa9;
            break;
        }
    }
    //���NFA
    // for(int i=0;i<point;i++){
    //     cout<<nfa[i].from<<" "<<nfa[i].c<<" "<<nfa[i].to<<endl;
    // }
}

set<int> empty_transfer(int id){
    set<int> set1;
    set<int> set2;
    set1.insert(id);
    //����NFA���������״̬���ϼ���set1��set2
    for(int i=0;i<point;i++){
        if(nfa[i].from==id && nfa[i].c == '#'){
            set1.insert(nfa[i].to);
            set2.insert(nfa[i].to);
        } 
    }
    //���������õ���set2���������ת�ƣ�ֱ��set2Ϊ�գ��õ���set1����״̬�Ŀ�ת��
    while(!set2.empty()){
        int temp = *set2.begin();
        set2.erase(set2.begin());
        for(int i=0;i<point;i++){
            if(nfa[i].from==temp && nfa[i].c == '#'){
                set2.insert(nfa[i].to);
                set1.insert(nfa[i].to);
            } 
        }
    }
    return set1;
}

set<int> empty_move(int id,char c){
    //����move��c��
	set<int> set1;
    for(int i=0;i<point;i++){
        if(nfa[i].from==id && nfa[i].c == c){
            set1.insert(nfa[i].to);
        } 
    }
    //�õ�move֮�����ת��
    set<int> set2;
    set2.insert(set1.begin(),set1.end());
    set<int>::iterator it1;
    for (it1 = set1.begin(); it1 != set1.end(); it1++){
        set<int> set3 = empty_transfer(*it1);
        set2.insert(set3.begin(),set3.end());
    }
    return set2;
}

void NFA_DFA(){

	DFA* dfa = new DFA();
	
    //��ȡ��ʼ״̬���ս�״̬
    State ss = stateStack[top];
    int begin = ss.begin;
    int end = ss.end;
    //��ʼ״̬�Ŀ�ת��
    set<int> initial = empty_transfer(begin);
    //״̬����
    set< set<int> > states;
    set< set<int> > states_temp;
    states.insert(initial);
    //������DFA
    dfa->dfa_id[initial] = ++dfa->id;
    //����״̬����
    while(!states.empty()){
        set<int> temp = *states.begin();
        states.erase(states.begin());
        set<int>::iterator it1;
        set<char>::iterator it2;
        //������������closeuure��move��
        for (it2 = operands.begin(); it2 != operands.end(); it2++){
            set<int> new_set;
            for (it1 = temp.begin(); it1 != temp.end(); it1++){
                set<int> set_temp = empty_move(*it1,*it2);
                new_set.insert(set_temp.begin(),set_temp.end());
            }
            if(states_temp.count(new_set) == 0 && !new_set.empty()){
               
                states.insert(new_set);
                states_temp.insert(new_set);
                dfa->dfa_id[new_set] = ++dfa->id;
            }
			//cout<<dfa->dfa_id[temp]<<" "<<*it2<<" "<<dfa->dfa_id[new_set]<<endl;
            dfa->tran[dfa->dfa_id[temp]][*it2-33] = dfa->dfa_id[new_set];
            if(new_set.count(end)){
                dfa->vn[dfa->dfa_id[new_set]] = true;
            }else{
                dfa->vn[dfa->dfa_id[new_set]] = false;
            }
        }
    }
	dfas[point_dfa++] = dfa;
}

void init_DFA(string cfg){
    operands.clear();
	cfg = infix_Suffix(cfg);
	CFG_NFA(cfg);
	NFA_DFA();
    minimize_DFA();
}

void minimize_DFA(){
    set<set<int> > sets;
    DFA* dfa = dfas[point_dfa-1];
    //��ʼ������̬����̬����
    int len = dfa->id;
    set<int> set1,set2;
    for(int i=1;i<=len;i++){
        if(dfa->vn[i]){
            set1.insert(i);
        }else{
            set2.insert(i);
        }
    }
    sets.insert(set1);
    sets.insert(set2);

    set<set<int> > begin_set;
    do{
        begin_set = sets;
        set<set<int> >::iterator it1;
        set<int> temp;
        //����״̬����
        for (it1 = begin_set.begin(); it1 != begin_set.end(); it1++){
            temp= *it1;
            set<char>::iterator it2;
            //����������
            for(it2 = operands.begin();it2 != operands.end();it2++){
                map<set<int>,set<int> > map_set;
                set<int>::iterator it3;
                for(it3 = temp.begin();it3 != temp.end();it3++){
                    int d = dfa->tran[*it3][*it2-33];
                    //�ж������ĸ�����
                    //cout<<*it3<<" "<<*it2<<" "<<d<<endl;
                    set<set<int> >::iterator it4;
                    int flag = 0;
                    set<int>temp2,temp3;
                    for(it4 = begin_set.begin();it4 != begin_set.end();it4++){
                        temp2 = *it4;
                        if(temp2.count(d)>0){
                            temp3 = map_set[temp2];
                            flag = 1;
                        }
                    }
                    //���״̬�������µ�״̬���ϣ� �����Էָ��ԭ����״̬ɾ����״̬���ϴ��뵽map�С�
                    if(flag == 1){
                        sets.erase(temp);
                        temp3.insert(*it3);
                        map_set[temp2] = temp3;
                    }
                }
                //��map�еļ��ϲ��뵽ԭ������
                map<set<int>,set<int> >::iterator it5;
                for(it5 = map_set.begin();it5 != map_set.end();it5++){
                    sets.insert(it5->second);
                }
                
            }
        }
        
    }while(begin_set != sets);
    //����С����DFA��������
    DFA* min_dfa = new DFA();
    set<set<int> >::iterator it6;
    set<set<int> > sets_temp;
    //������С��DFA�õ��ļ���
    for(it6 = sets.begin();it6 != sets.end();it6++){

        set<int> t_set = *it6;
        set<char>::iterator it7;
        int f = *t_set.begin();
        int id=0,to=0;
        //�½�DFA״̬
        if(sets_temp.count(t_set) == 0){
            sets_temp.insert(t_set);
            min_dfa->dfa_id[t_set] = ++min_dfa->id;  
        }
        id = min_dfa->dfa_id[t_set];
        //������������õ�״̬ת�ƾ�����ս������
        for(it7 = operands.begin();it7 != operands.end();it7++){
            to = 0;
            int d = dfa->tran[f][*it7 -33];
            set<set<int> >::iterator it8;
            for(it8 = sets.begin();it8 != sets.end();it8++){
                set<int> t_set2 = *it8;
                if(t_set2.count(d)>0){
                    if(sets_temp.count(t_set2) == 0){
                        min_dfa->dfa_id[t_set2] = ++min_dfa->id;
                        sets_temp.insert(t_set2);
                    }
                    to = min_dfa->dfa_id[t_set2];
                    if(min_dfa->vn[d]){
                        min_dfa->vn[to] = true;
                    }
                }
            }
            min_dfa->tran[id][*it7-33] = d;
        }
    }

    dfas[point_dfa] = min_dfa;
}

int distinguish(string s){
   for(int i = 0;i<point_dfa;i++){
        int cur = 1,flag = 1;
        DFA* dfa  = dfas[i];
        int to = 0;
        //һֱת�ƣ�֪�����ʽ���
		for(int j = 0;j<s.length();j++){
			to = dfa->tran[cur][s[j]-33];
			if(to == 0){
				flag = 0;
			}else{
				cur = to;
			}
		}
        //������������ȫ��״̬ת�ƣ�����ת�Ƶ���״̬Ϊ��̬����DFA��id���ء�
		if(flag ==1 && dfa->vn[to]){
			return i;
		}
   }
   return -1;
}

void myLex(string source,int floor){
	if(source == ""){return;
	}

    //ecucoder �����⣬���ؿ������У�educoder����Tab
    string s_new;
    for(int i=0;i<source.length();i++){
        if(source[i] != '\t'){
            s_new+=source[i];
        }
    }
	int ans = distinguish(s_new);
	if(ans !=-1){
        if(ans == 2 &&s_new.length()>8){
            cout<<"("<<"��ʶ�����ȳ���"<<","<<s_new<<","<<"�к�:"<<floor<<")"<<endl;
            return ;
        }else if(ans == 3 && s_new.length()>8){
            cout<<"("<<"�޷�������Խ��"<<","<<s_new<<","<<"�к�:"<<floor<<")"<<endl;
            return ;
        }
		cout<<"("<<names[ans]<<","<<s_new<<")"<<endl;
	}else{
        cout<<"("<<"�Ƿ��ַ�(��)"<<","<<s_new<<","<<"�к�:"<<floor<<")"<<endl;
    }
}

