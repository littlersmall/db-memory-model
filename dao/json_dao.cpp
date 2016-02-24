/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

/**
 * @file json_dao.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/09 16:17:48
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "json_dao.h"
#include <json/json.h>
#include <vector>
#include <string>
#include "schema_manager.h"
#include "table_manager.h"
#include "table_schema.h"
#include "table_index.h"
#include "row_index.h"
//#include "parser.h"
#include "comm_compute.h"
#include "comm_log.h"
#include "dao_data_path.h"
#include "mm_allocator.h"
#include "row_instance.h"

namespace lbs
{
    namespace dao
    {
        int JsonDao::write_json_to_file(std::string json, FILE* file, const db_mm::TableSchema* table_schema)
        {
            int ret = 0;

            Json::Reader reader;
            Json::Value value;

            if (0 == reader.parse(json, value, false))
            {
                return -1;
            }

            if (!value.isObject())
            {
                return -1;
            }

            int column_num = table_schema->get_column_num();

            for (int i = 0; i < column_num; i++)
            {
                std::string column_value;
                const char* column_name = table_schema->get_column_name(i);

                switch (value[column_name].type())
                {
                    case Json::intValue:
                        common::parse_value(value[column_name].asInt(), column_value);
                        break;
                    case Json::uintValue:
                        common::parse_value(value[column_name].asUInt(), column_value);
                        break;
                    case Json::stringValue:
                        common::parse_value(value[column_name].asString(), column_value);
                        break;
                    case Json::realValue:
                        common::parse_value(value[column_name].asDouble(), column_value);
                        break;
                    default:
                        ;
                }

                if (column_value.empty())
                {
                    fwrite("\0", 1, 1, file);
                }

                else
                {
                    fwrite(column_value.c_str(), column_value.size() + 1, 1, file);
                }
            }

            fwrite("\n", 1, 1, file);

            return ret;
        }

        int JsonDao::write_json_to_mem(std::string json, char* mem, 
                int& offset, const db_mm::TableSchema* table_schema)
        {
            int ret = 0;
            Json::Reader reader;
            Json::Value value;

            offset = 0;

            if (0 == reader.parse(json, value, false))
            {
                return -1;
            }

            if (!value.isObject())
            {
                return -1;
            }

            int column_num = table_schema->get_column_num();

            for (int i = 0; i < column_num; i++)
            {
                std::string column_value;
                const char* column_name = table_schema->get_column_name(i);

                switch (value[column_name].type())
                {
                    case Json::intValue:
                        common::parse_value(value[column_name].asInt(), column_value);
                        break;
                    case Json::uintValue:
                        common::parse_value(value[column_name].asUInt(), column_value);
                        break;
                    case Json::stringValue:
                        common::parse_value(value[column_name].asString(), column_value);
                        break;
                    case Json::realValue:
                        common::parse_value(value[column_name].asDouble(), column_value);
                        break;
                    default:
                        ;
                }

                if (column_value.empty())
                {
                    mem[offset++] = '\0';
                }

                else
                {
                    memcpy(mem + offset, column_value.c_str(), column_value.size() + 1);
                    offset += column_value.size() + 1;
                }
            }

            mem[offset++] = '\n';

            return ret;
        }

        int JsonDao::parse_row_to_json(db_mm::RowIndex* row_index, std::string& json)
        {
            const char* data = row_index->get_row_data();
            Json::Value json_value;
            const db_mm::TableSchema* table_schema = row_index->get_table_schema();
            int column_num = table_schema->get_column_num();

            for (int i = 0; i < column_num; i++)
            {
                db_mm::TYPE data_type = table_schema->get_column_type(i);
                const char* column_name = table_schema->get_column_name(i);

                int int_value = 0;
                double double_value = 0.0;
                std::string str_value;
                int offset = row_index->get_column_offset(i);

                if (strlen(data + offset) == 0)
                {
                    json_value[column_name] = "";
                    continue;
                }

                switch (data_type)
                {
                    case db_mm::INT_TYPE:
                        common::parse_str(data + offset, int_value);
                        json_value[column_name] = int_value;
                        break;
                    case db_mm::DOUBLE_TYPE:
                        common::parse_str(data + offset, double_value);
                        json_value[column_name] = double_value;
                        break;

                    case db_mm::VCHAR_TYPE:
                    default:
                        json_value[column_name] = data + offset;
                        break;
                }
            }

            Json::FastWriter writer;
            json = writer.write(json_value);

            return 0;
        }

        int JsonDao::init(std::vector<std::string>& json_vec)
        {
            int ret = 0;

            if (!is_init_)
            {
                db_mm::TableSchema* table_schema = table_manager_->get_schema_manager()->get_table_schema(sdk::POI_TABLE_ID);
                db_mm::TableIndex* table_index = table_manager_->get_table_index(sdk::POI_TABLE_ID);
                int table_id = table_schema->get_table_id();

                if (json_vec.size() > 1024)
                {
                    std::string file_name = get_file_name(table_id);
                    FILE* file = fopen(file_name.c_str(), "w+");

                    if (NULL == file)
                    {
                        COM_WRITELOG(COMLOG_WARNING, "JsonDao init, open file erro, file_name = %s", file_name.c_str());
                        ret = -1;
                    }

                    else
                    {
                        for (unsigned int i = 0; i < json_vec.size(); i++)
                        {
                            if (0 != (ret = write_json_to_file(json_vec[i], file, table_schema)))
                            {
                                COM_WRITELOG(COMLOG_WARNING, "write_json_to_file failed, json index is %u", i);
                                break;
                            }
                        }

                        if (0 == ret)
                        {
                            table_index->set_row_num((int) json_vec.size());
                            table_index->set_data_path(file_name);
                        }

                        if (NULL != file)
                        {
                            fclose(file);
                        }
                    }
                }

                else
                {
                    char* data = NULL;
                    //请求10M内存
                    table_manager_->get_allocator()->alloc_mm(10 * 1024 * 1024, data);

                    int offset = 0;

                    for (unsigned int i = 0; i < json_vec.size(); i++)
                    {
                        int tmp_offset = 0;

                        if (0 != (ret = write_json_to_mem(json_vec[i], data + offset, tmp_offset, table_schema)))
                        {
                            COM_WRITELOG(COMLOG_WARNING, "write_json_to_mem failed, json index is %u", i);
                            break;
                        }

                        offset += tmp_offset;
                    }

                    if (0 == ret)
                    {
                        table_index->set_row_num((int) json_vec.size());
                        table_index->set_data_path("memory");
                        table_index->set_data(data);
                    }
                }

                is_init_ = true;
            }

            return ret;
        }

        int JsonDao::init(Json::Value& json_value)
        {
            int ret = 0;

            if (!is_init_)
            {
                std::vector<std::string> json_vec;

                for (unsigned int i = 0; i < json_value.size(); i++)
                {
                    json_vec.push_back(json_value[i].toStyledString());
                }

                ret = init(json_vec);

                is_init_ = true;
            }

            return ret;
        }

        int JsonDao::save(std::vector<std::string>& json_vec)
        {
            db_mm::RowIndex* row_index = NULL;
            db_mm::TableIndex* table_index = table_manager_->get_table_index(sdk::POI_TABLE_ID);
            db_mm::TableIndex::RowIndexIter row_iter = table_index->iter();

            while (0 == row_iter.get_next(row_index))
            {
                if (db_mm::DELETE_ROW != row_index->get_row_stat())
                {
                    std::string json;
                    parse_row_to_json(row_index, json);
                    json_vec.push_back(json);
                }
            }

            return 0;
        }

        int JsonDao::save(Json::Value& json_value)
        {
            int ret = 0;
            std::vector<std::string> json_vec;

            save(json_vec);

            for (unsigned int i = 0; i < json_vec.size(); i++)
            {
                Json::Reader reader;
                Json::Value value;

                if (0 == reader.parse(json_vec[i], value, false))
                {
                    ret = -1;
                }

                json_value.append(value);
            }

            return 0;
        }
    }
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
