# Shell-Project
Creating a Shell in C.

# Team members and contributions:
### Irina Enescu:
- man
- parse command 
- cp 
- cd
- all commands 
- welcome
- code formatting
### Bogdan Putinelu:
- touch 
### Robert Udrea:
- print shell line
- read input
- clear
- history
- pipe
- unlimited power

# How to use this masterpiece
> The program runs correctly on Linux or MAC.
1. Save the shell.c file in a folder.

2. Open the folder in the terminal.

3. Run the following commands:
```sh
gcc shell.c -o shell
./shell
```
- If you don't have the GCC compiler installed, run the following commands in the terminal:
```sh
sudo apt update
sudo apt install build-essential
sudo apt install gcc
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
