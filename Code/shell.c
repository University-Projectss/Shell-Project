#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define charSize sizeof(char)

int ourAtoi(const char ourString[])
{
    int number = 0;
    int i = 0;
    int sign = 1;

    while (ourString[i] == ' ' || ourString[i] == '\n' || ourString[i] == '\t') i++;

    if (ourString[i] == '+' || ourString[i] == '-')
        if (ourString[i] == '-')
            sign = -1;

    i++;

    while (ourString[i] && (ourString[i] >= '0' && ourString[i] <= '9'))
    {
        number = number * 10 + (ourString[i] - '0');
        i++;
    }

    return sign * number;
}

void printShellLine()
{
    char *path = (char *)malloc(512 * charSize);
    getcwd(path, 500);
    printf("%s ~ >> ", path);
}

bool readInput(char *command)
{
    char *buff = (char *)malloc(strlen(command) * charSize);
    fgets(buff, 512, stdin);

    if (strcmp(buff, "\n") == 0)
        return false;

    buff[strlen(buff) - 1] = '\0';
    strcpy(command, buff);
    return true;
}

void clearCommand()
{
    system("clear"); // on mac is clear, on linux is cls
}

int main(int arg, char **argv)
{
    char *command = (char *)malloc(512 * charSize);
    while (true)
    {
        printShellLine();
        if (readInput(command))
        {
            if (strcmp(command, "clear") == 0)
            {
                clearCommand();
            }
            else
            {
                printf("Command not found\n");
            }
        }
    }

    return 0;
}
