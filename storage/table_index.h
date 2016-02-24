/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file table_index.h
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/21 15:22:28
 * @brief 
 *  
 **/




#ifndef  __TABLE_INDEX_H_
#define  __TABLE_INDEX_H_

#include <bsl/containers/hash/bsl_hashmap.h>
#include <bsl/deque.h>
#include <string>
#include "hash_char.h"
#include "comm_def.h"
#include "row_index.h"

namespace lbs
{
    namespace db_mm
    {
        class RowIndex;
        class TableSchema;
        class TableManager;

        class TableIndex
        {
            public:
            class RowIndexIter : public common::Iter<RowIndex*>
            {
                TableIndex* table_index_;
                int index_;
                int row_num;

                public:
                RowIndexIter(TableIndex* table_index) : table_index_(table_index),
                index_(0)
                {
                    row_num = table_index_->get_row_num(); 
                }

                /**
                 * @brief 遍历所有row
                 *
                 * @param [out] row_index   : RowIndex* &
                 * @return  int 
                 * @todo 
                **/
                virtual int get_next(RowIndex*& row_index)
                {
                    int ret = 0;

                    if (index_ < row_num)
                    { 
                        row_index = table_index_->get_row_index_inner(index_++);
                    }

                    else 
                    {
                        row_index = table_index_->get_row_index(index_++); 

                        if (NULL == row_index)
                        {
                            ret = common::ITERATOR_END;
                            index_ = 0;
                        }
                    }

                    return ret;
                }
            };
            
            private:
            const TableSchema* table_schema_;
            TableManager* table_manager_;
            //file data
            char* data_;
            //data size
            int64_t size_;
            int row_num_;
            RowIndex* row_index_array_;
            bsl::hashmap<char*, RowIndex*, 
                common::HashFun, common::EqualFun> hash_index_; 
            bool mmap_on_;
            bool has_data_;
            bool has_reverse_index_;
            std::string data_path_;

            bsl::deque<RowIndex*> insert_deque_;

            /**
             * @brief 1.open file
             *        2.load it into memory
             *
             * @param [in] file_name   : const char*
             * @param [in] size   : const int64_t
             * @return  int 
             * @todo 
            **/
            int set_data_and_size(const char* file_name, 
                    const int64_t size);
             /**
             * @brief 1.open file
             *        2.mmap the file
             *
             * @param [in] file_name   : const char*
             * @param [in] size   : const int64_t
             * @return  int 
             * @todo 
            **/
            int set_data_and_size_mmap(const char* file_name, 
                    const int64_t size);
            
            /**
             * @brief new row_index_aray_, construct column offset
             * build hash index
             *
             * @param [in] hash_index_on   : const bool
             * @return  int 
             * @todo 
            **/
            int set_row_offset_and_hash_index(const bool hash_index_on);

            public:
            TableIndex();

            /**
             * @brief init table_index, new row_index_array_
             * init each row_index
             *
             * @param [in] table_schema   : const TableSchema*
             * @param [in] hash_index_on   : const bool
             * @return  int 
             * @todo 
            **/
            int init(const TableSchema* table_schema, TableManager* table_manager,
                    const bool hash_index_on);

            /**
             * @brief 
             *
             * @return  int 
             * @todo 
            **/
            int set_reverse_index();

            /**
             * @brief 
             *
             * @param [out] table_schema   : const TableSchema* &
             * @return  void 
             * @todo 
            **/
            inline const TableSchema* get_table_schema() const
            {
                return table_schema_;
            }

            /**
             * @brief 
             *
             * @return  TableManager* 
             * @todo 
            **/
            inline TableManager* get_table_manager()
            {
                return table_manager_;
            }

            /**
             * @brief use hash index find row_index
             *
             * @param [in] row_key   : const char*
             * @param [out] row_index   : RowIndex* &
             * @return  int 
             * @todo 
            **/
            RowIndex* get_row_index(const char* key);

            /**
             * @brief use array index get row_index
             *
             * @param [in] index   : const int
             * @param [out] row_index   : RowIndex* &
             * @return  int 
             * @todo 
            **/
            RowIndex* get_row_index(const int index);

            /**
             * @brief 
             *
             * @param [in] index   : const int
             * @return  RowIndex* 
             * @todo 
            **/
            inline RowIndex* get_row_index_inner(const int index)
            {
                return &row_index_array_[index];
            }

            /**
             * @brief 
             *
             * @param [out] row_num   : int&
             * @return  void 
             * @todo 
            **/
            int get_row_num();

            /**
             * @brief 
             *
             * @return  RowIndexIter 
             * @todo 
            **/
            RowIndexIter iter()
            {
                return RowIndexIter(this); 
            }

            /**
             * @brief 是否有数据
             *
             * @return  bool 
             * @todo 
            **/
            bool has_data();

            /**
             * @brief 是否已建立反向索引
             *
             * @return  bool 
             * @todo 
            **/
            bool has_reverse_index();

            /**
             * @brief 添加hash索引，在插入新数据时使用
             *
             * @param [in/out] key   : char*
             * @param [in/out] value   : RowIndex*
             * @return  void 
             * @todo 
            **/
            void set_hash(char* key, RowIndex* value)
            {
                hash_index_.set(key, value);            
            }

            /**
             * @brief 
             *
             * @param [in] row_num   : int
             * @return  void 
             * @todo 
            **/
            void set_row_num(const int row_num)
            {
                row_num_ = row_num;
            }

            void set_data(char* data)
            {
                data_ = data;
            }

            /**
             * @brief 
             *
             * @param [in] data_path   : std::string
             * @return  void 
             * @todo 
            **/
            void set_data_path(std::string data_path);

            /**
             * @brief 
             *
             * @return  std::string 
             * @todo 
            **/
            std::string get_data_path();

            /**
             * @brief 添加一条数据，获得一个row_index,之后调用row_index的
             * set_column_data()设置数据
             *
             * @return  RowIndex* 
             * @todo 
            **/
            RowIndex* add();

            /**
             * @brief 
             *
             * @return  void 
             * @todo 
            **/
            void print();

            ~TableIndex();
        };
    }
}














#endif  //__TABLE_INDEX_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
