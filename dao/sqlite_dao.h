/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file sqlite_dao.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/22 14:10:11
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __SQLITE_DAO_H_
#define  __SQLITE_DAO_H_

#include "sqlite3.h"
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
        class SqliteDao : public SqlDb
        {
            std::string error_msg_;
            sqlite3* sqlite_;
            bool is_init_;

            int get_data(db_mm::TableSchema* table_schema, int& row_num); 

            protected:

            int start_transaction();
            int roll_back();
            int commit();
            int query(void* data_base, const char* sql);

            public:

            SqliteDao(db_mm::TableManager* table_manager) : SqlDb::SqlDb(table_manager),
                is_init_(false)
            {}

            int init(const char* data_file = "./data/poi.db3");
            std::string save();
            ~SqliteDao();
        };
    }
}
















#endif  //__SQLITE_DAO_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
