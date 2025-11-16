#include "MemoryManager.h"

#include <string>

#ifdef DEV_CONFIGURATION
#include "DebugMemoryConsole.h"
#endif // DEV_CONFIGURATION.

namespace ignite::mem
{

MemoryManager* MemoryManager::mInstance = nullptr;


MemoryManager::MemoryManager(const uint64_t sizeBytes)
{
    mSize = sizeBytes;
    mMemoryBlock = malloc(sizeBytes);
    DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::UNALLOCATED, mMemoryBlock, mSize));

    //todo make this a part of the reserved memory
    mRootNode = new Node{ .start = static_cast<std::byte*>(mMemoryBlock), .size = sizeBytes, .left = nullptr, .right = nullptr, .parent = nullptr };
    mSmallestBlock = mRootNode;
    mLargestBlock  = mRootNode;

#ifdef DEV_CONFIGURATION
    mThread = std::thread(&DebugMemoryConsole::Init);
#endif // DEV_CONFIGURATION.
}

MemoryManager::~MemoryManager()
{
    free(mMemoryBlock);
}

void MemoryManager::Init(const uint32_t sizeBytes)
{
#ifdef DEV_CONFIGURATION
    if (mInstance)
    {
        // todo assert/error as its already been init.
    }
#endif // DEV_CONFIGURATION.

    mInstance = new MemoryManager(sizeBytes);
}

void MemoryManager::Destroy()
{
#ifdef DEV_CONFIGURATION
    // Since the debug console is constructed on a different thread, it's possible to try close before the instance has been constructed.
    // Force stall until it's been created to ensure resources freed.
    while (!DebugMemoryConsole::Instance()) { }
    DebugMemoryConsole::Instance()->StopRunning();

    if (mInstance->mThread.joinable())
    {
        mInstance->mThread.join();
    }
#endif // DEV_CONFIGURATION.

    delete mInstance;
}

#ifdef DEV_CONFIGURATION
void MemoryManager::SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size)
{
    memset(memory, static_cast<int>(value), size);
}
#endif // DEV_CONFIGURATION.

void MemoryManager::TryMergeFreeBlockLeft(Node* node)
{
    for (Node** n : { &node->left, &node->right })
    {
        if ((*n) && (*n)->start + (*n)->size == node->start)
        {
            // We can merge size the start and end points are equal.
            node->size += (*n)->size;
            node->start = (*n)->start;

            // If we were the smallest block, and we merged, we should update the smallest block to be the merged block.
            if (*n == mSmallestBlock)
            {
                mSmallestBlock = node;
            }

            // Since we added to the size of node, we need to ensure that this isn't the new largest block.
            TryUpdateLargestBlock(node);

            // Since we have increased the size of the block, the right node which was larger, might not be larger anymore.
            if ((*n)->right && (*n)->right->size < node->size)
            {
                node->left = (*n)->right;

                //todo assert here as we shouldn't have a left node
                if ((*n)) __debugbreak();
            }
            // If it's not larger, we can put this as the first left node, and attach the existing left (if any) to the left of this.
            else
            {
                // This cache is needed to prevent a read access violation (??).
                const Node* cache = *n;

                // We would only get to this point if size(n->right) > size(node), which means the node can stay to the right.
                node->right = (*n)->right;

                // Attach the left node of n to our new merged block. We can safely attach it to the left since we know that 
                // size(n->left) < size(n) => and since size(node) = size(n) + size(node) > size(n), we can guarantee size(n->left) < size(node).
                node->left = cache->left;
            }

            if (node->parent && node->parent->size < node->size && node->parent->left == node)
            {
                // Since we merged, we now have a larger size than the parent. Since node was the left node, we need to move to the right node.
                if (node->parent->right)
                {
                    //todo assert.
                    // if we were the left node, the right node should be unoccupied.
                    __debugbreak();
                }
                else
                {
                    node->parent->left  = nullptr;
                    node->parent->right = node;
                }
            }

            // Finally, we can free the memory used for that node as it was merged.
            delete (*n);
            (*n) = nullptr;
        }
    }
}

bool MemoryManager::TryUpdateSmallestBlock(Node* potentialSmallBlock)
{
    // If the new potential block is smaller, always set the smallest block to this.
    if (mSmallestBlock->size > potentialSmallBlock->size)
    {
        mSmallestBlock = potentialSmallBlock;
        return true;
    }

    // If it's not smaller, the only other case we consider is if they are equal.
    if (mSmallestBlock->size != potentialSmallBlock->size)
    {
        return false;
    }

    // If the blocks are equal size we should always point to the left most address.
    if (mSmallestBlock->start > potentialSmallBlock->start)
    {
        mSmallestBlock = potentialSmallBlock;
        return true;
    }

    return false;
}

bool MemoryManager::TryUpdateLargestBlock(Node* potentialLargeBlock)
{
    // If the new potential block is larger, always set the larger block to this.
    if (mLargestBlock->size < potentialLargeBlock->size)
    {
        mLargestBlock = potentialLargeBlock;
        return true;
    }

    // If it's not larger, the only other case we consider is if they are equal.
    if (mLargestBlock->size != potentialLargeBlock->size)
    {
        return false;
    }

    // If the blocks are equal size we should always point to the left most address.
    if (mLargestBlock->start > potentialLargeBlock->start)
    {
        mLargestBlock = potentialLargeBlock;
        return true;
    }

    return false;
}

} // Namespace ignite::mem.