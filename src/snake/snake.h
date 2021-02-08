/* Dependencies
 * <stdint.h>
 * <stdbool.h>
 */

#ifndef snake_h
#define snake_h

enum SnakeDirection {
	SNAKE_DIRECTION_UP,
	SNAKE_DIRECTION_LEFT,
	SNAKE_DIRECTION_DOWN,
	SNAKE_DIRECTION_RIGHT
};

struct SnakeSegment {
	uint8_t x;
	uint8_t y;
};

struct Snake {
	size_t length;
	size_t max_length;

	bool alive;
	enum SnakeDirection direction;
	struct SnakeSegment segments[];
};

struct SnakeFood {
	uint8_t x;
	uint8_t y;
};

struct Snake *snake_create(size_t max_length, size_t init_length);
void          snake_destroy(struct Snake *p_snake);

void snake_move(struct Snake *p_snake, uint8_t bounds_width, uint8_t bounds_height);
void snake_turn_if_possible(struct Snake *p_snake, enum SnakeDirection direction);
void snake_eat_if_touching_food(struct Snake *p_snake, struct SnakeFood *p_food,
		uint8_t bounds_width, uint8_t bounds_height);
void snake_food_move(struct SnakeFood *p_food, struct Snake *p_snake, uint8_t bounds_width,
		uint8_t bounds_height);

void snake_seed_rng();

#endif // snake_h
