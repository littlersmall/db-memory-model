/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file mysql_dao.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/01/27 14:59:45
 * @version $Revision$ 
 * @brief get data from mysql
 *  
 **/


#ifndef  __DATABASE_MANANER_H_
#define  __DATABASE_MANANER_H_

#include "mysql.h"
#include "sql_db.h"
#include <string>

namespace lbs
{
    namespace db_mm
    {
        class TableSchema;
        class RowIndex;
        class TableManager;
    }

    namespace dao
    {
        class MysqlDao : public SqlDb
        {
            std::string error_msg_;
            MYSQL* mysql_;
            bool is_init_;

            int do_sql(db_mm::TableSchema* table_schema, int& row_num);
            protected:

            int start_transaction();

            /**
             * @brief 
             *
             * @return  int 
             * @todo 
            **/
            int roll_back();

            /**
             * @brief 
             *
             * @return  int 
             * @todo 
            **/
            int commit();

            int query(void* data_base, const char* sql);

            public:
            MysqlDao(db_mm::TableManager* table_manager) : SqlDb::SqlDb(table_manager),
                is_init_(false)
            {}

            /**
             * @brief initialization, then get data from database
             *
             * @param [in] conf   : comcfg::Configure&
             * @param [in] data_path   : const char*
             * @return  int 
             * @todo 
            **/
            int init(const char* host = "cq02-map-pic-rdtest00.cq02.baidu.com", 
                    const char* user = "root", 
                    const char* passwd = "root",
                    //const char* db = "ttf_150",
                    const char* db = "ttf_bj_5h", 
                    unsigned int port = 3306);

            std::string save();

            ~MysqlDao();
        };
    }
}
















#endif  //__DATABASE_MANANER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
