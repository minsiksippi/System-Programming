#define TRUE 1
#define FALSE 0
#define HEX 16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

unsigned char size[1<<20]; 

//history를 구현을 위한 linked list 구조체 선언 
typedef struct _NAMENODE
{  
    char firstname[100];
    struct _NAMENODE *nNode;
}NAMENODE;

//opcode hashtable 구조체 선언  
typedef struct _HASHNODE
{  
    char symbol[7], format[5];
    int opcode;
    struct _HASHNODE* nNode;
    
}HASHNODE;

NAMENODE *HNODE, *LNODE;  //HNODE : history입력 시 제일 첫 node 포인터, LNODE : 제일 최근 node 포인터
HASHNODE *Htable[20]; 

char ins[100], info[3][10]; //ins : 입력되는 명령어 저장 변수, info : 인자 정보 저장 변수
int addr = -1; //int addr: dump명령 시 메모리 저장을 위한 변수,
int lo_start = 0,lo_end = 0; //lo_start : 명령어에서 인자를 뺀 명령어의 시작 위치 변수 , lo_end : 명령어에서 인자 뺀 명령어 끝 위치 저장 변수
int switch1 = FALSE; // switch1 : opcode 확인 스위치 변

char *arr_fac[7] = {"du","dump","e","edit","f","fill","opcode"}; //두개 이상의 인자가 필요한 명령문들  
char *arr_ins[10] = {"h","help","d","dir","q","quit","hi","history","reset","opcodelist"}; //한개 인자가 필요한 명령문

//명령어 입력 받는 함수이다.
void Func_Input()
{ 
    int i;
    i = 0;
    while(1)
    {
        if(i > 100)
        {
            memset(ins, '\0', sizeof(ins)); //메모리 초기화 
            printf("Error : Too much Input size\n"); //입력 명령문의 길이가 너무 김 
            i = 0;
            break;
        }

        scanf("%c", &ins[i]);
        if(ins[i] == '\n')
        {
            ins[i] = '\0';
            i=i-1;
            break;
        }
        i=i+1;
    }
}

//instruction을 입력받을 때 instruction을 저장하는 함수이다.
void Func_instruction(char* Tex)
{
    int i, cnt_i;
    int j, k, forl; //loop문 변수 
    char real_ins[100];  //인자를 제외한 순수한 instruction만 저장하는 변수이다  
    memset(real_ins, '\0', sizeof(real_ins));

    for(i = 0; i < strlen(ins); i++) //입력 받은 문자 중 공백을 제외함. 
    {
        if((ins[i] == ' ' || ins[i] == '\t')==FALSE) break;
        j=j+1;
    }
    lo_start = i; //공백을 제외한 문자의 시작 지점

    
    for(k = lo_start; k < strlen(ins); k++) //lo_start 부터 탐색하여 순수 명령어를 real_ins변수에 저장한다. 
    {
        if(ins[k] == ' ' || ins[k] == '\t') break;
        real_ins[k - lo_start] = ins[k];
        cnt_i = k;
        j=j+1;
    }
    lo_end = k;

    cnt_i = ++lo_end;  //명령어 마지막 위치의 다음을 접근하기 위해 1 증가시킴

    for(forl = 0; forl < 10; forl++)
    {
        if(strcmp(real_ins, arr_ins[forl])==FALSE)
        {
            for(; cnt_i < strlen(ins); cnt_i++)
            {
                if((ins[cnt_i] == '\t') || (ins[cnt_i] == ' ')) break;
                else
                {
                    strcpy(Tex, "\0");
                    return;
                }
            }
            strcpy(Tex, real_ins);
            return; 
        }
    }

    for(i = 0; i < 7; i++)
    {
        if(strcmp(real_ins, arr_fac[i]) == FALSE)
        {
            strcpy(Tex, real_ins);
            return;
        }
        else strcpy(Tex, "\0");
    }
}

//instruction을 history에 저장하는 함수이다. 
void Func_History_Save()
{
    int cnt1;
    NAMENODE *newnew = (NAMENODE*)malloc(sizeof(NAMENODE));
    NAMENODE *save_new_node = (NAMENODE*)malloc(sizeof(NAMENODE));
    
    strcpy(newnew->firstname, ins);
    strcpy(save_new_node->firstname, ins);

    LNODE->nNode = newnew;
    LNODE = newnew;
    newnew->nNode = NULL;
}

//history 입력을 받았을 때 지금까지의 명령어를 출력해주는 함수이다.
void Func_History_Show()
{
    int cnt;

    NAMENODE *node = HNODE->nNode;
    NAMENODE *Show_node = HNODE->nNode;
    
    cnt=1;
    while(node != NULL)
    {
        printf("\t%d\t%s\n", cnt, node->firstname);
        node = node->nNode;
        cnt=cnt+1;
    }
}

//opcode list를 화면에 출력해주는 함수
void Func_OPlist()
{
    int i;
    for(i = 0; i < 20 ; i++)
    {
        HASHNODE *hn = Htable[i];
        printf("%d : ", i);

        while(hn)
        {
            printf("[%s,%02X]", hn->symbol, hn->opcode);
            if(hn->nNode != NULL) printf(" -> ");
            hn = hn->nNode;
        }
        printf("\n");
    }
}


//dir, d를 입력받았을 때 현재 디렉터리에 있는 파일들을 출력해주는 함수이다.
void Func_Dir()
{
	struct dirent *df;
    struct dirent *second_df;
	struct stat stdir;
	DIR *dir_fir;
	dir_fir = opendir(".");  //current directory를 연다.

	while ((df = readdir(dir_fir)) != NULL)
    {
		lstat(df->d_name, &stdir);
		printf("\t%s", df->d_name);
		if (S_ISDIR(stdir.st_mode)) printf("/");  //폴더 안에 폴더가 있으면 /를 출력 
		else if (S_IEXEC & stdir.st_mode) printf("*");  //실행 파일이면 *을 출력 
	}
	printf("\n");
	closedir(dir_fir);	//디렉토리 닫기
}

//instruction과 factor의 유효성을 판단하는 함수이다. 만약 인자가 유효하다면 인자의 개수를, 유효하지 않다면 -1을 리턴한다.
int Func_Cal_Fac()
{
    int com, switch_Fac, length_fac=0;
    int i,j,k;
    int cnt, cnt2;
    char arr_facstr[100];  //instruction 인자 위치부터 저장되는 문자열.

    memset(info, '\0', sizeof(info));
    memset(arr_facstr, '\0', sizeof(arr_facstr));

    com=0; 
    k=0;
    cnt = 0;
    cnt2=0;
    switch_Fac=FALSE; //인자의 유무를 나타내는 스위치 변수

    //명령어 다음 위치부터 문자열이 나오면 그 위치부터 새로운 문자열에 저장해준다.
    for(i = lo_end; i < strlen(ins); i++)
    {
        if(ins[i] != ' ' && ins[i] != '\t')
        {
            for(j = i; j < strlen(ins); j++) arr_facstr[j-i] = ins[j];
            break;
        }
    }

    arr_facstr[strlen(ins)-lo_end] = '\0';

    for(i = 0; i < strlen(arr_facstr); i++) //컴마의 개수를 구해서 저장
    {
        if(arr_facstr[i] == ',') com++; 
    }

    if(com > 2) //컴마 개수가 2초과면 입력 에러  
    {
        printf("Error : Wrong Input\n");
        return -1;
    }

    for(i = 0; i < strlen(arr_facstr); i++)
    {
        if(arr_facstr[i] != ' ' && arr_facstr[i] != '\t') switch_Fac = TRUE;
    }

    for(k = 0; k < strlen(arr_facstr); k++)
    {
        length_fac = length_fac + 1;
    }  

    if(switch_Fac==FALSE) return 0; //인자의 개수가 0 
        
    for(i = 0; i < strlen(arr_facstr); i++)
    {
        if(cnt2 > 5)
        {
                printf("Error : Wrong Input\n");
                return -1;
        }
        if(arr_facstr[i] == ',')
        {
            cnt = cnt + 1;
            cnt2 = 0;            
        }
        else 
        {  
            if(switch1 == FALSE && arr_facstr[i]!= ' ' && arr_facstr[i] != '\t')
            {
                if('0' <= arr_facstr[i] && arr_facstr[i] <= '9') //0~9까지만 가능하다 
                {
                    if(info[cnt][0] != '\0' && (arr_facstr[i-1] == '\t' || arr_facstr[i-1] == ' ')) return -1;
                    info[cnt][cnt2] = arr_facstr[i];
                    cnt2 = cnt2 + 1;
                }
                else if('A' <= arr_facstr[i] && arr_facstr[i] <= 'F') //A~F까지만 가능  
                {
                    if(info[cnt][0] != '\0' && (arr_facstr[i-1] == '\t' || arr_facstr[i-1] == ' ')) return -1;
                    info[cnt][cnt2] = arr_facstr[i];
                    cnt2 = cnt2 + 1;
                }
                else if('a' <= arr_facstr[i] && arr_facstr[i] <= 'f') //a~f까지만 가능  
                {
                    if(info[cnt][0] != '\0' && (arr_facstr[i-1] == '\t' || arr_facstr[i-1] == ' ')) return -1;
                    info[cnt][cnt2] = arr_facstr[i];
                    cnt2 = cnt2 + 1;
                }
                else return -1;
            }
            else if(switch1 && !(arr_facstr[i] == ' ' || arr_facstr[i] == '\t'))
            {
                if(info[cnt][0] != '\0' && (arr_facstr[i-1] == '\t' || arr_facstr[i-1] == ' ')) return -1;
                info[cnt][cnt2] = arr_facstr[i];
                cnt2 = cnt2 + 1;
            }
        }
    }

    cnt = 0;
    for(i = 0; i < com + 1; i++)
    {
        if(info[i][0]) cnt=cnt+1;  
    }
        
    if(cnt != com + 1) return -1;
    else return cnt;
}


//edit 를 입력받았을 때, 입력된 주소의 메모리 값을 입력받은 값으로 변경해주는 함수이다.  
void Func_Edit()
{
    if(Func_Cal_Fac() != 2) //edit을 입력받았을 때 인자가 2개가 아니라면 입력 에러
    {
        printf("Error : Wrong Input\n");
        return;
    }

    //주소나 그 값이 잘못 입력 되었을 경우 
    if(strtoul(info[0], NULL, HEX) < 0 || strtoul(info[0], NULL, HEX) > (1<<20)-1 || strtoul(info[1], NULL, HEX) > 0xFF || strtoul(info[1], NULL, HEX) < 0x00)
    {
        printf("Error : Wrong Input(Address, Value)\n");
        return;
    }
    size[strtoul(info[0], NULL, HEX)] = strtoul(info[1], NULL, HEX);
    Func_History_Save();
}

//입력받은 start에서 end 까지 value를 메모리에 입력해주는 함수이다.
void Func_Fill()
{
    int i;
    if(Func_Cal_Fac() != 3) //start, end, value 값이 제데로 입력되지 않은 경우
    {

        printf("Error : Wrong Input\n");
        return;
    }
    //start, end, value 값이 범위가 다를 경우 에러 
    if(strtoul(info[0], NULL, HEX) < 0 || strtoul(info[0], NULL, HEX) > (1<<20)-1 || strtoul(info[1], NULL, HEX) < 0 || strtoul(info[1], NULL, HEX) > (1<<20)-1 || strtoul(info[2], NULL, HEX) > 0xFF || strtoul(info[2], NULL, HEX) < 0x00) //start, end, value 범위가 벗어날 경우.
    {
        printf("Error : Wrong Input\n");
        return;
    }

    if(strtoul(info[0], NULL, HEX) > strtoul(info[1], NULL, HEX)) //시작 번호가 끝 번호보다 큰 경우.
    {
        printf("Error : Wrong Input\n");
        return;
    }

    for(i = strtoul(info[0], NULL, HEX); i < strtoul(info[1], NULL, HEX) + 1; i=i+1) size[i] = strtoul(info[2], NULL, HEX);
    Func_History_Save();
}

//dump 명령문이 입력되었을 때 그 결과를 출력해주는 함
void Func_Dump()
{
    int start, end, st_du, ed_du;
    int i, j;

    if(Func_Cal_Fac() == 2) //start와 end모두 입력 받을 때.
    {
        start = strtoul(info[0], NULL, HEX);
        end = strtoul(info[1], NULL, HEX);
        if(end > (1<<20) - 1)
        {
            printf("Error : Wrong Input\n");
            return;
        }
    }
    else if(Func_Cal_Fac() == 1) //start만 입력 받을 때.
    {
        start = strtoul(info[0], NULL, HEX);
        end = start + 159;
        if(end > (1<<20)-1) end = (1<<20)-1;
    }
    else if(Func_Cal_Fac() > 2 || Func_Cal_Fac() == -1) //인자가 두개 이상일 때 
    {
        printf("Error : Wrong Input\n");
        return;
    }
    else //인자가 없을 때.
    {
        if(addr >= (1<<20)-1) addr = -1; //address 가 1<<20 - 1 이상인 경우 초기화 시켜준다.
        start = addr+1;
        end = start + 159;
        if(end > (1<<20) - 1) end = (1<<20) - 1; //주소를 넘어간 경우 예외 처리를 해준다.
            
    }


    if(start < 0 || end < 0 || start > (1<<20) - 1) //start나  end가 음수이거나 최대 메모리를 초과한 경우
    {
        printf("Error : Wrong Input\n");
        return;
    }

    if(start > end) //시작번호가 끝보다 큰 경우 에러
    {
        printf("Error : Wrong Input\n");
        return;
    }

    for(i = start - start%HEX; i <= end - end%HEX; i=i+HEX)
    {
        printf("%05X ", i);
        for(j = i; j < i + HEX; j = j + 1)
        {
            if(j < start || j > end) printf("  ");
            else printf("%02X", size[j]);
            printf(" ");
        }
        printf("; ");
        for(j = i; j < i + HEX; j = j + 1)
        {
            if(size[j] < 20 || size[j] > 0x7E) printf(".");
            else printf("%c", size[j]);
        }
        printf("\n");
    }
    addr = end;
    Func_History_Save();
}

//opcode와 함께 입력 받은 mnemonic에 대한 opcode를 출력해주는 함수이다.  
void Func_OPcode()
{
    int result, switch_op;
    switch1 = TRUE;
    if(Func_Cal_Fac() != TRUE)
    {
        printf("Error : Wrong Input Opcode\n");
        return;
    }
    switch_op = -1;

    HASHNODE *fHn = Htable[(info[0][0] + info[0][strlen(info[0])-1]) %20]; //hashtable의 키값에 대한 hashtable node

    while(fHn != NULL)
    {
        if(!strcmp(fHn->symbol, info[0])) switch_op = fHn->opcode;
        fHn = fHn->nNode;
    }

    if(switch_op == -1)
    {
        printf("Error : Wrong Input\n");
        return;
    }
    else
    {
        printf("opcode is %02X\n", switch_op);
    }
    switch1 = FALSE;
    Func_History_Save();
}
