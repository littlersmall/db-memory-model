/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file dao_data_path.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/14 11:49:36
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __DAO_DATA_PATH_H_
#define  __DAO_DATA_PATH_H_

#include <string>

namespace lbs
{
    namespace dao
    {
        static const char* DATA_PATH = "./data/";
        std::string get_file_name(int table_id, std::string data_path = DATA_PATH);
    }
}
















#endif  //__DAO_PATH_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
