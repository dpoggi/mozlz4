#ifndef DECODE_H_
#define DECODE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
#define MZ4_EXTERN      extern "C"
#define MZ4_RESTRICT    __restrict__
#else
#define MZ4_EXTERN      extern
#define MZ4_RESTRICT    restrict
#endif  // __cplusplus

#if __has_attribute(warn_unused_result)
#define MZ4_NODISCARD   __attribute__((warn_unused_result))
#else
#define MZ4_NODISCARD
#endif  // __has_attribute(warn_unused_result)

#if __has_attribute(nothrow)
#define MZ4_NOEXCEPT    __attribute__((nothrow))
#else
#define MZ4_NOEXCEPT
#endif  // __has_attribute(warn_unused_result)

struct mozlz4_error {
    const char *prefix;
    const char *message;
};

MZ4_EXTERN void mozlz4_perror(struct mozlz4_error error) MZ4_NOEXCEPT;

MZ4_EXTERN uint8_t *mozlz4_decode(FILE *MZ4_RESTRICT stream, size_t *MZ4_RESTRICT out_size, struct mozlz4_error *MZ4_RESTRICT out_error) MZ4_NODISCARD MZ4_NOEXCEPT;

#endif  // DECODE_H_
