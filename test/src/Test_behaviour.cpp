#include "Behaviour.hpp"
#include "Component.hpp"
#include "Spawner.hpp"
#include <gtest/gtest.h>
struct Position;

TEST(BehaviourTest, TestMoveLeft)
{
// Create he initial state
Position pos = {0.0, 0.0};
float deltaTime = 1.0;

// Call the function to test
moveLeft(pos, deltaTime);

// Check the expected outcome
EXPECT_FLOAT_EQ(pos.x, -100.0);
EXPECT_FLOAT_EQ(pos.y, 0.0);
}

// Add more TEST() for other functions and cases
TEST(BehaviourTest, TestZigzagMovement)
{
// Create he initial state
Position pos = {0.0, 0.0};
float deltaTime = 1.0;

// Call the function to test
zigzagMovement(pos, deltaTime);

// Check the expected outcome
EXPECT_FLOAT_EQ(pos.x, -100.0);
EXPECT_FLOAT_EQ(pos.y, 1.0);
}

TEST(BehaviourTest, TestSliderLoopFixMovement)
{
// Create he initial state
Position pos = {0.0, 0.0};
float deltaTime = 1.0;

// Call the function to test
sliderLoopFixMovement(pos, deltaTime);

// Check the expected outcome
EXPECT_FLOAT_EQ(pos.x, 400.0);
EXPECT_FLOAT_EQ(pos.y, 1.0);
}

TEST(BehaviourTest, TestCircularMovement)
{
// Create he initial state
Position pos = {0.0, 0.0};
float deltaTime = 1.0;

// Call the function to test
circularMovement(pos, deltaTime);

// Check the expected outcome
EXPECT_FLOAT_EQ(pos.x, 400.0);
EXPECT_FLOAT_EQ(pos.y, 300.0);
}

TEST(BehaviourTest, TestDiagonalMovement)
{
// Create he initial state
Position pos = {0.0, 0.0};
float deltaTime = 1.0;

// Call the function to test
diagonalMovement(pos, deltaTime);

// Check the expected outcome
EXPECT_FLOAT_EQ(pos.x, -100.0);
EXPECT_FLOAT_EQ(pos.y, 100.0);
}

TEST(BehaviourTest, TestSineWaveMovement)
{
// Create he initial state
Position pos = {0.0, 0.0};
float deltaTime = 1.0;

// Call the function to test
sineWaveMovement(pos, deltaTime);

// Check the expected outcome
EXPECT_FLOAT_EQ(pos.x, -100.0);
EXPECT_FLOAT_EQ(pos.y, 300.0);
}