/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file table_manager.cpp
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/21 17:16:18
 * @brief 
 *  
 **/

#include "table_manager.h"
#include "schema_manager.h"
#include "table_schema.h"
#include "file_manager.h"
#include "table_index.h"
#include "row_index.h"
#include "comm_def.h"
#include "mm_allocator.h"
#include "comm_log.h"
#include "Configure.h"
#include <string>

namespace lbs
{
    namespace db_mm
    {
        TableManager::TableManager(SchemaManager* schema_manager) : schema_manager_(schema_manager)
        {
            int table_num = schema_manager_->get_table_num();

            mm_allocator_ = new common::MMAllocator;
            table_index_array_ = NULL;

            char* buf = NULL;
            int len = sizeof(TableIndex) * table_num + sizeof(int);
            common::MM_FLAG flag = common::TABLE_INDEX;

            mm_allocator_->alloc_mm(len, buf, flag);
            table_index_array_ = new(buf) TableIndex[table_num];
        } 
        
       int TableManager::set_reverse_index()
        {
            int ret = 0;
            int table_num = schema_manager_->get_table_num();
            
            COM_WRITELOG(COMLOG_NOTICE, "set_reverse_index start");
 
            for (int i = 0; i < table_num; i++)
            {
                if (table_index_array_[i].has_data() && !table_index_array_[i].has_reverse_index())
                {
                    if (0 != (ret = table_index_array_[i].set_reverse_index()))
                    {
                        break;
                    }
                }
            }

            COM_WRITELOG(COMLOG_NOTICE, "set_reverse_index end");

            return ret;
        }

        int TableManager::init_data(const bool hash_index_on,
                const bool reverse_index_on)
        {
            int ret = 0;
            int table_num = schema_manager_->get_table_num();

            if (NULL != table_index_array_)
            {
                for (int i = 0; i < table_num; i++)
                {
                    TableSchema* table_schema = schema_manager_->get_table_schema(i);

                    if (!table_index_array_[i].get_data_path().empty() && !table_index_array_[i].has_data())
                    {
                        if (0 != (ret = table_index_array_[i].init(table_schema, this, hash_index_on)))
                        {
                            break;
                        }  
                    }
                }
            }

            //only hash_index_on && reverse_index_on then set_reverse_index
            if (0 == ret && hash_index_on && reverse_index_on)
            {
                COM_WRITELOG(COMLOG_NOTICE, "table_index init success");
                ret = set_reverse_index();
            }

            if (0 == ret)
            {
                COM_WRITELOG(COMLOG_NOTICE, "table_manager init success");
            }

            else
            {
                COM_WRITELOG(COMLOG_WARNING, "set_reverse_index ret[%d]", ret);
            }

            return ret;
        }

        TableIndex* TableManager::get_table_index(const int index)
        {
            int table_num = schema_manager_->get_table_num();

            if (table_num <= index)
            {
                COM_WRITELOG(COMLOG_WARNING, 
                        "get_talbe_index error, out of index [%d]", index);
            }

            return &table_index_array_[index];
        }

        void TableManager::insert_incom_key(char* key)
        {
            if (NULL != key)
            {
                incomplete_set_.set(key);
            }
        }

        bool TableManager::incomplete(char* key)
        {
            bool ret = false;

            if (NULL != key)
            {
                ret = (bsl::HASH_EXIST == incomplete_set_.get(key));
            }

            return ret;
        }

        void TableManager::print()
        {
            printf("data_file:\n");

            schema_manager_->print();

            for (int i = 0; i < 80; i++)
            {
                printf("=");
            }

            printf("\ntable index info:\n");
            
            int table_num = schema_manager_->get_table_num();

            for (int i = 0; i < table_num; i++)
            {
                for (int j = 0; j < 80; j++)
                {
                    printf("-");
                }

                printf("\n");
                table_index_array_[i].print();
            }
        }

        void TableManager::destroy()
        {
        	int table_num = schema_manager_->get_table_num();

			for (int i = 0; i < table_num; i++)
			{
				table_index_array_[i].~TableIndex();
			}

			mm_allocator_->delete_mm();
			delete mm_allocator_;
        }

        TableManager::~TableManager()
        {
        	destroy();
        }
    }
}





















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
