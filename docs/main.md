# Hardware Programming Cookbook

## Introduction
This document contains information that will help with programming the ATMega328PB microprocessor used in the project hardware.

## ATMega328PB Pin Assignment and Peripherals
![ATMega328PB pinout](https://camo.githubusercontent.com/17f4bf2114dedf9ceddc91c5047ace57a72816c50dad7f2510a260e74bf143c7/68747470733a2f2f692e696d6775722e636f6d2f5a51736a4c774c2e6a7067)

* UART 0: Communication with Bluetooth module.
* UART 1: Debug serial communication. Shares pins with SPI 0.
* SPI 0: ISCP. Shares pins with UART 1.
* SPI 1: Communication with ADCs and IMU.
* I2C 0: Not used.
* I2C 1: Not used.
* Hardware timer 0: Both channels used for motor PWM control.
* Hardware timer 1: Both channels used for motor PWM control.
* Hardware timer 2: Channel B used for motor PWM control. Channel A unused.
* Hardware timer 3: Not used.
* Hardware timer 4: Not used.

| Pin identifier | Pin assignment |
| -------------- | -------------- |
| PB0 | Peripheral sleep/reset (GPIO) |
| PB1 | Timer/counter 1 output channel A (PWM) |
| PB2 | Timer/counter 1 output channel B (PWM) |
| PB3 | SPI 0 MISO; UART 1 TX |
| PB4 | SPI 0 MOSI; UART 1 RX |
| PB5 | SPI 0 SCK |
| PB6 | Motor 1 fault (GPIO interrupt) |
| PB7 | Motor 2 fault (GPIO interrupt) |
| PC0 | SPI 1 MISO |
| PC1 | SPI 1 SCK |
| PC2 | ADC 2 chip select (GPIO active-low) |
| PC3 | ADC 3 chip select (GPIO active-low) |
| PC4 | Motor phase control 2 (GPIO) |
| PC5 | IMU chip select (GPIO active-low) |
| PC6 | Reset input (GPIO active-low, external pull-up) |
| PD0 | UART 0 RX |
| PD1 | UART 0 TX |
| PD2 | Motor phase control 1 (GPIO) |
| PD3 | Timer/counter 2 output channel B (PWM) |
| PD4 | Motor 5 fault (GPIO interrupt) |
| PD5 | Timer/counter 0 output channel B (PWM) |
| PD6 | Timer/counter 0 output channel A (PWM) |
| PD7 | Motor phase control 3 (GPIO) |
| PE0 | Motor 3 fault (GPIO interrupt) |
| PE1 | Motor 4 fault (GPIO interrupt) |
| PE2 | ADC 1 chip select (GPIO active-low) |
| PE3 | SPI 1 MOSI |

### SPI pin names
* MOSI = Master Out Slave In
* MISO = Master In Slave Out
* SS/CS = Slave Select/Chip Select

All SPI devices share the same MOSI/MISO pins.
Each slave device has a unique CS pin.
CS pins are **ACTIVE-LOW**.
They must be kept high when idle, and should be pulled low by the master (our microcontroller) when we want to communicate with the slave.
Only one CS pin should be low at a time.

## ATMega328PB Programming Guide

## ADC Usage Guide

## Motor Driver Usage Guide

## IMU Usage Guide

## Bluetooth Usage Guide