/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** ENTITY
*/
#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <cstddef> // for size_t
using entity_t = std::size_t;
class Entity {
public:
    explicit Entity(std::size_t id) : id_(id) {}

    // Implicit conversion to size_t
    operator std::size_t() const {
        return id_;
    }

private:
    std::size_t id_;
};

#endif /* !ENTITY_HPP_ */
