#ifndef DECODE_H_
#define DECODE_H_

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
#define MZ4_EXTERN_         extern "C"
#define MZ4_RESTRICT        __restrict__
#else
#define MZ4_EXTERN_         extern
#define MZ4_RESTRICT        restrict
#endif  // __cplusplus

#ifndef __has_attribute
#define __has_attribute(_x) 0
#endif  // __has_attribute

#if __has_attribute(visibility)
#define MZ4_EXTERN          MZ4_EXTERN_ __attribute__((visibility("hidden")))
#else
#define MZ4_EXTERN          MZ4_EXTERN_
#endif  // __has_attribute(visibility)

#if __has_attribute(warn_unused_result)
#define MZ4_NODISCARD       __attribute__((warn_unused_result))
#else
#define MZ4_NODISCARD
#endif  // __has_attribute(warn_unused_result)

struct mozlz4_error {
    const char *prefix;
    const char *message;
};

MZ4_EXTERN void mozlz4_perror(struct mozlz4_error error);

MZ4_EXTERN char *mozlz4_decode(FILE *MZ4_RESTRICT stream, size_t *MZ4_RESTRICT out_size, struct mozlz4_error *MZ4_RESTRICT out_error) MZ4_NODISCARD;

#endif  // DECODE_H_
