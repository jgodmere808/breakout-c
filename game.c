
#include "game.h"

#define GAME_WIDTH  780
#define GAME_HEIGHT 430

#define PLATFORM_WIDTH  120
#define PLATFORM_HEIGHT 10
#define PLATFORM_SPEED  8

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

static bool gameStarted = false;
static bool gameOver = false;

void initGame()
{
    platform = (Platform){
        { (GAME_WIDTH - PLATFORM_WIDTH) / 2, GAME_HEIGHT - 2 * PLATFORM_HEIGHT },
        PLATFORM_WIDTH,
        PLATFORM_HEIGHT
    };
    ball = (Ball){
        { (GAME_WIDTH - BALL_WIDTH) / 2, GAME_HEIGHT - 2 * PLATFORM_HEIGHT - BALL_HEIGHT },
        { 4, -4 },
        BALL_WIDTH,
        BALL_HEIGHT
    };
    game = (Game){ &ball, &platform, GAME_WIDTH, GAME_HEIGHT };
}

void updateGame()
{
    if (gameOver) {
        return;
    }

    if (!gameStarted) {
        if (IsKeyDown(KEY_SPACE)) gameStarted = true;
        else return;
    }

    // platform controls
    if (IsKeyDown(KEY_LEFT))  platform.pos.x -= PLATFORM_SPEED;
    if (IsKeyDown(KEY_RIGHT)) platform.pos.x += PLATFORM_SPEED;

    // ball position update
    ball.pos.x += ball.vel.dx;
    ball.pos.y += ball.vel.dy;

    // ball platform collision
    if (ball.vel.dy > 0) { // ball is moving downward
        if (
            ball.pos.y + ball.height <= platform.pos.y &&
            ball.pos.y + ball.height + ball.vel.dy >= platform.pos.y &&
            ball.pos.x + ball.width >= platform.pos.x &&
            ball.pos.x <= platform.pos.x + platform.width
        ) {
            ball.vel.dy = -ball.vel.dy;
            ball.pos.y = platform.pos.y - ball.height;
        }
    }

    // platform wall collision
    if (platform.pos.x < 0) {
        platform.pos.x = 0;
    }
    if (platform.pos.x + platform.width > game.width) {
        platform.pos.x = game.width - platform.width;
    }

    // ball wall collision
    if (ball.pos.x + ball.vel.dx < 0) {
        ball.vel.dx = -ball.vel.dx;
        ball.pos.x = 0;
    }
    if (ball.pos.x + ball.vel.dx + ball.width > game.width) {
        ball.vel.dx = -ball.vel.dx;
        ball.pos.x = game.width - ball.width;
    }
    if (ball.pos.y + ball.vel.dy < 0) {
        ball.vel.dy = -ball.vel.dy;
        ball.pos.y = 0;
    }
    
    // game over logic
    if (ball.pos.y + ball.height > game.height) {
        gameOver = true;
    }
}

void drawGame(int posX, int posY)
{
    if (!gameStarted) {
        DrawText(
            "Press SPACE to start",
            posX + (game.width - 175) / 2,
            posY + game.height / 2,
            16,
            WHITE
        );
    }

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

    if (gameOver) {
        DrawText(
            "GAME OVER",
            posX + (game.width - 100) / 2,
            posY + game.height / 2,
            16,
            WHITE
        );
    }
}