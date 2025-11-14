#pragma once

#include <cstddef>
#include <cstdint>

struct Node
{
    std::byte* start;
    uint64_t   size;

    Node* left;
    Node* right;
    Node* parent;
};
