/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file data_manager.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/01/27 15:30:43
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "comm_log.h"
#include "comm_def.h"
#include "mysql_dao.h"
#include "schema_manager.h"
#include "table_schema.h"
#include "table_manager.h"
#include "mm_allocator.h"
#include "table_index.h"
#include "row_index.h"
#include "dao_data_path.h"

namespace lbs
{
    namespace dao
    {
        int MysqlDao::query(void* data_base, const char* sql)
        {
            return mysql_query((MYSQL*)data_base, sql);
        }        

        int MysqlDao::do_sql(db_mm::TableSchema* table_schema, int& row_num)
        {
            int ret = 0;
            int table_id = table_schema->get_table_id();
            std::string file_name = get_file_name(table_id);
            db_mm::TableIndex* table_index = table_manager_->get_table_index(table_id);

            row_num = 0;

            table_index->set_data_path(file_name);

            FILE* file = fopen(file_name.c_str(), "w+");

            do
            {
                if (0 != (ret = query(mysql_, sql_)))
                {
                    break;
                } 

                if (mysql_field_count(mysql_) > 0)
                {
                    MYSQL_RES *res;
                    MYSQL_ROW row = NULL;
                    int fields_num;
            
                    if (NULL == (res = mysql_store_result(mysql_)))
                    {
                        break;
                    }

                    fields_num = mysql_num_fields(res);

                    while (row = mysql_fetch_row(res))
                    {
                        row_num++;

                        int index = 0;

                        //printf("row_num = %d\n", row_num);

                        while (index < fields_num)
                        {
                            char* column = row[index++];
                            //all is string
                            //other type
                            //to be continue
                            if (NULL != column)
                            {
                                fwrite(column, strlen(column) + 1, 1, file);
                            }

                            else
                            {
                                fwrite("\0", 1, 1, file);
                            }
                        }
                            
                        fwrite("\n", 1, 1, file);
                    }

                    //release memory
                    mysql_free_result(res);
                }
            }while (0);

            if (NULL != file)
            {
                fclose(file);
            }
            
            return ret;
        }

        //1. connect mysql
        //2. yield sql string
        //3. execute sql then write the result to file
        //4. set row_num
        int MysqlDao::init(const char* host, const char* user, const char* passwd,
                const char* db, unsigned int port)
        {
            int ret = 0;

            if (!is_init_)
            {
                //1
                mysql_ = mysql_init(NULL);    
                data_base_ = mysql_;

                if (NULL != mysql_real_connect(mysql_, host, user, passwd, db, port, NULL, 0))
                {
                    mysql_set_character_set(mysql_, "utf8");

                    int row_num = 0;
                    db_mm::SchemaManager* schema_manager = table_manager_->get_schema_manager();
                    int table_num = schema_manager->get_table_num();

                    for (int i = 0; i < table_num; i++)
                    {
                        db_mm::TableIndex* table_index = table_manager_->get_table_index(i);
                        db_mm::TableSchema* table_schema = schema_manager->get_table_schema(i);

                        //2
                        if (0 != (ret = select_sql(table_schema)))
                        {
                            COM_WRITELOG(COMLOG_WARNING, 
                                "in mysql_dao, yield_sql err,ret[%d]", ret);
                            break;
                        }
                    
                        //3
                        else if (0 != (ret = do_sql(table_schema, row_num)))
                        {
                            COM_WRITELOG(COMLOG_WARNING, 
                                "in mysql_dao, do_sql sql[%s] err,ret[%d]", sql_, ret);
                            break;
                        }

                        //4
                        else
                        {
                            table_index->set_row_num(row_num);
                        }
                    }    
                }

                else
                {
                    COM_WRITELOG(COMLOG_WARNING, "mysql connect failed"); 
                    ret = common::MYSQL_CONNECT_FAILED;
                }

                if (0 == ret)
                {
                    COM_WRITELOG(COMLOG_NOTICE, "data_manager init success"); 
                }

                is_init_ = true;
            }
            
            return ret;
        }

        int MysqlDao::start_transaction()
        {
            return query(mysql_, "START TRANSACTION");
        }

        int MysqlDao::roll_back()
        {
            return query(mysql_, "ROLLBACK"); 
        }

        int MysqlDao::commit()
        {
            return query(mysql_, "COMMIT"); 
        }

        std::string MysqlDao::save()
        {
            int ret = 0;

            start_transaction();
             
            int table_num = table_manager_->get_schema_manager()->get_table_num();

            for (int i = 0; i < table_num; i++)
            {
                db_mm::TableIndex* table_index = table_manager_->get_table_index(i);
                db_mm::TableIndex::RowIndexIter row_iter = table_index->iter();

                if (!table_index->has_data())
                {
                    continue;
                }

                db_mm::RowIndex* row_index = NULL;

                while (0 == row_iter.get_next(row_index) && 0 == ret)
                {
                    if (db_mm::DELETE_ROW == row_index->get_row_stat())
                    {
                        ret = delete_data(row_index);
                    }

                    else if (db_mm::UPDATE_ROW == row_index->get_row_stat())
                    {
                        if (0 == ret && 0 == (ret = delete_data(row_index)))
                        {
                            ret = insert_data(row_index);
                        }
                    }
                }
            }

            if (0 == ret)
            {
                commit();
            }

            else
            {
                error_msg_ = mysql_error(mysql_);
                roll_back(); 
            }
            
            return error_msg_;
        }

        MysqlDao::~MysqlDao()
        {
            if (NULL != mysql_) 
            {
                mysql_close(mysql_);
                mysql_ = NULL;
                mysql_library_end();
            }
        }
    }
}




















/* vim: set ts=4 sw=4 sts=4 tw=100 */
