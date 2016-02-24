/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file schema_manager.cpp
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/20 11:29:41
 * @brief 
 *  
 **/

#include "schema_manager.h"
#include "Configure.h"
#include "table_schema.h"
#include "file_manager.h"
#include "comm_def.h"
#include "mm_allocator.h"
#include "comm_log.h"

namespace lbs
{
    namespace db_mm
    {
        SchemaManager::SchemaManager()
            : table_schema_array_(NULL),
            file_manager_(NULL)
        {
            file_manager_ = new TableConfFileManager();
        }        

        int SchemaManager::init(const char* path, int table_num)
        {
            const char* file_name = NULL;
            int test_table_num = 0;
            int ret = 0;

            table_num_ = table_num;

            table_schema_array_ = new TableSchema[table_num];
            ret = file_manager_->init(path);

            while (0 == file_manager_->get_next(file_name) && 0 == ret)
            {
                //config 
                comcfg::Configure conf; 
 
                //printf("%s\n", file_name); 

                ret = conf.load("", file_name);

                //printf("ret = %d\n", ret);

                if (0 == ret)
                {
                    ret = table_schema_array_[test_table_num++].init(conf);
                }

                else
                {
                    COM_WRITELOG(COMLOG_WARNING, "conf file load error, ret[%d]", ret);
                }

                //printf("%s\n", conf["table"]["table_name"].to_cstr());

            }

            //reset 
            file_manager_->reset();

            if (0 != ret)
            {
                COM_WRITELOG(COMLOG_WARNING, "table_schema init error, ret[%d]", ret);
            }

            else if (test_table_num != table_num_)
            {
                COM_WRITELOG(COMLOG_WARNING, 
                        "table num error, table_num[%d], real num[%d]", 
                        table_num, test_table_num);

                ret = common::TABLE_NUM_ERROR;
            }

            else
            {
                ret = set_foreign_key_ref();

                if (0 == ret)
                {
                    COM_WRITELOG(COMLOG_NOTICE, "schema_manager init success"); 
                }
            }

            return ret;
        }

        int SchemaManager::set_foreign_key_ref()
        {
            int ret = 0;

            for (int i = 0; i < table_num_; i++)
            {
                int cur_table_id = table_schema_array_[i].get_table_id();
                int foreign_key_num = table_schema_array_[i].get_foreign_key_num();

                for (int j = 0; j < foreign_key_num; j++)
                {
                    int ref_table_id = table_schema_array_[i].get_foreign_table_id(j);
                    int cur_column_id = table_schema_array_[i].get_foreign_key_id(j);
                            //build reverse index
                    table_schema_array_[ref_table_id].add_reverse_index(cur_table_id,
                                    cur_column_id);
                }
            }

            return ret;
        }

        TableSchema* SchemaManager::get_table_schema(const int index) const
        {
            if (index >= table_num_)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_schema err, out of index [%d]", index);
            } 

            else if (NULL == table_schema_array_)
            {
                COM_WRITELOG(COMLOG_WARNING, "get_schema err, table_schema_array_ is NULL");
            }

            return table_schema_array_ + index;
        }


        int SchemaManager::get_table_num() const
        {
            return table_num_;
        }

        void SchemaManager::print()
        {
            printf("configure_file :\n");
            file_manager_->print();
            printf("table_num = %d\n", table_num_); 

            for (int i = 0; i < table_num_; i++)
            {
                table_schema_array_[i].print(); 
            }
        }

        SchemaManager::~SchemaManager()
        {
            if (NULL != file_manager_)
            {
                delete file_manager_; 
                file_manager_ = NULL;
            } 

            if (NULL != table_schema_array_)
            {
                delete[] table_schema_array_;
                table_schema_array_ = NULL;
            }
        }
    }
}

















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
