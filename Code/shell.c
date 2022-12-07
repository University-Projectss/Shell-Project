#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#define charSize sizeof(char)

//prost eu, nu merge cum imi imaginam, scuze irina
// int ourAtoi(const char ourString[]) {
//     int number = 0;
//     int i = 0;
//     int sign = 1;

//     while (ourString[i] == ' ' || ourString[i] == '\n' || ourString[i] == '\t') i++;

//     if (ourString[i] == '+' || ourString[i] == '-')
//         if (ourString[i] == '-')
//             sign = -1;

//     i++;

//     while (ourString[i] && (ourString[i] >= '0' && ourString[i] <= '9')) {
//         number = number * 10 + (ourString[i] - '0');
//         i++;
//     }

//     return sign * number;
// }

void printShellLine() {
    char *path = (char *)malloc(512 * charSize);
    getcwd(path, 500);
    printf("%s ~ >> ", path);
}

bool readInput(char *command, int file) {
    char *buff = (char *)malloc(strlen(command) * charSize);
    fgets(buff, 512, stdin);

    if (strcmp(buff, "\n") == 0)    //if there's no command
        return false;               //there's nothing to show

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

void clearCommand() {
    system("clear"); // on mac is clear, on linux/windows is cls
}

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

int main(int arg, char **argv) {
    int history = open("shellHistory", O_RDWR | O_CREAT, S_IRWXU);
    
    char *command = (char *)malloc(512 * charSize);
    while (true) {
        printShellLine();

        if (readInput(command, history)) {
            if (strcmp(command, "clear") == 0) {
                clearCommand();
            } else if(strcmp(command, "history") == 0) {
                showHistory();
            } else {
                printf("Command not found\n");
            }
        }
    }

    return 0;
}
