#pragma once

namespace ignite::mem
{

template <typename T>
struct ListNode
{
    T         value;
    ListNode* next;
};

} // Namespace ignite::mem.