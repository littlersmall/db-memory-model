/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file json_file_dao.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/10 15:28:53
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __JSON_FILE_DAO_H_
#define  __JSON_FILE_DAO_H_

#include "json_dao.h"
#include <string>
#include <vector>

namespace lbs
{
    namespace db_mm
    {
        class TableManager;
    }

    namespace dao
    {
        class JsonFileDao
        {
            JsonDao json_dao_;
            bool is_init_;

            public:
            
            JsonFileDao(db_mm::TableManager* table_manager) : json_dao_(table_manager),
                is_init_(false) 
            {}
            int init(std::string file_name);

            int save(std::string file_name);
        };
    }
}
















#endif  //__JSON_FILE_DAO_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
