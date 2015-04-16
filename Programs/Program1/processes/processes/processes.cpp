//  This "processes" class receives one argument, (i.e., argv[1]) upon its invocation
//  and searches how many processes whose name is given in argv[1] are running on the
//  system where your program has been invoked. In another words, this class functions
//  the same as Linux command "$ ps -A | grep argv[1] | wc -l".
//
//  Created by DJJ on 4/14/15.
//  Copyright (c) 2015 DJJ. All rights reserved.
//

#include <iostream>
#include <unistd.h> // for system calls
#include <sys/types.h>
#include <sys/wait.h>   // for wait

using namespace std;

int main(int argc, const char * argv[]) {
    // reads in user input
    const char * target = argv[1];
    
    // check for invalid input
    if (argc < 2 || argc > 2) {
        std::cout << "Invalid arguments";
        return 1;
    }
    // initialize two set pipefd, [0] for in, [1] for out
    int pipefd1[2];     // ps -A "|" grep argv[1]
    int pipefd2[2];     // grep argv[1] "|" wc -l
    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1) {  // create pipe 1 and 2
        std::cout << "error creating pipe" << std::endl;
        return 1;
    }
    
    // fork child "wc" and wait for "grep"
    pid_t child = fork();
    if (child == 0) {   // fork successfully
        // fork grand child: "grep" and wait for "ps"
        pid_t grandChild = fork();
        if (grandChild == 0) {
            // fork great grand child: "ps" and exec
            pid_t greatGrandChild = fork();
            if (greatGrandChild == 0) {
                close(pipefd1[0]);  // close the read of pipe 1
                dup2(pipefd1[1], 1);    // redirect write of "ps" fork to cout
                execlp("ps", "ps", "-A", NULL);
            }
            wait(NULL);
            close(pipefd1[1]);  // close the write of pipe 1
            dup2(pipefd1[0], 0);    // redirect the read of pipe 1 to cin
            close(pipefd2[0]);  // close read of pipe 2
            dup2(pipefd2[1], 1);    // redirect write to cout
            execlp("grep", "grep", target, NULL);
        }
        // close pipe 1
        close(pipefd1[0]);
        close(pipefd1[1]);
        
        close(pipefd2[1]);  // close write of pipe 2
        dup2(pipefd2[0], 0);    // redirect read of pipe 2 to cin
        execlp("wc", "wc", "-l", NULL);
    }
    // close all
    for (int i = 0; i < 2; i++) {
        close(pipefd1[i]);
        close(pipefd2[i]);
    }
    wait(NULL);
    
    return 0;
}
