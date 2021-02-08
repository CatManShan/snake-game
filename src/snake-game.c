#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "simpcg/simpcg.h"
#include "simpcg/pixel-buffer.h"
#include "snake/snake.h"

struct CrossThreadData {
	volatile enum SnakeDirection input_direction;
	volatile bool quit;
};

static void *parse_input_func(void *vp_data);

static void draw_game_buffer(struct SCGPixelBuffer *p_game_buffer, struct Snake *p_snake, struct SnakeFood food);

static void log_append(const char *format, ...);

int main()
{
	// Set up buffer

	struct SCGPixelBuffer game_buffer = scg_pixel_buffer_create(32, 32);
	scg_pixel_buffer_make_space(&game_buffer);
	scg_input_adjust();

	// Set up game

	snake_seed_rng();

	size_t snake_max_length = game_buffer.width * game_buffer.height;
	size_t snake_init_length = 4;
	struct Snake *p_snake = snake_create(snake_max_length, snake_init_length);

	struct SnakeFood food = { 0, 0 };
	snake_food_move(&food, p_snake, game_buffer.width, game_buffer.height);

	// Set up input thread

	struct CrossThreadData data;
	data.input_direction = p_snake->direction;
	data.quit = false;

	pthread_t input_thread;
	pthread_create(&input_thread, NULL, &parse_input_func, (void *) &data);

	// Main loop

	while (!data.quit) {
		draw_game_buffer(&game_buffer, p_snake, food);
		scg_pixel_buffer_print(&game_buffer);

		usleep(1000000 / 15);

		if (p_snake->alive) {
			snake_turn_if_possible(p_snake, data.input_direction);
			snake_move(p_snake, game_buffer.width, game_buffer.height);
			snake_eat_if_touching_food(p_snake, &food, game_buffer.width, game_buffer.height);
		}
	}

	// Cleanup
	
	snake_destroy(p_snake);

	scg_pixel_buffer_remove_space(&game_buffer);
	scg_pixel_buffer_destroy(&game_buffer);
	scg_input_restore();

	return 0;
}

static void draw_game_buffer(struct SCGPixelBuffer *p_game_buffer, struct Snake *p_snake, struct SnakeFood food)
{
	scg_pixel_buffer_fill(p_game_buffer, SCG_COLOR_BRIGHT_BLACK);

	for (size_t segment_num = p_snake->length - 1; segment_num != (size_t) -1; segment_num--) {
		uint8_t x = p_snake->segments[segment_num].x;
		uint8_t y = p_snake->segments[segment_num].y;

		enum SCGColorCode segment_color;
		if (p_snake->alive) {
			segment_color = (segment_num == 0) ? SCG_COLOR_BRIGHT_GREEN : SCG_COLOR_GREEN;
		} else {
			segment_color = (segment_num == 0) ? SCG_COLOR_BRIGHT_RED : SCG_COLOR_RED;
		}
		scg_pixel_buffer_set(p_game_buffer, x, y, segment_color);
	}

	scg_pixel_buffer_set(p_game_buffer, food.x, food.y, SCG_COLOR_BRIGHT_WHITE);
}

static void *parse_input_func(void *vp_data)
{
	struct CrossThreadData *p_data = (struct CrossThreadData *) vp_data;

	char c;
	while (!p_data->quit) {
		c = getchar();

		switch (c) {
		case 'w':
			p_data->input_direction = SNAKE_DIRECTION_UP;
			break;
		case 'a':
			p_data->input_direction = SNAKE_DIRECTION_LEFT;
			break;
		case 's':
			p_data->input_direction = SNAKE_DIRECTION_DOWN;
			break;
		case 'd':
			p_data->input_direction = SNAKE_DIRECTION_RIGHT;
			break;
		case 'q':
			p_data->quit = true;
			break;
		}
	}
}

static void log_append(const char *format, ...)
{
	FILE *log_file = fopen("out.log", "a");

	va_list vargs;
	va_start(vargs, format);
	vfprintf(log_file, format, vargs);
	va_end(vargs);

	fclose(log_file);
}

