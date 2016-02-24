/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file file_manager.cpp
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/16 11:06:13
 * @brief 
 *  
 **/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#include "file_manager.h"
#include "comm_def.h"
#include "mm_allocator.h"
#include "comm_log.h"
#include <algorithm>

namespace lbs
{
    namespace db_mm
    {
        FileManager::FileManager() : index_(0)
        {}

        int FileManager::init(const char* path)
        {
            int ret = 0;
            ls_dir(path);

            if (0 == file_vec_.size())
            {
                COM_WRITELOG(COMLOG_WARNING, "file_manager open failed");
                ret = common::SIZE_ZERO; 
            }       
            
            else
            {
                std::sort(file_vec_.begin(), file_vec_.end());
            }

            return ret;
        }

        void FileManager::ls_dir(const char* path)
        {
            DIR* dir = NULL;
            struct dirent* ent = NULL;
            
            if (common::MAX_FILE_NUM <= (int) file_vec_.size() 
                    || NULL == (dir = opendir(path)))
            {
                return; 
            }

            char child_path[common::PATH_LENGTH] = {'\0'};
            
            while (NULL != (ent = readdir(dir)))
            {
                //if directory, recursion
                if (ent->d_type & DT_DIR)
                { 
                    if (0 == strcmp(ent->d_name, ".") ||
                        0 == strcmp(ent->d_name, ".."))
                    {
                        continue;
                    }

                    snprintf(child_path, sizeof(child_path),
                        "%s/%s", path, ent->d_name);

                    //printf("%s\n", child_path);
                    ls_dir(child_path);
                }

                else
                {
                    file_vec_.push_back(std::string(path) 
                            + std::string("/") 
                            + std::string(ent->d_name));

                    if (common::MAX_FILE_NUM <= (int) file_vec_.size())
                    {
                        return; 
                    }

                }
            }

            if (NULL != dir)
            {
                closedir(dir);
            }
        }

        int FileManager::get_next(const char*& file_name)
        {
            int ret = 0;

            if (index_ < file_vec_.size()) 
            {
                file_name = file_vec_[index_++].c_str();
            }

            else
            {
                ret = common::ITERATOR_END;
                index_ = 0;
            }

            return ret;
        }

        void FileManager::reset()
        {
            index_ = 0;
        }

        void FileManager::print()
        {
            const char* file_name = NULL;

            while (0 == get_next(file_name))
            {
                printf("%s\n", file_name);
            }     

            reset();
        }

        int TableConfFileManager::get_next(const char*& file_name)
        {
            int ret = 0;

            while (0 == (ret = FileManager::get_next(file_name)))
            {
                int len = strlen(file_name);
                const char* FILTER = "table";
                int fil_len = strlen(FILTER);

                if (len > fil_len && 0 == memcmp(file_name + len - fil_len, FILTER, fil_len))
                {
                    break;
                }
            }

            return ret;
        }

        int MysqlConfFileManager::get_next(const char*& file_name)
        {
            int ret = 0;

            while (0 == (ret = FileManager::get_next(file_name)))
            {
                int len = strlen(file_name);
                const char* FILTER = "mysql";
                int fil_len = strlen(FILTER);

                if (len > fil_len && 0 == memcmp(file_name + len - fil_len, FILTER, fil_len))
                {
                    break;
                }
            }

            return ret;
        }

        DataFileManager::DataFileManager()
        {
            char* buf = NULL;
            int len = sizeof(FileManager);
            common::MM_FLAG flag = common::FILE_MANAGER;

            common::MMAllocator::get_allocator()->alloc_mm(len, buf, flag);
            file_manager_ = new(buf) FileManager();
        }

        int DataFileManager::init(const char* path)
        {
            return file_manager_->init(path);
        }

        int DataFileManager::get_next(const char*& file_name, int64_t& size)
        {
            int ret = 0;
            
            while (0 == (ret = file_manager_->get_next(file_name)))
            {
                int len = strlen(file_name);
                const char* FILTER = "data";
                int fil_len = strlen(FILTER);

                if (len > fil_len && 0 == memcmp(file_name + len - fil_len, FILTER, fil_len))
                {
                    struct stat file_stat;              
                    stat(file_name, &file_stat);

                    size = file_stat.st_size;

                    break;
                } 
            }
            
            return ret;
        }

        void DataFileManager::reset()
        {
            file_manager_->reset();
        }


        void DataFileManager::print()
        {
            const char* file_name = NULL;
            int64_t size = 0;

            while (0 == get_next(file_name, size))
            {
                printf("%s, size=%ld\n", file_name, size);
            }

            reset();
        }

        DataFileManager::~DataFileManager()
        {}
    }
}




















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
