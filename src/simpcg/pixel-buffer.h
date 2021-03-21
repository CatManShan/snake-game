#ifndef scg_pixel_buffer_h
#define scg_pixel_buffer_h

#include <stdint.h>

#include "simpcg.h"

struct SCGPixelBuffer {
	struct SCGBuffer internal_buffer;
};

struct SCGPixelBuffer *scg_pixel_buffer_create(uint8_t width, uint8_t height);
void scg_pixel_buffer_destroy(struct SCGPixelBuffer *board);

void scg_pixel_buffer_set(struct SCGPixelBuffer *board, uint8_t col, uint8_t row,
		enum SCGColorCode color);
enum SCGColorCode scg_pixel_buffer_get(struct SCGPixelBuffer *board, uint8_t col, uint8_t row);

void scg_pixel_buffer_fill(struct SCGPixelBuffer *board, enum SCGColorCode color);

void scg_pixel_buffer_make_space(struct SCGPixelBuffer *board);
void scg_pixel_buffer_remove_space(struct SCGPixelBuffer *board);

void scg_pixel_buffer_print(struct SCGPixelBuffer *board);

#endif // scg_pixel_buffer_h

