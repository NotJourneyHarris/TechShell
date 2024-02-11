#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXARGS 3

// Constants for ShellCommand type
#define EXEC 0
#define REDIR 1
#define PIPE 2
#define LIST 3
#define BACK 4

//// Functions to implement:
// Display current working directory and return user input
char* CommandPrompt(){
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s $ ", cwd);
    }
    char* input = malloc(1024 * sizeof(char));
    fgets(input, 1024, stdin);
        if (strcmp(input, "q\n") == 0) {
            free(input);
            exit(EXIT_FAILURE); 
        return input;
    }
    return input;
    
}

struct ShellCommand {    
    int type; // Use the defined constants (EXEC, REDIR, PIPE, LIST, BACK)
    char* command;
    char* args[MAXARGS];
    char* inputFile;
    char* outputFile;
};
// this is basically the code from my Tokenizer HW assignment
    //Update: I had to change a lot more than initially expected for this to work.
        // Theseus's Tokenizer
int Tokenize(char* input, char** tokens) {
    if (tokens == NULL) {
        perror("Malloc failed");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(input, " ");
    int tokenCount = 0;

    while (token != NULL) {
        tokens[tokenCount] = strdup(token); // Save each token
        if (tokens[tokenCount] == NULL) {
            perror("strdup failed");
            exit(EXIT_FAILURE);
        }

        token = strtok(NULL, " ");
        tokenCount++;
    }

    tokens[tokenCount] = NULL; // Set the last element to NULL
    return tokenCount;
}
// Process the user input (As a shell command)
// Note the return type is a ShellCommand struct
struct ShellCommand ParseCommandLine(char* input) {
    struct ShellCommand command;
    command.inputFile = NULL;
    command.outputFile = NULL;
    
    char* tokens[MAXARGS];
    int tokenCount = Tokenize(input, tokens);

    // Check for special characters
    if (strchr(input, '>') != NULL) {
        command.type = REDIR;  // Redirection
        // Additional logic for parsing redirection
        // I think you need to identify input/output files based on the tokens

    } else if (strchr(input, '|') != NULL) {
        command.type = PIPE;   // Pipe
        // logic for pipe

    } else if (strchr(input, ';') != NULL) {
        command.type = LIST;   // Command list
        // logic for cmd list

    } else if (strstr(input, "&") != NULL) {
        command.type = BACK;   // Background command
        // logic for background command

    } else {
        command.type = EXEC;   // Default to EXEC if no special character is found
       // Copies the command + arguments
        command.command = strdup(tokens[0]);
        for (int i = 0; i < tokenCount - 1; ++i) {
            command.args[i] = strdup(tokens[i + 1]);
        }
        command.args[tokenCount - 1] = NULL; // Null-terminate the arguments array
    }

    return command;
}

// Execute a shell command
// Note the parameter is a ShellCommand struct
void ExecuteCommand(struct ShellCommand command) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("failed to create child process\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        // Child process
        if (command.inputFile != NULL) {
            int inFile = open(command.inputFile, O_RDONLY);
            if (inFile == -1) {
                fprintf(stderr, "Error opening input file: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            dup2(inFile, STDIN_FILENO);
            close(inFile);
        }

        // Output redirection
        if (command.outputFile != NULL) {
            FILE* outfile = fopen(command.outputFile, "w");
            if (outfile == NULL) {
                fprintf(stderr, "Error opening output file: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            dup2(fileno(outfile), STDOUT_FILENO);
            fclose(outfile);
        }
        if (execvp(command.command, command.args) == -1) {
            fprintf(stderr, "Error executing command: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        // Execute command
       } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() // MAIN
{
    char* input;
    struct ShellCommand command;
    char* path = "/bin";
    setenv("PATH", path, 1); // 1 indicates overwrite, so it replaces the existing PATH

// repeatedly prompt the user for input
    for (;;)
    {
     // (Uncomment for testing purposes)
        printf("Command Type: %d\n", command.type);
        printf("Command: %s\n", command.command);

        // get the user's input
        input = CommandPrompt();
        // parse the command line
        command = ParseCommandLine(input);
        // execute the command
        ExecuteCommand(command);
    }

    // The loop probably won't reach here, but I hear this is good practice
    free(input);
    free(command.command);
    for (int i = 0; i < MAXARGS; i++) {
        free(command.args[i]);
    }
    free(command.inputFile);
    free(command.outputFile);
    exit(0); 
}
                                    //Notes and Such//

    //parsecmd
    //runcmd

    // |--> typecode 3
    // ; --. sequencing --> list node
    // & --. background node with ptr to whatever needs to be executed
    // exec node --. shell takes input and puts it into a fixed size buffer to build the tree nodes
    // 2 arrays, argv --. points to filename containing exe. everything else points to options
    // Terminates with null
    // eargv points to byte after program name

/*
    // The following may just work to redirect stdin to a file:
    FILE* inFile = fopen(my_inputFile, "r");
        dup2(fileno(inFile), 0);
        fclose(inFile);
    // The following may just work to redirect stdout to a file:
    FILE* outfile = fopen(my_outputFile, "w");
        dup2(fileno(outfile), 1);
        fclose(outfile);
*/

/*

*/
