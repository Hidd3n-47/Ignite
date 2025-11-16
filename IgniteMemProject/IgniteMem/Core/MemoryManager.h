#pragma once

#include "Defines.h"
#include "Node.h"

#ifdef DEV_CONFIGURATION
#include <mutex>
#include <thread>
#include "DebugMemoryHexValues.h"
#endif // DEV_CONFIGURATION.


#ifdef MEM_MANAGER
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

namespace ignite::mem
{

class API MemoryManager
{
public:
    [[nodiscard]] inline static MemoryManager* Instance() { return mInstance; }

    MemoryManager(const MemoryManager&)            = delete;
    MemoryManager(MemoryManager&&)                 = delete;
    MemoryManager& operator=(MemoryManager&&)      = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    static void Init(const uint32_t sizeBytes);
    static void Destroy();

    template <typename T>
    T* New();

    template <typename T>
    void Delete(T* free);

#ifdef DEV_CONFIGURATION
    [[nodiscard]] inline Node*    GetSmallestBlockNode()  const { return mSmallestBlock; }
    [[nodiscard]] inline Node*    GetLargestBlockNode()   const { return mLargestBlock; }
    [[nodiscard]] inline Node*    GetRootNode()           const { return mRootNode; }
    [[nodiscard]] inline void*    GetStartOfMemoryBlock() const { return mMemoryBlock; }
    [[nodiscard]] inline uint64_t GetSize()               const { return mSize; }
    [[nodiscard]] inline uint64_t GetAllocated()          const { return mAllocated; }
    [[nodiscard]] inline uint64_t GetSizeFree()           const { return mSize - mAllocated; }

    static void SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size);
    inline static uint32_t GetMetadataPadding() { return METADATA_PADDING; }

private:
    std::thread mThread;
public:
    friend class DebugMemoryConsole;
#endif // DEV_CONFIGURATION.

private:
    MemoryManager(const uint64_t sizeBytes);
    ~MemoryManager();

    static MemoryManager* mInstance;

    void* mMemoryBlock;

    Node* mRootNode      = nullptr;
    Node* mSmallestBlock = nullptr;
    Node* mLargestBlock  = nullptr;

    uint64_t mSize;
    uint64_t mAllocated{};

    static constexpr uint32_t METADATA_PADDING{ 4 };

    void TryMergeFreeBlockLeft(Node* node);

    bool TryUpdateSmallestBlock(Node* potentialSmallBlock);
    bool TryUpdateLargestBlock(Node* potentialLargeBlock);
};

template <typename T>
T* MemoryManager::New()
{
    // todo assert if the small node has any children nodes.
    const uint64_t size = sizeof(T);
    if (size <= mSmallestBlock->size)
    {
        *reinterpret_cast<uint32_t*>(mSmallestBlock->start) = size;

        const uint64_t allocatedSize = size + METADATA_PADDING;
        mAllocated += allocatedSize;
        mSmallestBlock->start = mSmallestBlock->start + allocatedSize;
        mSmallestBlock->size -= allocatedSize;
        // todo assert if the start is before the parents start.

        void* typeAddress = mSmallestBlock->start - size;

        DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::NEWLY_ALLOCATED, typeAddress, size));

        return static_cast<T*>(typeAddress);
    }

    // todo not enough space, need to traverse up the tree.
    return nullptr;
}

template <typename T>
void MemoryManager::Delete(T* free)
{
    std::byte* freeAddress = (std::byte*)free;

    Node* node = mRootNode;

    //todo assert that free is never > start when node == root node.

    // Keep traversing down tree until ensuring that the address being free is less than the node address.
    while (freeAddress < node->start && (node->left || node->right))
    {
        const bool beforeLeft  = node->left  && freeAddress < node->left->start;
        const bool beforeRight = node->right && freeAddress < node->right->start;

        // If free address is less than the left and right child node, ensure that we pick the most left address and continue traversing down.
        if (beforeLeft && beforeRight)
        {
            node = node->left->start < node->right->start ? node->left : node->right;
            continue;
        }

        // If the free address is only less than the left address, choose that one and continue traversing down.
        if (beforeLeft)
        {
            node = node->left;
            continue;
        } 
        
        // If the free address is only less than the right address, choose that one and continue traversing down.
        if (beforeRight)
        {
            node = node->right;
            continue;
        }

        // If we have reached this point, then the child nodes either don't exist, or addresses are before the freed address, therefore stop traversing.
        break;
    }

    std::byte*      baseAddress = freeAddress - METADATA_PADDING;
    const uint32_t  size        = *reinterpret_cast<uint32_t*>(baseAddress) + METADATA_PADDING;

    DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::FREED, (void*)(free), size - METADATA_PADDING));
    mAllocated -= size;

    // Since out baseAddress + size is at the start of the node, this means we can merge our new size into that free block.
    if (baseAddress + size == node->start)
    {
        node->size += size;
        node->start = baseAddress;

        // Since this free block has increased size going leftwards, we need to check if we can merge to the block on the left.
        TryMergeFreeBlockLeft(node);

        return;
    }

    //todo assert if baseAddress + size > node->start;

    //MergeFreeBlocksLeft(node, baseAddress, size);
    for (Node* n : { node->left, node->right })
    {
        if (n && n->start + size == baseAddress)
        {
            n->size += size;

            if (n->parent && n->size >= n->parent->size)
            {
                n->parent->right = n;
                n->parent->left  = nullptr;
            }

            return;
        }
    }

    // If we get to this point, we are freeing an address not adjacent to other free block, therefore create a new free block.
    Node* newNode = new Node{ .start = baseAddress, .size = size, .left = nullptr, .right = nullptr, .parent = node };

    // If the new block is larger than the previous free block, the new node will have to be the right child.
    if (newNode->size > node->size)
    {
        //todo 
        if (node->right) __debugbreak();

        node->right = newNode;

        // Since this new node is on the right and right is direction of larger size, check if it's the new largest block.
        TryUpdateLargestBlock(newNode);
    }
    else
    {
        // New child size is less than or equal to the parent, therefore the new child will be the left child.

        //todo assert if node->left->start < newNode->start <- this should be impossible due to how we traverse the tree, if this address was < then it would have been chosen as the node.

        // If the parent already has a left node, we can check if the left child size is larger than the new node, if so, the current left child will be a right node.
        if (node->left)
        {
            if (node->left->size > newNode->size)
            {
                newNode->right = node->left;
            }
            else
            {
                newNode->left = node->left;
            }
        }

        node->left = newNode;

        // Since this new node is on the left and left is direction of smaller size, check if it's the new smallest block.
        TryUpdateSmallestBlock(newNode);
    }
}

} // Namespace ignite::mem;