#include "myshell.h"

void sigint_handler(int signal)
{
    printf("\n");
}

int Func_Free_Job(int id) //job structure free 시켜주는 함수
{
    struct JobStruct *Freejob = newShell->jobs[id];
    struct Myshells *FreeProcess, *FreeTemp;
    //메모리 free에 사용되는 구조체 포인터 변수

    if (id > 20 || newShell->jobs[id] == NULL) return ERROR;
    //free
    for (FreeProcess = Freejob->newjob; FreeProcess != NULL; )
    {
        FreeTemp = FreeProcess->next;
        free(FreeProcess->input_path);
        free(FreeProcess->cmd);
        free(FreeProcess->argv);
        free(FreeProcess);
        free(FreeProcess->output_path);
        FreeProcess = FreeTemp;
    }
    free(Freejob->cmd);
    free(Freejob);
    newShell->jobs[id] = NULL;

    return FALSE;
}



//프로세스의 set을 만드는 함수 
int Func_Make_Process_Set(int pid, int status)
{
    int i, count, switchset = TRUE;
    struct Myshells *MakeProc; //사용되는 포인터 구조체 변수 

    count = 0;
    for (i = 1; i <= 20; i++)
    {
        if (newShell->jobs[i] == NULL) continue;
        count = count + 1;
        //구조체 탐색 하면서 생성 
        for (MakeProc = newShell->jobs[i]->newjob; MakeProc != NULL; MakeProc = MakeProc->next)
        {
            count = count + 1;
            if (MakeProc->pid == pid)
            {
                MakeProc->status = status;
                switchset = TRUE;
                return FALSE;
            }
            switchset = FALSE;
        }
    }
    return ERROR;
}


//백그라운드 명령어가 끝났는지 확인하는 함수
//ex) sleep 명령어 등 
int Func_Wait_Job(int id) //
{
    struct Myshells *waitProc;
    //구조체 포인터 변수
    int count = 0, count2 = 0, status = 0;
    int wait_pid = -1, wait_count = 0; //값 저장하는 변수 
    int switchWait = FALSE; //wait가 끝났는지 끝나지 않았는지 표현할 수 있는 스위치 변수

    if (id > 20 || newShell->jobs[id] == NULL) //예외처리
    {
        count2 = ERROR;
        return ERROR;
    }

    for (waitProc = newShell->jobs[id]->newjob; waitProc != NULL; waitProc = waitProc->next)
    {
        if (waitProc->status != 1) count = count + 1; //기다리는 만큼 count 증가
    }
    count2 = count;

    do
    {
        wait_pid = waitpid(-newShell->jobs[id]->job_pid, &status, WUNTRACED);
        wait_count = wait_count + 1;
        switchWait = TRUE;

        if (WIFEXITED(status)) Func_Make_Process_Set(wait_pid, 1);
        else if (WIFSIGNALED(status)) Func_Make_Process_Set(wait_pid, 4);
        else if (WSTOPSIG(status))
        {
            status = -1;
            switchWait = TRUE;
            Func_Make_Process_Set(wait_pid, 2);
            
            
        }
    } while (wait_count < count2); //전체 기다리는 시간보다 작은 동안에만 반복문 실행
    return status;
}

//각 명령어에 따라 각각의 정수를 반환해주는 함수
int Func_Command_Set(char *cmd)
{
    if (strcmp(cmd, "exit") == FALSE) return 1;
    else if (strcmp(cmd, "cd") == FALSE) return 2;
    else return FALSE;
    
}

//cd 명령어 입력시 실행되는 함수
int Func_cd(int argc, char** argv)
{
    if (argc == 1) //cd만 입력시
    {
        chdir(newShell->shellpw);
        getcwd(newShell->shelldir, sizeof(newShell->shelldir));
        return FALSE;
    }

    if (chdir(argv[1]) == 0) //cd .. 입력시
    {
        getcwd(newShell->shelldir, sizeof(newShell->shelldir));
        return FALSE;
    }
    else //예외처리
    {
        printf("-bash: cd: %s: No such file or directory\n", argv[1]);
        return FALSE;
    }
}

//다음 job을 구할 때 사용하는 함수
int Func_Get_Nextjob()
{
    int i;
    //반복문을 통해 탐색하며 jobs를 구한다
    for (i = 1; i <= 20; i++)
    {
        if (newShell->jobs[i] == NULL) return i;
    }
    return ERROR;
}


//명령어가 끝났는지 끝나지 않았는지 판별하는 함수 
void Func_Check_Zombie()
{
    struct Myshells *ZombieProc, *ZombieProc2;
    //판별에 사용되는 구조체 포인터 변수 
    int status, pid;//값과 id 값 저장 변수 
    int job_id, i;
    int switch1 = TRUE;
    while ((pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED)) > 0)
    {
        if (WIFEXITED(status)) Func_Make_Process_Set(pid, 1);
        else if (WIFSTOPPED(status)) Func_Make_Process_Set(pid, 2);
        else if (WIFCONTINUED(status)) Func_Make_Process_Set(pid, 3);

        //탐색하여 id 저장 
        for (i = 1; i <= 20; i = i + 1)
        {
            if (newShell->jobs[i] != NULL)
            {
                for (ZombieProc = newShell->jobs[i]->newjob; ZombieProc != NULL; ZombieProc = ZombieProc->next)
                {
                    if (ZombieProc->pid == pid) job_id = i;
                }
            }
        }
        //탐색 후 끝났는지 끝나지 않았는지 스위치 변수에 저장
        for (ZombieProc2 = newShell->jobs[job_id]->newjob; ZombieProc2 != NULL; ZombieProc2 = ZombieProc2->next)
        {
            if (ZombieProc2->status != 1) switch1 = FALSE;
        }

        
    }
}

//명령어 실행 함수 
int Func_EXE(struct Myshells *exeProc)
{
    int switchCmd = TRUE;
    int numCmd;
    //각 명령어에 맞는 함수를 호출한다. 
    switch (exeProc->cmd_mode)
    {
        case 1:
            //exit 명령어
            numCmd = 1;
            exit(0);
            break;

        case 2:
            //cd 명령어
            Func_cd(exeProc->argc, exeProc->argv);
            numCmd = 2;
            break;
        
        default:
            switchCmd = FALSE;
            break;
    }

    return switchCmd;
}


//프로세스 시작 함수
int Func_Start_Process(struct JobStruct *job, struct Myshells *StartProc, int input_st, int output_st, int mode)
{
    pid_t ChildProcess; //fork를 통해 만들어지는 자식프로세스 저장 변수 
    int status = 0; //각 명령어의 모드 저장 변수
    int count1 = 0, count2 = 0;
    int switchP = FALSE, switchEND = FALSE; //명령어에 대한 스위치 변수 

    //초기화 
    StartProc->status = 0;
    if (StartProc->cmd_mode != 0 && Func_EXE(StartProc)) return FALSE;

    ChildProcess = fork(); //fork로 생성

    if (ChildProcess < 0) return ERROR;//예외처리
    else if (ChildProcess == 0)
    {
        switchP = TRUE;
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        StartProc->pid = getpid();
        if (job->job_pid > 0) setpgid(0, job->job_pid);
        else
        {
            switchP = TRUE;
            job->job_pid = StartProc->pid;
            setpgid(0, job->job_pid);
        }

        count1 = status + input_st;
        count2 = status + output_st;

        if (input_st != 0)
        {
            switchP = TRUE;
            dup2(input_st, 0);
            close(input_st);
        }

        if (output_st != 1)
        {
            dup2(output_st, 1);
            switchP = TRUE;
            close(output_st);
        }

        if (execvp(StartProc->argv[0], StartProc->argv) < 0) //예외 처리 
        {
            printf("-bash: %s: command not found\n", StartProc->argv[0]);
            switchP = TRUE;
            exit(0);
        }

        if(switchP == TRUE)
        {
            count1 = TRUE;
        }

        exit(0);
    }
    else
    {
        StartProc->pid = ChildProcess;
        if (job->job_pid > 0) setpgid(ChildProcess, job->job_pid);
        else
        {
            job->job_pid = StartProc->pid;
            setpgid(ChildProcess, job->job_pid);
        }

        if (mode == 1) //background 명령어라면 
        {
            tcsetpgrp(0, job->job_pid);
            status = Func_Wait_Job(job->id);
            switchEND = TRUE;
            signal(SIGTTOU, SIG_IGN);
            tcsetpgrp(0, getpid());
            signal(SIGTTOU, SIG_DFL);
        }
    }

    return status;
}


int Func_exe_job(struct JobStruct *job)
{
    struct Myshells *JobProcess;
    struct Myshells *newJobProc;
    //사용되는 구조체와 변수 
    int status = 0, fpint = 0, fdArr[2], job_id = -1;
    int id, fpout = 1, switchJob = FALSE, switchpr;
    int count1 = 0, count2 = 0, tempcount = 0;

    Func_Check_Zombie(); //명령어가 끝났는지 아닌지 확인 
    if (job->newjob->cmd_mode == 0)
    {
        id = Func_Get_Nextjob();
        job->id = id;
        newShell->jobs[id] = job;
        job_id = id;
        switchJob = TRUE;
    }
    
    //구조체 탐색
    for (JobProcess = job->newjob; JobProcess != NULL; JobProcess = JobProcess->next)
    {
        if (JobProcess == job->newjob && JobProcess->input_path != NULL)
        {
            tempcount = 0;
            fpint = open(JobProcess->input_path, O_RDONLY);
            
            
        }
        if (JobProcess->next != NULL) //다음 노드가 없으면 
        {
            pipe(fdArr);
            status = Func_Start_Process(job, JobProcess, fpint, fdArr[1], 2);
            close(fdArr[1]);
            fpint = fdArr[0];
        }
        else
        {
            if (JobProcess->output_path != NULL) //다음 노드가 있다면
            {
                count1 = count1 + 1;
                fpout = open(JobProcess->output_path, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); //open 
                switchJob = TRUE;
                if (fpout < 0) fpout = 1;
                
            }

            if(switchJob == TRUE)
            {
                tempcount = count1;
            }
            status = Func_Start_Process(job, JobProcess, fpint, fpout, job->mode);
        }
    }
    return status;
}

//명령어 입력시 표준 명령어로 변환해주는 함수 
struct Myshells* Parse_command_segment(char *segment)
{
    glob_t GB_Proc;
    struct Myshells *new_proc = (struct Myshells*) malloc(sizeof(struct Myshells));
    //명령어 표준화에 사용되는 변수 
    int glob_count = 0, i = 0, q;
    int SizeBuffer = 64, idxCommand = 0;
    char *cmd = strdup(segment), *SegToken; //명령어 한 단어씩 저장 변수 
    char **SegTokens = (char**) malloc(SizeBuffer * sizeof(char*));

    int argc = 0, count1 = 0, temp1 = 0;
    int switchSeg = FALSE, switch2 = FALSE;
    char *input_path = NULL, *output_path = NULL;

    if (!SegTokens) //알맞지 않은 입력시 에러 출력 
    {
        fprintf(stderr, "-bash: allocation error\n");
        switchSeg = TRUE;
        exit(EXIT_FAILURE);
    }

    SegToken = strtok(segment, TOKENARR);
    while (SegToken != NULL) //입력이 끝날때까지 무한 루프 
    {
        if (strchr(SegToken, '*') != NULL || strchr(SegToken, '?') != NULL) //*, ? 입력시 
        {
            glob(SegToken, 0, NULL, &GB_Proc);
            switchSeg = TRUE;
            glob_count = GB_Proc.gl_pathc;
        }

        if (idxCommand + glob_count >= SizeBuffer) //최대 문자열보다 큰 명령어 들어오면 
        {
            SizeBuffer = SizeBuffer + 64;
            SizeBuffer = SizeBuffer + glob_count;
            SegTokens = (char**) realloc(SegTokens, SizeBuffer * sizeof(char*)); //realloc으로 해결 
            if (!SegTokens) //예외처리 
            {
                fprintf(stderr, "-bash: allocation error\n");
                switchSeg = TRUE;
                exit(EXIT_FAILURE);
            }
        }
        if (switchSeg == TRUE)
        {
            count1 = switchSeg;
        }

        if (glob_count > 0)
        {
            for (i = 0; i < glob_count; i++) SegTokens[idxCommand++] = strdup(GB_Proc.gl_pathv[i]);
            globfree(&GB_Proc);
        }
        else
        {
            SegTokens[idxCommand] = SegToken;
            idxCommand = idxCommand + 1;
            switch2 = TRUE;
        }
        SegToken = strtok(NULL, TOKENARR);
    }

    //< , > 인덱스 착기 
    while (i < idxCommand)
    {
        if (SegTokens[i][0] == '<' || SegTokens[i][0] == '>') break;
        i = i + 1;
    }
    argc = i;

    int tempcommand = 0;
    int sizecommand = 0;

    for(q = 0; q < 10; q++)
    {
        tempcommand = tempcommand + q;
    }

    //redirection 
    for (; i < idxCommand; i = i + 1)
    {
        if (SegTokens[i][0] == '<')
        {
            if (strlen(SegTokens[i]) == 1)
            {
                //파일 생성
                sizecommand = strlen(SegTokens[i]);
                input_path = (char *) malloc((strlen(SegTokens[i + 1]) + 1) * sizeof(char));
                strcpy(input_path, SegTokens[i + 1]);
                switch2 = TRUE;
                i = i + 1;
            }
            else
            {
                input_path = (char *) malloc(strlen(SegTokens[i]) * sizeof(char));
                strcpy(input_path, SegTokens[i] + 1);
            }
        }
        else if (SegTokens[i][0] == '>')
        {
            if (strlen(SegTokens[i]) == 1)
            {
                //파일 out
                sizecommand = strlen(SegTokens[i]);
                output_path = (char *) malloc((strlen(SegTokens[i + 1]) + 1) * sizeof(char));
                strcpy(output_path, SegTokens[i + 1]);
                switch2 = TRUE;
                i = i + 1;
            }
            else
            {
                output_path = (char *) malloc(strlen(SegTokens[i]) * sizeof(char));
                strcpy(output_path, SegTokens[i] + 1);
            }
        }
        else break;

        if(switch2 == TRUE)
        {
            tempcommand = ERROR;
        }
        
    }

    for (i = argc; i <= idxCommand; i = i + 1) SegTokens[i] = NULL;
    //위에서 표준화한 명령어를 새로운 변수에 다시 저장한다.
    new_proc->argv = SegTokens;
    new_proc->cmd = cmd;
    new_proc->argc = argc;
    new_proc->pid = -1;
    new_proc->input_path = input_path;
    new_proc->cmd_mode = Func_Command_Set(SegTokens[0]);
    new_proc->next = NULL;
    new_proc->output_path = output_path;
    
    return new_proc;
    //그 구조체를 리턴시켜 새로운 명령에 사용한다. 
}

//입력값 처리 하는 함수 
struct JobStruct* Parse_Command(char *line)
{
    struct Myshells *RProc = NULL, *CProc = NULL;
    struct JobStruct *new_job = (struct JobStruct*) malloc(sizeof(struct JobStruct));
    //함수에 사용되는 구조체 변수

    char *headArray = line, *seg;
    char *commandline = strdup(line);
    char *tailArray = line + strlen(line);

    int sizecommand, lenseg = 0, mode = 1, switch3 = FALSE;
    //공백이 나올 때 까지 시작과 끝으로 부터 이동시킨다.
    while (*headArray == ' ') headArray++;
    while (*tailArray == ' ') tailArray--;
    *(tailArray + 1) = '\0';

    line = headArray;
    sizecommand = strlen(line);
    
    char *idxLine = line, *c = line;
    int countcommand = 0;

    while (1)
    {
        if (*c == '\0' || *c == '|') //\0 혹은 파이프라인 일 경우 
        {
            seg = (char*) malloc((lenseg + 1) * sizeof(char));
            countcommand = countcommand + 1;
            strncpy(seg, idxLine, lenseg);
            seg[lenseg] = '\0';

            struct Myshells* new_proc = Parse_command_segment(seg);
            //결과물 저장 구조체  
            if (!RProc)
            {
                switch3 = TRUE;
                RProc = new_proc;
                CProc = RProc;
            }
            else
            {
                CProc->next = new_proc;
                CProc = new_proc;
            }

            if (*c != '\0')
            {
                idxLine = c;
                switch3 = TRUE;
                while (*(++idxLine) == ' ');
                c = idxLine;
                countcommand = countcommand + 1;
                lenseg = 0;
                continue;
            }
            else break;
        }
        else
        {
            lenseg = lenseg + 1;
            c = c + 1;
        }
    }
    //새로운 구조체에 위의 결과들을 입력 
    new_job->mode = mode;
    new_job->cmd = commandline;
    new_job->newjob = RProc;
    new_job->job_pid = -1;
    
    return new_job;
}

//명령어 입력 함수 
char* Func_InputLine()
{
    int LineSize = 1024, idx = 0, wordLine;
    char *newLine = malloc(sizeof(char) * LineSize); //동적할당 
    int count1 = 0, count2 = 0;

    if (!newLine) //예외처리 
    {
        fprintf(stderr, "-bash: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        wordLine = getchar(); //입력 

        if (wordLine == EOF || wordLine == '\n') //엔터 혹은 eof 입력시까지 
        {
            newLine[idx] = '\0';
            count1 = count1 + 1;
            return newLine;
        }
        else newLine[idx] = wordLine;
        
        idx = idx + 1;
        count1 = count1 + 1;

        if (idx >= LineSize) //범위 초과하여 입력할 경우 relloc으로 해결 
        {
            LineSize = LineSize + 1024;
            count2 = count2 + LineSize;

            newLine = realloc(newLine, LineSize);
            if (!newLine)
            {
                fprintf(stderr, "-bash: allocation error\n");
                count2 = count2 + 1;
                exit(EXIT_FAILURE);
            }
        }
        count1 = 0;
        count2 = 0;
    }
}

void Func_sig()
{
    struct sigaction sigint_action =
    {
        .sa_handler = &sigint_handler,
        .sa_flags = 0
    };
    sigemptyset(&sigint_action.sa_mask);
    sigaction(SIGINT, &sigint_action, NULL);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
}

int main(int argc, char **argv)
{
    struct passwd *pw = getpwuid(getuid());

    pid_t pid = getpid();
    int i, CommandClass = 1;
    int sizeofCommand = 0;
    char *InputLine;
    struct JobStruct *mainjob;

    Func_sig();

    setpgid(pid, pid);
    tcsetpgrp(0, pid);

    newShell = (struct Shells*) malloc(sizeof(struct Shells));
    getlogin_r(newShell->shellUser, sizeof(newShell->shellUser));
    strcpy(newShell->shellpw, pw->pw_dir);

    for (i = 0; i < 20; i = i + 1) newShell->jobs[i] = NULL;
    getcwd(newShell->shelldir, sizeof(newShell->shelldir));

    while (1)
    {
        printf("CSE4100-SP-P4> ");
        InputLine = Func_InputLine();
        sizeofCommand = strlen(InputLine);
        if (sizeofCommand == 0)
        {
            Func_Check_Zombie();
            continue;
        }
        mainjob = Parse_Command(InputLine);
        CommandClass = Func_exe_job(mainjob);
    }

    return EXIT_SUCCESS;
}
