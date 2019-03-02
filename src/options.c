#include "options.h"
#include <stdio.h>
#include <stdlib.h>

// parse options as in myfind
struct list_options *get_options(char **argv, int argc)
{
    argv++;  // dummy
    argc--;  // dummy
    argc++;  // dummy
    argc--;  // dummy
    struct list_options *options = malloc(sizeof(struct list_options));
    options->len = 0;
    options->options = malloc(MAX_OPTIONS * (sizeof(struct option)));

    return options;
}

// ast print le fichier DOT ouvert est \"ast.dot\"
int ast_print()
{
    FILE *fp = fopen("../ast.dot", "r");
    char c;

    if (fp == NULL)
        return 1;

    for (; c = fgetc(fp) != EOF ;)
        printf("%c",c);

    fclose(fp);

    return 0;
}
