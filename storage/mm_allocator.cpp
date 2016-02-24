/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file mm_allocator.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/02/17 15:49:27
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "mm_allocator.h"
#include "comm_log.h"
#include <iostream>

namespace lbs
{
    namespace common
    {
        MMAllocator::MMAllocator()
        {
            memset(this, '\0', sizeof(*this));  

            chunk_[index_++] = new(std::nothrow) char[EACH_CHUNK]; 
            cur_ = chunk_[0];
            COM_WRITELOG(COMLOG_NOTICE, "alloc chunk [%d]", EACH_CHUNK);       
        }

        void MMAllocator::alloc_big_chunk(const int real_size, 
                char*& mem, MM_FLAG mm_flag)
        {
            big_chunk_[big_chunk_index_++] = new(std::nothrow) char[real_size];

            if (NULL == big_chunk_[big_chunk_index_-1])
            {
                COM_WRITELOG(COMLOG_WARNING, "alloc fail, no memory");
            }

            else
            {
                COM_WRITELOG(COMLOG_NOTICE, "alloc big chunk, size[%d]", real_size);
                mem = big_chunk_[big_chunk_index_-1];
                zone_memory_[(int) mm_flag] += real_size;
            }
        }

        void MMAllocator::alloc_chunk(const int real_size,
                char*& mem, MM_FLAG mm_flag)
        {
            chunk_[index_++] = new(std::nothrow) char[EACH_CHUNK];
            cur_ = chunk_[index_-1];
            cur_offset_ = 0;

            if (NULL == cur_)
            {
                COM_WRITELOG(COMLOG_WARNING, "alloc fail, no memory");
            }

            else
            {
                COM_WRITELOG(COMLOG_NOTICE, "alloc chunk [%d]", EACH_CHUNK);       
                mem = cur_;
                cur_offset_ += real_size;

                zone_memory_[(int) mm_flag] += real_size;
            }
        }

        MMAllocator* MMAllocator::get_allocator()
        {
            static MMAllocator* s_mmallocator = new MMAllocator();

            return s_mmallocator;
        }

        void MMAllocator::alloc_mm(const int size, char*& mem, MM_FLAG mm_flag)
        {
            // 8 byte align 
            int real_size = (size + 7) & 0x7ffffff8;

            mem = NULL;

            if (real_size >= EACH_CHUNK)
            {
                alloc_big_chunk(real_size, mem, mm_flag);
            }

            else if (real_size + cur_offset_ >= EACH_CHUNK)
            {
                alloc_chunk(real_size, mem, mm_flag);
            }

            else
            {
                mem = cur_ + cur_offset_;
                cur_offset_ += real_size;
                zone_memory_[(int) mm_flag] += real_size;
            }
        }

        void MMAllocator::mark_mm_flag(const int size, MM_FLAG mm_flag)
        {
            zone_memory_[(int) mm_flag] += size;
        }

        void MMAllocator::print()
        {
            for (int i = 0; i < (int) FLAG_MAX; i++)
            {
                printf("FLAG : %d --------- memory use: %ld\n", i, zone_memory_[i]); 
            }
        } 

        void MMAllocator::delete_mm()
        {
            for (int i = 0; i < index_; i++)
            {
                if (NULL != chunk_[i])
                {
                    delete[] chunk_[i];

                    chunk_[i] = NULL;
                }
            }            
            
            for (int i = 0; i < big_chunk_index_; i++)
            {
                if (NULL != big_chunk_[i])
                {
                    delete[] big_chunk_[i];

                    big_chunk_[i] = NULL;
                }
            }
 
        }
    }
}




















/* vim: set ts=4 sw=4 sts=4 tw=100 */
