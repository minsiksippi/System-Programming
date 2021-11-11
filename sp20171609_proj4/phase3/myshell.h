#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <glob.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define TOKENARR " \t\r\n\a"

const char* SetArray[] =
{
    "Running",
    "Done",
    "Stopped",
    "Continued",
    "Terminated"
};

struct Myshells
{
    pid_t pid;
    char *cmd, *input_path, *output_path;
    int argc, cmd_mode, status;
    char **argv;
    struct Myshells *next;
};

struct JobStruct
{
    pid_t job_pid;
    int id;
    struct Myshells *newjob;
    char *cmd;
    int mode;
};

struct Shells
{
    char shellpw[1024], shelldir[1024], shellUser[64];
    struct JobStruct *jobs[21];
};
struct Shells *newShell;