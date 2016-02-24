/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file pg_dao.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/06/11 16:35:36
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "comm_log.h"
#include "comm_def.h"
#include "pg_dao.h"
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
        int PgDao::query(void* data_base, const char* sql)
        {
            int ret = 0;
            PGresult* res = PQexec((PGconn*) data_base, sql);        

            if (PGRES_COMMAND_OK != PQresultStatus(res))
            {
                ret = -1;	
            }

            return ret;
        }

        int PgDao::do_sql(db_mm::TableSchema* table_schema, int& row_num)
        {
            int ret = 0;
            int table_id = table_schema->get_table_id();
            std::string file_name = get_file_name(table_id);
            db_mm::TableIndex* table_index = table_manager_->get_table_index(table_id);

            row_num = 0;

            table_index->set_data_path(file_name);

            FILE* file = fopen(file_name.c_str(), "w+");
            PGresult* res = PQexec(pg_, sql_);

            COM_WRITELOG(COMLOG_NOTICE, "in pg_dao, sql is : %s", sql_);

            do
            {
                if (PGRES_TUPLES_OK != PQresultStatus(res))
                {
                    COM_WRITELOG(COMLOG_NOTICE, "in pg_dao, error is : %s", 
                            PQresultErrorMessage(res));

                    ret = -1;	
                    break;
                }
        
                int fields_num = PQnfields(res);

                for (int i = 0; i < PQntuples(res); i++)
                {
                    row_num++;

                    for (int j = 0; j < fields_num; j++)
                    {
                        char* column = PQgetvalue(res, i, j);

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
            }while (0);

            PQclear(res); 

            if (NULL != file)
            {
                fclose(file);
            }
            
            return ret;
        }

        int PgDao::init(const char* host, const char* user, const char* passwd,
                const char* db, unsigned int port)
        {
            const char* info = 
                "host=%s \
                dbname=%s \
                port=%d \
                user=%s \
                password=%s";
            int ret = 0;

            if (!is_init_)
            {
                char conn_info[1024] = {}; 

                snprintf(conn_info, sizeof(conn_info), info, host, db, port, user, passwd);

                pg_ = PQconnectdb(conn_info);
                data_base_ = pg_;

                if (CONNECTION_OK != PQstatus(pg_)) 
                {
                    ret = -1;
                    COM_WRITELOG(COMLOG_WARNING, 
                        "in pg_dao, init err,ret[%d]", ret);
                }

                int row_num = 0;                    
                db_mm::SchemaManager* schema_manager = table_manager_->get_schema_manager();
                int table_num = schema_manager->get_table_num();

                for (int i = 0; i < table_num; i++)
                {
                    db_mm::TableIndex* table_index = table_manager_->get_table_index(i);
                    db_mm::TableSchema* table_schema = schema_manager->get_table_schema(i);

                    if (0 != (ret = select_sql(table_schema)))
                    {
                        COM_WRITELOG(COMLOG_WARNING, 
                            "in mysql_dao, yield_sql err,ret[%d]", ret);
                        break;
                    }
                
                    else if (0 != (ret = do_sql(table_schema, row_num)))
                    {
                        COM_WRITELOG(COMLOG_WARNING, 
                            "in mysql_dao, do_sql sql[%s] err,ret[%d]", sql_, ret);

                        ret = 0;
                        //表不存在
                        continue;
                    }

                    else
                    {
                        table_index->set_row_num(row_num);
                    }
                }

                is_init_ = true;
            }

            return ret;
        }

        int PgDao::start_transaction()
        {
            return query(pg_, "BEGIN");
        }

        int PgDao::roll_back()
        {
            return query(pg_, "ROLLBACK"); 
        }    

        int PgDao::commit()
        {
            return query(pg_, "END");
        }

        std::string PgDao::save()
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
                error_msg_ = "save data to pg error";
                roll_back(); 
            }
            
            return error_msg_;
        }

        PgDao::~PgDao()
        {
            PQfinish(pg_);
        }
    } 
}

















/* vim: set ts=4 sw=4 sts=4 tw=100 */
