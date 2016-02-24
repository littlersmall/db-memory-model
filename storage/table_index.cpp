/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file table_index.cpp
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/21 18:10:21
 * @brief 
 *  
 **/

#include "table_index.h"
#include "table_manager.h"
#include "table_schema.h"
#include "row_index.h"
#include <fcntl.h>
#include "comm_log.h"
#include <sys/mman.h>
#include "comm_def.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "mm_allocator.h"

#define TABLE_INDEX_ALLOC table_manager_->get_allocator()

namespace lbs
{
    namespace db_mm
    {
        TableIndex::TableIndex()
        {
            table_schema_ = NULL;
            table_manager_ = NULL;
            data_ = NULL;
            size_ = 0;
            row_num_ = 0;
            row_index_array_ = NULL;    
            mmap_on_ = true;
            has_data_ = false;
            has_reverse_index_ = false;
        }

        //1. foreach row's foreign_key
        //2. get row_index of the other table that reference by foreign key
        //3. add reverse index 
        int TableIndex::set_reverse_index()
        {
            int ret = 0;
            RowIndex* row_index = NULL;

            for (int index = 0; index < row_num_; index++)
            {
                if (NULL == (row_index = get_row_index_inner(index)))
                {
                    COM_WRITELOG(COMLOG_WARNING, 
                            "in set_reverse_index, get_row_index out of index [%d]", index);

                    ret = common::NULL_POINTER;
                    break;
                }

                int foreign_key_num = table_schema_->get_foreign_key_num();

                //1
                for (int i = 0; i < foreign_key_num; i++)
                {
                    int column_id = table_schema_->get_foreign_key_id(i);
                    int foreign_table_id = table_schema_->get_foreign_table_id(i);
                    int offset = row_index->get_column_offset(column_id);
                    const char* row_data = NULL;

                    if (NULL == (row_data = row_index->get_row_data()))
                    {
                        COM_WRITELOG(COMLOG_WARNING, 
                                "in set_reverse_index, get_row_data err");
                        continue;
                    }
                    
                    RowIndex* foreign_row_index = NULL;
                    //2
                    if (NULL == (foreign_row_index = 
                                table_manager_->get_table_index(foreign_table_id)->get_row_index(row_data+offset)))
                    {
                        COM_WRITELOG(COMLOG_NOTICE, 
                                "in set_reverse_index, key not found: table_name:[%s], foreign_table_id[%d] row_key[%s], foreign_key[%s], column_id[%d]", 
                                table_schema_->get_table_name(), foreign_table_id, row_data, row_data+offset, column_id);

                        //标记为不完整的要素
                        int table_id = table_schema_->get_table_id();
                        char incomplete_key[1024] = {};
                        
                        snprintf(incomplete_key, sizeof(incomplete_key), 
                                "%d_%s", table_id, row_data);

                        table_manager_->insert_incom_key(incomplete_key);

                        continue;
                    }

                    //3
                    if (0 != (ret = foreign_row_index->set_reverse_index(column_id,
                                row_index)))
                    {
                        COM_WRITELOG(COMLOG_WARNING, 
                                "in set_reverse_index, foreign_row_index->set_reverse_index err");
                        continue;
                    };
                }
            }

            if (0 == ret)
            {
                has_reverse_index_ = true;
            }

            return ret;
        }

        // 1. open file
        // 2. load it into memroy
        int TableIndex::set_data_and_size(const char* file_name, const int64_t size)
        {
            int ret = 0;

            char* buf = NULL;
            common::MM_FLAG flag = common::DATA_FILE;
            
            if (0 != size)
            {
                TABLE_INDEX_ALLOC->alloc_mm(size, buf, flag);
                data_ = new(buf) char[size];
                size_ = size;

                if (NULL == data_)
                {
                    COM_WRITELOG(COMLOG_WARNING, "table_index new data_ out of memory");
                    ret = common::NO_MEMORY;
                }

                else
                {
                    //1
                    int fd = open(file_name, O_RDONLY);
                    //2
                    int64_t len = read(fd, data_, size);

                    close(fd);

                    if (size != len)
                    {
                        COM_WRITELOG(COMLOG_WARNING, 
                                "table_index read file size err,size[%d], real_size[%d]",
                                size, len);
                        ret = common::READ_FILE_ERROR;
                    }
                }
            }

            //printf("%s\n", data_);
            return ret; 
        }

        //mmap version
        int TableIndex::set_data_and_size_mmap(const char* file_name, const int64_t size)
        {
            int ret = 0;
            int fd = open(file_name, O_RDONLY);

            size_ = size;

            if (0 != size)
            {
                data_ = (char*) mmap(NULL, size_, PROT_READ, MAP_SHARED, fd, 0);

                if ((void*) (-1) == (void*) data_)
                {
                    COM_WRITELOG(COMLOG_WARNING, "mmap error");
                    ret = common::MMAP_ERROR;
                }
            }

            close(fd);

            TABLE_INDEX_ALLOC->mark_mm_flag(size, common::DATA_FILE);

            //printf("%s\n", data_);
            return ret; 
        }

        int TableIndex::set_row_offset_and_hash_index(const bool hash_index_on)
        {
            int ret = 0;

            //init hashmap
            if (hash_index_on && table_schema_->has_row_key())
            {
                hash_index_.create(row_num_);
            }

            char* buf = NULL;
            int len = sizeof(RowIndex) * row_num_ + sizeof(int);
            common::MM_FLAG flag = common::ROW_INDEX;

            TABLE_INDEX_ALLOC->alloc_mm(len, buf, flag);

            //printf("in table_index, len = %d, buf = %p\n", len, buf);

            row_index_array_ = new(buf) RowIndex[row_num_];

            if (NULL == row_index_array_)
            {
                COM_WRITELOG(COMLOG_WARNING, "table_index new row_index out of memory");
                ret = common::NO_MEMORY;
            }

            if (0 == ret)
            {
                int64_t cur_offset = 0;
                int row_length = 0;

                for (int i = 0; i < row_num_; i++)
                {
                    if (0 == (ret = row_index_array_[i].init(this)))
                    {
                        if (0 == (ret = row_index_array_[i].set_offset(data_ + cur_offset,
                            row_length)))
                        {
                            if (hash_index_on && table_schema_->has_row_key())
                            {
                                //key: char*, row_key; value row_index, array index
                                hash_index_.set(data_ + cur_offset, &row_index_array_[i]);
                            }

                            cur_offset += row_length;
                        }
                        
                        else
                        {
                            COM_WRITELOG(COMLOG_WARNING, 
                                    "in set_row_offset_and_hash_index, row_index.set_offset err, ret[%d] row[%d]",
                                    ret, i);
                            break;
                        }
                    }

                    else
                    {
                        COM_WRITELOG(COMLOG_WARNING, "in set_row_offset_and_hash_index" 
                                " row_index init err, ret[%d]", ret);
                        break;
                    }
                }
            }

            return ret;
        }

        int TableIndex::init(const TableSchema* table_schema, TableManager* table_manager,
                const bool hash_index_on)
        {
            int ret = 0;
            int64_t size = 0;
            const char* file_name = NULL;
            
            if (NULL == (table_schema_ = table_schema))
            {
                COM_WRITELOG(COMLOG_WARNING, "table_index init err, table_schema is NULL");
                ret = common::NULL_POINTER;
            }

            else if (NULL == (table_manager_ = table_manager))
            {
                COM_WRITELOG(COMLOG_WARNING, "table_index init err, table_manager is NULL");
                ret = common::NULL_POINTER;
            }

            else if (NULL == data_)
            {
                file_name = data_path_.c_str();
                struct stat file_stat;              

                stat(file_name, &file_stat);
                size = file_stat.st_size;

                if (0 != (ret = (mmap_on_ ? 
                                set_data_and_size_mmap(file_name, size) : 
                                set_data_and_size(file_name, size))))
                {
                    COM_WRITELOG(COMLOG_WARNING, 
                            "table_index init err, set_data_and_size error ret[%d]", ret);
                }
            }

            else  //data_已设置
            {
                COM_WRITELOG(COMLOG_NOTICE, 
                            "table_index init, data_ is already set", ret);
            }

            if (0 < row_num_ && (0 != (ret = set_row_offset_and_hash_index(hash_index_on))))
            {
                COM_WRITELOG(COMLOG_WARNING, 
                        "table_index init err, set_row_offset_and_hash_index error ret[%d]", 
                        ret);
            }

            if (0 == ret)
            {
                const char* table_name = table_schema_->get_table_name();

                COM_WRITELOG(COMLOG_NOTICE,
                        "table:%s table_index init success, table_id[%d], row_num[%d]", 
                        table_name, table_schema_->get_table_id(), row_num_);

                has_data_ = true;
            }
             
            return ret;
        }

        RowIndex* TableIndex::get_row_index(const char* key)
        {
            RowIndex* row_index = NULL;

            if (bsl::HASH_EXIST != hash_index_.get((char*) key, &row_index))
            {
                COM_WRITELOG(COMLOG_NOTICE, "in hash_index key not found, key[%s]",
                       key); 
            }

            return row_index;
        }

        RowIndex* TableIndex::get_row_index(const int index)
        {
            RowIndex* row_index = NULL;
            int row_num = get_row_num();

            if (index < row_num)
            {
                row_index = &row_index_array_[index];
            }

            else if (index < row_num + (int) insert_deque_.size())
            {
                row_index = insert_deque_[index-row_num]; 
            }

            else
            {    
                COM_WRITELOG(COMLOG_DEBUG, 
                        "get_row_index: index > row_num, index[%d]", index);
            }

            return row_index;
        }

        int TableIndex::get_row_num()
        {
            return row_num_;
        }

        bool TableIndex::has_data()
        {
            return has_data_;
        }


        std::string TableIndex::get_data_path()
        {
            return data_path_; 
        }

        void TableIndex::set_data_path(std::string data_path)
        {
            data_path_ = data_path;
        }

        bool TableIndex::has_reverse_index()
        {
            return has_reverse_index_;
        }

        RowIndex* TableIndex::add()
        {
            char* buf = NULL;
            int len = sizeof(db_mm::RowIndex);
            common::MM_FLAG flag = common::ROW_INDEX;

            TABLE_INDEX_ALLOC->alloc_mm(len, buf, flag);
            db_mm::RowIndex* row_index = new(buf) RowIndex;

            row_index->init(this);
            row_index->set_row_stat(UPDATE_ROW);
            insert_deque_.push_back(row_index);

            return row_index;
        }

        void TableIndex::print()
        {
            const char* table_name = table_schema_->get_table_name();

            if (NULL != table_name)
            {
                printf("table_name = %s\n", table_name);
            }

            for (int i = 0; i < row_num_; i++)
            {
                get_row_index(i)->print(); 
            }
        }

        TableIndex::~TableIndex()
        {
            if (mmap_on_ && NULL != data_)
            {
                munmap(data_, size_);

                data_ = NULL;
            }
        }
    }
}





















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
