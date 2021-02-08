#include <stdint.h>
#include <stdlib.h>

#include "simpcg.h"

#include "pixel-buffer.h" // Interface

struct SCGPixelBuffer scg_pixel_buffer_create(uint8_t width, uint8_t height)
{
	struct SCGBuffer *p_buffer = scg_buffer_create(width * 2, height);

	struct SCGPixelBuffer board = { width, height, p_buffer };

	scg_buffer_fill_ch(board.p_buffer, ' ');

	return board;
}

void scg_pixel_buffer_destroy(struct SCGPixelBuffer *p_board)
{
	scg_buffer_destroy(p_board->p_buffer);
}

void scg_pixel_buffer_set(struct SCGPixelBuffer *p_board, uint8_t col, uint8_t row, enum SCGColorCode color)
{
	scg_buffer_set_bg_color(p_board->p_buffer, col * 2, row, color);
	scg_buffer_set_bg_color(p_board->p_buffer, col * 2 + 1, row, color);
}

enum SCGColorCode scg_pixel_buffer_get(struct SCGPixelBuffer *p_board, uint8_t col, uint8_t row)
{
	return scg_buffer_get_bg_color(p_board->p_buffer, col * 2, row);
}

void scg_pixel_buffer_fill(struct SCGPixelBuffer *p_board, enum SCGColorCode color)
{
	scg_buffer_fill_bg_color(p_board->p_buffer, color);
}

void scg_pixel_buffer_make_space(struct SCGPixelBuffer *p_board)
{
	scg_buffer_make_space(p_board->p_buffer);
}

void scg_pixel_buffer_remove_space(struct SCGPixelBuffer *p_board)
{
	scg_buffer_remove_space(p_board->p_buffer);
}

void scg_pixel_buffer_print(struct SCGPixelBuffer *p_board)
{
	scg_buffer_print(p_board->p_buffer);
}

