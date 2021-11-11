#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define AZ 26
#define MAXSIZE 100
#define MAX_NUM 0xFFFFF

unsigned char size[1048576]; //가상 메모리 저장 변수

int EndStore  = -1, historycount = 1; //dump에서 마지막으로 저장된 end 값, history number 값 저장 변수 
int StartAddress = 0, length = 0, symswitch = 0, opNum; //시작 주소, 길이 등 저장 변수 

struct dirent *Dirfile;
struct stat DirStat;

//history linked list 저장 구조체 
typedef struct _NAMENODE
{		
	char namenodename[100]; //이름 저장 변수 
	struct _NAMENODE *nNode; //다음 노드 link
}NAMENODE;
NAMENODE *HNode = NULL;	

//opcode hash table 저장 구조체 
typedef struct _HASHNODE
{
	int opcode;	//opcode 저장 변수
	char hashnodename[10], hashformat[5]; 
	struct _HASHNODE *nNode; //다음 노드 link
} HASHNODE;
HASHNODE *OPHnode[20];

//symbol table 저장 구조체 
typedef struct _SYMNODE
{
	int locctr;	//위치 저장 변수. 
	char symnodename[100];	//symbol 이름 저장 변수 
	struct _SYMNODE *nNode;	//다음 노드 link
}SYMNODE;
SYMNODE *SHnode[26];

//obj code linkedlist 저장 구조체 
typedef struct _OBJNODE
{		
	char objcode[100];	//이름 저장 변수 
	int OBJcnt;	
	int OBJswtich, OBJswitch2;	
	struct _OBJNODE *nNode; //다음 노드 link
}OBJNODE;
OBJNODE *OHnode = NULL;	

void FuncMakeOPlist();
void FuncAddHistory(char*);
void FuncLoadName(char*);
void FuncAddObjlist(char*, int, int);

int FuncSymbol();
int FuncDump(char*);
int FuncEdit(char*);
int FuncFill(char*);
int FuncHash(char*);
int FuncOpcode(char*);
int FuncType(char*);
int FuncAssemble(char*);
int FuncPass1(char*);
int FuncPass2(char*);






