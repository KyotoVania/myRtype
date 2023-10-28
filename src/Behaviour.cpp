
#include "Behaviour.hpp"
#include <cmath>


void moveLeft(Position& pos, float deltaTime) {
    pos.x += -1.0f * (100.0f * deltaTime);
}

void zigzagMovement(Position& pos, float deltaTime) {
    static bool goingUp = true;
    float speed = 100.0f * deltaTime;

    if (goingUp) {
        pos.y += 1;
    } else {
        pos.y -= 1;
    }

    if (pos.y > 400 || pos.y < 200) {
        goingUp = !goingUp;
    }
    pos.x -= speed;
}

void sliderLoopFixMovement(Position& pos, float deltaTime) {
    static bool goingUp = true;
    float speed = 100.0f * deltaTime;

    if (goingUp) {
        pos.y += 1;
    } else {
        pos.y -= 1;
    }

    if (pos.y > 400 || pos.y < 200) {
        goingUp = !goingUp;
    }
    pos.x = 400 + (std::sin(pos.y / 100.0f) * 100.0f);
}

void circularMovement(Position& pos, float deltaTime) {
    float speed = 1.0f;
    static float angle = 0.0f;
    angle += speed * deltaTime;
    pos.x = 400 + std::cos(angle) * 100;  // 400 is the center, 100 is the radius
    pos.y = 300 + std::sin(angle) * 100;  // 300 is the center, 100 is the radius
}
void diagonalMovement(Position& pos, float deltaTime) {
    float speed = 100.0f * deltaTime;
    pos.x -= speed;
    pos.y += speed;
}
void sineWaveMovement(Position& pos, float deltaTime) {
    pos.y = 300 + std::sin(pos.x / 100.0f) * 100.0f; // 300 is the vertical midpoint, 100 is the amplitude
    pos.x -= 100.0f * deltaTime;
}
void backAndForthMovement(Position& pos, float deltaTime) {
    static bool goingRight = false;
    float speed = 100.0f * deltaTime;

    if (goingRight) {
        pos.x += speed;
    } else {
        pos.x -= speed;
    }
    if (pos.x > 600 || pos.x < 200) {
        goingRight = !goingRight;
    }
}
void lShapeMovement(Position& pos, float deltaTime) {
    static bool goingLeft = true;
    static bool goingDown = true;
    float speed = 100.0f * deltaTime;

    if (goingLeft) {
        pos.x -= speed * 2;
        if (pos.x > 600) {
            goingLeft = false;
            goingDown = true;
        }
    }

    if (goingDown) {
        pos.y += speed;
        if (pos.y > 400) {
            goingDown = false;
            goingLeft = true;
        }
    }
}

void randomMovement(Position& pos, float deltaTime) {
    static bool goingRight = true;
    static bool goingDown = true;
    float speed = 100.0f * deltaTime;

    if (goingRight) {
        pos.x += speed;
        if (pos.x > 600) {
            goingRight = false;
            goingDown = true;
        }
    } else {
        pos.x -= speed;
        if (pos.x < 200) {
            goingRight = true;
            goingDown = false;
        }
    }

    if (goingDown) {
        pos.y += speed;
        if (pos.y > 400) {
            goingDown = false;
            goingRight = true;
        }
    } else {
        pos.y -= speed;
        if (pos.y < 200) {
            goingDown = true;
            goingRight = false;
        }
    }
}

void fixedMovement(Position& pos, float deltaTime) {
    pos.x = 0;
    pos.y = 0;
}

void loopDiagonalMovementRight(Position& pos, float deltaTime) {

    float speed = 100.0f * deltaTime;
    pos.x += speed;
    pos.y += speed;
    if (pos.x > 1920) {
        pos.x = 0;
    }
    if (pos.y > 1080) {
        pos.y = 0;
    }
}

