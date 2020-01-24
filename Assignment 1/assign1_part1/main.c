/*
About this program:
- This program counts words.
- The specific words that will be counted are passed in as command-line
  arguments.
- The program reads words (one word per line) from standard input until EOF or
  an input line starting with a dot '.'
- The program prints out a summary of the number of times each word has
  appeared.
- Various command-line options alter the behavior of the program.

E.g., count the number of times 'cat', 'nap' or 'dog' appears.
> ./main cat nap dog
Given input:
 cat
 .
Expected output:
 Looking for 3 words
 Result:
 cat:1
 nap:0
 dog:0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smp0_tests.h"

#define LENGTH(s) (sizeof(s) / sizeof(*s))

/* Structures */
typedef struct {
  char *word;
  int counter;
} WordCountEntry;


int process_stream(WordCountEntry entries[], int entry_count)
{
  short line_count = 0;
  char buffer[60];
  char s[2]=" ";
  char *token;
  while (fgets(buffer,60,stdin)) {
    if (*buffer == '.')
      break;
    /* Compare against each entry */
    int i = 0;
    while(buffer[i]!='\n'){
      i++;
    }
    buffer[i] = '\0';
    i=0;

    while (i < entry_count) {
      if (!strcmp(entries[i].word, buffer))
        entries[i].counter++;
      i++;
    }
    line_count++;
  }
  return line_count;
}

int Aprocess_stream(WordCountEntry entries[], int entry_count)
{
  short line_count = 0;
  char buffer[60];
  char s[2]=" ";
  char *token;
  int count = 1;
//printf("*******%d**********\n",count++);
  while (fgets(buffer,60,stdin)) {
    //printf("*******%d**********\n",count++);
    token = strtok(buffer,s);
    //printf("*******%d**********\n",count++);
    while(token){
      //printf("*******%d**********\n",count++);
        if (*token == '.')
          return line_count;
        //printf("%s\n",token);
        /* Compare against each entry */
        int i = 0;
       // printf("*******%d**********\n",count++);
        //printf("%s\n",token);
        while((token[i]!='\n') && (token[i]!='\0')){
          i++;
          //printf("*******%d**********\n",count++);
        }
       // printf("*******%d**********\n",count++);
        token[i] = '\0';
        i=0;
        while (i < entry_count) {
          //printf("*******%d**********\n",count++);
          if (!strcmp(entries[i].word, token))
          //printf("*******%d**********\n",count++);
            entries[i].counter++;
           // printf("*******%d**********\n",count++);
          i++;
        }
        //printf("*******%d**********\n",count++);
        line_count++;
        token = strtok(NULL,s);
    //printf("*******%d**********\n",count++);


     }
  }
  return line_count;
}


void print_result(WordCountEntry entries[], int entry_count, FILE * Foutput, int Fjudge)
{
  if(Fjudge==0){
    fprintf(stdout,"Result:\n");
    int i=1;
    while (i<entry_count) {
    //printf("%d\n",i);
      fprintf(stdout,"%s:%d\n", entries[i].word, entries[i].counter);
      //fprintf(Foutput,"%s:%d\n",entries->word, entries->counter);
      i++;
    }
  }else{
    fprintf(stdout,"Result:\n");
    fprintf(Foutput,"Result:\n");
    int i=0;
    while (i<entry_count) {
    //printf("%d\n",i);
      fprintf(stdout,"%s:%d\n", entries[i].word, entries[i].counter);
      fprintf(Foutput,"%s:%d\n",entries[i].word, entries[i].counter);
      i++;
    }
  }

  
}



void printHelp(const char *name)
{
  fprintf(stderr,"usage: %s [-h] <word1> ... <wordN>\n", name);
}


int main(int argc, char **argv)
{
  const char *prog_name = *argv;
  int number = argc ;
  WordCountEntry *entries = (WordCountEntry *)malloc(sizeof(WordCountEntry) * number);
  int entryCount = 0;
  
  //create a file stored outputs
  FILE *Foutput;
  char Fname[20];
  int judgeFile = 0;

  /* Entry point for the testrunner program */
  if (argc > 1 && !strcmp(argv[1], "-test")) {
    run_smp0_tests(argc - 1, argv + 1);
    return EXIT_SUCCESS;
  }

  while (*argv != NULL) {
    if (**argv == '-') {
      entryCount = 0;
      switch ((*argv)[1]) {
        case 'h':{
          printHelp(prog_name);
          break;
        }
        case 'f':{
          int num = 0;
          judgeFile = 1;
          while((*argv)[num+2]){
            Fname[num] = (*argv)[num+2];
            num++;
          }
          Fname[num] = '\0';
          Foutput = fopen(Fname,"w+");
          break;
        }
        default:{
          fprintf(stderr,"%s: Invalid option %s. Use -h for help.\n",
                 prog_name, *argv);
          break;
        }
      }
    } else {
      if(judgeFile == 0){
      if (entryCount < number) {
        entries[entryCount].word = *argv;
        entries[entryCount++].counter = 0;
      }
      }else{
        if (entryCount <= number) {
        entries[entryCount].word = *argv;
        entries[entryCount++].counter = 0;
      }
      }
    }
    argv++;
  }
  //printf("entryCount =  %d \n",entryCount );
  if (entryCount == 0) {
    fprintf(stderr,"%s: Please supply at least one word. Use -h for help.\n",
           prog_name);
    return EXIT_FAILURE;
  }
  if(judgeFile == 1){
    if (entryCount == 1) {
        fprintf(stdout,"Looking for a single word\n");
        fprintf(Foutput,"Looking for a single word\n");
      } else {
        fprintf(stdout,"Looking for %d words\n", entryCount);
        fprintf(Foutput,"Looking for %d words\n", entryCount);
      }
    }
  else{
    if (entryCount == 1) {
          fprintf(stdout,"Looking for a single word\n");
        } else {
          fprintf(stdout,"Looking for %d words\n", entryCount-1);
        }
  }
Aprocess_stream(entries, entryCount);
print_result(entries, entryCount,Foutput,judgeFile);

  return EXIT_SUCCESS;
    
}
