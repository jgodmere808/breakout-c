
#include "game.h"

#define GAME_WIDTH  780
#define GAME_HEIGHT 430

#define PLATFORM_WIDTH  120
#define PLATFORM_HEIGHT 10
#define PLATFORM_SPEED  8

#define BALL_WIDTH  10
#define BALL_HEIGHT 10

#define BLOCK_WIDTH  40
#define BLOCK_HEIGHT 9

#define BLOCKS_COLS 16
#define BLOCKS_ROWS 8

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

typedef struct _block {
    Position pos;
    int active;
    int width;
    int height;
} Block;

typedef struct _game {
    Ball *ball;
    Platform *platform;
    Block blocks[BLOCKS_ROWS][BLOCKS_COLS];
    int width;
    int height;
} Game;

typedef enum {
    HIT_NONE,
    HIT_SIDE,
    HIT_VERTICAL,
    HIT_BLOCK_SIDE,
    HIT_BLOCK_VERTICAL,
    HIT_FLOOR
} CollisionType;

static Platform platform;
static Ball ball;
static Game game;

static bool gameStarted = false;
static bool gameOver = false;

void initGame()
{
    int row, col;

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
    game = (Game){
        .ball = &ball,
        .platform = &platform,
        .width = GAME_WIDTH,
        .height = GAME_HEIGHT
    };

    for (row = 0; row < BLOCKS_ROWS; row++) {
        for (col = 0; col < BLOCKS_COLS; col++) {
            game.blocks[row][col] = (Block){
                .pos = {32.5 + col * 45, 40 + row * 12 },
                .width = BLOCK_WIDTH,
                .height = BLOCK_HEIGHT,
                .active = 1
            };
        }
    }
}

void updateGame()
{
    int row, col, hitRow = -1, hitCol = -1;
    float oldBallX, oldBallY, nextBallX, nextBallY, hitTime = 2.0f;
    float time, impactX;
    CollisionType hitType = HIT_NONE;

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

    // platform wall collision
    if (platform.pos.x < 0) {
        platform.pos.x = 0;
    }
    if (platform.pos.x + platform.width > game.width) {
        platform.pos.x = game.width - platform.width;
    }

    oldBallX = ball.pos.x;
    oldBallY = ball.pos.y;
    nextBallX = oldBallX + ball.vel.dx;
    nextBallY = oldBallY + ball.vel.dy;

    // Find the first surface the ball reaches during this frame.
    if (ball.vel.dx < 0 && nextBallX <= 0) {
        hitTime = -oldBallX / ball.vel.dx;
        hitType = HIT_SIDE;
    }
    if (ball.vel.dx > 0 && nextBallX + ball.width >= game.width) {
        hitTime = (game.width - ball.width - oldBallX) / ball.vel.dx;
        hitType = HIT_SIDE;
    }
    if (ball.vel.dy < 0 && nextBallY <= 0) {
        time = -oldBallY / ball.vel.dy;
        if (time < hitTime) {
            hitTime = time;
            hitType = HIT_VERTICAL;
        }
    }

    // The paddle is a one-way surface: only its top bounces the ball.
    if (ball.vel.dy > 0 && oldBallY + ball.height <= platform.pos.y &&
        nextBallY + ball.height >= platform.pos.y) {
        time = (platform.pos.y - oldBallY - ball.height) / ball.vel.dy;
        impactX = oldBallX + ball.vel.dx * time;
        if (impactX + ball.width > platform.pos.x &&
            impactX < platform.pos.x + platform.width && time < hitTime) {
            hitTime = time;
            hitType = HIT_VERTICAL;
        }
    }

    if (ball.vel.dy > 0 && nextBallY + ball.height >= game.height) {
        time = (game.height - ball.height - oldBallY) / ball.vel.dy;
        if (time < hitTime) {
            hitTime = time;
            hitType = HIT_FLOOR;
        }
    }

    // ball block collision
    for (row = 0; row < BLOCKS_ROWS; row++) {
        for (col = 0; col < BLOCKS_COLS; col++) {
            if (!game.blocks[row][col].active) continue;

            Block *block = &game.blocks[row][col];
            float left = block->pos.x;
            float right = left + block->width;
            float top = block->pos.y;
            float bottom = top + block->height;
            float impactY;

            // bottom collision
            if (ball.vel.dy < 0 && oldBallY >= bottom && nextBallY <= bottom) {
                time = (bottom - oldBallY) / ball.vel.dy;
                impactX = oldBallX + ball.vel.dx * time;
                if (impactX + ball.width > left && impactX < right && time < hitTime) {
                    hitTime = time;
                    hitRow = row;
                    hitCol = col;
                    hitType = HIT_BLOCK_VERTICAL;
                }
            }

            // top collision
            if (ball.vel.dy > 0 && oldBallY + ball.height <= top &&
                nextBallY + ball.height >= top) {
                time = (top - oldBallY - ball.height) / ball.vel.dy;
                impactX = oldBallX + ball.vel.dx * time;
                if (impactX + ball.width > left && impactX < right && time < hitTime) {
                    hitTime = time;
                    hitRow = row;
                    hitCol = col;
                    hitType = HIT_BLOCK_VERTICAL;
                }
            }

            // left collision
            if (ball.vel.dx > 0 && oldBallX + ball.width <= left &&
                nextBallX + ball.width >= left) {
                time = (left - oldBallX - ball.width) / ball.vel.dx;
                impactY = oldBallY + ball.vel.dy * time;
                if (impactY + ball.height > top && impactY < bottom && time < hitTime) {
                    hitTime = time;
                    hitRow = row;
                    hitCol = col;
                    hitType = HIT_BLOCK_SIDE;
                }
            }

            // right collision
            if (ball.vel.dx < 0 && oldBallX >= right && nextBallX <= right) {
                time = (right - oldBallX) / ball.vel.dx;
                impactY = oldBallY + ball.vel.dy * time;
                if (impactY + ball.height > top && impactY < bottom && time < hitTime) {
                    hitTime = time;
                    hitRow = row;
                    hitCol = col;
                    hitType = HIT_BLOCK_SIDE;
                }
            }
        }
    }

    if (hitType == HIT_NONE) {
        ball.pos.x = nextBallX;
        ball.pos.y = nextBallY;
    } else {
        ball.pos.x = oldBallX + ball.vel.dx * hitTime;
        ball.pos.y = oldBallY + ball.vel.dy * hitTime;
        if (hitType == HIT_SIDE) {
            ball.pos.x = ball.vel.dx < 0 ? 0 : game.width - ball.width;
            ball.vel.dx = -ball.vel.dx;
        } else if (hitType == HIT_BLOCK_SIDE) {
            Block *block = &game.blocks[hitRow][hitCol];
            ball.pos.x = ball.vel.dx > 0 ? block->pos.x - ball.width :
                block->pos.x + block->width;
            ball.vel.dx = -ball.vel.dx;
        } else if (hitType == HIT_BLOCK_VERTICAL) {
            Block *block = &game.blocks[hitRow][hitCol];
            ball.pos.y = ball.vel.dy > 0 ? block->pos.y - ball.height :
                block->pos.y + block->height;
            ball.vel.dy = -ball.vel.dy;
        } else if (hitType == HIT_FLOOR) {
            ball.pos.y = game.height - ball.height;
            gameOver = true;
        } else {
            ball.pos.y = ball.vel.dy < 0 ? 0 : platform.pos.y - ball.height;
            ball.vel.dy = -ball.vel.dy;
        }
    }

    if (hitType == HIT_BLOCK_SIDE || hitType == HIT_BLOCK_VERTICAL) {
        game.blocks[hitRow][hitCol].active = 0;
    }
}

void drawGame(int posX, int posY)
{
    int row, col;

    if (!gameStarted) {
        DrawText(
            "Press SPACE to start",
            posX + (game.width - 175) / 2,
            posY + game.height / 2,
            16,
            WHITE
        );
    }

    // Blocks
    for (row = 0; row < BLOCKS_ROWS; row++) {
        for (col = 0; col < BLOCKS_COLS; col++) {
            if (!game.blocks[row][col].active) continue;

            DrawRectangle(
                posX + game.blocks[row][col].pos.x,
                posY + game.blocks[row][col].pos.y,
                game.blocks[row][col].width,
                game.blocks[row][col].height,
                RED
            );
        }
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
