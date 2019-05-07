
#include "memorypool.h"
#include <cassert>

//-----------------------------------------------------------------------------

MemoryPool::MemoryBlock::MemoryBlock( MemoryPool* owner_pool ) :
            next_alloc(0),
            next_block(0),
            owner(owner_pool)
{

    //Allocate the memory block
    data = new char[ owner->blk_size ];
    assert( data != 0 );
}

//-----------------------------------------------------------------------------

MemoryPool::MemoryBlock::~MemoryBlock() {
    //Free the memory block
    delete [] data;
}

//-----------------------------------------------------------------------------

void* MemoryPool::MemoryBlock::Alloc() {
    //Check if we have any more free objects to return
    if ( next_alloc < owner->objs_per_block ) {
        //Increment next_alloc and return the availiable object
        void* ret = data + (next_alloc * owner->obj_size);
        ++next_alloc;
        return ret;
    }

    //We could not make the allocation from this block, return NULL
    return 0;
}

//-----------------------------------------------------------------------------

MemoryPool::MemoryPool( size_t object_size, size_t objects_per_block  ) {

    obj_size = object_size;
    objs_per_block = objects_per_block;

    //Allign size to 32-byte boundaries
    if ( obj_size % 32 != 0 ) {
        obj_size += 32 - (obj_size % 32);
    }

    // Make sure we use a block size that's padded to 4096 to match the windows memory page size.
    blk_size = objs_per_block * obj_size;

    if ( blk_size % 4096 != 0 ) {
        blk_size += 4096 - (blk_size % 4096);
    }

    //Don't waste any space in the blocks, so re-calculate objects per block.
    objs_per_block = blk_size / obj_size;

    //Allocate a single memory block to start
    AllocFirstBlock();
}

//-----------------------------------------------------------------------------

void MemoryPool::AllocFirstBlock() {
    first_block = new MemoryBlock(this);
    last_block = first_block;
}

//-----------------------------------------------------------------------------

void* MemoryPool::Alloc() {
    //First attempt to allocate from the last block (the one that still may have free spaces)
    void* new_obj = last_block->Alloc();
    if ( new_obj ) {
        //Allocation was successful
        return new_obj;
    }

    //Allocation was not successful, so we need a new block
    MemoryBlock* new_block = new MemoryBlock(this);
    assert(new_block != 0);

    //Hook up the new block to the linked list
    last_block->next_block = new_block;
    last_block = new_block;

    //The allocation should now succeed for sure
    return last_block->Alloc();
}

//-----------------------------------------------------------------------------

void MemoryPool::Flush() {
    //Remove all memory blocks
    FreeAllBlocks();

    //Start over with one block allocated
    AllocFirstBlock();
}

//-----------------------------------------------------------------------------

void MemoryPool::FreeAllBlocks() {
    //Traverse the linked list of blocks, freeing each one
    MemoryBlock* current_block = first_block;
    MemoryBlock* next_block = 0;
    while (current_block != 0) {
        next_block = current_block->next_block;
        delete current_block;
        current_block = next_block;
    }
}

//-----------------------------------------------------------------------------

MemoryPool::~MemoryPool() {
    FreeAllBlocks();
}

//-----------------------------------------------------------------------------
