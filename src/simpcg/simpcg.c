#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "simpcg.h" // Interface

static void scg_print_cell(struct SCGCell cell);
static uint8_t scg_color_code_to_ansi_fg(enum SCGColorCode color_code);
static uint8_t scg_color_code_to_ansi_bg(enum SCGColorCode color_code);

struct SCGBuffer *scg_buffer_create(uint8_t width, uint8_t height)
{
	size_t cells_size = width * height;
	struct SCGBuffer *p_buffer = malloc(sizeof *p_buffer + sizeof p_buffer->cells[0] * cells_size);

	p_buffer->width = width;
	p_buffer->height = height;

	return p_buffer;
}

void scg_buffer_destroy(struct SCGBuffer *p_buffer)
{
	free(p_buffer);
}

inline void scg_buffer_set_ch(struct SCGBuffer *p_buffer, uint8_t col, uint8_t row, char ch)
{
	p_buffer->cells[row * p_buffer->width + col].ch = ch;
}

inline char scg_buffer_get_ch(struct SCGBuffer *p_buffer, uint8_t col, uint8_t row)
{
	return p_buffer->cells[row * p_buffer->width + col].ch;
}

inline void scg_buffer_set_fg_color(struct SCGBuffer *p_buffer, uint8_t col, uint8_t row, enum SCGColorCode fg_color)
{
	p_buffer->cells[row * p_buffer->width + col].fg_color = fg_color;
}

inline enum SCGColorCode scg_buffer_get_fg_color(struct SCGBuffer *p_buffer, uint8_t col, uint8_t row)
{
	return p_buffer->cells[row * p_buffer->width + col].fg_color;
}

inline void scg_buffer_set_bg_color(struct SCGBuffer *p_buffer, uint8_t col, uint8_t row, enum SCGColorCode bg_color)
{
	p_buffer->cells[row * p_buffer->width + col].bg_color = bg_color;
}

inline enum SCGColorCode scg_buffer_get_bg_color(struct SCGBuffer *p_buffer, uint8_t col, uint8_t row)
{
	return p_buffer->cells[row * p_buffer->width + col].bg_color;
}

void scg_buffer_fill_ch(struct SCGBuffer *p_buffer, char ch)
{
	uint8_t width = p_buffer->width;
	uint8_t height = p_buffer->height;
	for (uint8_t row = 0; row < height; row++) {
		for (uint8_t col = 0; col < width; col++) {
			scg_buffer_set_ch(p_buffer, col, row, ch);
		}
	}
}

void scg_buffer_fill_fg_color(struct SCGBuffer *p_buffer, enum SCGColorCode fg_color)
{
	uint8_t width = p_buffer->width;
	uint8_t height = p_buffer->height;
	for (uint8_t row = 0; row < height; row++) {
		for (uint8_t col = 0; col < width; col++) {
			scg_buffer_set_fg_color(p_buffer, col, row, fg_color);
		}
	}
}

void scg_buffer_fill_bg_color(struct SCGBuffer *p_buffer, enum SCGColorCode bg_color)
{
	uint8_t width = p_buffer->width;
	uint8_t height = p_buffer->height;
	for (uint8_t row = 0; row < height; row++) {
		for (uint8_t col = 0; col < width; col++) {
			scg_buffer_set_bg_color(p_buffer, col, row, bg_color);
		}
	}
}

void scg_buffer_make_space(struct SCGBuffer *p_buffer)
{
	for (uint8_t row = 0; row < p_buffer->height; row++) {
		printf("\n\x1b[G"); // Add <height> lines to bottom of console
	}
}

void scg_buffer_remove_space(struct SCGBuffer *p_buffer)
{
	printf("\x1b[%dA", p_buffer->height); // Move to top of buffer
	printf("\x1b[G"); // Move to 1st column
	printf("\x1b[J"); // Clear to bottom line
}

void scg_buffer_print(struct SCGBuffer *p_buffer)
{
	uint8_t width = p_buffer->width;
	uint8_t height = p_buffer->height;

	printf("\x1b[G"); // Move to 1st column
	printf("\x1b[%dA", height); // Move to top of buffer
	for (uint8_t row = 0; row < height; row++) {
		for (uint8_t col = 0; col < width; col++) {
			scg_print_cell(p_buffer->cells[row * width + col]);
		}
		printf("\x1b[B"); // Move down 1 line
		printf("\x1b[G"); // Move to 1st column
	}

	fflush(stdout);
}

// Static Functions

static void scg_print_cell(struct SCGCell cell)
{
	uint8_t ansi_fg_code = scg_color_code_to_ansi_fg(cell.fg_color);
	uint8_t ansi_bg_code = scg_color_code_to_ansi_bg(cell.bg_color);
	printf("\x1b[%d;%dm%c\x1b[0m", ansi_fg_code, ansi_bg_code, cell.ch); // Print char with specified fg and bg color
}

static uint8_t scg_color_code_to_ansi_fg(enum SCGColorCode color_code)
{
	return color_code + 39;
}

static uint8_t scg_color_code_to_ansi_bg(enum SCGColorCode color_code)
{
	return color_code + 49;
}

void scg_input_adjust()
{
	system("stty raw -echo");
}

void scg_input_restore()
{
	system("stty cooked echo");
}

