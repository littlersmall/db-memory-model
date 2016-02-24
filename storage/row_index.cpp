/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file row_index.cpp
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/22 12:14:31
 * @brief 
 *  
 **/

#include "row_index.h"
#include "table_index.h"
#include "table_schema.h"
#include "comm_def.h"
#include "mm_allocator.h"
#include "comm_log.h"
#include "table_manager.h"

#define ROW_INDEX_ALLOC table_index_->get_table_manager()->get_allocator()

namespace lbs
{
    namespace db_mm
    {
        IndexList::IndexList()
        {
            memset(this, '\0', sizeof(*this));
        }

        IndexList::~IndexList()
        {
            IndexList* cur = this->next;
            IndexList* next = NULL;

            while (cur)
            {
                next = cur->next;
                delete cur;
                cur = next;
            }
        }

        RowIndex::RowIndex() 
        {
            row_data_ = NULL;
            column_array_ = NULL;
            table_index_ = NULL;
            reverse_index_list_ = NULL;

            row_stat_ = NORMAL_ROW;
        }

        void RowIndex::push_back(const int column_id, RowIndex* row_index)
        {
            IndexList** tail = NULL;

            if (NULL == reverse_index_list_)
            {
                tail = &reverse_index_list_; 
            }

            else
            {
                tail = &reverse_index_list_->next;

                while (NULL != *tail)
                {
                    tail = &((*tail)->next);
                }
            }
                    
            char* buf = NULL;
            int len = sizeof(IndexList);
            common::MM_FLAG flag = common::REVERSE_INDEX;

            ROW_INDEX_ALLOC->alloc_mm(len, buf, flag);

            *tail = new(buf) IndexList();
            (*tail)->column_id = column_id;
            (*tail)->row_index = row_index;
        }

       
        int RowIndex::init(TableIndex* table_index)
                //const bool reverse_index_on)
        {
            int ret = 0;

            if (NULL != table_index)
            {
                table_index_ = table_index;

                int column_num = table_index_->get_table_schema()->get_column_num();

                char* buf = NULL;
                int len = sizeof(int16_t) * (column_num - 1) + sizeof(int);
                common::MM_FLAG flag = common::ROW_INDEX;

                ROW_INDEX_ALLOC->alloc_mm(len, buf, flag);
                column_array_ = new(buf) int16_t[column_num - 1];

                if (NULL == column_array_)
                {
                    COM_WRITELOG(COMLOG_WARNING, "row_index init out of memory");
                    ret = common::NO_MEMORY;
                }
            }

            else
            {
                COM_WRITELOG(COMLOG_WARNING, "row_index init err, table_index is NULL");
                ret = common::NULL_POINTER;
            }

            return ret;
        }

        int RowIndex::set_offset(const char* row_data, 
                int& row_end_offset)
        {
            //for debug
            //printf("%s ", row_data);

            int ret = 0;

            if (NULL == (row_data_ = row_data))
            {
                COM_WRITELOG(COMLOG_NOTICE, "row_index init table data is NULL");
                row_end_offset = 0;
                ret = 0;
            } 

            else
            {
                int cur_offset = 0;
                int column_num = table_index_->get_table_schema()->get_column_num();
 
                for (int i = 0; i < column_num; i++)
                {
                    int str_len = strlen(row_data_ + cur_offset);
                    
                    //'\0'
                    cur_offset += str_len + 1;

                    if (column_num - 1 > i)
                    {
                        column_array_[i] = (int16_t) cur_offset;

                        //for debug
                        //printf("%s ", row_data + cur_offset);
                    }

                    // column_num - 1 == i
                    else
                    {
                        if ('\n' == *(row_data_ + cur_offset))
                        {
                            row_end_offset = cur_offset + 1;  

                            //for debug
                            //printf("\n");
                        }

                        else
                        {
                            COM_WRITELOG(COMLOG_WARNING, 
                                    "row_index init err, row_data [%s] end row is not \\n", 
                                    row_data_);
                            ret = common::ROW_ERROR;
                        }
                    }
                }
            }

            return ret;
        }

        const char* RowIndex::get_row_data() const
        {
            if (NULL == row_data_)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_row_data err, row_data is NULL");
            }

            return row_data_;
        }

        int RowIndex::get_column_offset(const int column_id) const
        {
            return (0 == column_id ? 0 : column_array_[column_id-1]);
        }

        int RowIndex::get_column_id(const char* column_name)
        {
            return table_index_->get_table_schema()->get_column_id(column_name);
        }

        int RowIndex::set_reverse_index(const int column_id,
                RowIndex* row_index)
        {
            int ret = 0;

            if (NULL == row_index)
            {
                COM_WRITELOG(COMLOG_WARNING, 
                        "set_reverse_index err, row_index is NULL");
                ret = common::NULL_POINTER; 
            }

            else 
            {
                push_back(column_id, row_index); 
            }

            return ret; 
        }

        const TableSchema* RowIndex::get_table_schema() const
        {
            return table_index_->get_table_schema();
        }

        IndexList* RowIndex::get_reverse_index()
        {
            return reverse_index_list_; 
        }

        int RowIndex::get_table_id()
        {
            return get_table_schema()->get_table_id();
        }

        const char* RowIndex::get_table_name()
        {
            return get_table_schema()->get_table_name();
        }

        const char* RowIndex::get_column_name(const int index)
        {
            return get_table_schema()->get_column_name(index);
        }

        int RowIndex::get_column_num()
        {
            return get_table_schema()->get_column_num();
        }

        // 1. calculate the new row's len
        // 2. new a row in mm_alloc
        // 3. write data to the new row
        // 4. if foreign key, set reverse_index
        // 5. update column_offset and data
        // 6. put table_id && row's index into a hashset for updating db
        int RowIndex::set_column_data(const int column_id, const char* value)
        {
            int ret = 0;
            int len = 0;
            int column_num = get_column_num();
            char tmp_row[1024] = {};

            if (0 == column_id && NULL != row_data_)
            {                
                COM_WRITELOG(COMLOG_WARNING, 
                        "set_row_key err, cannot set row_key");
 
                ret = -1; 
            }

            else if (NULL != value)
            {
                int value_len = strlen(value);

                if (0 == column_id)
                {
                    //设置主键的hash索引
                    if (NULL != value)
                    {
                        table_index_->get_table_manager()->get_table_index(get_table_id())->set_hash((char*) value, this); 
                    }
                }

                if (NULL == row_data_)
                {
                    row_data_ = tmp_row;
                }

                //1
                len = strlen(row_data_) + 1;

                for (int i = 0; i < column_id - 1; i++)
                {
                    len += strlen(row_data_ + column_array_[i]) + 1; 
                }    

                len += value_len + 1;

                for (int i = column_id; i < column_num - 1; i++)
                {
                    len += strlen(row_data_ + column_array_[i]) + 1;
                }

                // '\n'
                len += 1;

                //2
                char* buf = NULL;
                common::MM_FLAG flag = common::UPDATE_DATA;

                ROW_INDEX_ALLOC->alloc_mm(len, buf, flag);

                //3
                int cur_offset = 0;
                
                memcpy(buf, row_data_, cur_offset = strlen(row_data_) + 1);

                //主键为NULL
                if (1 == cur_offset && 0 == column_id)
                {
                    cur_offset = 0;

                }
                
                for (int i = 0; i < column_id - 1; i++)
                {
                    int tmp_len = strlen(row_data_ + column_array_[i]);

                    memcpy(buf + cur_offset, row_data_ + column_array_[i], tmp_len + 1);
                    cur_offset += tmp_len + 1;
                }

                memcpy(buf + cur_offset, value, value_len + 1);
                cur_offset += value_len + 1; 

                for (int i = column_id; i < column_num - 1; i++)
                {
                    int tmp_len = strlen(row_data_ + column_array_[i]);

                    memcpy(buf + cur_offset, row_data_ + column_array_[i], tmp_len + 1);
                    cur_offset += tmp_len + 1;
                }

                buf[cur_offset++] = '\n';
                
                //4
                for (int i = 0; i < get_table_schema()->get_foreign_key_num(); i++)
                {   
                    int tmp_column_id = get_table_schema()->get_foreign_key_id(i);

                    if (column_id == tmp_column_id)
                    {
                        int foreign_table_id = get_table_schema()->get_foreign_table_id(i); 
                        int offset = get_column_offset(column_id);
                        TableIndex* table_index = table_index_->get_table_manager()->get_table_index(foreign_table_id);
                        RowIndex* foreign_row_index = table_index->get_row_index(row_data_+offset);

                        if (foreign_row_index)
                        {
                            foreign_row_index->del_reverse_index(this);
                        }

                        RowIndex* new_foreign_row_index = table_index->get_row_index(value);

                        if (new_foreign_row_index)
                        {
                            new_foreign_row_index->set_reverse_index(column_id, this);
                        }

                        break;
                    }
                }

                int test_offset = 0;

                //5
                ret = set_offset(buf, test_offset);

                if (test_offset != cur_offset)
                {
                    COM_WRITELOG(COMLOG_WARNING, "set_column_data error");
                }
                
                //6
                row_stat_ = UPDATE_ROW; 

                //log
                int table_id = get_table_id();

                COM_WRITELOG(COMLOG_NOTICE, "changed row, table_id[%d], row_key[%s]", table_id, row_data_);
            }

            else
            {
                ret = common::NULL_POINTER;
                COM_WRITELOG(COMLOG_WARNING, "set_column_data, value is NULL");
            }

            return ret;
        }

        void RowIndex::del_reverse_index(RowIndex* row_index)
        {
            while (reverse_index_list_ && row_index == reverse_index_list_->row_index)
            {
                reverse_index_list_ = reverse_index_list_->next;
            }

            IndexList* pre = reverse_index_list_;
            IndexList* cur = reverse_index_list_;

            for (; cur; cur = cur->next)
            {
                if (cur->row_index == row_index)
                {
                    pre->next = cur->next; 
                }

                else
                {
                    pre = cur;
                }
            }
        }

        bool RowIndex::del()
        {
            int ret = false;

            if (NULL == reverse_index_list_)
            {
                for (int i = 0; i < get_table_schema()->get_foreign_key_num(); i++)
                {   
                    int column_id = get_table_schema()->get_foreign_key_id(i);
                    int foreign_table_id = get_table_schema()->get_foreign_table_id(i);
                    int offset = get_column_offset(column_id);
                    TableIndex* table_index = table_index_->get_table_manager()->get_table_index(foreign_table_id);
                    RowIndex* foreign_row_index = table_index->get_row_index(row_data_+offset);

                    if (foreign_row_index)
                    {
                        foreign_row_index->del_reverse_index(this);
                    }
                }

                row_stat_ = DELETE_ROW;
                ret = true;
            }
            
            return ret; 
        }

        TableManager* RowIndex::get_table_manager()
        {
            return table_index_->get_table_manager();
        }

        void RowIndex::print()
        {
            printf("row_key: %s\n", row_data_); 
            printf("column_offset: ");

            for (int i = 0; i < get_column_num() - 1; i++)
            {
                printf("%d ", (int) column_array_[i]);
            }

            printf("\n");

            IndexList* index_list = reverse_index_list_;

            for (; index_list; index_list = index_list->next)
            {
                printf("reverse index:\n");

                RowIndex* row_index = index_list->row_index;
                int column_id = index_list->column_id;

                const TableSchema* reverse_table_schema = row_index->get_table_schema();
                int table_id = reverse_table_schema->get_table_id();
                
                printf("\ttable_id = %d, column_id = %d\t", table_id, column_id);

                const char* reverse_row_data = row_index->get_row_data();
                
                printf("row_key = %s\n", reverse_row_data);

                index_list = index_list->next;
            }
        }

        RowIndex::~RowIndex()
        {}
    }
}





















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
