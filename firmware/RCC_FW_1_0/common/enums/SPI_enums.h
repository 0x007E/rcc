/**
 * @file SPI_enums.h
 * @brief Enumerations for SPI interface configurations and status.
 * 
 * This header file defines enumerations used to represent various configuration parameters and status codes for the SPI (Serial Peripheral Interface) communication protocol. It includes enums for bit order (MSB/LSB), clock edge selections for polarity and phase, SPI enable/disable states, and communication status including reception and collision indicators.
 * 
 * @note These enums improve code readability by providing meaningful symbolic names for SPI configuration values and status codes, facilitating protocol-compliant and maintainable software design.
 * 
 * @author g.raf
 * @date 2025-09-28
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 * 
 * @note This file is part of a larger project and subject to the license specified in the repository. For updates and the complete revision history, see the GitHub repository.
 * 
 * @see https://github.com/0x007e/avr-common  "AVR Common GitHub Repository"
 */

#ifndef SPI_ENUM_H_
#define SPI_ENUM_H_

    /**
     * @enum SPI_Direction_t
     * @brief Defines the bit order for SPI data transmission.
     *
     * @details
     * This enumeration specifies the order in which bits are transmitted over the SPI bus. Selecting either most significant bit (MSB) first or least significant bit (LSB) first affects how data bytes are shifted out and received on the SPI interface.
     */
    enum SPI_Direction_t
    {
        SPI_MSB=0,  /**< Data is transmitted with the most significant bit first */
        SPI_LSB     /**< Data is transmitted with the least significant bit first */
    };

    /**
     * @typedef SPI_Direction
     * @brief Alias for enum SPI_Direction_t representing SPI bit order options.
     */
    typedef enum SPI_Direction_t SPI_Direction;

    /**
     * @enum SPI_Edge_t
     * @brief Defines the clock edge options for SPI polarity and phase.
     *
     * @details
     * This enumeration specifies the clock edges that control when data is sampled and shifted in SPI communication. The edge can be either the rising edge or the falling edge of the clock signal.
     *
     * @note Rising edge refers to the transition of the clock signal from low to high, while falling edge  refers to the transition from high to low. The choice affects the timing of data clocking and must match the SPI slave device's expectations.
     */
    enum SPI_Edge_t
    {
        SPI_Rising=0,   /**< Data is sampled or shifted on the rising clock edge */
        SPI_Falling     /**< Data is sampled or shifted on the falling clock edge */
    };

    /**
     * @typedef SPI_Polarity
     * @brief Alias for enum SPI_Edge_t representing the clock polarity edge.
     */
    typedef enum SPI_Edge_t SPI_Polarity;
    /**
     * @typedef SPI_Phase
     * @brief Alias for enum SPI_Edge_t representing the clock phase edge.
     */
    typedef enum SPI_Edge_t SPI_Phase;

    /**
     * @enum SPI_Select_t
     * @brief Defines the SPI enable or disable states.
     *
     * @details
     * This enumeration specifies whether the SPI interface is enabled or disabled. It is used to control the activation state of the SPI bus for managing slave select (SS) signal or SPI operation mode.
     */
    enum SPI_Select_t
    {
        SPI_Disable=0,  /**< SPI interface disabled */
        SPI_Enable      /**< SPI interface enabled */
    };

    /**
     * @typedef SPI_Select
     * @brief Alias for enum SPI_Select_t representing SPI enable states.
     */
    typedef enum SPI_Select_t SPI_Select;

    /**
     * @enum SPI_Status_t
     * @brief Represents the status of SPI communication events.
     *
     * @details
     * This enumeration indicates various SPI data reception and error states.
     * - `SPI_Received` signals that data has been successfully received.
     * - `SPI_Collision` indicates a collision error, such as simultaneous data transmission.
     * - `SPI_None` means that no SPI event or status is currently active.
     */
    enum SPI_Status_t
    {
        SPI_Received = 0,  /**< Data has been received successfully */
        SPI_Collision,     /**< Collision detected during SPI communication */
        SPI_Master_Abort,  /**< Master mode initialization aborted */
        SPI_None           /**< No SPI event or status */
    };

    /**
     * @typedef SPI_Status
     * @brief Alias for enum SPI_Status_t representing SPI communication status codes.
     */
    typedef enum SPI_Status_t SPI_Status;

#endif /* SPI_ENUM_H_ */