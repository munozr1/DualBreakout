/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <vector>

enum BallBlockColor {
    white,
    black
};
struct Block {
    SDL_FRect rect;
    BallBlockColor color;
};
struct Ball {
    BallBlockColor color;
    float x;
    float y;
    float radius;
    float speed;
    float angle;
};


static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static int screen_width = 780;
static int screen_height = 600;

/* BLOCKS  */
std::vector<Block> blocks;
Ball white_ball;
Ball black_ball;

void HandleBallHitWall(Ball& ball, int screen_w, int screen_h);
void DrawCircle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color);

bool DidHitBlock(float cx, float cy, float radius, const SDL_FRect& rect);
void UpdateBallPosition(Ball& ball);
    
void RenderBlocks(SDL_Renderer * renderer, std::vector<Block>& blocks, Ball& ball){
    for (auto& block : blocks){
        if(ball.color == block.color && DidHitBlock(ball.x, ball.y, ball.radius, block.rect)){
            block.color = ball.color == white ? black : white;
            float jitter = ((rand() % 21) - 10) * (M_PI / 180.0); // -10° to +10°
            ball.angle = -ball.angle + jitter;
        }

        
        if (block.color == white) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        if (block.color == black) SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &block.rect);
    }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", screen_width, screen_height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    /* Create blocks */
    const float block_width     = 30.0f;
    const float block_height    = 20.0f;
    const int blocks_per_row    = screen_width / block_width;
    const int blocks_per_col    = screen_height / block_height; // only fill half the screen
    const int number_of_blocks  = blocks_per_col * blocks_per_row;
    blocks = std::vector<Block>(number_of_blocks);
    for (int i = 0; i < number_of_blocks; ++i) {
        int col = i % blocks_per_row;
        int row = i / blocks_per_row;
        Block& block = blocks[i];
        if (row < blocks_per_col / 2) {
            block.color = white;
        }else{
            block.color = black;
        }

        block.rect = SDL_FRect{
            col * block_width,
            row * block_height,
            block_width,
            block_height
        };
    }
    
    white_ball = {
        white,
        500,  //y
        390,  //x
        15,   //radius
        80.0f, //speed
        17.4f  //angle
    };
    black_ball = {
        black,
        500,  //y
        110,  //x
        15,   //radius
        80.0f, //speed
        -5.8f  //angle
    };
    
    
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    if (event->type == SDL_EVENT_KEY_DOWN) SDL_Log("Key pressed: %d", event->key.key);
    return SDL_APP_CONTINUE;
}



/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    
    /* Check for collisions */
    HandleBallHitWall(white_ball, screen_width, screen_height);
    HandleBallHitWall(black_ball, screen_width, screen_height);


    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
   

    /* Render active blocks */
    RenderBlocks(renderer, blocks, white_ball);
    RenderBlocks(renderer, blocks, black_ball);

    /* Update ball position and direction */
    UpdateBallPosition(white_ball);
    UpdateBallPosition(black_ball);
     
    /* Draw Circle */
    DrawCircle(renderer, white_ball.x, white_ball.y, white_ball.radius, {255,255,255,255});
    DrawCircle(renderer, black_ball.x, black_ball.y, black_ball.radius, {0,0,0,255});

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}



/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_Log("Quitting Game");
}


void DrawCircle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void ReflectIfWallCollision(Ball& ball, float ball_radius) {
    /* Left or right wall */
    if (ball.x - ball_radius < 0 || ball.x + ball_radius > screen_width) {
        ball.angle = M_PI - ball.angle;
    }

    /* Top or Bottom wall */
    if (ball.y - ball_radius < 0 || ball.y + ball_radius > screen_height) {
        ball.angle = -ball.angle;
    }
}

bool DidHitBlock(float cx, float cy, float radius, const SDL_FRect& rect) {
    // Find the closest point on the rectangle to the circle
    float closestX = std::max(rect.x, std::min(cx, rect.x + rect.w));
    float closestY = std::max(rect.y, std::min(cy, rect.y + rect.h));

    // Calculate distance between circle center and this closest point
    float dx = cx - closestX;
    float dy = cy - closestY;

    // If the distance is less than the radius, they collide
    return (dx * dx + dy * dy) < (radius * radius);
}

void HandleBallHitWall(Ball& ball, int screen_w, int screen_h) {
    /* Left or Right walls */
    if (ball.x - ball.radius <= 0) {
        ball.x = ball.radius; // prevent sticking to wall
        ball.angle = M_PI - ball.angle;
    }
    else if (ball.x + ball.radius >= screen_w) {
        ball.x = screen_w - ball.radius;
        ball.angle = M_PI - ball.angle;
    }
    /* Top or Bottom walls */
    if (ball.y - ball.radius <= 0) {
        ball.y = ball.radius;
        ball.angle = -ball.angle;
    }
    else if (ball.y + ball.radius >= screen_h) {
        ball.y = screen_h - ball.radius;
        ball.angle = -ball.angle;
    }
}

void UpdateBallPosition(Ball& ball){
    const float dt = 1.0 / 60.0;
    float dx = ball.speed * cos(ball.angle);
    float dy = ball.speed * sin(ball.angle);
    ball.x += dx * dt;
    ball.y += dy * dt;
}
