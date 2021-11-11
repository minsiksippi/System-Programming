#include "20171609.h"

int main()
{

	FuncMakeOPlist(); //opcode.txt를 이용하여 opcode list 해시테이블 생성 함수 호출

	NAMENODE *Hfreetmp, *minsiktmp1, *HistoryNode; //history linked list  에 사용되는변수
	HASHNODE *Ofreetmp, *minsiktmp2; //opcode의 linked list  변수 
	SYMNODE *SyPointer, *SyTmptr; //symbol  linked list에 사용되는 변수
	OBJNODE *OBJPointer, *OBJtmptr;
	DIR *Dirpointer;

	int i;
	char input1[100], input2[100], input3[100];
	//input1 : 처음으로 입력 받는 명령어
	//input2 : 명령어 부분만 입력받는 변수
	//input3 : 명령어 뒷부분 저장하는 변수

	while (1)
	{
		printf("sicsim> ");

		//변수 초기화
		memset(input1, '\0', sizeof(input1));
		memset(input2, '\0', sizeof(input2));
		memset(input3, '\0', sizeof(input3));

		fgets(input1, sizeof(input1), stdin); //명령어 입력 
		int mm1 = 0, mm2 = 0, inttmp = 0;
		input1[(unsigned)strlen(input1) - 1] = '\0';
		while (input1[mm1] == '\t' || input1[mm1] == ' ') mm1 = mm1 + 1; //공백 혹은 탭인 문자열 index 저장

		//공백이나 NULL 값이 나오지 전까지 값을 저장해준다. 즉 명령어 부분만 저장시킨다.
		while (input1[mm1] != '\t' && input1[mm1] != '\0' && input1[mm1] != ' ')
		{
			input2[mm2] = input1[mm1];
			mm1 = mm1 + 1;
			inttmp = mm1;
			mm2 = mm2 + 1;
			inttmp = mm2;
		}

		//명령어 뒤에 다른 문이 있을 경우
		if (input1[mm1] != '\0')
		{	
			mm2 = 0;
			while (input1[mm1] == '\t' || input1[mm1] == ' ') mm1 = mm1 + 1;
			while (input1[mm1] != '\0') //NULL이 나오기 전까지 인덱스를 저ㅈ
			{	
				input3[mm2] = input1[mm1];
				mm1 = mm1 + 1;
				inttmp = mm1;
				mm2 = mm2 + 1;
				inttmp = mm2;
			}
		}
		inttmp = inttmp + 1;

		//명령어가 h, help일 때
		if (strcmp(input2, "h")==FALSE || strcmp(input2, "help")==FALSE)
		{
			if (input3[0] == '\0') 
			{
				FuncAddHistory(input1); //명령어 history에 추가
				printf("h[elp]\n");
				printf("d[ir]\n");
				printf("q[uit]\n");
				printf("hi[story]\n");
				printf("du[mp] [start, end]\n");
				printf("e[dit] address, value\n");
				printf("f[ill] start, end, value\n");
				printf("reset\n");
				printf("opcode mnemonic\n");
				printf("opcodelist\n");
				printf("assemble filename\n");
				printf("type filename\n");
				printf("symbol\n");
			}
			else printf("Invalid Command\n"); //인덱스가 비어있지 않으면 에러 출력
		}
		else if (strcmp(input2, "q")==FALSE || strcmp(input2, "quit")==FALSE) // q, quit일 경ㅇ
		{
			if (input3[0] == '\0') break; //반복문 종료
			else printf("Invalid Command\n");
		}
		else if (strcmp(input2, "d")==FALSE || strcmp(input2, "dir")==FALSE) //d, dir인 경우
		{
			if (input3[0] == '\0')
			{
				int cnt_dir = 0;
				FuncAddHistory(input1); //명령어 history에 추가

				Dirpointer = opendir(".");	
				while ((Dirfile = readdir(Dirpointer)) != NULL)
				{
					stat(Dirfile->d_name, &DirStat);	
					printf("\t%s", Dirfile->d_name);
					if (S_ISDIR(DirStat.st_mode)) printf("/"); //폴더인 경우 /추가
					else if (S_IXUSR & DirStat.st_mode) printf("*"); //실행파일인 경우 *추가
					cnt_dir = cnt_dir + 1;
					if (cnt_dir % 5 == 0) printf("\n");
				}
				if (cnt_dir % 5 != 0) printf("\n");
				closedir(Dirpointer);
			}
			else printf("Invalid Command\n");
		}
		else if (strcmp(input2, "hi")==FALSE || strcmp(input2, "history")==FALSE) //hi, history 입력시
		{
			if (input3[0] == '\0')
			{
				FuncAddHistory(input1); //명령어 history에 추가
				historycount = 1; //history 개수 나타내는 변수
				
				HistoryNode = HNode;
				while (HistoryNode != NULL)
				{		
					printf("\t%-5d%s\n", historycount, HistoryNode->namenodename);
					historycount = historycount + 1;
					HistoryNode = HistoryNode->nNode;
				}	
			}
			else printf("Invalid Command\n");
		}
		else if (strcmp(input2, "du")==FALSE || strcmp(input2, "dump")==FALSE) //Du, dump인 경우
		{
			if (FuncDump(input3) == TRUE) FuncAddHistory(input1); //명령어 history에 추가 
		}
		else if (strcmp(input2, "f")==FALSE || strcmp(input2, "fill")==FALSE) //f, fill 인 경우
		{
			if (FuncFill(input3) == TRUE) FuncAddHistory(input1); //명령어 history에 추가 
		}
		else if (strcmp(input2, "e")==FALSE || strcmp(input2, "edit")==FALSE) //e, edit인 경우  
		{
			if (FuncEdit(input3) == TRUE) FuncAddHistory(input1); //명령어 history에 추가 
		}
		else if (strcmp(input2, "reset")==FALSE) //reset인 경우
		{	
			if (input3[0] == '\0')
			{
				FuncAddHistory(input1);	//명령어 history에 추가 
				memset(size, 0, sizeof(size)); //초기화
			}
			else printf("Invalid Command\n");
		}
		else if (strcmp(input2, "opcode")==FALSE)
		{	
			if (FuncOpcode(input3) == TRUE) FuncAddHistory(input1);	//명령어 history에 추가
		}
		else if (strcmp(input2, "opcodelist")==FALSE)
		{
			if (input3[0] == '\0')
			{
				FuncAddHistory(input1); //명령어 history에 추가

				HASHNODE *OPHashptr; //linked list를 탐색할 Pointer
				for (int op_i = 0; op_i < 20; op_i++)
				{
					printf("%d : ", op_i);
					OPHashptr = OPHnode[op_i];
					if (OPHashptr) //opcodelist의 모든 linked list를 출력한다. 
					{
						while (OPHashptr -> nNode != NULL)
						{
							printf("[%s,%02X] -> ", OPHashptr->hashnodename, OPHashptr->opcode);
							OPHashptr = OPHashptr->nNode;
						}
						printf("[%s,%02X]", OPHashptr->hashnodename, OPHashptr->opcode);
					}
					printf("\n");
				}			
			}
			else printf("Invalid Command\n");
		}
		else if (strcmp(input2, "assemble")==FALSE) //assemble 명령어 입력시
		{
			if (FuncAssemble(input3) ==TRUE) FuncAddHistory(input1); //명령어 history에 추가
		}
		else if (strcmp(input2, "symbol")==FALSE) //symbol 명령어 입력 
		{
			if (input3[0] == '\0')
			{	
				if (FuncSymbol() == TRUE) FuncAddHistory(input1); //명령어 history에 추가
			}
			else printf("Invalid Command\n");
		}
		else if (strcmp(input2, "type")==FALSE) //type 명령어 입력
		{
			if (FuncType(input3) == TRUE) FuncAddHistory(input1); //명령어 history에 추가
		}
		
		else printf("Invalid Command\n"); // 위의 명령어를 제외한 어떤 입력이 들어오면 에러 출력
	}

	//history의 Linked list free
	Hfreetmp = HNode;
	while (Hfreetmp != NULL)
	{
		minsiktmp1 = Hfreetmp;
		Hfreetmp = Hfreetmp -> nNode;
		free(minsiktmp1);
	}

	//objcode Linked list free
	OBJPointer = OHnode;
	while (OBJPointer)
	{
		OBJtmptr = OBJPointer;
		OBJPointer = OBJPointer -> nNode;
		free(OBJtmptr);
	}
	return FALSE;

	//sybol table의 Linked list free
	for (i = 0; i < AZ; i++)
	{
		SyPointer = SHnode[i];
		while (SyPointer)
		{
			SyTmptr = SyPointer;
			SyPointer = SyPointer -> nNode;
			free(SyTmptr);
		}
		SHnode[i] = NULL;
	}

	//opcode list의 linked list free
	for (i = 0 ; i < 20 ; i++)
	{
		Ofreetmp = OPHnode[i];
		while (Ofreetmp != NULL)
		{
			minsiktmp2 = Ofreetmp;
			Ofreetmp = Ofreetmp->nNode;
			free(minsiktmp2);
		}
	}
}

//입력된 명령어의 history를 저장시켜주는 함수
void FuncAddHistory(char* T)
{
	NAMENODE *HisNode = (NAMENODE*)malloc(sizeof(NAMENODE)); //명령어를 저장하는 노드 
	NAMENODE *Hisptr = HNode; //linked list 탐색에 쓰이는 노드

	int cntHistory = 0;
	//node에 입력받는 명령어를 저장한다. 
	strcpy(HisNode->namenodename, T);
	HisNode->nNode = NULL;

	//노드의 헤드가 NULL이면 head를 할당 후 새로운 노드를 저장한다. 만약 NULL이 아니면 새 노드를 연결할 노드까지 이동시켜 연결한다. 
	if (HNode == NULL)
	{
		HNode = (NAMENODE*)malloc(sizeof(NAMENODE));
		HNode = HisNode;
		cntHistory = cntHistory + 1;
	}
	else
	{	
		while (Hisptr->nNode != NULL) Hisptr = Hisptr->nNode;	
		Hisptr-> nNode = HisNode;
	}
}

//dump명령시 실행되는 함수 
int FuncDump(char* T)
{
	int start, end, i = 0, j, Dmpcnt = ERROR;  // start, end: 메모리 시작, 마지막 값을 나타내는 변수, Dmpcnt : comma의 위치를 나타내는 변수
	int k, dumpstart = 0;
	char Dmptmp[MAXSIZE];	
	//comma의 위치 저장
	while (T[i] != '\0')
	{
		if (T[i] == ',')
		{
			Dmpcnt = i;
			break;
		}
		i = i + 1;
	} 

	//comma가 존재한다면 
	if (Dmpcnt >= 0)
	{
		if (sscanf(T, "%x \t , \t %x %s", &start, &end, Dmptmp) != 2)
		{
			//parameter가 두개 존재하지 않거나 16진수가 아니면 에러 출력  
			for(k = 0; k < 16; k++)
			{
				dumpstart = Dmpcnt;
            }
			printf("Invalid Command(Parameter)\n");
			return FALSE;
		}
	}
	else //comma가 존재하지 않는다면
	{ 
		if (T[0] == '\0') //dump 뒤에 들어온 변수가 없으면 
		{ 
			start = EndStore + 1;
			end = start + 159;
		}
		else 
		{
			if (sscanf(T, "%x %s", &start, Dmptmp) != 1)
			{
				//parameter의 개수가 한개가 아니거나 16진수가 아니면 에러 출력. 
				for(k = 0; k < 16; k++)
				{
					dumpstart = dumpstart + 1;
				}
				printf("Invalid Command(Parameter)\n");
				return 0;
			}
			end = start + 159;
		}
	}

	//시작과 끝 위치가 지정한 범위를 벗어나는 경우 에러 출력. 
	if (end > MAX_NUM) end = MAX_NUM; 
	if (start < 0 || end < 0 || start > MAX_NUM || end > MAX_NUM || start > end)
	{
		printf("Invalid Range\n");
		return FALSE;
	}

	//메모리 주소값을 16진수로 표현하여 출력한다. 
	for (i = start / 16 * 16; i <= end / 16 * 16; i = i + 16)
	{
		printf("%05X ", i);	
		for (j = i; j < i + 16; j++)
		{	
			if (j >= start && j <= end) printf("%02X ", size[j]);
			else printf("   ");
		}
		printf("; ");

		//메모리값을 아스키 값으로 표현한다. 그 이외의 값은 .으로 출력하여 나타낸다. 
		for (j = i; j < i + 16; j++)
		{	
			if(j >= start && j <= end && size[j] >= 0X20 && size[j] <= 0x7E) putchar(size[j]);
			else putchar('.');
		}
		printf("\n");
	}

	EndStore = end;	

	if (EndStore >= MAX_NUM) EndStore = ERROR;

	return TRUE;
}

//메모리 주소값을 변경해주는 함수 
int FuncEdit(char* location)
{
	int Editloc = ERROR, i = 0;	//comma 위치 나타내는 변수.
	int EditAdr, Editval, cntEdit = 0, cntError = 0; //edit 함수에서 주소, 메모리를 나타내는 변수. 
	char Edittmp[MAXSIZE]; //tmp 변ㅅ

	//comma의 위치 읽는다. 
	while (location[i] != '\0')
	{
		if (location[i] == ',')
		{
			Editloc = i;
			cntEdit = cntEdit + 1;
			break;
		}
		i = i + 1;
	}

	//comma가 존재하지 않으면 에러를 출력한다.
	if (Editloc == ERROR)
	{ 
		printf("Invalid Command(Parameter)\n");
		return FALSE;
	}
	else
	{
		if (sscanf(location, "%x \t , \t %x %s", &EditAdr, &Editval, Edittmp) != 2)
		{
			/*
			comma가 존재하면, parameter의 개수가 2개가 아니거나 16진수가 아니면 에러를 출력한다.
			*/
			printf("Invalid Command(Parameter)\n");
			for(int q = 0; q < 10; q++)
			{
				cntError = cntError + 1;
            }
			return FALSE;
		}
	}

	//주소값과 value값이 지정된 범위를 벗어나게 되는 경우 에러를 출력한다. 
	if (EditAdr < 0x00000 || EditAdr > MAX_NUM)
	{
		printf("Invalid Range\n");
		for(int q = 0; q < 10; q++)
		{
			cntError = cntError + 1;
        }
		return FALSE;
	}

	if (Editval < 0 || Editval > 0xFF ) 
	{
		printf("Invalid Range\n");
		for(int q = 0; q < 10; q++)
		{
			cntError = cntError + 1;
        }
		return FALSE;
	}

	size[EditAdr] = Editval;
	return TRUE;
}

//지정된 범위에 입력된 value값을 저장시켜주는 함수 
int FuncFill(char* location)
{
	int Fillloc = ERROR, Fillloc2 = ERROR; //comma 위치 변수
	int i = 0, start, end, value; //메모리 주소의 시작, 끝, value 값 저장 변수
	char Filltmp[MAXSIZE]; // tmp 변수

	//comma 위치 탐색 
	while (location[i] != '\0')
	{		
		if (location[i] == ',')
		{
			Fillloc = i;
			break;
		}
		i = i + 1;
	}
	i = i + 1;

	while (location[i] != '\0')
	{	
		if (location[i] == ',')
		{
			Fillloc2 = i;
			break;
		}
		i = i + 1;
	}

	//commaRㅏ 2개가 아니면 에러를 출력한다. 
	if (Fillloc == ERROR || Fillloc2 == ERROR) 
	{	
		printf("Invalid Command(Parameter)\n");
		return FALSE;
	}

	//입력받은 parameter값이 3개가 아니면 에러를 출력한다. 
	if (sscanf(location, "%x \t , \t %x \t , \t %x %s", &start, &end, &value, Filltmp) != 3)
	{
		printf("Invalid Command(Parameter)\n");
		return FALSE;
	}

	//시작과 끝 주소값, 입력받은 value	값이 지정된 범위를 벗어나게 되면 에러 출력. 
	if (start > end || end < 0 || start < 0 || end > MAX_NUM || start > MAX_NUM)
	{
		printf("Invalid Range\n");
		return FALSE;
	}
	if (value < 0 || value > 0xFF)
	{
	
		printf("Invalid Range\n");
		return FALSE;
	}

	for (i = start; i <= end; i++) size[i] = value;	// 메모리에 입력받은 값 저장한다. 
	return TRUE;
}

//opcodelist를 만드는 함수 
void FuncMakeOPlist()
{
	FILE *OPfp = fopen("opcode.txt", "r");
	char OPinput1[10], OPformat[5];
	int MOPnum = 0;

	//file의 포인터가 NULL이면 에러 출력. 
	if (OPfp == NULL)
	{
		printf("Opcode list file is not exist.\n");
		return;
	}
	
	while (1)
	{
		if (fscanf(OPfp, "%x %s %s", &opNum, OPinput1, OPformat) == EOF) break;	
		//파일 정보 저장 
		HASHNODE *HHNode = (HASHNODE*)malloc(sizeof(HASHNODE));//정보를 저장하는 노드 변수 
		HHNode->opcode = opNum;

		strcpy(HHNode->hashnodename, OPinput1);
		strcpy(HHNode->hashformat, OPformat);
		HHNode->nNode = NULL;
		if (OPHnode[FuncHash(OPinput1)] == NULL)
		{
			MOPnum = opNum;
        }

		//hash function 암호값으로 변환하고 새로운 노드를 linked list에 연결시킨다. 
		if (OPHnode[FuncHash(OPinput1)] == NULL) OPHnode[FuncHash(OPinput1)] = HHNode;	
		else
		{					
			HASHNODE *OPHpointer = OPHnode[FuncHash(OPinput1)];
			while (OPHpointer->nNode != NULL) OPHpointer = OPHpointer->nNode;
			OPHpointer->nNode = HHNode;
		}
		MOPnum = MOPnum + 1;
	}
	fclose(OPfp);
}

//hash 함수를 return 시키는 함수.
int FuncHash(char* inst)
{
	return (inst[0] + inst[strlen(inst) - 1] + strlen(inst)) % 20;
}

//opcode 검색 시 알려주는 함수 
int FuncOpcode(char* idx)
{
	HASHNODE *HOPptr; //opcode linked list 탐색할 때 사용하는 노ㄷ 변수
	
	char OPtmp[MAXSIZE], OPinst[10]; //opcode 검색시 사용되는 명령어 저장 변수. 

	//parameter가 1개가 아니라면 에러를 출력한다. 
	if (sscanf(idx, "%s %s", OPinst, OPtmp) != TRUE)
	{
		printf("Invalid Command(Mnemonic)\n");
		return FALSE;
	}

	HOPptr = OPHnode[FuncHash(OPinst)];

	//입력받은 명령어를 linked list에서 검색하고, 찾으면 반복문을 멈춘다. 
	while (HOPptr != NULL)
	{
		if (strcmp(HOPptr->hashnodename, OPinst)==FALSE) break;
		HOPptr = HOPptr->nNode;
	}

	//없으면 에러 출력하고 찾으면 결과를 출력해준다. 
	if (HOPptr == NULL)
	{
		printf("Invalid Command(Mnemonic)\n");
		return FALSE;
	}
	printf("opcode is %02X\n", HOPptr->opcode);
		
	return TRUE;
}

//파일 이름 불러오는 함수. 
void FuncLoadName(char *T)
{
	int funidx = 0, tmptmp, funcswitch = FALSE;	//공백 제거 필요 유뮤 스위치 변수
	while (T[funidx] != '\0')
	{
		//입력 된 문자의 끝에 NULL인지 확인한다. 
		if (funcswitch == FALSE)
		{
			if (T[funidx] == ' ' || T[funidx] == '\t')
			{
				funcswitch = TRUE;
				tmptmp = funidx;
			}
		}
		else if (funcswitch == TRUE)
		{
			if (T[funidx] != ' ' && T[funidx] != '\t') funcswitch = FALSE;
		}

		funidx = funidx + 1;
	}

	//만약 공백제거가 필요하다면 공백을 제거한다. 
	if (funcswitch == TRUE)
	{
		while (T[tmptmp] != '\0')
		{
			T[tmptmp] = '\0';
			tmptmp = tmptmp + 1;
		}
	}
}

//type 입력시 실행되는 함수 
int FuncType(char *name)
{
	FILE *fp;
	char typetmp;
	//파일을 불러들인다. 
	FuncLoadName(name);
	fp = fopen(name, "r");

	//만약 파일이 없다면 에러를 출력하고 종료한다. 
	if (!fp)
	{
		printf("No File In This Directory\n");
		return FALSE;
	}
	//파일 내용을 출력한다.
	while (1)
	{
		typetmp = fgetc(fp);
		if (typetmp == EOF) break;
		printf("%c", typetmp);
	}

	fclose(fp);
	return TRUE;
}


void FuncAddObjlist(char* objcode, int location, int swwitch)
{
	OBJNODE *OBJNnode = (OBJNODE*)malloc(sizeof(OBJNODE));
	OBJNODE *OBJPtrnode = OHnode;

	int i, ObjCnt=0;

	strcpy(OBJNnode->objcode, objcode);

	OBJNnode->nNode = NULL;
	OBJNnode->OBJcnt = location;
	OBJNnode->OBJswtich = FALSE;
	OBJNnode->OBJswitch2 = FALSE;

	if (OHnode == NULL)
	{
		OHnode = (OBJNODE*)malloc(sizeof(OBJNODE));
		OHnode = OBJNnode;
		for(i = 0; i < 10; i++)
		{
			ObjCnt = ObjCnt + 1;
        }
		if (swwitch == TRUE) OBJNnode->OBJswitch2 = TRUE;
	}
	else
	{	
		while (OBJPtrnode->nNode != NULL) OBJPtrnode = OBJPtrnode->nNode;
		OBJPtrnode-> nNode = OBJNnode;	
		if ((OBJNnode->OBJcnt) - (OBJPtrnode->OBJcnt) >= 4096) OBJNnode->OBJswtich = TRUE; 
		if (swwitch == TRUE) OBJNnode->OBJswitch2 = TRUE;
	}
}

//assemble 입력시 실행되는 함수. 
int FuncAssemble(char *name)  
{
	FILE *fp;
	SYMNODE *Assemptr, *Assemptrtmp; //symbol table 사용되는 노드 변수
	OBJNODE *AssemObjptr, *Assemobjtmp; //objcode linked list 노드 변수

	int i = 0;
	char Assemtmp[MAXSIZE], Assemfile[MAXSIZE];
	
	memset(Assemfile,'\0',sizeof(Assemfile)); //초기화

	FuncLoadName(name); //파일 오픈함수 호출
	//파일 없으면 에러 출력
	fp = fopen(name, "r");
	if (!fp)
	{
		printf("No File In This Directory\n");
		return FALSE;
	}

	//symbol table free시킨다
	for (i = 0; i < AZ; i++)
	{
		Assemptr = SHnode[i];
		while (Assemptr)
		{
			Assemptrtmp = Assemptr;
			Assemptr = Assemptr -> nNode;
			free(Assemptrtmp);
		}
		SHnode[i] = NULL;
	}
	symswitch = FALSE;

	//objcode list free 
	AssemObjptr = OHnode;
	while (AssemObjptr)
	{
		Assemobjtmp = AssemObjptr;
		AssemObjptr = AssemObjptr -> nNode;
		free(Assemobjtmp);
	}
	OHnode = NULL;

	i = 0;
	while (1)
	{
		Assemtmp[i] = name[i];
		i = i + 1;
		if (name[i] == '.')	break; 
		
	}
	//.asm 파일인지 확인한다. asm이 아니면 에러메세지 출력
	i = i + 1;
	if (strcmp(name+i, "asm"))
	{
		printf("Invalid File\n"); 
		return FALSE;	
	}
	//pass1, pass2에서 오류가 발생한다면 파일 삭제 후 종료한다. 
	if (FuncPass1(name) == FALSE)
	{
		strcpy(Assemfile, Assemtmp);
		strcat(Assemfile, ".lst");
		remove(Assemfile);
		memset(Assemfile, '\0', sizeof(Assemfile));
		strcpy(Assemfile, Assemtmp);
		strcat(Assemfile, ".obj");
		remove(Assemfile);
		remove("sp_mid");
		return FALSE;
	}
	remove("sp_mid");

	//오류가 발생하지 않는다면 파일명을 출력해준다.
	printf("\t[%s.lst], [%s.obj]\n", Assemtmp, Assemtmp);
	symswitch = TRUE;

	fclose(fp);
	return TRUE;
}

//symbol 입력시 호출되는 함수
int FuncSymbol()
{
	SYMNODE *SymbolcmdNode; //symbol table 노드 변수
	int q = 0;

	if (symswitch == TRUE) //symbol table이 만들어져 있다면
	{
		//출력한다. 
		while (q < AZ)
		{
			SymbolcmdNode = SHnode[q];
			while (SymbolcmdNode)
			{
				printf("\t%s\t%04X\n", SymbolcmdNode->symnodename, SymbolcmdNode->locctr);
				SymbolcmdNode = SymbolcmdNode -> nNode;
			}
			q = q + 1;	
		}
	}
	else  //만들어져있지 않아면 에러 출력
	{
		printf("There is not exist Symbol File\n");
		return FALSE;
	}
	return TRUE;
}

int FuncPass1(char *name)
{
	StartAddress = 0, length = 0; //시작 주소와 길이 저장 변수. 
	FILE *fppass1, *fppass;
	SYMNODE *PassPtr, *PassPreNode, *PassCurNode;
	HASHNODE *PassHPtr;

	fppass1 = fopen(name, "r");
	fppass = fopen("sp_mid", "w");

	int locctr = 0, LineNum = 5, PassSwitch = FALSE, CommaLoc = ERROR; //컴마의 위치 변수, 문자의 위치 변수
	int Passidx, Passtmp = 0, PasstmpNum, Passtmptmp = 1, PassError = 0, i = 0, j = 0, k = 0;
	int SL1, SL2, SL3, SL4; //각 문자열의 크기를 저장하는 변수 
	char cmdLine[300]; //command line 저장 변수. 
	char line1[MAXSIZE], line2[MAXSIZE], line3[MAXSIZE], line4[MAXSIZE], PassTemp[MAXSIZE]; //문자열 입력 저장 변수
	//line1 : label, line2 : directive, line3 : operand, line4 : second operand
	
	fgets(cmdLine, sizeof(cmdLine), fppass1); //한 줄 읽고 저장

	//저장된 문장에서 각각의 인자를 받고, 그 크기를 따로 저장한다. 
	Passtmp = sscanf(cmdLine, "%s %s %x %s", line1, line2, &StartAddress, PassTemp);
	SL1 = sizeof(line1);
	SL2 = sizeof(line2);
	SL3 = sizeof(line3);
	SL4 = sizeof(line4);

	if (Passtmp)
	{
		//문장에서 start가 있고 정상이라면 출력하고, 아니면 에러를 출력한다. 
		if (strcmp(line2, "START")==FALSE)
		{
			if (Passtmp == 3)
			{
				j = j + 1;
				locctr = StartAddress;
				fprintf(fppass, "%d\t%d\t", LineNum, locctr);
				fprintf(fppass, "%s", cmdLine);
				Passtmptmp = Passtmptmp + 5;
				LineNum = LineNum + 5;
			}
			else
			{
				printf("%d: Error: Start Line Error\n", LineNum);
				return FALSE;
			}
		}
		else
		{
			fclose(fppass1);
			fppass1 = fopen(name, "r");
		}
	}
	memset(cmdLine, '\0', sizeof(cmdLine));

	while (fgets(cmdLine, sizeof(cmdLine), fppass1) != NULL) //끝까지 읽기
	{
		memset(line1, '\0', SL1);
		memset(line2, '\0', SL2);
		memset(line3, '\0', SL3);
		memset(line4, '\0', SL4);
		memset(PassTemp, '\0', sizeof(PassTemp));
		//모든 문장 저장 변수를 초기화 해준다.
		
		Passtmp = 0;
		CommaLoc = ERROR;

		/* .과 같은 주석일 경우 */
		if (cmdLine[0] == '.')
		{
			fprintf(fppass, "%d\t%s", LineNum, cmdLine);
			LineNum = LineNum + 5;
		}
		else //주석이 아니라면
		{
			//comma의 유무를 확인하여 저장한다.
			i = 0;
			while (cmdLine[i] != '\0')
			{
				if (cmdLine[i] == ',')
				{
					CommaLoc = i;
					break;
				}
				i = i + 1;
			} 


			if (cmdLine[0] == '\t' || cmdLine[0] == ' ')
			{
				if (CommaLoc != ERROR) //comma가 존재한다면
				{
					Passtmp = sscanf(cmdLine, "%s \t %s \t %s \t %s", line2, line3, line4, PassTemp);
					//command line 저장
					if (cmdLine[CommaLoc+1] != '\t' && cmdLine[CommaLoc+1] != ' ')
					{
						if (cmdLine[CommaLoc -1] != '\t' && cmdLine[CommaLoc -1] != ' ')
						{
							k = 0;
							while (line3[k] != ',') k = k + 1;
							k = k + 1;
							strcpy(line4, line3+k);
							Passtmptmp = Passtmptmp + 5;
							Passtmp = Passtmp + 1;
						}
					}
					else if (line4[0] == ',')
					{
						if (strlen(line4) == 1)
						{
							strcpy(line4, PassTemp);
							Passtmp = Passtmp - 1;
						}
						else
						{
							strcpy(line4, line4+1);
							line4[strlen(line4) - 1] = '\0';
						}
					}

					if (Passtmp != 3) //인자가 3개가 아니면 에러를 출력한다.
					{
						printf("%d: Error: Wrong Input in Command Line\n", LineNum);
						return FALSE;
					}
					Passtmp = Passtmp - 1;
				}
				else //comma가 없다면
				{
					Passtmp = sscanf(cmdLine, "%s \t %s \t %s", line2, line3, PassTemp);
					if (Passtmp != 2)
					{
						if (Passtmp == 1 && strcmp(line2, "RSUB")==FALSE) Passtmp = 1;
						else
						{
							printf("%d: Error: Wrong Input in Command Line\n", LineNum);
							return FALSE;
						}
					}
				}
			}
			else if (cmdLine[0] <= '9' && cmdLine[0] >= '0')
			{
				printf("%d: Error: First Char Cannot be a Number\n", LineNum);
				return FALSE;
			}
			//Label이 입력되었을 경우
			else
			{
				if (CommaLoc != ERROR) //컴마가 존재한다면
				{
					Passtmp = sscanf(cmdLine, "%s \t %s \t %s \t %s \t %s", line1, line2, line3, line4, PassTemp);
					//command line 입력
					if (cmdLine[CommaLoc+1] != ' ' && cmdLine[CommaLoc+1] != '\t')
					{
						if(cmdLine[CommaLoc-1] != '\t' && cmdLine[CommaLoc-1] != ' ')
						{
							k = 0;
							while (line3[k] != ',') k = k + 1;
							k = k + 1;
							strcpy(line4, line3+k);
							for(int y = 0; y < 10; y++) Passtmptmp = Passtmptmp + 5;
							Passtmp = Passtmp + 1;
						}
					}
					else if (line4[0] == ',')
					{
						if (strlen(line4) == 1)
						{
							strcpy(line4, PassTemp);
							Passtmp = Passtmp - 1;
						}
						else
						{
							strcpy(line4, line4 + 1);
							for(int y = 0; y < 10; y++) Passtmptmp = Passtmptmp + 5;
							line4[strlen(line4) - 1] = '\0';
						}
					}

					if (Passtmp != 4) //인자가 4개가 아니면 에러 출력 
					{
						printf("%d: Error: Wrong Input in Command Line\n", LineNum);
						for(int y = 0; y < 10; y++)
						{
							PassError = PassError + 1;
                        }
						return FALSE;
					}
					Passtmp = Passtmp - 1;
				}
				else
				{
					Passtmp = sscanf(cmdLine, "%s \t %s \t %s \t %s", line1, line2, line3, PassTemp);
					if (Passtmp != 3)
					{
						printf("%d: Error: Wrong Input in Command Line\n", LineNum);
						for(int y = 0; y < 10; y++)
						{
							PassError = PassError + 1;
                        }
						return FALSE;
					}
				}
			}
			
			if (line1[0] != '\0')
			{
				SYMNODE *PassSymNode = (SYMNODE*)malloc(sizeof(SYMNODE)); //symbol table 노드 변수 
				Passidx = (line1[0] - 'A');
				PassPtr = SHnode[(line1[0] - 'A')];
				while (PassPtr)
				{
					if (strcmp(PassPtr->symnodename, line1)==FALSE) break;
					PassPtr = PassPtr -> nNode;
				}

				if (PassPtr) //head가 null이면 head가 새로운 노드가 된다. 
				{
					printf("%d: Error: Duplicate Symbol - %s\n", LineNum, line1);
					for(int y = 0; y < 10; y++)
					{
						PassError = PassError + 1;
                    }
					return FALSE;
				}
				else //head가 null이 아니라면
				{
					
					strcpy(PassSymNode->symnodename, line1);
					PassSymNode->nNode = NULL;
					PassSymNode->locctr = locctr;
					// 내림 차순으로 linked list에 저장
					if (SHnode[(line1[0] - 'A')] == NULL) SHnode[(line1[0] - 'A')] = PassSymNode;
					else
					{
						PassPreNode = SHnode[(line1[0] - 'A')];
						PassCurNode = PassPreNode;
						while (PassCurNode)
						{
							k = 0;
							while (1)
							{
								if (PassCurNode->symnodename[k] != line1[k]) break;
								k = k + 1;
								PassError = PassError - 1;
							}
							if (PassCurNode->symnodename[k] > line1[k]) break;

							PassPreNode = PassCurNode;
							PassCurNode = PassCurNode -> nNode;
						}
						
						if (PassCurNode == PassPreNode)
						{
							SHnode[(line1[0] - 'A')] = PassSymNode;
							SHnode[(line1[0] - 'A')]->nNode = PassCurNode;
							for(int y = 0; y < 10; y++)
							{
								PassError = PassError + 1;
							}
						}
						else
						{
							PassPreNode->nNode = PassSymNode;
							PassSymNode->nNode = PassCurNode;
						}
					}
				}
			}

			if (line2[0] == '+') Passidx = FuncHash(line2+1);
			else Passidx = FuncHash(line2);
			PassHPtr = OPHnode[Passidx];
			while (PassHPtr)
			{
				if (line2[0] != '+')
				{
					if (strcmp(PassHPtr->hashnodename, line2)==FALSE) break;
				}
				else
				{
					if (strcmp(PassHPtr->hashnodename, line2+1)==FALSE) break;
				}
				PassHPtr = PassHPtr -> nNode;
			}

			if (PassHPtr) //directive 존재한다면
			{
				//command line을 읽고, 출력한다.
				if (line1[0] != '\0') fprintf(fppass, "%d\t%d\t%s", LineNum, locctr, cmdLine);
				else fprintf(fppass, "%d\t%d\t-%s", LineNum, locctr, cmdLine);

				//상황에 맞게 위치를 증가시킨다.
				if (strcmp(PassHPtr->hashformat, "1")==FALSE) locctr = locctr + 1;
				else if (strcmp(PassHPtr->hashformat, "2")==FALSE) locctr = locctr + 2;
				else if (line2[0] == '+') locctr = locctr + 4;
				else locctr = locctr + 3;
				LineNum = LineNum + 5; //라인 넘버 증가

			}
			else if (strcmp(line2, "BASE")==FALSE) //directive가 base 라면
			{
				fprintf(fppass, "%d\t-1\t-%s", LineNum, cmdLine);
				for(int y = 0; y < 10; y++) Passtmptmp = Passtmptmp + 5;
				LineNum = LineNum + 5; //라인 넘버 증가
			}
			else if (strcmp(line2, "END")==FALSE && Passtmp <= 2) //directive가 End 라면
			{
				fprintf(fppass, "%d\t-1\t-%s", LineNum, cmdLine);
				LineNum = LineNum + 5; //라인 넘버 증가
				PassSwitch = TRUE;
				for(int y = 0; y < 10; y++) Passtmptmp = Passtmptmp + 5;
				break;
			}
			else if (strcmp(line2, "RESW")==FALSE && Passtmp == 3)  //directive가 resw 라면
			{
				fprintf(fppass, "%d\t%d\t%s", LineNum, locctr, cmdLine);
				LineNum = LineNum + 5; //라인 넘버 증가
				locctr += 3 * atoi(line3);
			}
			else if (strcmp(line2, "BYTE")==FALSE && Passtmp == 3) //directive가 byte 라면 
			{
				fprintf(fppass, "%d\t%d\t", LineNum, locctr);
				if (line3[0] == 'C') locctr += strlen(line3) -3;  //뒤 글자가 C일 경우
				else if (line3[0] == 'X') locctr += (strlen(line3) - 3) / 2; //X일 경우
				else //둘다 아니면 에러 출력
				{
					printf("%d: Error: 'BYTE' command Input Error\n", LineNum);
					for(int y = 0; y < 10; y++)
					{
						PassError = PassError + 1;
                    }
					return FALSE;
				}
				LineNum = LineNum + 5;
				fprintf(fppass, "%s", cmdLine);
			}
			else if (strcmp(line2, "RESB")==FALSE && Passtmp == 3) //directive가 resb 라면
			{
				fprintf(fppass, "%d\t%d\t%s", LineNum, locctr, cmdLine); //출력
				LineNum = LineNum + 5; //라인 넘버 증가
				Passtmptmp = Passtmptmp + 5;
				locctr += atoi(line3);
			}
			else if (strcmp(line2, "WORD")==FALSE && Passtmp == 3) //directive가 word 라면
			{
				fprintf(fppass, "%d\t%d\t%s", LineNum, locctr, cmdLine); //출력한다. 
				locctr = locctr + 3;
				memset(PassTemp, '\0', sizeof(PassTemp));
				Passtmp = sscanf(line3, "%d %s", &PasstmpNum, PassTemp);
				if (Passtmp != TRUE)
				{
					printf("%d: Error: 'WORD' Operand Error - %s\n", LineNum, line3);
					for(int y = 0; y < 10; y++)
					{
						PassError = PassError + 1;
                    }
					return FALSE;
				}
			}
			else //모두 아니면 에러 출력
			{
				printf("%d: Error: Invlid Directive Error - %s\n", LineNum, line2);
				for(int y = 0; y < 10; y++)
				{
					PassError = PassError + 1;
                }
				return FALSE;
			}
		}

		memset(cmdLine, '\0', sizeof(cmdLine)); //초기화
	}
	//end 명령어가 없으면 에러 출력
	if (PassSwitch == FALSE)
	{
		printf("Error: There is Eot Exist END Command \n");
		return FALSE;
	}

	fclose(fppass1);
	fclose(fppass);

	length = locctr - StartAddress;
	
	if (FuncPass2(name) == FALSE) return FALSE; //pass2 함수에서 오류 발생한다면 pass1도 종료. 

	return TRUE;
}


int FuncPass2(char *_filename)
{
	FILE *Pass2fp, *Pass2objfp, *Pass2lstfp;

	HASHNODE *Pass2HPtr; //hash table linked list 변수
	SYMNODE *PassPtr; //symbol table linked lista 변수

	char P2OBJName[MAXSIZE], P2LSTName[MAXSIZE], line[500], line1[MAXSIZE], line2[MAXSIZE], line3[MAXSIZE], line4[MAXSIZE];

	int Pass2idx = 0, Pass2idx2 = 0, tmp = 0, tmp2 = 0, tmp3 = 0, CommaLoc = ERROR;
	int Pass2Adr, Pass2Adr2 = 0, LineNum, locctr = 0, minsikNum = 0;
	int PassByun, PasstmpNum, format, Passidx, PassError2 = 0;
	int Num1, Num2, NumX, NumB, NumP, NumE, NumPass, PSwitch = FALSE; 

	char PassReg[10][2], objcode[10], PassStrtmp[MAXSIZE], PassTemp[MAXSIZE], PassBytetmp[MAXSIZE];
	char PassA[62];

	Pass2Adr = StartAddress;

	//레지스터 값에 각각을 저장 	
	strcpy(PassReg[0], "A"); strcpy(PassReg[1], "X"); strcpy(PassReg[2], "L"); strcpy(PassReg[3], "B"); strcpy(PassReg[4], "S");
	strcpy(PassReg[5], "T"); strcpy(PassReg[6], "F"); strcpy(PassReg[7], " "); strcpy(PassReg[8], "PC"); strcpy(PassReg[9], "SW");

	//각각 변수 초기화 
	memset(P2OBJName, '\0', sizeof(P2OBJName));
	memset(P2LSTName, '\0', sizeof(P2LSTName));

	//파일 열기
	while (_filename[Pass2idx] != '.')
	{
		tmp3 = Pass2idx;
		P2OBJName[Pass2idx] = _filename[Pass2idx];
		P2LSTName[Pass2idx] = _filename[Pass2idx];
		Pass2idx = Pass2idx + 1;
	}

	strcat(P2OBJName, ".obj");
	strcat(P2LSTName, ".lst");

	PSwitch = PSwitch + 1;
	Pass2fp = fopen("sp_mid", "r");
	Pass2objfp = fopen(P2OBJName, "w");
	Pass2lstfp = fopen(P2LSTName, "w");

	fgets(line, sizeof(line), Pass2fp); //불러오기
	tmp = sscanf(line, "%d \t %d \t %s \t %s \t %s", &LineNum, &locctr, line1, line2, line3);
	//각각에 맞게 명령어 저장
	
	 //start 명령어 존재하면 lst파일과 obj 파일을 쓴다. 존재하지 않으면 obj파일의 H부분을 작성한다.
	if (tmp)
	{
		if(tmp && strcmp(line2, "START")==FALSE)
		{
			NumPass = tmp;
        }

		if (strcmp(line2, "START")==FALSE)
		{
			fprintf(Pass2lstfp, "%d\t%04X\t%s\t%s\t%X\n", LineNum, locctr, line1, line2, StartAddress);
			fprintf(Pass2objfp, "H%-6s%06X%06X\n", line1, StartAddress, length);
		}
		else
		{
			fclose(Pass2fp);
			Pass2fp = fopen("sp_mid", "r");
			fprintf(Pass2objfp, "H      %06X%06X\n", StartAddress, length);
		}
	}

	/* lst 파일 출력 */
	while (1)
	{
		Num1 = TRUE, Num2 = TRUE;
		NumX = FALSE, NumB = FALSE, NumP = FALSE, NumE = FALSE;
		//각각변수 초기화 
		memset(line, '\0', sizeof(line));
		memset(line1, '\0', sizeof(line1));
		memset(line2, '\0', sizeof(line2));
		memset(line3, '\0', sizeof(line3));
		memset(line4, '\0', sizeof(line4));
		memset(objcode, '\0', sizeof(objcode));
		memset(PassStrtmp, '\0', sizeof(PassStrtmp));
		memset(PassBytetmp, '\0', sizeof(PassBytetmp));
		memset(PassTemp, '\0', sizeof(PassTemp));
		CommaLoc = ERROR; //comma 위ㅊ
		tmp = 0;
		PassByun = 0;

		fgets(line, sizeof(line), Pass2fp);
		if (line[0] == '\0') break;

		Pass2idx = 0;
		while (line[Pass2idx] != '\0')
		{
			if (line[Pass2idx] == ',')
			{
				CommaLoc = Pass2idx;
				break;
			}
			Pass2idx = Pass2idx + 1;
		}
		//comma 유무를 확인한다.

		if (CommaLoc == ERROR) //콤마 존재하지 않으면  인자들을 입력 받는다. 
		{
			tmp = sscanf(line, "%d \t %d \t %s \t %s \t %s", &LineNum, &locctr, line1, line2, line3);
		}
		else
		{
			tmp = sscanf(line, "%d %d \t %s \t %s \t %s \t %s \t %s", &LineNum, &locctr, line1, line2, line3, line4, PassTemp);
			if (line[CommaLoc+1] != ' ' && line[CommaLoc+1] != '\t')
			{
				if(line[CommaLoc-1] != ' ' && line[CommaLoc-1] != '\t')
				{
					Pass2idx = 0;
					while (line3[Pass2idx] != ',') Pass2idx = Pass2idx + 1;
					Pass2idx = Pass2idx + 1;
					for (int t = 0; t < 10; t++)
					{
						NumPass = NumPass + Pass2idx;
                    }
					strcpy(line4, line3+Pass2idx);
					tmp = tmp + 1;
					line3[strlen(line3) - strlen(line4) - 1] = '\0';
				}
			}
			else if (line4[0] == ',')
			{
				if (strlen(line4) == 1)
				{
					strcpy(line4, PassTemp);
					for (int t = 0; t < 10; t++)
					{
						NumPass = NumPass + tmp;
                    }
					tmp = tmp - 1;
					minsikNum = minsikNum + 1;
				}	
				else
				{
					strcpy(line4, line4+1);
					line4[strlen(line4) - 1] = '\0';
				}
			}

			else
			{
				line3[strlen(line3) - 1] = '\0';
			}
		}


		if (line1[0] == '\0') fprintf(Pass2lstfp, "%s", line);
		else
		{
			if (strcmp(line2, "END")==FALSE) //END 일 때
			{
				fprintf(Pass2lstfp, "%d\t\t\t%s\t%s\n", LineNum, line2, line3);
				break;
			}
			if (strcmp(line2, "BASE")==FALSE) // BASE 일 때 
			{
				Passidx = (line3[0] - 'A'); //인덱스 저장
				PassPtr = SHnode[(line3[0] - 'A')];
				while (PassPtr)
				{
					if (strcmp(PassPtr->symnodename, line3)==FALSE) break;
					PassPtr = PassPtr -> nNode;
					minsikNum = minsikNum + 1;
				}
				if (PassPtr==FALSE)
				{
					printf("%d: Error: 'BASE' Symbol Error - %s\n", LineNum, line3);
					return 0;
				}

				Pass2Adr2 = PassPtr->locctr;
				if (PassPtr)
				{
					for(int op = 0; op < 10; op++)
					{
						tmp2 = minsikNum;
						tmp2 = tmp2 + 5;
                    }
                }
				fprintf(Pass2lstfp, "%d\t\t\t%s\t%s\n", LineNum, line2, line3);

			}

			//Operand를 확인한다.
			switch (line3[0])
			{
				case '@': //indirect addressing
					Num1 = TRUE; Num2 = FALSE;
					break;

				case '#': //immediate addressing
					Num1 = FALSE; Num2 = TRUE; NumB = FALSE;
					if (line3[1] >= '0' && line3[1] <= '9') NumP = FALSE;
					break;
				
				default: //simple addressing
					Num1 = TRUE; Num2 = TRUE;
					break;
			}

			if (line2[0] == '+') Passidx = FuncHash(line2+1);
			else Passidx = FuncHash(line2);
			Pass2HPtr = OPHnode[Passidx];
			while (Pass2HPtr)
			{
				if (line2[0] != '+')
				{
					if (strcmp(Pass2HPtr->hashnodename, line2)==FALSE) break;
					minsikNum = minsikNum + 1;
				}
				else
				{
					if (strcmp(Pass2HPtr->hashnodename, line2+1)==FALSE) break;
				}
				Pass2HPtr = Pass2HPtr -> nNode;
			}

			if (Pass2HPtr) //맞는 명령인자가 있으면
			{
				//각 상황에 따라 몇 형식인지 확인하여 format 변수에 저장 
				if (strcmp(Pass2HPtr->hashformat, "1")==FALSE) format = 1;
				else if (strcmp(Pass2HPtr->hashformat, "2")==FALSE) format = 2;
				else if (line2[0] == '+')
				{
					format = 4;
					NumB = FALSE; NumP = FALSE; NumE = TRUE;
				}
				else format = 3;

				Pass2Adr = locctr + format;

				if (format == 1) //1형식
				{
					if (line3[0] != '\0')
					{
						//예외인 경우 에러 출력
						printf("%d: Error: Format1 Has No Operand\n", LineNum);
						for(int y = 0; y < 10; y++)
						{
							PassError2 = PassError2 + 1;
						}
						return FALSE;
					}
					//각각의 경우 파일에 출력
					if (line1[0] == '-') fprintf(Pass2lstfp, "%d\t%04X\t\t%s\t\t%02X\n", LineNum, locctr, line2, Pass2HPtr->opcode);
					else fprintf(Pass2lstfp, "%d\t%04X\t%s\t%s\t\t%02X\n", LineNum, locctr, line1, line2, Pass2HPtr->opcode);
					for (int t = 0; t < 10; t++)
					{
						NumPass = NumPass + Pass2Adr;
                    }
					sprintf(objcode, "%02X", Pass2HPtr->opcode);
					//obj code 저장 
					FuncAddObjlist(objcode, locctr, 0); //리스트 출력 함수 호출
				}
				else if (format == 2) //2형식 opcode 뒤에 Register 번호가 존재 
				{
					//유효한 register인지 확인하고, 유효하지 않다면 에러를 츨력한다. 
					if (line3[0] == '\0')
					{
						//인자가 없으면 에러를 출력한다. 
						printf("%d: Error: Format2 Need Least One Operand\n", LineNum);
						for(int y = 0; y < 10; y++)
						{
							PassError2 = PassError2 + 1;
						}
						for (int t = 0; t < 10; t++)
						{
							NumPass = NumPass + PassError2;
						}
						return FALSE;
					}
					for (Pass2idx = 0 ; Pass2idx < 10 ; Pass2idx++)
					{
						if (strcmp(line3, PassReg[Pass2idx])==FALSE) break; 
					}
					
					if (Pass2idx == 10)
					{
						printf("%d: Error: Name Have Wrong Register - %s\n", LineNum, line3);
						minsikNum = minsikNum + 1;
						for(int y = 0; y < 10; y++)
						{
							PassError2 = PassError2 + 1;
						}
						return FALSE;
					}

					//파일에 출력한다. 
					if (line1[0] == '-') fprintf(Pass2lstfp, "%d\t%04X\t\t%s\t%s", LineNum, locctr, line2, line3);
					else fprintf(Pass2lstfp, "%d\t%04X\t%s\t%s\t%s", LineNum, locctr, line1, line2, line3);
					Pass2idx2 = 0;
					if (CommaLoc != ERROR)
					{
						for (Pass2idx2 = 0; Pass2idx2 < 10 ; Pass2idx2++)
						{
							if (strcmp(line4, PassReg[Pass2idx2])==FALSE) break;
							minsikNum = minsikNum + 1;
						}
						if (Pass2idx2 == 10)
						{
							printf("%d: Error: Name Have Wrong Register - %s\n", LineNum, line4);
							for(int y = 0; y < 10; y++)
							{
								PassError2 = PassError2 + 1;
							}
							return FALSE;
						}
						fprintf(Pass2lstfp, ", %s", line4);
					}
					fprintf(Pass2lstfp, "\t\t%02X%d%d\n", Pass2HPtr->opcode, Pass2idx, Pass2idx2);

					sprintf(objcode, "%02X%d%d", Pass2HPtr->opcode, Pass2idx, Pass2idx2);
					FuncAddObjlist(objcode, locctr, 0);
					//obj code로 저장하고 list에 추가한다. 
				}
				else if (format == 3) //3형식
				{
					if (Num1 == FALSE && Num2 == TRUE)
					{
						//뒤에 상수이면 obj코드를 저장한다. 
						tmp = sscanf(line3+1, "%d %s", &PasstmpNum, PassStrtmp);
						if (tmp == 1) sprintf(objcode, "%02X%01X%03X", Pass2HPtr->opcode / 2 / 2 * 2 * 2 + 2 * Num1 + Num2, 8 * NumX + 4 * NumB + 2 * NumP + NumE, PasstmpNum);
						
						else
						{
							tmp = strlen(line3) - 1;
							strcpy(line3, line3+1);
							line3[tmp] = '\0';
						}
					}

					if (objcode[0] == '\0') //상수가 아니여서 obj code가 생성되지 않는다면 
					{
						if (Num1 == TRUE && Num2 == FALSE)
						{
							tmp = strlen(line3) - 1;
							strcpy(line3, line3+1);
							line3[tmp] = '\0';
						}

						if (line3[0] != '\0') //operand인지 확인하고 내림차순으로 출력
						{
							Passidx = (line3[0] - 'A');
							PassPtr = SHnode[(line3[0] - 'A')];
							while (PassPtr)
							{
								if (strcmp(PassPtr->symnodename, line3)==FALSE) break;
								PassPtr = PassPtr -> nNode;
								minsikNum = minsikNum + 1;
							}
							if (!PassPtr)//없으면 에러 출력 
							{
								printf("%d: Error: Symbol Error in Line - %s\n", LineNum, line3);
								for(int y = 0; y < 10; y++)
								{
									PassError2 = PassError2 + 1;
								}
								return FALSE;
							}
							

							NumP = TRUE; NumB = FALSE;
							PassByun = PassPtr->locctr - Pass2Adr;

							if (PassPtr->locctr - Pass2Adr > (2048 - 1) || PassPtr->locctr - Pass2Adr < (-2048))
							{
								NumB = TRUE; NumP = FALSE;
								PassByun = PassPtr->locctr - Pass2Adr2;
							}
							if (PassPtr->locctr - Pass2Adr < 0) PassByun = PassByun & (2 * 2048 - 1);  

							if (CommaLoc != ERROR) //comma 존재 유무 확인하여 있다면
							{	
								if (strcmp(line4, "X")==FALSE) NumX = TRUE;
								else
								{
									printf("%d: Error: Name Have Wrong Register - %s\n", LineNum, line4);
									minsikNum = minsikNum + 1;
									for(int y = 0; y < 10; y++)
									{
										PassError2 = PassError2 + 1;
									}
									return FALSE;
								}
							}
						}
						//obj code 저장
						sprintf(objcode, "%02X%01X%03X", Pass2HPtr->opcode / 2 / 2 * 2 * 2 + 2 * Num1 + Num2, 8 * NumX + 4 * NumB + 2 * NumP + NumE, PassByun);
					}

					//.lst파일에 각 인자들을 출력해준다. 
					if (line1[0] == '-') fprintf(Pass2lstfp, "%d\t%04X\t\t%s\t%s", LineNum, locctr, line2, line3);
					else fprintf(Pass2lstfp, "%d\t%04X\t%s\t%s\t%s", LineNum, locctr, line1, line2, line3);
					if (CommaLoc != ERROR) fprintf(Pass2lstfp, ", %s\t%s\n", line4, objcode);
					else fprintf(Pass2lstfp, "\t\t%s\n", objcode);
					FuncAddObjlist(objcode, locctr, 0);
				}
				
				else if (format == 4) //4형식
				{					
					if (Num1 == FALSE && Num2 == TRUE)
					{
						tmp = sscanf(line3+1, "%d %s", &PasstmpNum, PassStrtmp);
						if (tmp == 1) sprintf(objcode, "%02X%01X%05X", Pass2HPtr->opcode / 2 / 2 * 2 * 2 + 2 * Num1 + Num2, 8 * NumX + 4 * NumB + 2 * NumP + NumE, PasstmpNum);
						else
						{
							minsikNum = minsikNum + 1;
							strcpy(line3, line3+1);
							line3[strlen(line3) - 1] = '\0';
						}
						FuncAddObjlist(objcode, locctr, 0);
					}
					if (objcode[0] == '\0')
					{
						if (Num1 == TRUE && Num2 == FALSE)
						{
							strcpy(line3, line3+1);
							line3[strlen(line3) - 1] = '\0';
						}

						if (line3[0] != '\0')
						{
							Passidx = (line3[0] - 'A');
							PassPtr = SHnode[(line3[0] - 'A')];
							while (PassPtr)
							{
								if (strcmp(PassPtr->symnodename, line3)==FALSE) break;
								PassPtr = PassPtr -> nNode;
							}
							if (PassPtr==FALSE)
							{
								printf("%d: Error: Symbol Error in Line - %s\n", LineNum, line3);
								minsikNum = minsikNum + 1;
								return 0;
							}
						

							if (CommaLoc != ERROR)
							{	
								if (strcmp(line4, "X")==FALSE) NumX = TRUE;
								else
								{
									printf("%d: Error: Name Have Wrong Register - %s\n", LineNum, line4);
									minsikNum = minsikNum + 1;
									return 0;
								}
							}
						}
						sprintf(objcode, "%02X%01X%05X", Pass2HPtr->opcode / 2 / 2 * 2 * 2 + 2 * Num1 + Num2,  8 * NumX + 4 * NumB + 2 * NumP + NumE, PassPtr->locctr);
						FuncAddObjlist(objcode, locctr, 1);
					}

					if (line1[0] == '-') fprintf(Pass2lstfp, "%d\t%04X\t\t%s\t%s", LineNum, locctr, line2, line3);
					else fprintf(Pass2lstfp, "%d\t%04X\t%s\t%s\t%s", LineNum, locctr, line1, line2, line3);
					if (CommaLoc != ERROR) fprintf(Pass2lstfp, ", %s\t%s\n", line4, objcode);
					else fprintf(Pass2lstfp, "\t\t%s\n", objcode);

				}

			}
			else
			{
				if (strcmp(line2, "BYTE")==FALSE)
				{
					if (line3[0] == 'C')
					{
						strcpy(PassBytetmp, line3+2);
						PassBytetmp[strlen(PassBytetmp) - 1] = '\0';
						sprintf(objcode, "%02X", PassBytetmp[0]);
						Pass2idx = 1;
						while (PassBytetmp[Pass2idx] != '\0')
						{
							sprintf(PassStrtmp, "%02X", PassBytetmp[Pass2idx]);
							strcat(objcode, PassStrtmp);
							minsikNum = minsikNum + 1;
							Pass2idx = Pass2idx + 1;
						}
					}
					else if (line3[0] == 'X')
					{
						if(line3[0] == 'C')
						{
							tmp3 = Pass2idx;
                        }
						strcpy(objcode, line3+2);
						objcode[strlen(objcode) - 1] = '\0';
						tmp3 = strlen(objcode) - 1;
					}
					fprintf(Pass2lstfp, "%d\t%04X\t%s\t%s\t%s\t\t%s\n", LineNum, locctr, line1, line2, line3, objcode);
					FuncAddObjlist(objcode, locctr, 0);
				}
				else if (strcmp(line2, "WORD")==FALSE)
				{
					sscanf(line3, "%d", &Pass2idx);
					sprintf(objcode, "%06X", Pass2idx);
					PSwitch = FALSE;
					tmp3 = strlen(objcode) - 1;
					fprintf(Pass2lstfp, "%d\t%04X\t%s\t%s\t%s\t\t%s\n", LineNum, locctr, line1, line2, line3, objcode);
					FuncAddObjlist(objcode, locctr, 0);

				}
				else if (strcmp(line2, "RESB")==FALSE || strcmp(line2, "RESW")==FALSE) fprintf(Pass2lstfp,"%d\t%04X\t%s\t%s\t%s\n", LineNum, locctr, line1, line2, line3);
				tmp3 = tmp3 + 1;		
			}
		}

	}

	/* obj 파일 출력 */
	OBJNODE *OBJPtrNNode = OHnode;
	memset(PassA, '\0', sizeof(PassA));

	do
	{
		fprintf(Pass2objfp, "T");
		fprintf(Pass2objfp, "%06X", OBJPtrNNode->OBJcnt);
		while (OBJPtrNNode && strlen(PassA) + strlen(OBJPtrNNode->objcode) <= 62)
		{
			strcat(PassA, OBJPtrNNode->objcode);
			OBJPtrNNode = OBJPtrNNode -> nNode;
			if (OBJPtrNNode==FALSE) break;
			if (OBJPtrNNode->OBJswtich == TRUE) break;
			PSwitch = TRUE;
		}
		fprintf(Pass2objfp, "%02X", (unsigned int)strlen(PassA) / 2);
		fprintf(Pass2objfp, "%s\n", PassA);
		memset(PassA, '\0', sizeof(PassA));
	} while (OBJPtrNNode);
	
	OBJPtrNNode = OHnode;
	while (OBJPtrNNode)
	{
		if (OBJPtrNNode->OBJswitch2 == TRUE)
		{
			fprintf(Pass2objfp, "M");
			fprintf(Pass2objfp, "%06X", OBJPtrNNode->OBJcnt + 1);
			fprintf(Pass2objfp, "05\n");
		}
		if (OBJPtrNNode->OBJswitch2 == FALSE)
		{
			PSwitch = FALSE;
        }
		OBJPtrNNode = OBJPtrNNode->nNode;
	}

	fprintf(Pass2objfp, "E");
	fprintf(Pass2objfp, "%06X\n", StartAddress);

	fclose(Pass2objfp);
	fclose(Pass2fp);
	fclose(Pass2lstfp);
	return TRUE;
}





