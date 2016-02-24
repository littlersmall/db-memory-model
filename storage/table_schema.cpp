/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file table_schema.cpp
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/20 15:01:53
 * @brief 
 *  
 **/

#include "table_schema.h"
#include "comm_def.h"
#include "comm_log.h"

namespace lbs
{
    namespace db_mm
    {
        TableSchema::TableSchema()
        {
            memset(table_name_, '\0', sizeof(table_name_));
            table_id_ = 0;
            column_num_ = 0;
            cell_array_ = NULL;

            //默认有主键
            has_row_key_ = true;
        }

        int TableSchema::init(const comcfg::Configure& conf)
        {
            int ret = 0;

            //init table name
            memcpy(table_name_, conf["table"]["table_name"].to_cstr(), 
                    strlen(conf["table"]["table_name"].to_cstr()));
            //init table id
            table_id_ = conf["table"]["table_id"].to_int32();
            column_num_ = conf["table"]["column_num"].to_int32(); 
            row_key_.column_id = conf["table"]["row_key"].to_int32();
            foreign_key_.foreign_key_num = conf["table"]["foreign_key_num"].to_int32();

            if (0 > row_key_.column_id)
            {
                has_row_key_ = false;
            }

            column_hash_.create(column_num_);

            if (0 < foreign_key_.foreign_key_num)
            {
                foreign_key_.foreign_key_array = 
                    new int[foreign_key_.foreign_key_num]; 

                foreign_key_.table_id_array = 
                    new int[foreign_key_.foreign_key_num]; 
                
                if (NULL == foreign_key_.foreign_key_array || 
                    NULL == foreign_key_.table_id_array)
                {
                    COM_WRITELOG(COMLOG_WARNING, "table_schema out of memory");
                    ret = common::NO_MEMORY; 
                }
            }

            if (0 == ret)
            {
                for (int i = 0; i < foreign_key_.foreign_key_num; i++)
                {
                    foreign_key_.foreign_key_array[i] = 
                        conf["table"]["foreign_key"][i]["column_id"].to_int32();
                    foreign_key_.table_id_array[i] = 
                        conf["table"]["foreign_key"][i]["table_id"].to_int32();
                }

                cell_array_ = new Cell[column_num_];

                if (NULL == cell_array_)
                {
                    COM_WRITELOG(COMLOG_WARNING, "table_schema out of memory");
                    ret = common::NO_MEMORY;
                }

                else
                {
                    for (int i = 0; i < column_num_; i++)
                    {
                        memcpy(cell_array_[i].column_name, 
                            conf["table"]["column"][i]["column_name"].to_cstr(),
                            strlen(conf["table"]["column"][i]["column_name"].to_cstr()));
                        cell_array_[i].column_id = 
                            conf["table"]["column"][i]["column_id"].to_int32();
                        cell_array_[i].data_type = 
                            TYPE(conf["table"]["column"][i]["column_type"].to_int32());
            
                        column_hash_.set(cell_array_[i].column_name, cell_array_[i].column_id);
                    }
                }
            }

            return ret;
        }
        
        int TableSchema::get_table_id() const
        {
            return table_id_;
        }

        int TableSchema::get_foreign_table_id(const int index) const
        {
            if (index >= foreign_key_.foreign_key_num)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_foreign_table_id out of index");
            }

            return foreign_key_.table_id_array[index];
        }

        void TableSchema::add_reverse_index(const int t_id, const int c_id)
        { 
            row_key_.index_table_array.push_back(RowKey::TableAndColumnId(t_id, c_id));
            row_key_.reverse_index_num++;
        }

        int TableSchema::get_foreign_key_id(const int index) const
        {
            if (index >= foreign_key_.foreign_key_num)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_foreign_key_id out of index");
            }

            return foreign_key_.foreign_key_array[index];
        }
            
        int TableSchema::get_foreign_key_num() const
        {
            return foreign_key_.foreign_key_num;
        }
            
        int TableSchema::get_reverse_index_num() const
        {
            return row_key_.reverse_index_num; 
        }

        int TableSchema::get_reverse_table_info(const int index,
                int& table_id, int& column_id) const
        {
            int ret = 0;

            if (index >= row_key_.reverse_index_num)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_reverse_table_info out of index");
                ret = common::OUT_OF_INDEX;
            }

            else
            {
                table_id = row_key_.index_table_array[index].table_id;
                column_id = row_key_.index_table_array[index].column_id;
            }

            return ret;
        }

        int TableSchema::get_column_num() const
        {
            return column_num_;
        }

        TYPE TableSchema::get_column_type(const int index) const
        {
            if (index >= column_num_)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_column_type out of index");
            } 

            return cell_array_[index].data_type;
        }

        const char* TableSchema::get_column_name(const int index) const 
        {
            if (index >= column_num_)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_column_name out of index");
            }

            return cell_array_[index].column_name;
        }

        const char* TableSchema::get_table_name() const
        {
            if (NULL == table_name_)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_table_name err, NULL");
            }
            
            return table_name_;
        }

        int TableSchema::get_column_id(const char* column_name) const
        {
            int column_id = common::ITERATOR_END;

            if (bsl::HASH_EXIST != column_hash_.get((char*) column_name, &column_id))
            {
                COM_WRITELOG(COMLOG_NOTICE, "in column_hash column_name not found, key[%s]",
                       column_name); 
            } 

            return column_id;
        }

        bool TableSchema::has_row_key() const
        {
            return has_row_key_;
        }

        void TableSchema::print()
        {
            for (int i = 0; i < 80; i++)
            {
                printf("-");
            }

            printf("\n");
            printf("table_name = %s\n", table_name_);
            printf("table_id = %d\n", table_id_);
            printf("column_num = %d\n", column_num_);
            printf("row_key:\n");
            row_key_.print();
            printf("foreign_key:\n");
            foreign_key_.print();

            printf("column:\n");
            for (int i = 0; i < column_num_; i++)
            {
                cell_array_[i].print();
                printf("\n");
            }
        }

        TableSchema::~TableSchema()
        {
            if (NULL != cell_array_)
            {
                delete[] cell_array_;
                cell_array_ = NULL;
            } 
        }
    }
}





















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
