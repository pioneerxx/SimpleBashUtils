#include "s21_cat.h"

int main(int argc, char *argv[]) {
  struct flags options = {0};
  int i = 1;
  int flag = 0;
  whatOptions(argv, argc, &options, &flag);
  if (flag == 1) return 1;
  for (int q = 1; q < argc; q++)
    if (argv[q][0] != '-') {
      i = q;
      break;
    }
  while (i < argc) {
    print(argv[i], &options);
    i++;
  }
  return 0;
}

void print(char *filename, struct flags *options) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "cat: %s: No such file or directory\n", filename);
    return;
  }
  int q = 1;
  int b1 = -1;
  int b2 = -1;
  int c = 0;
  while ((c = fgetc(fp)) != EOF) {
    if (options->s == 1 && (b2 == -1 || b2 == '\n') && b1 == '\n' &&
        c == '\n') {
      b2 = b1;
      b1 = c;
      continue;
    }
    if (options->b == 1 && (b1 == -1 || b1 == '\n') && c != '\n') {
      printf("%6d\t", q++);
    }
    if (options->n == 1 && (b1 == '\n' || b1 == -1)) {
      printf("%6d\t", q++);
    }
    if (options->v == 1 && (c > 126 || c < 32) && c != 9 && c != 10) {
      int p = c < 32 ? c + 64 : c - 64;
      printf("^%c", p);
    } else if (options->t == 1 && c == '\t') {
      printf("^I");
    } else if (options->e == 1 && c == '\n') {
      printf("$");
      printf("%c", c);
    } else {
      printf("%c", c);
    }
    b2 = b1;
    b1 = c;
  }
  fclose(fp);
}

void whatOptions(char *argv[], int argc, struct flags *options, int *flag) {
  const char *shorts;
  opterr = 0;
#ifdef __APPLE__
  shorts = "+benstvET";
#else
  shorts = "benstvET";
#endif
  const struct option longs[] = {
      {"number-nonblank", 0, NULL, 'b'},
      {"number", 0, NULL, 'n'},
      {"squeeze-blank", 0, NULL, 's'},
  };
  int rez;
  int optind;
  while ((rez = getopt_long(argc, argv, shorts, longs, &optind)) != -1) {
    whichOption(rez, options, flag);
    if (*flag == 1) {
      return;
    }
  }
  if (options->b == 1 && options->n == 1) options->n = 0;
}

void whichOption(int rez, struct flags *options, int *flag) {
  switch (rez) {
    case 'b':
      options->b = 1;
      break;
    case 'e':
      options->e = 1;
      options->v = 1;
      break;
    case 'E':
      options->e = 1;
      break;
    case 'n':
      options->n = 1;
      break;
    case 's':
      options->s = 1;
      break;
    case 'T':
      options->t = 1;
      break;
    case 't':
      options->t = 1;
      options->v = 1;
      break;
    case 'v':
      options->v = 1;
      break;
    case '?':
    default:
      fprintf(stderr, "cat: illegal option\nusage: cat [-benstv] [file ...]");
      *flag = 1;
      return;
      break;
  }
}
