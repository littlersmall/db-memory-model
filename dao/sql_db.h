/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file sql_db.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/09 11:52:50
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __SQL_DB_H_
#define  __SQL_DB_H_

#include "comm_def.h"

namespace lbs
{
    namespace db_mm
    {
        class TableManager;
        class TableSchema;
        class RowIndex;
    }

    namespace dao
    {
        class SqlDb
        {
            protected:
            void* data_base_;
            char sql_[common::SQL_LENGTH];
            db_mm::TableManager* table_manager_;

            protected:
            int delete_data(db_mm::RowIndex* row_index);
            int insert_data(db_mm::RowIndex* row_index);

            //sql
            int insert_sql(db_mm::RowIndex* row_index);
            int delete_sql(db_mm::RowIndex* row_index);
            int select_sql(db_mm::TableSchema* table_schema);

            virtual int start_transaction() = 0;
            virtual int commit() = 0;
            virtual int roll_back() = 0;
            virtual int query(void* data_base, const char* sql) = 0;

            public:
            SqlDb(db_mm::TableManager* table_manager) : table_manager_(table_manager)
            {}

            virtual ~SqlDb() {}
        };
    }
}
















#endif  //__SQL_DB_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
