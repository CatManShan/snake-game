/* Dependencies:
 * <stdint.h>
 * "../simpcg/simpcg.h"
 */

#ifndef scg_pixel_buffer_h
#define scg_pixel_buffer_h

struct SCGPixelBuffer {
	uint8_t width;
	uint8_t height;

	struct SCGBuffer *p_buffer;
};

struct SCGPixelBuffer scg_pixel_buffer_create(uint8_t width, uint8_t height);
void scg_pixel_buffer_destroy(struct SCGPixelBuffer *p_board);

void scg_pixel_buffer_set(struct SCGPixelBuffer *p_board, uint8_t col, uint8_t row,
		enum SCGColorCode color);
enum SCGColorCode scg_pixel_buffer_get(struct SCGPixelBuffer *p_board, uint8_t col, uint8_t row);

void scg_pixel_buffer_fill(struct SCGPixelBuffer *p_board, enum SCGColorCode color);

void scg_pixel_buffer_make_space(struct SCGPixelBuffer *p_board);
void scg_pixel_buffer_remove_space(struct SCGPixelBuffer *p_board);

void scg_pixel_buffer_print(struct SCGPixelBuffer *p_board);

#endif // scg_pixel_buffer_h

