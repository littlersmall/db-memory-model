/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file dao_data_path.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/14 11:53:28
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "dao_data_path.h"
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

namespace lbs
{
    namespace dao
    {
        //建立pid文件夹
        //构建文件名
        std::string get_file_name(int table_id, std::string data_path)
        {
            char table_id_str[128] = {};
            char pid_str[128] = {};
            int pid = getpid();

            snprintf(table_id_str, sizeof(table_id_str), "%02d.data", table_id);  
            snprintf(pid_str, sizeof(pid_str), "%d", pid);
            DIR * dir;
            dir = opendir((data_path + "/" + pid_str).c_str());
            if (NULL == dir)
            {
                mkdir(data_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                mkdir((data_path + "/" + pid_str).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            }
            else
            {
                closedir(dir);
            }

            return data_path + "/" + pid_str + "/" + table_id_str;
        }
    }
}



















/* vim: set ts=4 sw=4 sts=4 tw=100 */
