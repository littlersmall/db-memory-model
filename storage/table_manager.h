/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file table_manager.h
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/21 15:13:03
 * @brief 
 *  
 **/




#ifndef  __TABLE_MANAGER_H_
#define  __TABLE_MANAGER_H_

#include <vector>
#include <string>
#include "hash_char.h"
#include <bsl/set.h>

namespace lbs
{
    namespace common
    {
        class MMAllocator;
    }

    namespace db_mm
    {
        class SchemaManager;
        class DataFileManager;
        class TableIndex;
        class RowIndex;

        class TableManager
        {
            common::MMAllocator* mm_allocator_;
            SchemaManager* schema_manager_;     
            //table_index
            TableIndex* table_index_array_;
            bsl::hashset<char*, common::HashFun, common::EqualFun> incomplete_set_;
            
            /**
             * @brief build reverse index
             *
             * @return  int 
             * @todo 
            **/
            int set_reverse_index();
            
            public:

            TableManager(SchemaManager* schema_manager);

            /**
             * @brief 载入数据
             *
             * @param [in] hash_index_on   : const bool
             * @param [in] reverse_index_on   : const bool
             * @return  int 
             * @todo 
            **/
            int init_data(const bool hash_index_on = true,
                    const bool reverse_index_on = true);

            /**
             * @brief 
             *
             * @param [out] schema_manager   : SchemaManager* &
             * @return  void 
             * @todo 
            **/
            SchemaManager* get_schema_manager()
            {
                return schema_manager_;
            }

            /**
             * @brief 
             *
             * @param [in] index   : const int
             * @param [out] table_index   : TableIndex* &
             * @return  int 
             * @todo 
            **/
            TableIndex* get_table_index(const int index);

            /**
             * @brief 插入不完整要素的 table_id + '_' + row_key 
             *
             * @param [in] key   : char*
             * @return  void 
             * @todo 
            **/
            void insert_incom_key(char* key);

            /**
             * @brief 
             *
             * @param [in] key   : char*
             * @return  bool 
             * @todo 
            **/
            bool incomplete(char* key);

            /**
             * @brief for debug only
             * 1. print schema_manager
             * 2. print each table's index
             *
             * @return  void 
             * @todo 
            **/
            void print();

            /**
             * @brief 内存分配器
             *
             * @return  common::MMAllocator* 
             * @todo 
            **/
            common::MMAllocator* get_allocator()
            {
                return mm_allocator_;
            }

            void destroy();
            ~TableManager();
        };
    }
}













#endif  //__TABLE_MANAGER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
