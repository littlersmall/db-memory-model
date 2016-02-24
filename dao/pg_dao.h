/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file pg_dao.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/06/11 16:22:58
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __PG_DAO_H_
#define  __PG_DAO_H_

#include "sql_db.h"
#include <string>
#include <libpq-fe.h>

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
        class PgDao : public SqlDb
        {
            std::string error_msg_; 
            PGconn* pg_;
            bool is_init_;
            
            int do_sql(db_mm::TableSchema* table_schema, int& row_num);

            protected:

            int start_transaction();

            int roll_back();

            int commit();

            int query(void* data_base, const char* sql);

            public:

            PgDao(db_mm::TableManager* table_manager) : SqlDb::SqlDb(table_manager),
                is_init_(false)
            {}

            int init(const char* host = "cq02-map-pic-rdtest00.cq02.baidu.com",
                    const char* user = "pg",
                    const char* passwd = "",
                    const char* db = "datacheck_beijing",
                    unsigned int port = 8900);

            std::string save();

            ~PgDao();
        };
    }
}
















#endif  //__PG_DAO_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
