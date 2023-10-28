/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** REGISTRY
*/
#ifndef BEHAVIOUR_HPP_
#define BEHAVIOUR_HPP_
#include "Component.hpp"
#include <functional>
//need to move to behavior.cpp
using BehaviorFunc = std::function<void(Position&, float)>;

void moveLeft(Position& pos, float deltaTime);
void zigzagMovement(Position& pos, float deltaTime);
void sliderLoopFixMovement(Position& pos, float deltaTime);
void circularMovement(Position& pos, float deltaTime);
void diagonalMovement(Position& pos, float deltaTime);
void sineWaveMovement(Position& pos, float deltaTime);
void backAndForthMovement(Position& pos, float deltaTime);
void lShapeMovement(Position& pos, float deltaTime);
void randomMovement(Position& pos, float deltaTime);
void fixedMovement(Position& pos, float deltaTime);
void loopDiagonalMovementRight(Position& pos, float deltaTime);
#endif /* !BEHAVIOUR_HPP_ */