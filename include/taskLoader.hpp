/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** taskLoader
*/

#ifndef TASKLOADER_HPP_
#define TASKLOADER_HPP_
#include "Component.hpp"
#include "SparseArray.hpp"
#include "GUI/GUI.hpp"


void executeLoadingTasks(GUI& gui, const std::vector<std::function<void(int&)>>& tasks);

#endif /* !TASKLOADER_HPP_*/