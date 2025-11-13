#pragma once

#include <cstdint>

struct Node
{
    void* start;
    uint64_t size;

    Node* left;
    Node* right;
    Node* parent;
};
