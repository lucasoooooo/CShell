/*UID: 115334465*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <err.h>
#include <sysexits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "command.h"
#include "executor.h"
/**
 * By Lucas Balangero
 * 5/5/2020
 * lbalange
 * CMSC216 Project 6
 * In this project, you will write the guts of a shell that
 *  will support boolean operations, pipes, and file redirection.
 */

/*
static void print_tree(struct tree *t);
*/

/*Takes tree from the parser and execute the commands in the tree. Recursively*/
static int recursion_t (struct tree *t, int in, int out){
   int proc, stat;
   pid_t child;
   /*NONE*/
   if (t->conjunction == NONE){
      /*change directory to 2nd argument */
      if((strcmp(t->argv[0], "cd")) == 0 && !(chdir(t->argv[1]))){
         perror(t->argv[1]);
      }
      /*exit*/
      else if (strcmp (t->argv[0], "cd") == 0){
         exit(0);
      }
      else{
         if ((child = fork()) <= -1){
            err(EX_OSERR, "Fork Error");
         }
         else{
            if (child){
               wait(&proc);
               return proc;
            }
            else if(child == 0){
               if (t->input){
                  if ((in = open(t->input, O_RDONLY)) <= -1){
                     err(EX_OSERR, "Input file didn't open\n");
                  }
                  if (dup2(in, STDIN_FILENO) <= -1){
                     err(EX_OSERR, "dup2 failed\n");
                  }
                  close(in);
               }
               else if (t->output){
                  if ((out = open(t->output, O_CREAT)) <= -1){
                     err(EX_OSERR, "Output file didn't open\n");
                  }
                  if (dup2(in, STDIN_FILENO) <= -1){
                     err(EX_OSERR, "dup2 failed\n");
                  }
                  close(out);
               }
               execvp(t->argv[0], t->argv);
               fprintf(stderr, "Failed to execute %s\n", t->argv[0]);
               exit(-1);
            }
         }
      }
   }
   /*PIPE*/
   else if(t->conjunction == PIPE){ 
      int pipe_[2];
      if (t->left->output){
         printf("Ambiguous output redirect.\n");
      }
      else
         if(t->right->input){
            printf("Ambiguous input redirect.\n");
         }
         else{
            if (t->output && (out = open(t->input, O_CREAT)) <= -1){
               err(EX_OSERR, "Could not open output file\n");
            }
            if (t->input && (in = open(t->input, O_RDONLY)) <= -1){
               err(EX_OSERR, "Could not open input file\n");
            }
            if((pipe(pipe_)) <= -1){
               err(EX_OSERR, "Pipe err");
            }
            if((child = fork()) <= -1){
               err(EX_OSERR, "Fork err");
            }
            if (child == 0){
               close(pipe_[0]);
               if(dup2(pipe_[1], STDOUT_FILENO) <= -1){
                  err(EX_OSERR, "dup2 err");
               }
               recursion_t(t->left, in, pipe_[1]);
               close(pipe_[1]);
               exit(0);
            }
            if(child > 0 || child < 0){
               close(pipe_[1]);
               if(dup2(pipe_[0], STDIN_FILENO <= -1)){
                  err(EX_OSERR, "dup2 err");
               }
               recursion_t(t->right, pipe_[0], out);
               close(pipe_[0]);
               wait(NULL);
            }
         }
   }
   /*AND*/
   else if(t->conjunction == AND){
      if (t->output && (out = open(t->input, O_CREAT)) <= -1){
         err(EX_OSERR, "Could not open output file\n");
      }
      if (t->input && (in = open(t->input, O_RDONLY)) <= -1){
         err(EX_OSERR, "Could not open input file\n");
      }
      if(!(stat = recursion_t(t->left, in, out))){ 
         recursion_t(t->right, in, out);
      } 
   }
   /*SUBSHELL*/
   else if(t->conjunction == SUBSHELL){
      if (t->output && (out = open(t->input, O_CREAT)) <= -1){
         err(EX_OSERR, "Could not open output file\n");
      }
      if (t->input && (in = open(t->input, O_RDONLY)) <= -1){
         err(EX_OSERR, "Could not open input file\n");
      }
      if((child = fork()) <= -1){
         err(EX_OSERR, "Fork err");
      }
      else{
         if (child){
            wait(NULL);
         }
         if(!child){
            recursion_t(t->left, in, out);
            exit(0);
         }
      }
   }
   return 0;
}

int execute(struct tree *t) {
   if (t){
      return recursion_t(t, -1, -1);
   }
   return 0;
}

/*
static void print_tree(struct tree *t) {
   if (t != NULL) {
      print_tree(t->left);

      if (t->conjunction == NONE) {
         printf("NONE: %s, ", t->argv[0]);
      } else {
         printf("%s, ", conj[t->conjunction]);
      }
      printf("IR: %s, ", t->input);
      printf("OR: %s\n", t->output);

      print_tree(t->right);
   }
}

*/