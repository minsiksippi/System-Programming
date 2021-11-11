#define TRUE 1
#define FALSE 0

#include "20171609.h"

int main()
{
    FILE *fp;
    fp = fopen("opcode.txt", "r"); //opcode.txt read

    HNODE = (NAMENODE*)malloc(sizeof(NAMENODE));
    LNODE = HNODE;

    memset(size, '\0', sizeof(size));
    int opcode;
    int minsik=0, key;
    int size_in;
    char symbol[7], format[5], button[11]; //명령에 대한 각각의 인자 변 

    while(fscanf(fp, "%X %s %s", &opcode, symbol, format) != EOF)
    {
        HASHNODE *First_hash = (HASHNODE*)malloc(sizeof(HASHNODE));
        HASHNODE *Second_hash = (HASHNODE*)malloc(sizeof(HASHNODE));
        
        key = (symbol[0] + symbol[strlen(symbol)-1]) %20;  //input 받은 mnemonic에 대한 hashtable의 키값을 계산 

        strcpy(First_hash->symbol, symbol);
        strcpy(First_hash->format, format);

        strcpy(Second_hash->symbol, symbol);
        strcpy(Second_hash->format, format);
        Second_hash->opcode = opcode;

        if(Htable[key] == NULL)
        {
            Htable[key] = Second_hash;
            Second_hash->nNode = NULL;
        }      
        else
        {
            Second_hash->nNode = Htable[key];
            Htable[key] = Second_hash;
        }
    }

    while(1)
    {
        printf("sicsim> ");
        Func_Input(); //instruction을 입력 받는 함수 호출

        if(ins[0] != '\0')
        {
            Func_instruction(button);
            if(strcmp(button,"h")==FALSE || strcmp(button,"help")==FALSE) //help 혹은 h 입력 받으면  
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    printf("h[elp]\n");
                    printf("d[ir]\n");
                    printf("q[uit]\n");
                    printf("hi[story]\n");
                    printf("du[mp] [start, end]\n");
                    printf("e[dit] address, value\n");
                    printf("f[ill] start, end, value\n");
                    printf("reset\nopcode mnemonic\n");
                    printf("opcodelist\n\n");
                    Func_History_Save(); //history에 저장
                    minsik=minsik+1;
                }
                else printf("Error : Wrong Input\n");
            } 
            else if(strcmp(button,"q")==FALSE || strcmp(button,"quit")==FALSE) //q, quit를 입력받으면 실행파일 종료
            {
                if(Func_Cal_Fac()==FALSE) break;
                else printf("Error : Wrong Input\n");
                minsik=minsik+1;
            }
            else if(strcmp(button,"d")==FALSE || strcmp(button,"dir")==FALSE) //d, dir을 입력 받으면 현재 디렉토리 화면에 출력
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    Func_Dir();
                    Func_History_Save(); //dir 또한 history에 저장
                    minsik=minsik+1;
                }
                else printf("Error : Wrong Input\n");
            }
            else if(strcmp(button,"hi")==FALSE || strcmp(button,"history")==FALSE) //hi, history 입력 받으면 지금까지 입력받은 내용들 출력
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    Func_History_Save();  //현재 명령어도 포함되어야 하므로 먼저 실행해야 한다.
                    Func_History_Show(); //history 출력하는 함수 호출
                    minsik=minsik+1;
                }
                else printf("Error : Wrong Input\n");
            }
            else if(strcmp(button,"opcodelist")==FALSE) //opcodelist 입력시
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    Func_OPlist();
                    Func_History_Save(); //history에 저장
                    minsik=minsik+1;
                }
                else printf("Error : Wrong Input\n");
            }
            else if(strcmp(button,"reset")==FALSE)
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    memset(size, '\0', sizeof(size)); //memory 모든 값을 0으로 초기화  
                    Func_History_Save();
                    minsik=minsik+1;
                }
                else printf("Error : Wrong Input\n");
            }
            else if(strcmp(button,"opcode")==FALSE) Func_OPcode();
            else if(strcmp(button,"du")==FALSE || strcmp(button,"dump")==FALSE) Func_Dump();
            else if(strcmp(button,"f")==FALSE || strcmp(button,"fill")==FALSE) Func_Fill();
            else if(strcmp(button,"e")==FALSE || strcmp(button,"edit")==FALSE) Func_Edit();
            else printf("Error : Wrong Input\n");
        }
        size_in = sizeof(ins);
        memset(ins, '\0', size_in);
    }
    fclose(fp);
    return 0;
}