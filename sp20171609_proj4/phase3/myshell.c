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

//job 입력시 구조체 안의 내용을 출력시켜주는 함수
int Func_Print_JobSet(int id)
{
    struct Myshells *PrintProc;
    //출력에 사용되는 구조체포인터 변수

    int count = 0;
    int switchp;
    

    if (id > 20 || newShell->jobs[id] == NULL) return ERROR; //예외처리
    printf("[%d]", id);
    count = 0;
    switchp = FALSE;
    //노드 탐색 후 알맞은 형식에 따라 출력
    for (PrintProc = newShell->jobs[id]->newjob; PrintProc != NULL; PrintProc = PrintProc->next)
    {
        if(switchp == FALSE)
        {
            printf("\t%d\t%s\t%s", PrintProc->pid, SetArray[PrintProc->status], PrintProc->cmd);
            switchp = TRUE;
        }
        else printf("%s", PrintProc->cmd);

        count = count + 1;
        if (PrintProc->next != NULL) printf(" | ");
        else printf("\n");
    }
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

int Func_Make_Job_Set(int id, int status) //job의 set(status)을 형성시켜주는 함수
{
    struct Myshells *MakesetProc;
    //shell 구조체 포인터 변수

    int i, count = 0; //탐색에 사용되는 변수
    if (id > 20 || newShell->jobs[id] == NULL) return ERROR; //예외처리

    for(i=0; i < 20; i = i + 1)
    {
        count = id;
        count = count + status;
    }
    //구조체 변수의 status를 대입시켜줌
    for (MakesetProc = newShell->jobs[id]->newjob; MakesetProc != NULL; MakesetProc = MakesetProc->next)
    {
        if (MakesetProc->status != 1) MakesetProc->status = status;
    }

    return FALSE;
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
            if (wait_count == count2) Func_Print_JobSet(id);
            
        }
    } while (wait_count < count2); //전체 기다리는 시간보다 작은 동안에만 반복문 실행
    return status;
}

//각 명령어에 따라 각각의 정수를 반환해주는 함수
int Func_Command_Set(char *cmd)
{
    if (strcmp(cmd, "exit") == FALSE) return 1;
    else if (strcmp(cmd, "cd") == FALSE) return 2;
    else if (strcmp(cmd, "jobs") == FALSE) return 3;
    else if (strcmp(cmd, "fg") == FALSE) return 4;
    else if (strcmp(cmd, "bg") == FALSE) return 5;
    else if (strcmp(cmd, "kill") == FALSE) return 6;
    else return FALSE;
    
}

//pid 구하는 함수
int Func_Get_Pid(int id)
{
    struct JobStruct *inputJob;

    if(id > 20) inputJob = NULL; //예외처리
    else inputJob = newShell->jobs[id]; //id값 대입
 
    if (inputJob == NULL) return ERROR;
    return inputJob->job_pid;
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

//jobs 입력시
int Func_jobs(int argc, char **argv)
{
    int i;
    for (i = 0; i < 20; i++)
    {
        if (newShell->jobs[i] != NULL) Func_Print_JobSet(i); //job 구조체를 모두 출력하는 함수 호출
    }

    return FALSE;
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

//foreground, fg를 입력하였을 때 실행되는 함수
int Func_fg(int argc, char **argv)
{
    pid_t pid;
    int status = 0, temp, job_id = ERROR;
    int count1 = 0, count2 = 0;
    int switchfg = FALSE; //fg의 가능 여부 스위치 변수

    if (argc < 2) //fg만 입력하면
    {
        printf("-bash: fg: current: no such job\n");
        return ERROR;
    }

    if (argv[1][0] == '%') //%뒤에 숫자가 들어오면
    {
        job_id = atoi(argv[1] + 1); //문자를 숫자로 변환하여 
        pid = Func_Get_Pid(job_id); //함수 호출
        if (pid < 0) //음수이면 예외처리
        {
            printf("-bash: fg: %s: no such job\n", argv[1]);
            count1 = pid;
            switchfg = TRUE;
            return ERROR;
        }
    }
    else pid = atoi(argv[1]); //%가 아니라 숫자로 입력이 된다면 pid에 저장 

    if (kill(-pid, SIGCONT) < 0) //pid가 음수여도 예외 처리 
    {
        printf("-bash: fg: %d: no such job\n", pid);
        return ERROR;
    }

    tcsetpgrp(0, pid);
    count2 = count2 + 1;

    if (job_id > 0) //id가 양수면
    {
        Func_Make_Job_Set(job_id, 3); //fg함수 실행 
        switchfg = TRUE;
        Func_Print_JobSet(job_id); //결과 출력  
        if (Func_Wait_Job(job_id) >= 0) Func_Free_Job(job_id);
        //free
    }
    else
    {
        waitpid(pid, &status, WUNTRACED);
        switchfg = FALSE;
        count1 = pid;
        if (WIFEXITED(status)) Func_Make_Process_Set(pid, 1);
        else if (WIFSIGNALED(status)) Func_Make_Process_Set(pid, 4);
        else if (WSTOPSIG(status))
        {
            status = ERROR;
            Func_Make_Process_Set(pid, 2);
        }
    }
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(0, getpid());
    signal(SIGTTOU, SIG_DFL);
    count1 = 0;
    count2 = 0;

    return FALSE;
}


//bg, background 명령어 입력시 
int Func_bg(int argc, char **argv)
{
    pid_t pid;
    int job_id = ERROR, i;
    int switchbg = FALSE, countbg = 0;

    if (argc < 2) //bg만 입력
    {
        printf("-bash: bg: current: no such job\n");
        switchbg = TRUE;
        return ERROR;
        
    }

    if (argv[1][0] == '%') //%뒤에 숫자 입력
    {
        job_id = atoi(argv[1] + 1); //숫자로 변환
        pid = Func_Get_Pid(job_id); //함수 호출하여 pid 계산
        if (pid < 0) //음수면 예외처리
        {
            printf("-bash: bg: %s: no such job\n", argv[1]);
            switchbg = TRUE;
            return ERROR;
        }
    }
    else pid = atoi(argv[1]); //% 아니면 pid로 입력

    for(i = 0; i < 10; i = i + 1)
    {
        countbg = countbg + i;
    }
    //예외처리 
    if (kill(-pid, SIGCONT) < 0)
    {
        printf("-bash: bg: %d: no such job\n", pid);
        switchbg = TRUE;
        return ERROR;
    }

    if (job_id > 0)
    {
        //올바른 입력이 들어오면 함수 실행 후 출력 
        Func_Make_Job_Set(job_id, 3);
        Func_Print_JobSet(job_id);
    }

    return FALSE;
}

//kill 명령어 입력시 
int Func_kill(int argc, char **argv)
{
    pid_t pid;
    int job_id = ERROR;

    if (argc < 2) //kill만 입력시 예외출력
    {
        printf("kill: usage: kill [-s sigspec | -n signum | -sigspec] pid | jobspec ... or kill -l [sigspec]\n");
        return ERROR;
    }

    if (argv[1][0] == '%') //%뒤에 숫자입력
    {
        job_id = atoi(argv[1] + 1); //숫자 저장
        pid = Func_Get_Pid(job_id);
        if (pid < 0) //예외처리
        {
            printf("-bash: kill: %s: no such job\n", argv[1]);
            return ERROR;
        }
        pid = -pid;
    }
    else pid = atoi(argv[1]); //프로세스 번호 저장

    //예외처리
    if (kill(pid, SIGKILL) < 0)
    {
        printf("-bash: kill: %d: no such job\n", pid);
        return FALSE;
    }

    if (job_id > 0)
    {
        //올바른 입력 시 함수 호출 및 출력 실행 
        Func_Make_Job_Set(job_id, 4);
        Func_Print_JobSet(job_id);
        if (Func_Wait_Job(job_id) >= 0) Func_Free_Job(job_id);
        
    }

    return TRUE;
}

//job이 완료되었는지 확인하는 함수 
int Func_Complete(int id)
{
    struct Myshells *ComProc;
    if (id > 20 || newShell->jobs[id] == NULL) return FALSE; //예외처리
    
    for (ComProc = newShell->jobs[id]->newjob; ComProc != NULL; ComProc = ComProc->next)
    {
        if (ComProc->status != TRUE) return FALSE;
    }
    return TRUE;
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
        
        if (job_id > 0 && Func_Complete(job_id))
        {
            Func_Print_JobSet(job_id);
            Func_Free_Job(job_id);
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

        case 3:
            //jobs 명령어
            Func_jobs(exeProc->argc, exeProc->argv);
            numCmd = 3;
            break;

        case 4:
            //fg 명령어
            Func_fg(exeProc->argc, exeProc->argv);
            numCmd = 4;
            break;

        case 5:
            //bg 명령어
            Func_bg(exeProc->argc, exeProc->argv);
            numCmd = 5;
            break;

        case 6:
            //kill 명령어
            Func_kill(exeProc->argc, exeProc->argv);
            numCmd = 6;
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
            
            if (fpint < 0) //open 실패시 예외 처리 
            {
                printf("-bash: no such file or directory: %s\n", JobProcess->input_path);
                Func_Free_Job(job_id);
                return ERROR;
            }
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

    
    switchpr  = FALSE;
    if (job->newjob->cmd_mode == 0)
    {
        if (status >= 0 && job->mode == 1) Func_Free_Job(job_id);
        else if (job->mode == 0)
        {
            printf("[%d]", job_id);
            for (newJobProc = newShell->jobs[job_id]->newjob; newJobProc != NULL; newJobProc = newJobProc->next)
            {
                if(switchpr == FALSE) printf(" %d", newJobProc->pid);
                switchpr = TRUE;
            }
            printf("\n");
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

    //백그라운드면 mode 는 0, &은 \0으로 변환 
    if (line[strlen(line) - 1] == '&')
    {
        mode = 0;
        line[strlen(line) - 1] = '\0';
    }

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

//시그널 함수 
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
    //디렉토리 위치 저장 구조체

    pid_t pid = getpid();
    int i, CommandClass = 1;
    int sizeofCommand = 0;
    char *InputLine;
    struct JobStruct *mainjob;

    //시그널 함수 호출 
    Func_sig();

    setpgid(pid, pid);
    tcsetpgrp(0, pid);

    newShell = (struct Shells*) malloc(sizeof(struct Shells));
    getlogin_r(newShell->shellUser, sizeof(newShell->shellUser));
    strcpy(newShell->shellpw, pw->pw_dir);

    for (i = 0; i < 20; i = i + 1) newShell->jobs[i] = NULL;
    getcwd(newShell->shelldir, sizeof(newShell->shelldir));

    //exit 명령어 입력까지 무한루프 
    while (1)
    {
        printf("CSE4100-SP-P4> ");
        InputLine = Func_InputLine(); //명령어 input 함수 호출 
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
