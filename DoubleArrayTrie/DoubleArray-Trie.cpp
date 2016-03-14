#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string.h>

#define READ_FILE
#define MAXN 256
#define TEXTMAXN 30000
#define STMXN 1000
using namespace std;

/* Data structures*/
typedef struct lrPart
{
	char lPart[MAXN];		//the left part to be processed
	char rPart[MAXN];		//the right processed part of the pattern
	vector<int> seq;		//a vector storing the states of this pattern
	int oriPos;				//the original posistion(raw) in the patternList 
	//comparision between patterns, used for sort 
	bool operator < (const lrPart &m) const
	{
		int cmpRst = strcmp(this->rPart, m.rPart);
		if (cmpRst > 0)		// s1 > s2 return positive
			return false;
		else if (cmpRst == 0)
		{
			if (strcmp(this->lPart, m.lPart) <= 0)
				return true;
			else
				return false;
		}
		else
			return true;
	} 
	//init with a pattern and the pos of that pattern in the patternList
	void init(char* s, int pos)
	{
		memset(lPart, 0, sizeof(0));
		memset(rPart, 0, sizeof(0));
		strcpy(lPart, s);
		seq.clear();
		oriPos = pos;
	}
	//print current processing status of this pattern
	void Print()
	{
		cout << lPart << " " << rPart << " ";
		for (size_t i = 0; i < this->seq.size(); i++)
			cout << seq[i] << " ";
		cout << endl;
	}
	//move the first character to the end of the right part
	void shift()
	{
		char tmp[MAXN];
		strcpy(tmp, lPart);
		*(rPart + strlen(rPart) + 1) = '\0';
		rPart[strlen(rPart)] = lPart[0];
		strcpy(lPart, tmp + 1);
	}

}lrPart;
typedef struct state
{
	int depth;
	char ch;			//the character before this state
	int isFstChild;		//if this state is the first child of his parent
	int stPosInNext;	//pos in the next table
	vector<char(*)[MAXN]> ptnList;  // used for output function, storing the should-output patterns
	//init work
	state()
	{
		depth = -1;
		ch = '\0';
		isFstChild = -1;
		stPosInNext = -1;
	}
	//set depth, ch, and isFstChild attributes
	void setBasic(int _depth, char _ch, int _isFstChild)
	{
		depth = _depth;
		ch = _ch;
		isFstChild = _isFstChild;
	}
	//set posInNext
	void setPosInNext(int _posInNext)
	{
		stPosInNext = _posInNext;
	}
}state;

vector<lrPart>::iterator it;
vector<char(*)[MAXN]>::iterator outIt;


char patternList[MAXN][MAXN];
char text[TEXTMAXN];
vector<lrPart> vec;
state stList[STMXN];
int nextTable[MAXN];
int baseTable[MAXN];
int checkTable[MAXN];
int failTable[MAXN];


/* Functions */
int StateTrans(int s, char c, int pos);
int StateTrans(int s, char c);
int Fail(int s)
{
	int t;
	if (s == 0 || stList[s].depth == 1)
		return 0;
	else
	{
		t = StateTrans(Fail(checkTable[s]), stList[s].ch);
		if (stList[t].ptnList.size() != 0)
		{
			for (size_t i = 0; i < stList[t].ptnList.size(); i++)
			{
				stList[s].ptnList.push_back(stList[t].ptnList[i]);
			}
		}
		return t;
	}
}
int StateTrans(int s, char c)
{
	int nextState;
	int t = nextTable[stList[s].stPosInNext + baseTable[s] + c];
	if (checkTable[t] == s)
		nextState = t;
	else
		nextState = Fail(s);
	return nextState;
}
int StateTrans(int s, char c, int pos)
{
	int nextState;
	int t = nextTable[stList[s].stPosInNext + baseTable[s] + c];
	if (checkTable[t] == s)
	{
		nextState = t;
		size_t sPtnListSize = stList[t].ptnList.size();
		if (sPtnListSize > 0)
		{
			for (size_t i = 0; i < sPtnListSize; i++)
			{
				cout << "Found " << *stList[t].ptnList[i] << " in position " << pos - strlen(*stList[t].ptnList[i]) + 1 << endl;
			}
		}
	}
	else if (s == 0)
		return  0;
	else
		nextState = StateTrans(failTable[s], c, pos);
	return nextState;
}
void Filter()
{
	for (it = vec.begin(); it != vec.end();)
	{
		if ((*it).lPart[0] == '\0')
		{
			it = vec.erase(it);		
		}
		else
			++it;
	}
}
void Output()
{
	cout << "State Table\t";
	for (int i = 0; i < 20; i++)
		printf("%4d ", i);
	cout << endl;

	cout << "IsFirstChild\t";
	for (int i = 0; i < 20; i++)
		printf("%4d ", stList[i].isFstChild);
	cout << endl;

	cout << "Char Table\t";
	for (int i = 0; i < 20; i++)
		printf("%4c ", stList[i].ch);
	cout << endl;

	cout << "Depth Table\t";
	for (int i = 0; i < 20; i++)
		printf("%4d ", stList[i].depth);
	cout << endl;

	cout << "Check Table\t";
	for (int i = 0; i < 20; i++)
		printf("%4d ", checkTable[i]);
	cout << endl;

	cout << "Base Table\t";
	for (int i = 0; i < 20; i++)
		printf("%4d ", baseTable[i]);
	cout << endl;

	cout << "Next Table\t";
	for (int i = 0; i < 20; i++)
		printf("%4d ", nextTable[i]);
	cout << endl;

	cout << "Fail Func\t";
	for (size_t i = 0; i < 20; i++)
	{
		printf("%4d ", failTable[i]);
	}
	cout << endl;

	cout << "Output Function" << endl;
	for (int i = 0; i < 20; i++)
	{
		size_t stPtnNum = stList[i].ptnList.size();
		if (stPtnNum >0)
		{
			for (outIt = stList[i].ptnList.begin(); outIt != stList[i].ptnList.end(); outIt++)
				cout << "State " << i << ' ' << (**outIt) << ' ';
			cout << endl;
		}
	}
}
int GetNextBlank()
{
	int i = 1;
	while (nextTable[i] != -1)
		i++;
	return i;
}
void OutputNextAndBase()
{
	cout << "state table\t";
	for (int i = 0; i < 20; i++)
		printf("%4d", i);
	cout << endl;

	cout << "Base Table\t";
	for (int i = 0; i < 20; i++)
		printf("%4d", baseTable[i]);
	cout << endl;

	cout << "Next Table\t";
	for (int i = 0; i < 20; i++)
		printf("%4d", nextTable[i]);
	cout << endl;
	cout << "----------------"<<endl;
}
//void PreInit() 
//Fill check table, init the basic info of every state(i.e. depth, whether it is the first child of its parent state,
//the input char). Meanwhile, construct the seperate output pattern set of each final state.
void PreInit()
{
	int stateCount = 0;
	int depth = 1;
	sort(vec.begin(), vec.end(), less<lrPart>());
	while (!vec.empty())
	{
		int prIndex = 0;	
		int listSize;
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (i == 0)
			{
				vec[0].seq.push_back(++stateCount);		// New state found
				stList[stateCount].setBasic(depth, vec[0].lPart[0], 1);
			}
			else if (vec[i].lPart[0] == vec[prIndex].lPart[0] && strcmp(vec[i].rPart, vec[prIndex].rPart)==0)
			{
				vec[i].seq.push_back(stateCount);
			}
			else if (strcmp(vec[i].rPart, vec[prIndex].rPart) == 0)
			{
				vec[i].seq.push_back(++stateCount);
				stList[stateCount].setBasic(depth, vec[i].lPart[0], 0);
			}
			else
			{
				vec[i].seq.push_back(++stateCount);
				stList[stateCount].setBasic(depth, vec[i].lPart[0], 1);
			}
			prIndex = i;

			//Fill in check Table using state-number vector raw by raw(pattern by pattern)
			listSize = vec[i].seq.size();
			if (listSize == strlen(vec[i].lPart) + strlen(vec[i].rPart))
			{
				if (listSize != 1)
				{
					for (int j = 1; j < listSize; j++)
						checkTable[vec[i].seq[j]] = vec[i].seq[j - 1];
					checkTable[vec[i].seq[0]] = 0;
				}
				else
				{
					checkTable[vec[i].seq[0]] = 0;
				}					
			}

			//Construct seperate output function in the form of vector whose element is char(*)[MAXN] 
			if (strlen(vec[i].lPart) == 1)
			{
				stList[stateCount].ptnList.push_back(&patternList[vec[i].oriPos]);
			}
			
		}
		for (size_t i = 0; i < vec.size(); i++)
			vec[i].shift();
		Filter();
		depth++;
	}
}
//void Init()
//Fill base table and next table, set the attribute - stPosInNext of every state.
void Init()
{
	int blankIndex = 1;
	int posTrans;
	stList[0].setPosInNext(0);
	for (int i = 1; stList[i].isFstChild != -1; i++)
	{	
		if (stList[i].isFstChild == 1)
		{
			blankIndex = GetNextBlank();
			nextTable[blankIndex] = i;
			stList[i].setPosInNext(blankIndex);
			baseTable[checkTable[i]] = blankIndex - stList[i].ch - stList[checkTable[i]].stPosInNext; //base[s] = &t - c - &s
			  
			//cout << "i " << i << ' ' << checkTable[i] << endl;
			//cout << blankIndex << ' ' << stList[i].ch << ' ' << stList[checkTable[i]].stPosInNext << endl;
		}
		else
		{
			posTrans = blankIndex + stList[i].ch - stList[i-1].ch ;
			nextTable[posTrans] = i;
			stList[i].setPosInNext(posTrans);
		}
		//OutputNextAndBase();
	}
	for (int i = 1; stList[i].isFstChild != -1; i++)
	{
		failTable[i] = Fail(i);
	}
}

void Query()
{
	fstream textIn;
	textIn.open("TextIn.txt");
	if (textIn.is_open())
	{
		textIn >> text;
	}
	textIn.close();
	char *crtCh = text;
	int crtState = 0;
	int pos = 0;
	while (*crtCh != '\0')
	{
		crtState = StateTrans(crtState, *crtCh, pos);
		crtCh++;
		pos++;
	}
	textIn.close();
}
int CheckFillRatio()
{
	int sum = 0;
	for (size_t i = 0; i < MAXN; i++)
	{
		if (nextTable[i] != -1)
			sum++;
	}
	return sum * 1.0 / MAXN * 100;
}
int main()
{
#ifdef READ_FILE
	freopen("DoubleArray-Trie-In.txt", "r", stdin);
	freopen("DoubleArray-Trie-out.txt", "w", stdout);
#endif
	memset(checkTable, -1, sizeof(checkTable));
	memset(nextTable, -1, sizeof(nextTable));
	memset(baseTable, -1, sizeof(baseTable));
	int patternNum;
	scanf("%d", &patternNum);
	for (int i = 0; i < patternNum; i++)
	{
		lrPart tmp;
		scanf("%s", &patternList[i]);
		tmp.init(patternList[i],i);
		tmp.Print();
		vec.push_back(tmp);
	}
	PreInit();
	Init();
	cout << "Contents of tables" << endl;
	Output();
	cout <<endl;
	float fillR = CheckFillRatio();
	cout << "The fill ratio of next table is " << fillR << "%." << endl << endl;
	cout << "Qury result " << endl ;
	Query();
	return 0;
}