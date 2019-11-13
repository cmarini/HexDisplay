
#include "display_snake.h"


bool collisionEnabled = true;
bool collision = false;
int collision_blinkCount = 0;
bool isWrapAround = false;
bool appleEnabled = true;

#define SNAKE_MAX_LENGTH 20
Pixel* snake[SNAKE_MAX_LENGTH];
int snakeLength = 0;
dir_t snakeHeading = DIR_FIRST;
bool snakeGrow = false;
int yetToTravel = 0;

Pixel* apple = NULL;


rgba_t bgColor = {0,0,0};
rgba_t snakeColor = {0,0,B_VAL_LIMIT};
rgba_t snakeColor_collision1 = {0,0,B_VAL_LIMIT};
rgba_t snakeColor_collision2 = {0,0,B_VAL_LIMIT};
rgba_t appleColor1 = {R_VAL_LIMIT,0,0};
rgba_t appleColor2 = {R_VAL_LIMIT,G_VAL_LIMIT/4,0};


bool updateSnake();
void appleCallback_1(Pixel*);
void appleCallback_2(Pixel*);
void setupNewApple();

void deadSnakeCompleteCallback(Pixel* p)
{
    p->unsetCallback();
    snakeLoop(STATE_INIT);
}

void deadSnakeCallback(Pixel* p)
{
    if (collision_blinkCount > 0) {
        if (isEven(collision_blinkCount)) {
            for (int i = 0; i < snakeLength; i++) {
                snake[i]->
                    unsetCallback()->
                    setDuration(250)->
                    setHold(0)->
                    setTransition(&snakeColor_collision1, millis());
            }
        }
        else {
            for (int i = 0; i < snakeLength; i++) { 
                snake[i]->
                    unsetCallback()->
                    setDuration(250)->
                    setHold(0)->
                    setTransition(&snakeColor_collision2, millis());
            }
        }
        snake[0]->setCallback(deadSnakeCallback);
        collision_blinkCount--;
    }
    else {
        int i;
        for (i = 0; i < snakeLength; i++) {
            snake[i]->
                unsetCallback()->
                setDuration(250)->
                setHold(0)->
                setTransition(&bgColor, millis() + i*150);
        }
        snake[i]->
            setHold(500)->
            setCallback(deadSnakeCompleteCallback);
    }
}

void snakeLoop(funcState_t state)
{
    static hsva_t hsvA = {0,S_VAL_LIMIT,V_VAL_LIMIT};
    rgba_t white = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT};
    uint32_t now = millis();
    static uint32_t now_prev;
    switch(state)   {
        case STATE_INIT:
            /* Clear all pixels */
            for (int i = 0; i < PIXEL_COUNT; i++) {
                pixels[i].
                    setDuration(500)->
                    setHold(0)->
                    setTransition(&bgColor, millis());
            }
            snake[0] = getRandomPixel();
            snakeLength = 1;
            snakeHeading = dir_random();
            if (appleEnabled) {
                setupNewApple();
            }
        break;
        case STATE_RUN:
            if (collision) {
                /* Process collision */
                deadSnakeCallback(snake[0]);
            } else {
                if (snake[0]->isTransitionComplete()) {
                    now_prev = now;
                    if (!updateSnake()) {
                        collision = true;
                        collision_blinkCount = 6;
                    }
                    if (!appleEnabled && snakeLength < 1) {
                        snakeGrow = true;
                    }
                }
            }
        break;
        case STATE_DEINIT:
            for (int i = 0; i < PIXEL_COUNT; i++) {
                pixels[i].unsetCallback();
            }
        break;
    }
}


void shiftSnake(Pixel* newHead) {
    if (snakeGrow) {
        snakeLength++;
        snakeGrow = false;
    } else {
        snake[snakeLength-1]->
            setDuration(500)->
            setHold(0)->
            setTransition(&bgColor, millis());
    }
    for (int i = snakeLength-1; i > 0; i--) {
        snake[i] = snake[i-1];
    }
    snake[0] = newHead;
    newHead->
        setDuration(500)->
        setHold(0)->
        setTransition(&snakeColor, millis());
}



bool updateSnake()
{
    
    if (!isWrapAround && !snake[0]->hasAdjacent(snakeHeading)) {
        yetToTravel = 0;
    }
    if (yetToTravel <= 0) {
        yetToTravel = randomRange(1, HEX_SIDE_LENGTH);
        // Pick a new direction
        dir_t from = dir_opposite(snakeHeading);
        while (true) {
            snakeHeading = dir_random();
            if (snakeHeading == from) {
                continue;
            }
            /*
            if (isWrapAround) {
                break;
            }
            */
            if (snake[0]->hasAdjacent(snakeHeading)) {
                break;
            }
        }
    }
    
    Pixel* nextHead = snake[0]->getAdjacent(snakeHeading);
    if (collisionEnabled) {
        for (int i = 1; i < snakeLength; i++) {
            if (nextHead == snake[i]) {
                /* COLLISION! */
                yetToTravel = 0;
                return false;
            }
        }
    }
    if (nextHead == apple) {
        apple->unsetCallback();
    }
    shiftSnake(nextHead);
    yetToTravel--;
    if (nextHead == apple) {
        snakeGrow = true;
        setupNewApple();
    }
    return true;
}


void setupNewApple()
{
    Pixel* prevApple = apple;
    bool unique;
    do {
        unique = true;
        apple = getRandomPixel();
        if (apple == prevApple) {
            unique = false;
            continue;
        }
        for (int i = 0; i < snakeLength; i++) {
            if (snake[i] == apple) {
                unique = false;
                break;
            }
        }
    } while (!unique);
    if (prevApple == NULL) {
        apple = getPixel(0,0);
    }
//    apple = getPixel(0,0);
    apple->setCallback(appleCallback_1)->doCallback();
}

void appleCallback_1(Pixel* p)
{
    p->
        setCallback(appleCallback_2)->
        setDuration(1000)->
        setHold(0)->
        setTransition(&appleColor1, millis());
}
void appleCallback_2(Pixel* p)
{
    p->
        setCallback(appleCallback_1)->
        setDuration(500)->
        setHold(500)->
        setTransition(&appleColor2, millis());
}
