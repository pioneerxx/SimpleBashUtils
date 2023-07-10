#ifndef S21_CAT_H
#define S21_CAT_H

#include <getopt.h>
#include <stdio.h>

struct flags {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
};

void print(char *filename, struct flags *options);
void whatOptions(char *argv[], int argc, struct flags *options, int *flag);
void whichOption(int rez, struct flags *options, int *flag);
int endOfOptions(char *argv[], int argc);

#endif
