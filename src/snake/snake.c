#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "snake.h" // Interface

static void snake_move_head(struct Snake *p_snake, uint8_t bounds_width, uint8_t bounds_height);
static bool snake_is_head_overlapping_tail(struct Snake *p_snake);
static void snake_expand(struct Snake *p_snake);
static bool snake_is_touching_food(struct Snake *p_snake, struct SnakeFood *p_food);

struct Snake *snake_create(size_t max_length, size_t init_length)
{
	struct Snake *p_snake = malloc(sizeof *p_snake + max_length * sizeof p_snake->segments[0]);

	p_snake->alive = true;
	p_snake->direction = SNAKE_DIRECTION_DOWN;
	p_snake->max_length = max_length;
	p_snake->length = init_length;

	for (size_t segment_num = 1; segment_num < init_length; segment_num++) {
		p_snake->segments[segment_num] = (struct SnakeSegment) { 0, 0 };
	}

	return p_snake;
}

void snake_destroy(struct Snake *p_snake)
{
	free(p_snake);
}

void snake_move(struct Snake *p_snake, uint8_t bounds_width, uint8_t bounds_height)
{
	for (size_t segment_num = p_snake->length - 1; segment_num > 0; segment_num--) {
		struct SnakeSegment *p_segment = &p_snake->segments[segment_num];
		struct SnakeSegment *p_segment_next = &p_snake->segments[segment_num - 1];

		p_segment->x = p_segment_next->x;
		p_segment->y = p_segment_next->y;
	}

	snake_move_head(p_snake, bounds_width, bounds_height);

	if (snake_is_head_overlapping_tail(p_snake)) {
		p_snake->alive = false;
	}
}

void snake_turn_if_possible(struct Snake *p_snake, enum SnakeDirection direction)
{
	/* UP % 2 == DOWN % 2
	 *   and
	 * LEFT % 2 == RIGHT % 2
	 *
	 * snake can only turn perpendicular to current direction
	 */
	bool can_turn = (direction % 2 != p_snake->direction % 2);

	if (can_turn) {
		p_snake->direction = direction;
	}
}

void snake_eat_if_touching_food(struct Snake *p_snake, struct SnakeFood *p_food, uint8_t bounds_width, uint8_t bounds_height)
{
	struct SnakeSegment head = p_snake->segments[0];

	bool touching_food = head.x == p_food->x && head.y == p_food->y;
	if (touching_food) {
		snake_expand(p_snake);
		if (p_snake->length < p_snake->max_length) {
			snake_food_move(p_food, p_snake, bounds_width, bounds_height);
		}
	}
}

void snake_food_move(struct SnakeFood *p_food, struct Snake *p_snake, uint8_t bounds_width, uint8_t bounds_height)
{
	while (snake_is_touching_food(p_snake, p_food)) {
		p_food->x = rand() % bounds_width;
		p_food->y = rand() % bounds_height;
	}
}

static void snake_move_head(struct Snake *p_snake, uint8_t bounds_width, uint8_t bounds_height)
{
	struct SnakeSegment *p_head = &p_snake->segments[0];

	switch (p_snake->direction) {
	case SNAKE_DIRECTION_UP:
		if (p_head->y > 0) {
			--p_head->y;
		} else {
			p_snake->alive = false;
		}
	break;
	case SNAKE_DIRECTION_DOWN:
		if (p_head->y < bounds_width - 1) {
			++p_head->y;
		} else {
			p_snake->alive = false;
		}

	break;
	case SNAKE_DIRECTION_LEFT:
		if (p_head->x > 0) {
			--p_head->x;
		} else {
			p_snake->alive = false;
		}

	break;
	case SNAKE_DIRECTION_RIGHT:
		if (p_head->x < bounds_height - 1) {
			++p_head->x;
		} else {
			p_snake->alive = false;
		}

	break;
	}
}

static bool snake_is_head_overlapping_tail(struct Snake *p_snake)
{
	struct SnakeSegment head = p_snake->segments[0];
	for (size_t segment_num = 1; segment_num < p_snake->length; segment_num++) {
		struct SnakeSegment segment = p_snake->segments[segment_num];
		if (head.x == segment.x && head.y == segment.y) {
			return true;
		}
	}

	return false;
}

static void snake_expand(struct Snake *p_snake)
{
	if (p_snake->length < p_snake->max_length) {
		++p_snake->length;
		p_snake->segments[p_snake->length - 1] = p_snake->segments[p_snake->length - 2];
	}
}

static bool snake_is_touching_food(struct Snake *p_snake, struct SnakeFood *p_food)
{
	for (size_t segment_num = 0; segment_num < p_snake->length; segment_num++) {
		struct SnakeSegment segment = p_snake->segments[segment_num];
		if (segment.x == p_food->x && segment.y == p_food->y) {
			return true;
		}
	}

	return false;
}

void snake_seed_rng()
{
	srand(time(NULL));
}

