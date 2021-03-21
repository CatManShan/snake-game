#include <stdlib.h>
#include <time.h>

#include "../mem-macros/mem-macros.h"

#include "snake.h" // Interface

#ifdef MEM_DEBUG
#include "../mem-debug/mem-debug.h"
#endif

static void snake_move_head(struct Snake *snake, uint8_t bounds_width, uint8_t bounds_height);
static bool snake_is_head_overlapping_tail(struct Snake *snake);
static void snake_expand(struct Snake *snake);
static bool snake_is_touching_food(struct Snake *snake, struct SnakeFood *food);

struct Snake *snake_create(size_t max_length, size_t init_length)
{
	struct Snake *snake = ALLOC_FLEX_STRUCT(snake, segments, max_length);

	snake->alive = true;
	snake->direction = SNAKE_DIRECTION_DOWN;
	snake->max_length = max_length;
	snake->length = init_length;

	for (size_t segment_num = 1; segment_num < init_length; segment_num++) {
		snake->segments[segment_num] = (struct SnakeSegment) { 0, 0 };
	}

	return snake;
}

void snake_destroy(struct Snake *snake)
{
	free(snake);
}

void snake_move(struct Snake *snake, uint8_t bounds_width, uint8_t bounds_height)
{
	for (size_t segment_num = snake->length - 1; segment_num > 0; segment_num--) {
		struct SnakeSegment *p_segment = &snake->segments[segment_num];
		struct SnakeSegment *p_segment_next = &snake->segments[segment_num - 1];

		p_segment->x = p_segment_next->x;
		p_segment->y = p_segment_next->y;
	}

	snake_move_head(snake, bounds_width, bounds_height);

	if (snake_is_head_overlapping_tail(snake)) {
		snake->alive = false;
	}
}

void snake_turn_if_possible(struct Snake *snake, enum SnakeDirection direction)
{
	/* UP % 2 == DOWN % 2
	 *   and
	 * LEFT % 2 == RIGHT % 2
	 *
	 * (snake can only turn perpendicular to current direction)
	 */
	bool can_turn = (direction % 2 != snake->direction % 2);

	if (can_turn) {
		snake->direction = direction;
	}
}

void snake_eat_if_touching_food(struct Snake *snake, struct SnakeFood *food, uint8_t bounds_width, uint8_t bounds_height)
{
	struct SnakeSegment head = snake->segments[0];

	bool touching_food = head.x == food->x && head.y == food->y;
	if (touching_food) {
		snake_expand(snake);
		if (snake->length < snake->max_length) {
			snake_food_move(food, snake, bounds_width, bounds_height);
		}
	}
}

void snake_food_move(struct SnakeFood *food, struct Snake *snake, uint8_t bounds_width, uint8_t bounds_height)
{
	while (snake_is_touching_food(snake, food)) {
		food->x = rand() % bounds_width;
		food->y = rand() % bounds_height;
	}
}

static void snake_move_head(struct Snake *snake, uint8_t bounds_width, uint8_t bounds_height)
{
	struct SnakeSegment *p_head = &snake->segments[0];

	switch (snake->direction) {
	case SNAKE_DIRECTION_UP:
		if (p_head->y > 0) {
			--p_head->y;
		} else {
			snake->alive = false;
		}
	break;
	case SNAKE_DIRECTION_DOWN:
		if (p_head->y < bounds_height - 1) {
			++p_head->y;
		} else {
			snake->alive = false;
		}

	break;
	case SNAKE_DIRECTION_LEFT:
		if (p_head->x > 0) {
			--p_head->x;
		} else {
			snake->alive = false;
		}

	break;
	case SNAKE_DIRECTION_RIGHT:
		if (p_head->x < bounds_width - 1) {
			++p_head->x;
		} else {
			snake->alive = false;
		}

	break;
	}
}

static bool snake_is_head_overlapping_tail(struct Snake *snake)
{
	struct SnakeSegment head = snake->segments[0];
	for (size_t segment_num = 1; segment_num < snake->length; segment_num++) {
		struct SnakeSegment segment = snake->segments[segment_num];
		if (head.x == segment.x && head.y == segment.y) {
			return true;
		}
	}

	return false;
}

static void snake_expand(struct Snake *snake)
{
	if (snake->length < snake->max_length) {
		++snake->length;
		snake->segments[snake->length - 1] = snake->segments[snake->length - 2];
	}
}

static bool snake_is_touching_food(struct Snake *snake, struct SnakeFood *food)
{
	for (size_t segment_num = 0; segment_num < snake->length; segment_num++) {
		struct SnakeSegment segment = snake->segments[segment_num];
		if (segment.x == food->x && segment.y == food->y) {
			return true;
		}
	}

	return false;
}

void snake_seed_rng()
{
	srand(time(NULL) + clock());
}

