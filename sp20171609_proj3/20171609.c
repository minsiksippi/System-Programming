#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "20171609.h"

int main()
{
    FILE *fp;

    HNODE = (NAMENODE*)malloc(sizeof(NAMENODE));
    LNODE = HNODE;

    char input1[7], input2[5], input3[11];
    //input1 : 처음으로 입력 받는 명령어
	//input2 : 명령어 부분만 입력받는 변수
	//input3 : 명령어 뒷부분 저장하는 변수

    int InputKey, opcode;
    int i;
    
    memset(size, '\0', sizeof(size)); //변수 초기화

    fp = fopen("opcode.txt", "r");

    while(fscanf(fp, "%X %s %s", &opcode, input1, input2) != EOF)
    {
        InputKey = (input1[0] + input1[strlen(input1)-1]) %20;

        HASHNODE *First_hash = (HASHNODE*)malloc(sizeof(HASHNODE));
        strcpy(First_hash->hashnodename, input1);
        strcpy(First_hash->hashformat, input2);
        First_hash->opcode = opcode;

        if(OPHnode[InputKey] != NULL)
        {
            First_hash->nNode = OPHnode[InputKey];
            OPHnode[InputKey] = First_hash;
        }
        else
        {
            OPHnode[InputKey] = First_hash;
            First_hash->nNode = NULL;
        }
    }
    fclose(fp);

    for(i = 0; i < 3; i = i + 1) ESTable[i] = (ESTABNODE*)malloc(sizeof(ESTABNODE));

    while(1)
    {
        printf("sicsim> ");
        Func_Input();
        if(ins[0] != '\0')
        {
            Func_instruction(input3);
            if(strcmp(input3,"help")==FALSE || strcmp(input3,"h")==FALSE) //명령어가 h, help일 때
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\n");
                    printf("f[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
                    printf("assemble filename\ntype filename\nsymbol\n");
                    printf("progaddr address\nloader object-filename1 object-filename2 ...\nrun\nbp\nbp clear\nbp address\n");
                    Func_History_Save(); //명령어 history에 추가
                }
                else printf("Error : Input Error.\n"); //인덱스가 비어있지 않으면 에러 출력
            }
            else if(strcmp(input3,"dir")==FALSE || strcmp(input3,"d")==FALSE) // d, dir일 경우 
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    DIR *dir;
                    struct dirent *files;
	                struct stat f_stat;
	                
	                dir = opendir("."); 
	                while ((files = readdir(dir)) != NULL)
                    {
		                lstat(files->d_name, &f_stat);
		                printf("\t%s", files->d_name);
		                if (S_ISDIR(f_stat.st_mode)) printf("/");  //폴더인 경우 /추가
		                else if (S_IEXEC & f_stat.st_mode) printf("*");  //실행파일인 경우 *추가
	                }
	                printf("\n");
	                closedir(dir);
                    Func_History_Save(); //명령어 history에 추가
                }
                else printf("Error : Input Error.\n"); //인덱스가 비어있지 않으면 에러 출력
            }
            else if(strcmp(input3,"quit")==FALSE || strcmp(input3,"q")==FALSE) // q, quit일 경우 
            {
                if(Func_Cal_Fac()==FALSE) break;
                else printf("Error : Input Error.\n"); //인덱스가 비어있지 않으면 에러 출력
            }
            else if(strcmp(input3,"history")==FALSE || strcmp(input3,"hi")==FALSE)
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    Func_History_Save();  
                    int count = 1; //history 개수 나타내는 변수
                    NAMENODE *Show_node = HNODE->nNode;
                    while(Show_node != NULL)
                    {
                        printf("%d\t%s\n", count, Show_node->namenodename);
                        Show_node = Show_node->nNode;
                        count = count + 1;
                    }
                }
                else printf("Error : Input Error.\n"); //인덱스가 비어있지 않으면 에러 출력
            }
            else if(strcmp(input3,"reset")==FALSE)
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    memset(size, '\0', sizeof(size));
                    Func_History_Save(); //명령어 history에 추가
                }
                else printf("Error : Input Error.\n"); //인덱스가 비어있지 않으면 에러 출력
            }
            else if(strcmp(input3,"opcodelist")==FALSE) //opcodelist 입력시
            {
                if(Func_Cal_Fac()==FALSE)
                {
                    for(int i = 0; i < 20 ; i++)
                    {
                        printf("%d : ", i);
                        HASHNODE *node = OPHnode[i];

                        while(node)
                        {
                            printf("[%s,%02X]", node->hashnodename, node->opcode);
                            if(node->nNode != NULL) printf(" -> ");
                            node = node->nNode;
                        }
                        printf("\n");
                    }


                    Func_History_Save(); //명령어 history에 추가
                }
                else printf("Error : Input Error.\n"); //인덱스가 비어있지 않으면 에러 출력
            }
            else if(strcmp(input3,"opcode")==FALSE)
            {
                Func_OPcode();
                switch1 = 0;
            }
            else if(strcmp(input3,"dump")==FALSE || strcmp(input3,"du")==FALSE) Func_Dump(); //d, dump 입력 
            else if(strcmp(input3,"fill")==FALSE || strcmp(input3,"f")==FALSE) Func_Fill();//f, fill 인 경우
            else if(strcmp(input3,"edit")==FALSE || strcmp(input3,"e")==FALSE) Func_Edit(); //e, edit인 경우 
            else if(strcmp(input3, "assemble")==FALSE) //assemble 입력
            {
                Assemble();
                switchFilename = 0; 
            } 
            else if(strcmp(input3, "type")==FALSE) //type 입력
            {
                Func_Type();
                switchFilename = 0;  
            }
            else if(strcmp(input3, "symbol")==FALSE) Func_Symbol(); //symbol 입력
            else if(strcmp(input3, "run")==FALSE) Func_Run(); //run  입력
            else if(strcmp(input3, "bp")==FALSE) Func_Bp(); //bp  입력
            else if(strcmp(input3, "loader")==FALSE) Func_Loader(); //loader 입력
            else if(strcmp(input3, "progaddr")==FALSE) Func_Progaddr(); //progaddr 입력 
            else printf("Error : Input Error.\n");
        }
        memset(ins, '\0', sizeof(ins));  
    }

    return 0;
}
