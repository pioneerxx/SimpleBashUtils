#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (warning(argc, argv)) {
    return 1;
  }
  Flags options = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char templates[BUF_SIZE][STR_SIZE];
  int templatecount = 0;
  char files[BUF_SIZE][STR_SIZE];
  int filecount = 0;
  int errflag = 0;
  getOptions(argv, argc, &options, templates, &templatecount, &errflag);
  if (errflag) {
    return 1;
  }
  if (!options.e && !options.f) {
    getTemplate(argv, argc, templates, &templatecount, &errflag);
    if (errflag) {
      return 1;
    }
  }
  getFiles(argv, argc, files, &filecount, options, &errflag);
  if (errflag) {
    return 1;
  }
  for (int i = 0; i < filecount; i++) {
    execute(files[i], templates, options, filecount, templatecount);
  }
  return 0;
}

void getFiles(char *argv[], int argc, char files[BUF_SIZE][STR_SIZE],
              int *filecount, Flags options, int *flag) {
  if (!options.e && !options.f) {
    int skip = 1;
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-') {
        if (skip == 1) {
          skip = 0;
          continue;
        }
        strcpy(files[*filecount], argv[i]);
        *filecount += 1;
      }
    }
  } else {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-' && argv[i][0] != '\0') {
        strcpy(files[*filecount], argv[i]);
        *filecount += 1;
      }
    }
  }
  if (!*filecount) {
    fprintf(stderr, "grep: No files entered\n");
    *flag = 1;
  }
}

void optionE(char *argv[], int i, int q, char templates[BUF_SIZE][STR_SIZE],
             int *templatecount) {
  int symb = 0;
  char str[30];
  if (q) {
    while (argv[i][q] != '\0') {
      str[symb] = argv[i][q];
      symb++;
      q++;
    }
    str[symb] = '\0';
    strcpy(templates[*templatecount], str);
  } else {
    strcpy(templates[*templatecount], argv[i]);
  }
  *templatecount += 1;
}

void optionF(char *argv[], int i, int q, char templates[BUF_SIZE][STR_SIZE],
             int *templatecount, int *flag) {
  char str[30];
  int symb = 0;
  while (argv[i][q] != '\0') {
    str[symb] = argv[i][q];
    q++;
    symb++;
  }
  str[symb] = '\0';
  FILE *fs = fopen(str, "r");
  if (!fs) {
    fprintf(stderr, "grep: %s: No such file or directory\n", str);
    *flag = 1;
    return;
  }
  char *c = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&c, &len, fs)) != -1) {
    if (c[strlen(c) - 1] == '\n' && strlen(c) != 1) {
      c[strlen(c) - 1] = '\0';
    }
    strcpy(templates[*templatecount], c);
    *templatecount += 1;
  }
  free(c);
  fclose(fs);
}

void getOptions(char *argv[], int argc, Flags *options,
                char templates[BUF_SIZE][STR_SIZE], int *templatecount,
                int *flag) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      int q = 1;
      while (argv[i][q] != '\0') {
        switch (argv[i][q]) {
          case 'e':
            options->e = 1;
            if (argv[i][q + 1] != '\0') {
              optionE(argv, i, q + 1, templates, templatecount);
              memset(argv[i], '\0', strlen(argv[i]));
            } else {
              if (i == (argc - 1)) {
                fprintf(stderr, "grep: option requires an argument -- %c\n",
                        argv[i][q]);
                *flag = 1;
                return;
              }
              optionE(argv, i + 1, 0, templates, templatecount);
              memset(argv[i + 1], '\0', strlen(argv[i + 1]));
            }
            break;
          case 'i':
            options->i = 1;
            break;
          case 'v':
            options->v = 1;
            break;
          case 'c':
            options->c = 1;
            break;
          case 'l':
            options->l = 1;
            break;
          case 'n':
            options->n = 1;
            break;
          case 'h':
            options->h = 1;
            break;
          case 's':
            options->s = 1;
            break;
          case 'f':
            options->f = 1;
            if (argv[i][q + 1] != '\0') {
              optionF(argv, i, q + 1, templates, templatecount, flag);
              memset(argv[i], '\0', strlen(argv[i]));
            } else {
              if (i == (argc - 1)) {
                fprintf(stderr, "grep: Option requires an argument -- %c\n",
                        argv[i][q]);
                *flag = 1;
                return;
              }
              optionF(argv, i + 1, 0, templates, templatecount, flag);
              memset(argv[i + 1], '\0', strlen(argv[i]));
            }
            if (*flag) {
              return;
            }
            break;
          case 'o':
            options->o = 1;
            break;
          default:
            fprintf(stderr, "grep: Invalid value\n");
            *flag = 1;
            return;
            break;
        }
        q++;
      }
    }
  }
}

int ifOptions(struct flags options) {
  int res = 0;
  if (options.e || options.i || options.v || options.c || options.l ||
      options.n || options.h || options.s || options.f || options.o) {
    res = 1;
  }
  return res;
}

void getTemplate(char *argv[], int argc, char templates[BUF_SIZE][STR_SIZE],
                 int *templatecount, int *flag) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      strcpy(templates[*templatecount], argv[i]);
      *templatecount += 1;
      break;
    }
  }
  if (!*templatecount) {
    *flag = 1;
    fprintf(stderr, "grep: No templates entered\n");
  }
}

void execute(char *filename, char templates[BUF_SIZE][STR_SIZE], Flags options,
             int filecount, int templatecount) {
  FILE *fs = fopen(filename, "r");
  if (fs == NULL) {
    if (options.s == 0) {
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    }
    return;
  }
  char *c = NULL;
  size_t len = 0;
  ssize_t read;
  c = NULL;
  int lines = 0;
  int printlines = 0;
  int nolines = 0;
  while ((read = getline(&c, &len, fs)) != -1) {
    print(c, templates, templatecount, options, lines, &printlines, &nolines,
          filename, filecount);
    lines++;
    if ((options.l && !options.v && printlines) ||
        (options.l && options.v && nolines)) {
      break;
    }
  }
  free(c);
  fclose(fs);
  if (options.c) {
    if (!options.h && filecount > 1) {
      printf("%s:", filename);
    }
    if (options.v) {
      printf("%d\n", nolines);
    } else {
      printf("%d\n", printlines);
    }
  }
  if ((options.l && !options.v && printlines) ||
      (options.l && options.v && nolines)) {
    printf("%s\n", filename);
  }
}

int finder(char *fileline, char templates[BUF_SIZE][STR_SIZE],
           char foundTemplates[BUF_SIZE][STR_SIZE], int templatecount,
           Flags options) {
  int index = 0;
  for (int i = 0; i < templatecount; i++) {
    regex_t regex;
    if (options.i) {
      regcomp(&regex, templates[i], REG_ICASE);
    } else {
      regcomp(&regex, templates[i], REG_EXTENDED);
    }
    regmatch_t find;
    size_t reg_offset = 0;
    size_t linel = strlen(fileline);
    for (int j; (j = regexec(&regex, fileline + reg_offset, 1, &find, 0)) == 0;
         index++) {
      int l = find.rm_eo - find.rm_so;
      memcpy(foundTemplates[index], fileline + find.rm_so + reg_offset, l);
      foundTemplates[index][l] = '\0';
      reg_offset += find.rm_eo;
      if (reg_offset > linel) {
        break;
      }
    }
    regfree(&regex);
  }
  return index;
}

void print(char *fileline, char templates[BUF_SIZE][STR_SIZE],
           int templatecount, Flags options, int lines, int *printlines,
           int *nolines, char *filename, int filecount) {
  char foundTemplates[BUF_SIZE][STR_SIZE];
  int printflag = 0;
  int foundtemplatecount =
      finder(fileline, templates, foundTemplates, templatecount, options);
  if (foundtemplatecount > 0) {
    printflag = 1;
    *printlines += 1;
    if (options.l && !options.v) {
      return;
    }
  } else {
    *nolines += 1;
    if (options.l && options.v) {
      return;
    }
  }
  if (!options.c) {
    if ((options.v && !printflag) || (!options.v && printflag)) {
      if (filecount > 1 && !options.h) {
        printf("%s:", filename);
      }
      if (options.n) {
        lines++;
        printf("%d:", lines);
      }
    }
    if (options.v && !printflag) {
      if (fileline[strlen(fileline) - 1] == '\n') {
        printf("%s", fileline);
      } else {
        printf("%s\n", fileline);
      }
    } else if (!options.v && printflag) {
      if (options.o) {
        for (int i = 0; i < foundtemplatecount; i++) {
          printf("%s\n", foundTemplates[i]);
        }
      } else {
        if (fileline[strlen(fileline) - 1] == '\n') {
          printf("%s", fileline);
        } else {
          printf("%s\n", fileline);
        }
      }
    }
  }
}

void toLower(char *text) {
  for (int i = 0; i < (int)strlen(text); i++) {
    text[i] = tolower(text[i]);
  }
}

int warning(int argc, char *argv[]) {
  int flag = 0;
  for (int i = 1; i < argc; i++) {
    for (int j = 0; j <= (int)strlen(argv[i]) - 1; j++) {
      int a = argv[i][j];
      int b = argv[i][j + 1];
      if (a == 92 && b == 0) {
        fprintf(stderr, "grep: trailing backslash (\\)\n");
        flag = 1;
      }
    }
  }
  return flag;
}