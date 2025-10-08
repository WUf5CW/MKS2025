/*
 * sct.c
 *
 *  Created on: Oct 8, 2025
 *      Author: xhomol26
 */


#include "sct.h"

void sct_init(void) {
    sct_led(0);
}

void sct_led(uint32_t value) {
    for (uint32_t i = 0; i < 32; i++) {
        // Write LSB to SDI
        HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin, (value >> i) & 0b1);

        // Pulse CLK
        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 1);
        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 0);
    }

    // Pulse NLA to latch data
    HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 1);
    HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 0);
}
