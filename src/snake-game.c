#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mem-macros/mem-macros.h"
#include "option-map/option-map.h"
#include "simpcg/simpcg.h"
#include "simpcg/pixel-buffer.h"
#include "snake/snake.h"

#ifdef MEM_DEBUG
#include "mem-debug/mem-debug.h"
#endif

struct CrossThreadData {
	volatile enum SnakeDirection input_direction;
	volatile bool quit;
};

struct GameOptions {
	bool quit_on_loss;
	uint8_t speed;
	uint8_t width;
	uint8_t height;
};

static struct GameOptions parse_options(int argc, char **argv);
static void draw_game_buffer(struct SCGPixelBuffer *game_buffer, struct Snake *snake, struct SnakeFood food);
static void *parse_input_func(void *vp_data);

int main(int argc, char **argv)
{
#ifdef MEM_DEBUG
	FILE *log_file = fopen("mem-debug.log", "w");
	debug_start();
	debug_set_out_stream(log_file);
#endif

	struct GameOptions game_options = parse_options(argc - 1, &argv[1]);

	// Game Setup

	struct SCGPixelBuffer *game_buffer = scg_pixel_buffer_create(game_options.width, game_options.height);
	scg_pixel_buffer_make_space(game_buffer);
	scg_input_adjust();

	size_t snake_max_length = game_options.width * game_options.height;
	size_t snake_init_length = 4;
	struct Snake *snake = snake_create(snake_max_length, snake_init_length);

	struct SnakeFood food = { 0, 0 };
	snake_seed_rng();
	snake_food_move(&food, snake, game_options.width, game_options.height);

	// Thread Setup

	struct CrossThreadData data;
	data.input_direction = snake->direction;
	data.quit = false;

	pthread_t input_thread;
	pthread_create(&input_thread, NULL, &parse_input_func, (void *) &data);

	// Main loop

	while (!data.quit) {
		draw_game_buffer(game_buffer, snake, food);
		scg_pixel_buffer_print(game_buffer);

		usleep(1000000 / game_options.speed);

		if (snake->alive) {
			snake_turn_if_possible(snake, data.input_direction);
			snake_move(snake, game_options.width, game_options.height);
			snake_eat_if_touching_food(snake, &food, game_options.width, game_options.height);
		} else if (game_options.quit_on_loss) {
			data.quit = true;
		}
	}

	pthread_join(input_thread, NULL);

	// Cleanup
	
	snake_destroy(snake);

	scg_pixel_buffer_remove_space(game_buffer);
	scg_pixel_buffer_destroy(game_buffer);
	scg_input_restore();

#ifdef MEM_DEBUG
	fprintf(log_file, "Unfreed pointers:\n");
	debug_print_allocated();
	debug_end();
	fclose(log_file);
#endif

	return EXIT_SUCCESS;
}

static struct GameOptions parse_options(int argc, char **argv)
{
	const char *MISSING_ARG_MSG = "the '%s' option requires an argument";
	const char *UNKOWN_OPTION_MSG = "unknown option '%s'";

	char *quit_on_loss_aliases[] = {"--quit-on-loss",    "-q", NULL};
	char *size_aliases[]         = {"--size",            "-s", NULL};
	char *speed_aliases[]        = {"--speed",           "-S", NULL};

	struct OptionMapOption options[] = {
		{ .aliases = quit_on_loss_aliases, .takes_value = false },
		{ .aliases = size_aliases,         .takes_value = true  },
		{ .aliases = speed_aliases,        .takes_value = true  }
	};

	struct OptionMap *option_map = option_map_create(options, 3);
	struct OptionMapError error = option_map_set_options(option_map, argc, argv);

	if (error.error_code != OM_NO_ERROR) {
		option_map_print_error_message(stderr, "snake-game: ", error);
		exit(EXIT_FAILURE);
	}

	struct GameOptions game_options = {
		.quit_on_loss = false,
		.speed = 15,
		.width = 32,
		.height = 32,
	};

	// Parse options
	
	game_options.quit_on_loss = option_map_is_option_given(option_map, "--quit-on-loss");

	if (option_map_is_option_given(option_map, "--size")) {
		char *value = option_map_get_option_value(option_map, "--size");
		sscanf(value, "%hhux%hhu", &game_options.width, &game_options.height);
	}
	
	if (option_map_is_option_given(option_map, "--speed")) {
		char *value = option_map_get_option_value(option_map, "--speed");
		sscanf(value, "%hhu", &game_options.speed);
	}

	option_map_destroy(option_map);

	return game_options;
}

static void draw_game_buffer(struct SCGPixelBuffer *game_buffer, struct Snake *snake, struct SnakeFood food)
{
	scg_pixel_buffer_fill(game_buffer, SCG_COLOR_BRIGHT_BLACK);

	for (size_t segment_num = snake->length - 1; segment_num != (size_t) -1; --segment_num) {
		uint8_t x = snake->segments[segment_num].x;
		uint8_t y = snake->segments[segment_num].y;

		enum SCGColorCode segment_color;
		if (snake->alive) {
			segment_color = (segment_num == 0) ? SCG_COLOR_BRIGHT_GREEN : SCG_COLOR_GREEN;
		} else {
			segment_color = (segment_num == 0) ? SCG_COLOR_BRIGHT_RED : SCG_COLOR_RED;
		}
		scg_pixel_buffer_set(game_buffer, x, y, segment_color);
	}

	scg_pixel_buffer_set(game_buffer, food.x, food.y, SCG_COLOR_BRIGHT_WHITE);
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

	return NULL;
}

