/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** SYSTEM
*/
#ifndef ISYSTEM_HPP_
#define ISYSTEM_HPP_
#include <SFML/Graphics.hpp>
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <functional>
#include "Entity.hpp"
#include "SparseArray.hpp"
#include "Component.hpp"
#include <fftw3.h>




class ISystemBase {
public:
    virtual void PostUpdate() {}
    virtual ~ISystemBase() = default;
};

template<typename... Components>
class ISystem : public ISystemBase {
public:
    virtual void operator()(entity_t entity, sparse_array<Components>&... components) = 0;
    virtual ~ISystem() = default;
};
#endif /* !ISYSTEM_HPP_ */