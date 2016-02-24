/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file mm_allocator.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/02/13 17:30:05
 * @version $Revision$ 
 * @brief global memroy allocator
 *  
 **/


#ifndef  __MM_ALLOCATOR_H_
#define  __MM_ALLOCATOR_H_

#include <stdint.h>
#include "comm_def.h"

namespace lbs
{
    namespace common
    {
        class MMAllocator
        {
            //64M each chunk
            static const int EACH_CHUNK = 64 * 1024 * 1024;
            //max 16K 
            static const int MAX_CHUNK_NUM = 16 * 1024;

            char* chunk_[MAX_CHUNK_NUM];
            char* cur_; 
            int cur_offset_;
            int index_;

            //over 64M
            char* big_chunk_[MAX_CHUNK_NUM];
            int big_chunk_index_;

            //each zone's memroy use
            int64_t zone_memory_[128];

            /**
             * @brief alloc big chunk(over 64M)
             * and return it to user
             *
             * @param [in] real_size   : const int
             * @param [out] mem   : char* &
             * @param [in] mm_flag   : MM_FLAG
             * @return  void 
             * @todo 
            **/
            void alloc_big_chunk(const int real_size, 
                    char*& mem, MM_FLAG mm_flag = OTHER);

            /**
             * @brief alloc chunk (64M) and return it
             * to user
             *
             * @param [in] real_size   : const int
             * @param [out] mem   : char* &
             * @param [in] mm_flag   : MM_FLAG
             * @return  void 
             * @todo 
            **/
            void alloc_chunk(const int real_size, 
                    char*& mem, MM_FLAG mm_flag = OTHER);
            
            private:
            MMAllocator(const MMAllocator&);
             
            public:
            MMAllocator();
            static MMAllocator* get_allocator();

            /**
             * @brief 
             *
             * @param [in] size   : int
             * @param [out] mem   : void* &
             * @param [in] mm_flag   : MM_FLAG
             * @return  void 
             * @todo 
            **/
            void alloc_mm(const int size, char*& mem, MM_FLAG mm_flag = OTHER);

            /**
             * @brief 
             *
             * @param [in] size   : const int
             * @param [in] mm_flag   : MM_FLAG
             * @return  void 
             * @todo 
            **/
            void mark_mm_flag(const int size, MM_FLAG mm_flag);

            /**
             * @brief for debug only
             *
             * @return  void 
             * @todo 
            **/
            void print();

            void delete_mm();
        };
    }
}
















#endif  //__MM_ALLOCATOR_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
