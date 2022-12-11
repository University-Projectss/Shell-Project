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

int handlePipe(char *command) {
    int fd[2]; //0 is for reading, 1 is for writing
    pid_t pid1, pid2;

    //for now I will handle only one pipe
    if(pipe(fd) < 0) {
        perror("Could not open the pipe");
        return errno;
    }

    char *clearPipe[2];
    for(int i = 0; i < 2; i++) {
        clearPipe[i] = strsep(&command, "|");
    }

    char **firstCommand = (char **)malloc(1024 * 1024 * charSize),
     **secondCommand = (char **)malloc(1024 * 1024 * charSize);
    parseCommandForExec(clearPipe[0], firstCommand);
    parseCommandForExec(clearPipe[1], secondCommand);

    pid1 = fork();

    if(pid1 < 0) {
        perror("Fork error first proccess");
        return errno;
    } else if(pid1 == 0) {
        //first child, is only writing so we close the read descriptor
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        if(firstCommand[0] == NULL || strlen(firstCommand[0]) < 2) {
            printf("Invalid command\n");
            return 0;
        }
        if(execvp(firstCommand[0], firstCommand) < 0) {
            perror("Error runing the first command");
            return errno;
        }
    } else {
        //parent process, born the second
        pid2 = fork();

        if(pid2 < 0) {
            perror("Fork error second proccess");
            return errno;
        } else if(pid2 == 0) {
            //second child, is only reading so we close the write descriptor
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            if(secondCommand[0] == NULL) {
                printf("Invalid command\n");
                return 0;
            }
            if(execvp(secondCommand[0], secondCommand) < 0) {
                perror("Error runing the second command");
                return errno;
            }

        } else {
            //back on parent
            close(fd[0]);
            close(fd[1]);
            wait(NULL);
            wait(NULL);
        }
    }
    return 1;
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
