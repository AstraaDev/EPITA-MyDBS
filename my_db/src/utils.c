#include "utils.h"

#include <stdlib.h>
#include <string.h>

void free_parse(char **parse)
{
    int i = 0;
    while (parse[i] != NULL)
    {
        free(parse[i++]);
    }
    free(parse);
}

char **parser(char *input, int *nbArg)
{
    int size = 5;
    char **parse_input = calloc(size, sizeof(char *));

    if (input[0] == '\0')
        return NULL;

    int i = 1;
    int count = 1;
    while (input[i - 1] != '\0')
    {
        if (input[i] == ' ' || input[i] == '\0')
        {
            char *new = calloc(count + 1, sizeof(char));
            memcpy(new, &input[i - count], count);
            parse_input[(*nbArg)++] = new;
            if (size <= (*nbArg))
            {
                size *= 2;
                parse_input = realloc(parse_input, size * (sizeof(char *)));
            }
            count = 0;
        }
        else
        {
            count++;
        }
        i++;
    }
    return parse_input;
}
