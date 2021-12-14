#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
pid_t pid;

void timer(int signum){
  kill(pid,SIGKILL);
}
void handleSig(int signum){
  kill(pid,SIGKILL);
}

int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    	
    while (true) {
      
        do{ 
           char path[4096];
           char* dir = getcwd(path, sizeof(path));
           printf("%s ", dir);
           printf("%s ", prompt);
           fflush(stdout);
            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 1. Tokenize the command line input (split it on whitespace)
         char* itr = command_line;
         itr = strsep(&itr, "\n");
         int i;
         for( i = 0; i<MAX_COMMAND_LINE_LEN; i++){
           char* chr = strsep(&itr, " ");
           if (chr == NULL){break;}
           if (strchr(chr, '$')!=NULL){
             memmove(chr, chr+1, strlen(chr));
             chr = getenv(chr);
           };
           arguments[i] = chr;
         }
         char* comm = arguments[0];
         
      
        // 2. Implement Built-In Commands
        if (strcmp(comm, "echo")==0){
          for(i = 1; i<MAX_COMMAND_LINE_ARGS; i++){
            if (arguments[i] == NULL){break;}
            printf("%s ", arguments[i]);
          }
        } else if (strcmp(comm, "env") == 0){
          for(i = 0; environ[i]!=NULL;i++){
            printf("%s \n", environ[i]);
          }
        } else if (strcmp(comm, "cd") == 0){
          if(arguments[1] == NULL){
            printf("Missing argument");
          }
          else{chdir(arguments[1]);}
        }else if (strcmp(comm, "exit") == 0){exit(0);}
        else if (strcmp(comm, "pwd") == 0){
          char maxPath[4096];
          char* dir = getcwd(maxPath, sizeof(maxPath));
          printf("%s ", dir);
        } else if (strcmp(comm, "setenv") == 0){
          char* var[2];
          char* rest = arguments[1];
          for (i = 0; i<2; i++){
            char *chr = strsep(&rest, "=");
            var[i] = chr;
          }
          setenv(var[0], var[1], 1);
        } else{
          // 3. Create a child process which will execute the command line input
          pid = fork();
          int isBackground = 0;
          int i = 0;
          for (i = 0; MAX_COMMAND_LINE_ARGS; i++){
            if (arguments[i] == NULL){ break;}
          }
          i--;
          if(arguments[i]){
              if(strcmp(arguments[i], "&") == 0){
                isBackground = 1;
                arguments[i] = NULL;
              }else{
                isBackground = 0;
              }
            }
            int status;
            if(pid<0){
              printf("Oops, something went wrong");
              exit(1);
            }else if(pid == 0){
              if(execvp(arguments[0], arguments) == -1){
                printf("Execvp() failed: No file or dir");
              }
              signal(SIGINT, handleSig);
              exit(1);
            }
            else{
              signal(SIGINT, handleSig);
              signal(SIGALRM, timer);
              alarm(10);
              
              if (isBackground == 1){
                waitpid(pid, &status, 0);
              } else{
                wait(NULL); 
              }
            }
          }
          printf("\n");
        }
    return -1;
    }      
  
        // 4. The parent process should wait for the child to complete unless its a background process
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
  
