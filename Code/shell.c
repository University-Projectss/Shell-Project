#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define charSize sizeof(char)
#define intSize sizeof(int)
#define longSize sizeof(long)

#define maxSizeCommand 1000

 /// Functie pentru conversia unui String in Int

 int ourAtoi(const char ourString[]) {
     int number = 0;
     int i = 0;
     int sign = 1;

     while (ourString[i] == ' ' || ourString[i] == '\n' || ourString[i] == '\t') i++;

     if (ourString[i] == '+' || ourString[i] == '-')
         if (ourString[i] == '-')
             sign = -1;

     i++;

     while (ourString[i] && (ourString[i] >= '0' && ourString[i] <= '9')) {
         number = number * 10 + (ourString[i] - '0');
         i++;
     }

     return sign * number;
 }

 /// Functie ce genereaza o noua linie in Shell

void printShellLine() {
    char *path = (char *)malloc(512 * charSize);
    getcwd(path, 500);
    printf("%s ~ >> ", path);
}

 /// Functie ce citeste comanda introdusa

bool readInput(char *command, int file) {
    char *buff = (char *)malloc(strlen(command) * charSize);
    fgets(buff, 512, stdin);

    /// Daca nu avem o comanda, nu avem ce afisa

    if (strcmp(buff, "\n") == 0)
        return false;

    buff[strlen(buff) - 1] = '\0';
    strcpy(command, buff);

    int n = write(file, command, strlen(command));
    int m = write(file, "\n", sizeof(char));
    if( n < 0 || m < 0 ) {
        perror("Could not save the command in history");
        return errno;
    }

    return true;
}

 /// Functie ce imparte comanda in cuvinte

int parseCommand (char *str, char arr[100][512])
{
    char *token;
    int i=0;

    token = strtok(str, " ");

    while(token != NULL ) {
        strcpy(arr[i++], token);
        token = strtok(NULL, " ");
    }
    return i;
}

 /// Functia CLEAR ce elibereaza Shell-ul

void clearCommand() {

    /// Pe Mac avem clear
    /// Pe Linux si Windows avem cls

    system("clear");
}

 /// Functie ce afiseaza istoricul comenzilor

void showHistory() {
    char buff[513];
    int n, index = 1;
    int file = open("shellHistory", O_RDONLY);

    n = read(file, &buff, sizeof(buff));
    char *p = strtok(buff, "\n");
    while(p && strlen(p) > 0) {
        printf("%d %s\n", index++, p);
        p = strtok(NULL, "\n");
    }
}

 /// Functia CP pentru copierea unui fisier in altul

bool cp (char* inFile, char* outFile)
{
    int n, inF, outF;
    char* buf = (char *)malloc(1024 * charSize);

    /// Daca nu putem deschide fisierele, returnam o eroare

    inF = open(inFile, O_RDONLY);
    if (inF < 0)
    {
        perror("Could not open the in file");
        return errno;
    }

    outF = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (outF < 0)
    {
        perror("Could not open the out file");
        return errno;
    }

    /// Citim caracter cu caracter fisierul si il copiem in celalalt

    n = read(inF, buf, 1024 * charSize);
    
    while (n > 0)
    {
        write(outF, buf, strlen(buf));
        n = read(inF, buf, 1024 * charSize);
    }

    return true;
}

 /// Functia TOUCH pentru crearea unui fisier

bool touch(const char* file ){
    /// In file avem numele fisierului (+ extensie) pe care il vom crea

    // In success este returnat un file descriptor la fisierul creat:
    // - punem flag-ul O_CREAT pentru a crea fisierul
    // - flag-urile S_IRWXU, S_IRWXG, S_IRWXO sunt pentru a da permisiuni de citire, scriere si executare user-ilor, grupurilor si others

    int success = open(file, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

    // Success va avea valoarea -1 doar daca a avut loc o eroare la crearea file descriptor-ului

    if(success == -1)
        return false;

    // Inchidem file descriptor-ul

    close(success);

    return true;
}

void parseCommandForExec(char* str, char** command) {
    char *p = (char *)malloc(strlen(str) * charSize), *tok;
    int i = 1;
    strcpy(p, str);
    tok = strtok(p, " ");
    command[0] = tok;
    
    while(tok != NULL) {
        tok = strtok(NULL, " ");
        command[i++] = tok;
    }
}

void handlePipe(char *command) {
    int fd[2], oldPipe = STDIN_FILENO; //0(in) is for reading, 1(out) is for writing
    //oldPipe is IN because every proccess will read from the pipe behind 
    pid_t pid;

    char *clearPipe[100];   //if you enter 101 commands I gave you 10 lei
    int n;
    for(n = 0; n < 100; n++) {
        clearPipe[n] = strsep(&command, "|");

        if(clearPipe[n] == NULL)
            break;
    }

    for(int i = 0; i < n - 1; i++) {//n - 1 cus the last command will execute in parent

        char **commandExec = (char **)malloc(1024 * 1024 * charSize);
        parseCommandForExec(clearPipe[i], commandExec);

        if(pipe(fd) < 0) {
            perror("Could not open the pipe");
            exit(0);
        }

        pid = fork();
        if(pid < 0) {
            perror("Fork error");
            exit(0);
        }else if(pid == 0) {
            //'rotate' the old pipe
            if(oldPipe != STDIN_FILENO) {
                dup2(oldPipe, STDIN_FILENO);
                close(oldPipe);
            }

            //this proccess will write in the current pipe, there's no need to read
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);

            if(execvp(commandExec[0], commandExec) < 0) {
                perror("Error runing one of the commands.");
                exit(0);
            }
        } else {
            close(oldPipe);
            close(fd[1]);
            oldPipe = fd[0];
        }
    }

    for(int i = 0; i < n - 1; i++)
        wait(NULL);

    if(oldPipe != STDIN_FILENO) {
        dup2(oldPipe, STDIN_FILENO);
        close(oldPipe);
    }

    char **commandExec = (char **)malloc(1024 * 1024 * charSize);
    parseCommandForExec(clearPipe[n - 1], commandExec);

    if(execvp(commandExec[0], commandExec) < 0) {
        perror("Error runing the last command.");
        exit(0);
    }
}

bool checkForPipe(char *command) {
    char *p = strstr(command, "|");
    return !(p == NULL);
}

void allCommands(char *command, int history)
{
    if (readInput(command, history)) {
        if(checkForPipe(command)) {
            //we have the pipe
            handlePipe(command);
        } else {
            //we don't have pipe
            char parsed[100][512];
            int dim = 0;
            dim = parseCommand(command, parsed);
            if (strcmp(parsed[0], "clear") == 0) {
                clearCommand();
            } else if(strcmp(parsed[0], "history") == 0) {
                showHistory();
            } else if (strcmp(parsed[0], "cp") == 0) {
                cp(parsed[1], parsed[2]);
            } else if (strcmp(parsed[0], "touch") == 0){
                touch(parsed[1]);
            }
            else {
                printf("Command not found\n");
            }
        }
    }
}

int main(int arg, char **argv) {
    int history = open("shellHistory", O_RDWR | O_CREAT, S_IRWXU);
    
    char *command = (char *)malloc(512 * charSize);
    while (true) {
        printShellLine();
        allCommands(command, history);
    }

    return 0;
}
