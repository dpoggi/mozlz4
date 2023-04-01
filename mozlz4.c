#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "decode.h"

int main(int argc, char *argv[])
{
    char *data;
    size_t size;
    struct mozlz4_error error;

    if (argc > 1) {
        FILE *file = fopen(argv[1], "rb");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        data = mozlz4_decode(file, &size, &error);

        (void)fclose(file);
    } else {
        (void)freopen(NULL, "rb", stdin);
        data = mozlz4_decode(stdin, &size, &error);
    }

    if (data == NULL) {
        mozlz4_perror(error);
        exit(EXIT_FAILURE);
    }

    (void)fwrite(data, sizeof(*data), size, stdout);

    if (data[size - 1] != '\n') {
        (void)fputc('\n', stdout);
    }

    free(data);
}
