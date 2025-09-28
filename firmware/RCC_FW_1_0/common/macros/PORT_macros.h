
/**
 * @file PORT_macros.h
 * @brief Macro definitions for port operations.
 * 
 * This header file contains essential macros for handling port and pin names.
 * 
 * @note These macros serve to improve code clarity and maintain protocol compliance by providing meaningful names for standard port operation indicators.
 * 
 * @author g.raf
 * @date 2025-09-14
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 * 
 * @note This file is part of a larger project and subject to the license specified in the repository. For updates and the complete revision history, see the GitHub repository.
 * 
 * @see https://github.com/0x007e/avr-common  "AVR Common GitHub Repository"
 */

#ifndef PORT_MACROS_H_
#define PORT_MACROS_H_

    /**
     * @def SET_PIN
     * @brief Concatenates pin identifier components into a full pin name.
     *
     * @details
     * This macro concatenates the token `PIN`, the given pin `NUMBER`, and the specified `ARGUMENT` to form a complete pin identifier.
     * 
     * For example, `SET_PIN(3, _CTRL)` expands to `PIN3_CTRL`.
     * 
     * This is useful for handling pin-related registers or bitfields in a flexible and generic way within embedded code.
     *
     * @note The macro uses the token-pasting operator (`##`), so arguments must be valid tokens suitable for concatenation.
     */
    #ifndef SET_PIN
        #define SET_PIN(NUMBER, ARGUMENT) PIN ## NUMBER ## ARGUMENT
    #endif

    /**
     * @def SET_PORT
     * @brief Concatenates port identifier components into a full port name.
     *
     * @details
     * This macro concatenates the token `PORT`, the given port `NAME`, and the specified `ARGUMENT` to form a complete port identifier.
     *
     * For example, `SET_PORT(A, .DIR)` expands to `PORTA.DIR`.
     *
     * This is useful for handling port-related registers or bitfields in a flexible and generic way within embedded code.
     *
     * @note The macro uses the token-pasting operator (`##`), so arguments must be valid tokens suitable for concatenation.
     */
    #ifndef SET_PORT
        #define SET_PORT(NAME, ARGUMENT) PORT ## NAME ## ARGUMENT
    #endif

#endif /* PORT_MACROS_H_ */