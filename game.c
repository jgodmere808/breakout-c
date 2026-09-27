
#include "game.h"

#define GAME_WIDTH  780
#define GAME_HEIGHT 430

#define PLATFORM_WIDTH  120
#define PLATFORM_HEIGHT 10

#define BALL_WIDTH  10
#define BALL_HEIGHT 10

typedef struct _position {
    float x;
    float y;
} Position;

typedef struct _velocity {
    float dx;
    float dy;
} Velocity;

typedef struct _ball {
    Position pos;
    Velocity vel;
    int width;
    int height;
} Ball;

typedef struct _platform {
    Position pos;
    int width;
    int height;
} Platform;

typedef struct _game {
    Ball *ball;
    Platform *platform;
    int width;
    int height;
} Game;

static Platform platform;
static Ball ball;
static Game game;

void initGame()
{
    platform = (Platform){
        { (GAME_WIDTH - PLATFORM_WIDTH) / 2, GAME_HEIGHT - 2 * PLATFORM_HEIGHT },
        PLATFORM_WIDTH,
        PLATFORM_HEIGHT
    };
    ball = (Ball){
        { (GAME_WIDTH - BALL_WIDTH) / 2, GAME_HEIGHT - 2 * PLATFORM_HEIGHT - BALL_HEIGHT },
        { 0, 0 },
        BALL_WIDTH,
        BALL_HEIGHT
    };
    game = (Game){ &ball, &platform, GAME_WIDTH, GAME_HEIGHT };
}

void updateGame()
{

}

void drawGame(int posX, int posY)
{
    // Ball
    DrawRectangle(
        posX + ball.pos.x,
        posY + ball.pos.y,
        ball.width,
        ball.height,
        WHITE
    );

    // Platform
    DrawRectangle(
        posX + platform.pos.x,
        posY + platform.pos.y,
        platform.width,
        platform.height,
        WHITE
    );

    // Game Field
    DrawRectangleLines(posX, posY, game.width, game.height, WHITE);
}