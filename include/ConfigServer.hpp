/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** ConfigServer
*/
#ifndef CONFIGSERVER_HPP_
#define CONFIGSERVER_HPP_
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>


std::string readFromConfig(const std::string& key);
void loadConfig(const std::string& filePath);
#endif /* !CONFIGSERVER_HPP_ */
