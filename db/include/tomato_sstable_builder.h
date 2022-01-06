/*
 * @Author: Tomato
 * @Date: 2022-01-05 23:14:28
 * @LastEditTime: 2022-01-06 22:54:40
 */
#ifndef TOMATO_DB_DB_INCLUDE_TOMATO_SSTABLE_BUILDER_H
#define TOMATO_DB_DB_INCLUDE_TOMATO_SSTABLE_BUILDER_H

#include "tomato_data.h"

#include <tomato_io.h>
#include <vector>

namespace tomato {

class BlockBuilder {
public:
    BlockBuilder(const TableConfig&);
    ~BlockBuilder();
    BlockBuilder(const BlockBuilder&) = delete;
    BlockBuilder& operator=(const BlockBuilder&) = delete;

    /**
     * @brief 向一个block中添加key-value
     * 
     * @param key 键 
     * @param value 值
     */
    void add(const std::string&key, const std::string& value);

    /**
     * @brief 重置数据
     * 
     */
    void reset();
private:
    /**
     * @brief 所有的键值对内容
     * 
     */
    std::string contents_;

    /**
     * @brief 上一个被添加的key
     * 
     */
    std::string last_key_;

    /**
     * @brief 所有完整key的相对偏移量
     * 
     */
    std::vector<uint64_t> restarts_;

    /**
     * @brief 一组有多少个键值对
     * 
     */
    int group_size_;

    /**
     * @brief 当前组有多少元素
     * 
     */
    int current_group_size_;
};

class SSTableBuilder {
public:
    SSTableBuilder();

    void add(const std::string&key, const std::string& value);
    
private:
    BlockBuilder data_block_builder_;
    BlockBuilder index_builder_;
    AppendOnlyFile* file_;
    uint64_t offset_;
};


}
#endif