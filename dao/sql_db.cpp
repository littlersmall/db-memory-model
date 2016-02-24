/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file sql_db.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/21 16:02:36
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "sql_db.h"
#include "comm_log.h"
#include "row_index.h"
#include "table_manager.h"
#include "comm_compute.h"
#include <string>

namespace lbs
{
    namespace dao
    {
        int SqlDb::delete_data(db_mm::RowIndex* row_index)
        {
            int ret = 0;

            if (0 != (ret = delete_sql(row_index)))
            {
                COM_WRITELOG(COMLOG_WARNING, "sql error: sql[%s]", sql_); 
            } 

            else if (0 != (ret = query(data_base_, sql_)))
            {
                COM_WRITELOG(COMLOG_WARNING, "sql error: sql[%s]", sql_); 
            }

            else
            {
                COM_WRITELOG(COMLOG_NOTICE, "sql[%s]", sql_); 
            } 

            return ret;
        }

        int SqlDb::insert_data(db_mm::RowIndex* row_index)
        {
            int ret = 0;

            if (0 != (ret = insert_sql(row_index)))
            {
                COM_WRITELOG(COMLOG_WARNING, "sql error: sql[%s]", sql_); 
            } 

            else if (0 != (ret = query(data_base_, sql_)))
            {
                COM_WRITELOG(COMLOG_WARNING, "sql error: sql[%s]", sql_); 
            }

            else
            {
                COM_WRITELOG(COMLOG_NOTICE, "sql[%s]", sql_); 
            } 

            return ret;
        }

        int SqlDb::insert_sql(db_mm::RowIndex* row_index)
        {
            const char* table_name = row_index->get_table_name();
            int column_num = row_index->get_column_num();
            char column_names[common::SQL_LENGTH] = {};
            int name_pos = 0;
            char column_values[common::SQL_LENGTH] = {};
            int value_pos = 0;
            const char* data = row_index->get_row_data();
            int real_value_num = 0;

            //构造 column_name 字符串和 value字符串 
            for (int i = 0; i < column_num; i++)
            {
                const char* column_name = row_index->get_column_name(i);
                int offset = row_index->get_column_offset(i);
                db_mm::TYPE type = row_index->get_table_schema()->get_column_type(i);

                if ('\0' == *(data+offset))
                {
                    continue;
                }

                else
                {
                    real_value_num++;
                }

                if (real_value_num > 1)
                {
                    column_names[name_pos++] = ',';
                    column_values[value_pos++] = ',';
                }

                memcpy(column_names+name_pos, column_name, strlen(column_name));
                name_pos += strlen(column_name);

                if (db_mm::VCHAR_TYPE == type)
                {
                    column_values[value_pos++] = '\'';
                }

                std::string tmp_str = data + offset;
                std::string tmp = tmp_str;
                common::replace_char(tmp_str);
                //tmp_str.replace(tmp_str.begin(), tmp_str.end(), "'", "''");

                memcpy(column_values+value_pos, tmp_str.c_str(), tmp_str.size());
                value_pos += tmp_str.size();

                if (db_mm::VCHAR_TYPE == type)
                {
                    column_values[value_pos++] = '\'';
                }
            }

            snprintf(sql_, sizeof(sql_), "insert into %s(%s) values(%s)", table_name,
                    column_names, column_values);

            return 0;
        }

        int SqlDb::delete_sql(db_mm::RowIndex* row_index)
        {
            const char* format = "delete from %s where %s=\'%s\'";

            snprintf(sql_, sizeof(sql_), format,
                    row_index->get_table_name(), row_index->get_column_name(0),
                    row_index->get_row_data());

            return 0;
        }

        int SqlDb::select_sql(db_mm::TableSchema* table_schema)
        {
            const char* format = "select %s from %s";
            char column_name_array[common::SQL_LENGTH] = {};
            const char* column_name;
            int cur_offset = 0;
            int column_num = table_schema->get_column_num();

            for (int i = 0; i < column_num; i++)
            {
                int len = 0;
                column_name = table_schema->get_column_name(i);

                if (0 == strcmp(column_name, "geometry"))
                {
                    static char* str_geo = "st_astext(geometry)";
                
                    column_name = str_geo;
                }
                
                len = strlen(column_name);

                memcpy(column_name_array + cur_offset, column_name, len);
                cur_offset += len;

                if (column_num - 1 != i)
                {
                    column_name_array[cur_offset++] = ',';
                }

                else
                {
                    column_name_array[cur_offset] = '\0';
                }
            }

            snprintf(sql_, sizeof(sql_), format, 
                    column_name_array, table_schema->get_table_name());
              
            return 0;
        }
    }
}



















/* vim: set ts=4 sw=4 sts=4 tw=100 */
