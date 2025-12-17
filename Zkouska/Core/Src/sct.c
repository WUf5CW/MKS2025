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



static const uint32_t outer_segments[] = {										// Segments of the display in order
    (1UL << 0),
    (1UL << 1),
    (1UL << 5),
	(1UL << 17),
	(1UL << 18),
	(1UL << 28),
	(1UL << 29),
	(1UL << 31),
	(1UL << 9),
    (1UL << 11),
	(1UL << 13),
	(1UL << 14),
};

static int8_t current_index = 0;												// Variable to store the current position

void sct_cycle_outer_segments(uint8_t direction) {
	uint8_t count = sizeof(outer_segments) / sizeof(outer_segments[0]);         // Cycle through outer segments in a loop
	if (direction == 0) {  														// Backward
		sct_led(outer_segments[current_index]);
		current_index++;
		if (current_index >= count) current_index = 0;  					// Wrap around
	} else {  																// Forward
		sct_led(outer_segments[current_index]);
		current_index--;
		if (current_index < 0) current_index = count - 1;  					// Wrap around
	}
}
