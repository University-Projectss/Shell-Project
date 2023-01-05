# Shell-Project
Creating a Shell in C.

# Team members and contributions:
### Irina Enescu:
- man
- parse command 
- cd
- all commands 
- welcome
- code formatting
- logical expressions

### Bogdan Putinelu:
- suspending a program
- executing a job in the background (&)
- chaining commands with semi-colon (;)
- added the functionality of using logical operators together (ex. cmd1 && cmd2 || cmd3)
- added the functionality of semi-colon inside logical expressions (&&, ||)

### Robert Udrea:
- print shell line
- read input
- clear
- history / recover history
- pipe
- unlimited power

# How to use this masterpiece
> The program runs correctly on Linux or MAC.
1. Save the shell.c file in a folder.

2. Open the folder in the terminal.

3. Run the following commands:
```sh
gcc shell.c -o shell -lreadline
./shell
```
- If you don't have the GCC or the Readline library compiler installed, run the following commands in the terminal:
```sh
sudo apt update
sudo apt install build-essential
sudo apt install gcc
sudo apt-get install libreadline-dev
```
- To validate that the GCC compiler is successfully installed, run the following command in the terminal:
```sh
gcc --version
```

4. Our Shell works! Type man to see all the commands:
```sh
man
```

5. Here are some examples:
```sh
touch firstFile.txt
ls
touch secondFile.txt
ls
pwd
rm secondFile.txt
ls
mkdir firstDir
ls
rmdir firstDir
ls
echo hello
clear
history
quit
```
> Have fun!
