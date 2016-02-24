/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file json_dao.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/09 15:48:53
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __JSON_DAO_H_
#define  __JSON_DAO_H_

#include <vector>
#include <string>
#include <json/json.h>

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
        class JsonDao
        {
            db_mm::TableManager* table_manager_;
            int write_json_to_file(std::string json, FILE* file, const db_mm::TableSchema* table_schema);
            int write_json_to_mem(std::string json,
                    char* mem, int& offset, const db_mm::TableSchema* table_schema);
            int parse_row_to_json(db_mm::RowIndex* row_index, std::string& json);

            bool is_init_;

            public:
            
            JsonDao(db_mm::TableManager* table_manager) : table_manager_(table_manager),
                is_init_(false) 
            {}

            int init(std::vector<std::string>& json_vec); 
            int save(std::vector<std::string>& json_vec);

            int init(Json::Value& json_value);
            int save(Json::Value& json_value);

        };
    }
}
















#endif  //__JSON_DAO_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
