#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
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

  /// Functia MAN ce ofera un manual de utilizare al shell-ului

  void man(){
      printf("MANUAL\n");
      printf("01. MAN: shows all the commands\n");
      printf("    Command: man\n");
      printf("02. HISTORY: shows all the commands that ran in the current session\n");
      printf("    Command: history\n");
      printf("03. CLEAR: clears the terminal\n");
      printf("    Command: clear\n");
      printf("04. PWD: prints the path of the current directory\n");
      printf("    Command: pwd\n");
      printf("05. LS: lists all files and directories of the current directory\n");
      printf("    Command: ls\n");
      printf("06. CD: changes the current directory\n");
      printf("    Command: cd new_path\n");
      printf("07. TOUCH: creates a new empty file\n");
      printf("    Command: touch file_name\n");
      printf("08. RM: removes a file\n");
      printf("    Command: rm file_name\n");
      printf("09. CP: copies the content of a file (first file in the command) to another file (second file in the command)\n");
      printf("    Command: cp source_file destination_file\n");
      printf("10. MKDIR: creates a new empty directory\n");
      printf("    Command: mkdir directory_name\n");
      printf("11. RMDIR: removes a directory\n");
      printf("    Command: rmdir directory_name\n");
      printf("12. ECHO: displays the string given as an argument\n");
      printf("    Command: echo string\n");
      printf("13. MV: moves a file to a specific destinatoin\n");
      printf("    Command: mv file_name destination\n");
      printf("14. DF: display current directory's system disk space usage\n");
      printf("    Command: df -m\n");
      printf("15. QUIT: exit\n");
      printf("    Command: quit\n");
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

int parseCommand (char *str, char arr[100][512]){
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

bool cp (char* inFile, char* outFile){
    int n, inF, outF;
    char* buf = (char *)malloc(1024 * charSize);

    /// Daca nu putem deschide fisierele, returnam o eroare

    inF = open(inFile, O_RDONLY);
    if (inF < 0){
        perror("Could not open the in file");
        return errno;
    }

    outF = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (outF < 0){
        perror("Could not open the out file");
        return errno;
    }

    /// Citim caracter cu caracter fisierul si il copiem in celalalt

    n = read(inF, buf, 1024 * charSize);

    while (n > 0){
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

 /// Functia CD pentru schimbarea directorului

void cd(char* f){

    if (chdir(f))
    {
        perror("Could not change the directory");
        exit(0);
    }
}

 /// Functie ce imparte comanda in cuvinte pentru a apela exec

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

 /// Functie pentru tratarea cazurilor cu pipe (|)

void handlePipe(char *command) {

    /// 0 (in) este pentru citire, iar 1 (out) este pentru scriere

    int fd[2], oldPipe = STDIN_FILENO;
    pid_t pid, pidOriginal;

    /// Daca introduci 101 comenzi, iti dau 10 lei

    char *clearPipe[100];
    int n;

    for(n = 0; n < 100; n++) {
        clearPipe[n] = strsep(&command, "|");

        if(clearPipe[n] == NULL)
            break;
    }

    pidOriginal = fork();

    if(pidOriginal < 0) {

        perror("Something went wrong!");
        exit(0);

    } else if(pidOriginal == 0) {

            for(int i = 0; i < n - 1; i++) {

            // Mergem pana la n - 1 deoarece ultima comanda se va executa in parinte

            char **commandExec = (char **)malloc(1024 * 1024 * charSize);
            parseCommandForExec(clearPipe[i], commandExec);

            if(pipe(fd) < 0) {
                perror("Could not open the pipe!");
                exit(0);
            }

            pid = fork();

            if(pid < 0) {

                perror("Fork error");
                exit(0);

            } else if(pid == 0) {

                /// 'Rotim' pipe-ul vechi

                if(oldPipe != STDIN_FILENO) {
                    dup2(oldPipe, STDIN_FILENO);
                    close(oldPipe);
                }

                /// Acest proces va scrie in pipe-ul curent, nu necesita citire

                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);

                if(execvp(commandExec[0], commandExec) < 0) {
                    perror("Error runing one of the commands!");
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
            perror("Error runing the last command!");
            exit(0);
        }
    } else {

        wait(NULL);
    }
}

 /// Functie pentru a verifica daca avem cazul unui pipe (|)

bool checkForPipe(char *command) {
    char *p = strstr(command, "|");
    return !(p == NULL);
}

/// Functie magica ce utilizeaza execvp pentru a trata comenzi precum ls etc.

void unlimitedPower(char command[100][512], int dim) {

    char *nameThis[100];
    int j;

    for(j = 0; j < dim; j++)
        nameThis[j] = command[j];

    nameThis[j] = NULL;

    pid_t pid = fork();

    if(pid < 0) {
        perror("Fork error sir");
        exit(0);

    } else if(pid == 0) {

        if(execvp(nameThis[0], nameThis) < 0) {
            perror("Command not found");
            exit(0);
        }
    } else {

        wait(NULL);
    }
}

 /// Functie ce decide ce comanda va fi executata la fiecare instructiune

void allCommands(char *command, int history)
{
    if (readInput(command, history)) {
        //Daca avem pipe, apelam functia ce trateaza acest caz, altfel verificam ce comanda avem

        if(checkForPipe(command)) {
            handlePipe(command);

        } else {

            char parsed[100][512];
            int dim = 0;
            dim = parseCommand(command, parsed);

            if (strcmp(parsed[0], "clear") == 0) {
                if (dim != 1){
                    printf("Incorrect command! Check our manual -> MAN\n");
                    man();
                }
                else clearCommand();

            } else if(strcmp(parsed[0], "history") == 0) {
                if (dim != 1){
                    printf("Incorrect command! Check our manual -> MAN\n");
                    man();
                }
                else showHistory();

            } else if (strcmp(parsed[0], "cp") == 0) {
                if (dim != 3){
                    printf("Incorrect command! Check our manual -> MAN\n");
                    man();
                }
                else cp(parsed[1], parsed[2]);

            } else if (strcmp(parsed[0], "touch") == 0){
                if (dim != 2){
                    printf("Incorrect command! Check our manual -> MAN\n");
                    man();
                }
                else touch(parsed[1]);

            } else if (strcmp(parsed[0], "man") == 0){
                if (dim != 1){
                    printf("Incorrect command! Check our manual -> MAN\n");
                    man();
                }
                else man();

            } else if (strcmp(parsed[0], "cd") == 0){
                if (dim != 2){
                    printf("Incorrect command! Check our manual -> MAN\n");
                    man();
                }
                else cd(parsed[1]);

            } else if (strcmp(parsed[0], "quit") == 0){
                if (dim != 1){
                    printf("Incorrect command! Check our manual -> MAN\n");
                    man();
                }
                else { printf("\n"); exit(0);}

            } else {
                unlimitedPower(parsed, dim);
            }
        }
    }
}

 /// Functie frumoasa

 void welcome()
{
	printf("Welcome to Hedgehog Shell!\n\n");
	printf("  .|||||||||.          .|||||||||.\n");
	printf(" |||||||||||||        |||||||||||||\n");
	printf("|||||||||||' .\      /. `|||||||||||\n");
	printf("`||||||||||_,__o    o__,_||||||||||'\n\n");
	sleep(2);
}

int main(int arg, char **argv) {

    clearCommand();
    welcome();

    int history = open("shellHistory", O_RDWR | O_CREAT, S_IRWXU);

    char *command = (char *)malloc(512 * charSize);

    while (true) {
        printShellLine();
        allCommands(command, history);
    }

    return 0;
}
