/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** ClientMain
*/

#ifndef CLIENTMAIN_HPP_
#define CLIENTMAIN_HPP_
#include "Component.hpp"
#include "SparseArray.hpp"
#include "GUI/GUI.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>
#include <thread>
#include <iostream>



char getch();
void initializeGameEntitiesClient(Registry& registry);

#endif /* !CLIENTMAIN_HPP_ */