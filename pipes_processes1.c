// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

#define BUFFER_SIZE 512

void read_input(char *prompt, char *buffer, size_t size) 
{
  if (prompt != NULL) {
    printf("%s", prompt);
    fflush(stdout);
  }
  if (fgets(buffer, size, stdin) != NULL) {
    buffer[strcspn(buffer, "\r\n")] = '\0';
  }
}
  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str1[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org";

    char input_str1[BUFFER_SIZE]; 
    char input_str2[BUFFER_SIZE];

    pid_t p; 
  
    if (pipe(fd1) == -1) 
    { 
        fprintf(stderr, "Pipe 1 Failed\n"); 
        return 1; 
    } 

    if (pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe 2 Failed\n"); 
        return 1; 
    } 
  
    printf("Other string is: %s\n", fixed_str1);

    read_input("Input : ", input_str1, sizeof(input_str1));

    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed\n"); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
  
        close(fd1[0]);  // Close reading end of pipes 
        close(fd2[1]);
  
        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str1, strlen(input_str1) + 1); 
        close(fd1[1]);

        char received_str[BUFFER_SIZE * 2];

        read(fd2[0], received_str, sizeof(received_str));
        close(fd2[0]);

        // Wait for child to print the concatenated string 
        wait(NULL);

        strcat(received_str, fixed_str2);

        printf("Output : %s\n", received_str);

    } 
  
    // child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]); 
      
        // Read a string using first pipe 
        char concat_str[BUFFER_SIZE * 2]; 

        read(fd1[0], concat_str, sizeof(concat_str)); 
        close(fd1[0]);

        strcat(concat_str, fixed_str1);
        
        printf("Output : %s\n", concat_str);

        read_input("Input : ", input_str2, sizeof(input_str2));

        strcat(concat_str, input_str2);

        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]);

        exit(0);
    } 

    return 0;
} 