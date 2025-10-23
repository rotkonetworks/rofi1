/*
 * rofi - Safe Mathematics Helpers
 *
 * MIT/X11 License
 * Copyright © 2025
 *
 * Safe arithmetic operations with overflow detection.
 */

#ifndef ROFI_SAFE_MATH_H
#define ROFI_SAFE_MATH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

/**
 * Safe multiplication with overflow detection
 *
 * @param a First operand
 * @param b Second operand
 * @param[out] result Pointer to store result (only written on success)
 * @return true if multiplication succeeded, false if overflow occurred
 *
 * Example:
 *   size_t size;
 *   if (!safe_mul(num_items, item_size, &size)) {
 *       g_error("Integer overflow in allocation");
 *       return NULL;
 *   }
 *   buffer = g_malloc(size);
 */
static inline bool safe_mul_size(size_t a, size_t b, size_t *result) {
    #if __GNUC__ >= 5 || __clang__
        // Use compiler builtin (fastest, most reliable)
        return !__builtin_mul_overflow(a, b, result);
    #else
        // Fallback implementation
        if (a != 0 && b > SIZE_MAX / a) {
            return false;  // Overflow would occur
        }
        *result = a * b;
        return true;
    #endif
}

/**
 * Safe addition with overflow detection
 *
 * @param a First operand
 * @param b Second operand
 * @param[out] result Pointer to store result
 * @return true if addition succeeded, false if overflow occurred
 */
static inline bool safe_add_size(size_t a, size_t b, size_t *result) {
    #if __GNUC__ >= 5 || __clang__
        return !__builtin_add_overflow(a, b, result);
    #else
        if (a > SIZE_MAX - b) {
            return false;
        }
        *result = a + b;
        return true;
    #endif
}

/**
 * Safe unsigned integer multiplication
 */
static inline bool safe_mul_uint(unsigned int a, unsigned int b, unsigned int *result) {
    #if __GNUC__ >= 5 || __clang__
        return !__builtin_mul_overflow(a, b, result);
    #else
        if (a != 0 && b > UINT_MAX / a) {
            return false;
        }
        *result = a * b;
        return true;
    #endif
}

/**
 * Safe unsigned integer addition
 */
static inline bool safe_add_uint(unsigned int a, unsigned int b, unsigned int *result) {
    #if __GNUC__ >= 5 || __clang__
        return !__builtin_add_overflow(a, b, result);
    #else
        if (a > UINT_MAX - b) {
            return false;
        }
        *result = a + b;
        return true;
    #endif
}

/**
 * Safe allocation size calculation for arrays
 *
 * Computes n * size with overflow check, suitable for use with g_malloc0_n
 *
 * @param n Number of elements
 * @param size Size of each element
 * @param[out] total Pointer to store total size
 * @return true if calculation succeeded, false if overflow occurred
 *
 * Example:
 *   size_t total;
 *   if (!safe_alloc_size(num_entries, sizeof(Entry), &total)) {
 *       g_error("Array allocation would overflow");
 *       return NULL;
 *   }
 *   array = g_malloc0(total);
 */
static inline bool safe_alloc_size(size_t n, size_t size, size_t *total) {
    return safe_mul_size(n, size, total);
}

/**
 * Safe array reallocation size
 *
 * Computes new_size = old_size + (n * element_size) with overflow checking
 *
 * @param old_size Current allocation size
 * @param n Number of elements to add
 * @param element_size Size of each element
 * @param[out] new_size Pointer to store new total size
 * @return true if calculation succeeded, false if overflow occurred
 */
static inline bool safe_realloc_size(size_t old_size, size_t n, size_t element_size, size_t *new_size) {
    size_t additional;
    if (!safe_mul_size(n, element_size, &additional)) {
        return false;
    }
    return safe_add_size(old_size, additional, new_size);
}

/**
 * Safe alignment calculation
 *
 * Rounds size up to next multiple of alignment
 *
 * @param size Size to align
 * @param alignment Alignment (must be power of 2)
 * @param[out] aligned Pointer to store aligned size
 * @return true if calculation succeeded, false if overflow occurred
 */
static inline bool safe_align(size_t size, size_t alignment, size_t *aligned) {
    // alignment must be power of 2
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        return false;
    }

    size_t mask = alignment - 1;
    size_t temp;

    if (!safe_add_size(size, mask, &temp)) {
        return false;
    }

    *aligned = temp & ~mask;
    return true;
}

#endif /* ROFI_SAFE_MATH_H */
