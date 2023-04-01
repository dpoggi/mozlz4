#include "decode.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lz4.h>

#define COUNT_OF(_x) (sizeof(_x) / sizeof(0[(_x)]))

static const size_t kBufferSize = 4096UL;
static const size_t kBufferGrowthFactor = 2UL;

static const char kMagicNumber[] = {'m', 'o', 'z', 'L', 'z', '4', '0', '\0'};

static const char *const kEOFErrorMessage = "Unexpected end of file";
static const char *const kUnknownReadErrorMessage = "fread unable to read enough bytes, but ferror and feof return 0";
static const char *const kUnrecognizedSignatureErrorMessage = "Unrecognized file signature";
static const char *const kUnableToDecodeErrorMessage = "Unable to decode LZ4 data";

void mozlz4_perror(struct mozlz4_error error)
{
    (void)fprintf(stderr, "%s: %s\n", error.prefix, error.message);
}

#define RETURN_ERROR(_prefix, _message)         \
    do {                                        \
        if (out_error != NULL) {                \
            out_error->prefix = (_prefix);      \
            out_error->message = (_message);    \
        }                                       \
        return NULL;                            \
    } while (false)

char *mozlz4_decode(FILE *stream, size_t *out_size, struct mozlz4_error *out_error)
{
    char signature[COUNT_OF(kMagicNumber)];
    size_t chars_read = fread(signature, sizeof(*signature), COUNT_OF(kMagicNumber), stream);
    if (chars_read < COUNT_OF(kMagicNumber)) {
        int err = ferror(stream);
        if (err != 0) {
            RETURN_ERROR("fread", strerror(err));
        } else {
            RETURN_ERROR("mozlz4_decode", feof(stream) != 0 ? kEOFErrorMessage : kUnknownReadErrorMessage);
        }
    }

    if (memcmp(signature, kMagicNumber, COUNT_OF(kMagicNumber)) != 0) {
        RETURN_ERROR("mozlz4_decode", kUnrecognizedSignatureErrorMessage);
    }

    uint32_t dec_size;
    size_t u32s_read = fread(&dec_size, sizeof(dec_size), 1UL, stream);
    if (u32s_read < 1UL) {
        int err = ferror(stream);
        if (err != 0) {
            RETURN_ERROR("fread", strerror(err));
        } else {
            RETURN_ERROR("mozlz4_decode", feof(stream) != 0 ? kEOFErrorMessage : kUnknownReadErrorMessage);
        }
    }

    if (dec_size > (uint32_t)INT_MAX) {
        RETURN_ERROR("mozlz4_decode", strerror(EFBIG));
    }

    size_t enc_data_size = kBufferSize;
    char *enc_data = malloc(enc_data_size);
    if (enc_data == NULL) {
        RETURN_ERROR("malloc", strerror(ENOMEM));
    }

    size_t enc_size = 0UL;
    while (true) {
        size_t read_size = enc_data_size - enc_size;
        if (read_size == 0) {
            if (SIZE_MAX / enc_data_size < kBufferGrowthFactor) {
                free(enc_data);

                RETURN_ERROR("mozlz4_decode", strerror(ENOMEM));
            }

            enc_data_size *= kBufferGrowthFactor;
            read_size = enc_data_size - enc_size;

            char *new_enc_data = realloc(enc_data, enc_data_size);
            if (new_enc_data == NULL) {
                free(enc_data);

                RETURN_ERROR("realloc", strerror(ENOMEM));
            }

            enc_data = new_enc_data;
        }

        size_t bytes_read = fread(&enc_data[enc_size], sizeof(*enc_data), read_size, stream);
        bool is_eof = feof(stream) != 0;

        if (!is_eof && bytes_read < read_size) {
            free(enc_data);

            int err = ferror(stream);
            if (err != 0) {
                RETURN_ERROR("fread", strerror(err));
            } else {
                RETURN_ERROR("mozlz4_decode", kUnknownReadErrorMessage);
            }
        }

        enc_size += bytes_read;

        if (is_eof) {
            break;
        }
    }

    if (enc_size > (size_t)INT_MAX) {
        free(enc_data);

        RETURN_ERROR("mozlz4_decode", strerror(EFBIG));
    }

    char *dec_data = malloc(dec_size);
    if (dec_data == NULL) {
        free(enc_data);

        RETURN_ERROR("malloc", strerror(ENOMEM));
    }

    int bytes_decoded = LZ4_decompress_safe(enc_data, dec_data, (int)enc_size, (int)dec_size);
    if (bytes_decoded < (int)dec_size) {
        free(dec_data);
        free(enc_data);

        RETURN_ERROR("LZ4_decompress_safe", kUnableToDecodeErrorMessage);
    }

    free(enc_data);

    *out_size = dec_size;
    return dec_data;
}

#undef RETURN_ERROR
