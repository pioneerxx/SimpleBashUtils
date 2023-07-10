#ifndef S21_GREPH_H
#define S21_GREPH_H

#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 400
#define STR_SIZE 255

typedef struct flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} Flags;

void getFiles(char *argv[], int argc, char files[BUF_SIZE][STR_SIZE],
              int *filecount, Flags options, int *flag);
void optionE(char *argv[], int i, int q, char templates[BUF_SIZE][STR_SIZE],
             int *templatecount);
void optionF(char *argv[], int i, int q, char templates[BUF_SIZE][STR_SIZE],
             int *templatecount, int *flag);
void getOptions(char *argv[], int argc, Flags *options,
                char templates[BUF_SIZE][STR_SIZE], int *templatecount,
                int *flag);
int ifOptions(struct flags options);
void getTemplate(char *argv[], int argc, char templates[BUF_SIZE][STR_SIZE],
                 int *templatecount, int *flag);
void execute(char *filename, char templates[BUF_SIZE][STR_SIZE], Flags options,
             int filecount, int templatecount);
void print(char *fileline, char templates[BUF_SIZE][STR_SIZE],
           int templatecount, Flags options, int lines, int *printlines,
           int *nolines, char *filename, int filecount);
int finder(char *fileline, char templates[BUF_SIZE][STR_SIZE],
           char foundTemplates[BUF_SIZE][STR_SIZE], int templatecount,
           Flags options);
void toLower(char *text);
int warning(int argc, char *argv[]);
int isTemplate(char c, char templates[BUF_SIZE][STR_SIZE], int templatecount);

#endif
