/*
 * @Author: Tomato
 * @Date: 2021-12-18 13:19:56
 * @LastEditTime: 2021-12-18 20:43:22
 */
#include "../include/tomato_allocator.h"
#include <cassert>

namespace tomato {

static const int POOL_BLOCK_BYTES = 4096;
static const int BIG_BYTES_THRESHOLD = POOL_BLOCK_BYTES / 4;

Allocator::Allocator()
    : pool_begin_(nullptr),
      current_remaining_(0),
      allocated_size_(0) {}

Allocator::~Allocator() {
    for (size_t i = 0; i < pool_.size(); ++i) {
        delete[] pool_[i];
    }
}

char* Allocator::allocate(size_t bytes) {
    assert(bytes > 0);

    // 仍有可用内存
    if (bytes <= current_remaining_) {
        char* result = pool_begin_;
        pool_begin_ += bytes;
        current_remaining_ -= bytes;
        return result;
    }

    // 大对象直接分配
    if (bytes > BIG_BYTES_THRESHOLD) {
        return doAllocate(bytes);
    }

    // 更新新的当前内存分配点位
    char* result = doAllocate(POOL_BLOCK_BYTES);
    pool_begin_ = result + bytes;
    current_remaining_ = POOL_BLOCK_BYTES - bytes;
    return result;
}

char* Allocator::allocateAligned(size_t bytes) {
    assert(bytes > 0);

    // 计算需要补齐的字节数，使内存块首地址是align的整数倍
    const int align = (sizeof(void*) > 8) ? 8 : sizeof(void*);
    size_t mod = reinterpret_cast<uintptr_t>(pool_begin_) & (align - 1);
    size_t needToAdd = align - mod;
    size_t totalNeed = bytes + needToAdd;

    // 若有可分配内存调整可分配水位后直接返回，否则重新分配
    char* result = nullptr;
    if (totalNeed <= current_remaining_) {
        result = pool_begin_ + needToAdd;
        pool_begin_ += totalNeed;
        current_remaining_ -= totalNeed;
    } else {
        // 重新分配new出来的内存一定是对齐的
        result = doAllocate(bytes);
    }
    
    assert((reinterpret_cast<uintptr_t>(result) & (align - 1)) == 0);
    return result;

}

char* Allocator::doAllocate(size_t bytes) {
    char* result = new char[bytes];
    pool_.push_back(result);

    // 加上new时额外存储的数组尺寸
    allocated_size_.fetch_add(sizeof(char*) + bytes, std::memory_order_relaxed);
    return result;
}

}