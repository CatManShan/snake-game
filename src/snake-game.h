#ifndef snake_game_h
#define snake_game_h

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

#undef // snake_game_h

