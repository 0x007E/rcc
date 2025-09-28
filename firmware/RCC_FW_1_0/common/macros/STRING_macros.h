/**
 * @file STRING_macros.h
 * @brief Helper macros for stringification in C preprocessor.
 * 
 * This header file provides macros for converting tokens or macro values into string literals using the C preprocessor.
 * 
 * It contains two essential macros:
 * - `_STR2(x)` : Performs direct stringization of a token without expansion.
 * - `_STR(x)`  : Expands a macro argument before converting it into a string literal.
 * 
 * These utilities are commonly used when embedding macro constants into message strings, compiler attributes, or debug output.
 * 
 * @note `_STR()` should be preferred over `_STR2()` since it ensures proper expansion of macro arguments before stringization.
 * 
 * @author g.raf
 * @date 2025-09-18
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 * 
 * @note This file is part of a larger project and subject to the license specified in the repository. For updates and the complete revision history, see the GitHub repository.
 * 
 * @see https://github.com/0x007e/avr-common  "AVR Common GitHub Repository"
 */

#ifndef STRING_MACROS_H_
#define STRING_MACROS_H_

    /**
     * @def _STR2
     * @brief Converts a macro argument into a string literal (internal helper).
     *
     * @details
     * This macro takes a single argument `x` and applies the preprocessor stringization operator (`#`), converting the token into a string literal. It is usually not used directly, but rather invoked via the higher-level macro `_STR()`, which ensures proper macro expansion before stringization.
     *
     * @param x Token to be converted into a string literal.
     *
     * @return A string literal representing the input token.
     *
     * @note This macro does not expand other macros passed as `x` before stringizing. Use `_STR()` for full expansion.
     */
    #ifndef _STR2
        #define _STR2(x) #x
    #endif

    /**
     * @def _STR
     * @brief Converts a macro argument into a string literal (with expansion).
     *
     * @details
     * This macro first expands its argument `x` (if it is another macro), and then calls `_STR2(x)` to convert it into a string literal.
     *
     * For example:
     * @code
     * #define VALUE 42
     * const char *str1 = _STR(VALUE); // expands to "42"
     * const char *str2 = _STR(TEXT);  // expands to "TEXT"
     * @endcode
     *
     * @param x Token or macro to be expanded and converted into a string literal.
     *
     * @return A string literal representing the expanded input.
     *
     * @note This macro is the preferred way to stringify macros, since it guarantees full expansion before stringization.
     */
    #ifndef _STR
        #define _STR(x) _STR2(x)
    #endif

#endif /* STRING_MACROS_H_ */