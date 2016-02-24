/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file file_manager.h
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/15 15:59:58
 * @brief 
 *  
 **/




#ifndef  __FILE_MANAGER_H_
#define  __FILE_MANAGER_H_

#include <string>
#include <vector>
#include "comm_def.h"

namespace lbs
{
    namespace db_mm
    {
        class FileManager
        {
            //iteration variable
            unsigned int index_;
            std::vector<std::string> file_vec_;

            /**
             * @brief get file name with recusion  (path + file_name)
             *
             * @param [out] path   : const char*
             * @return  void 
             * @todo 
            **/
            void ls_dir(const char* path);

            public:

            /**
             * @brief set index_ = 0
             *
            **/
            FileManager();

            /**
             * @brief
             *
             * @param [in] path   : const char*
             * @return  int, 0 for success
             * @todo 
            **/
            int init(const char* path);

            /**
             * @brief like an iterator 
             *
             * @param [out] file_name   : const char* &
             * @return  int, -1 for end
             * @todo 
            **/
            virtual int get_next(const char*& file_name);

            /**
             * @brief reset index_ to 0
             *
             * @return  int 
             * @todo 
            **/
            void reset();

            /**
             * @brief for debug only
             *
             * @return  void 
             * @todo 
            **/
            void print();

            virtual ~FileManager() 
            {}
        };

        class TableConfFileManager : public FileManager
        {

            public:

            /**
             * @brief add filter on FileManager::get_next
             * file name end with "table"
             *
             * @param [in/out] file_name   : const char* &
             * @return  int 
             * @todo 
            **/
            int get_next(const char*& file_name);

            ~TableConfFileManager()
            {}
        };

        class MysqlConfFileManager : public FileManager
        {
            public:

            /**
             * @brief 
             *
             * @param [in/out] file_name   : const char* &
             * @return  int 
             * @todo 
            **/
            int get_next(const char*& file_name);

            ~MysqlConfFileManager()
            {}
        };

        class DataFileManager
        {
            FileManager* file_manager_;

            public: 
            DataFileManager();

            /**
             * @brief
             *
             * @param [in] path   : const char*
             * @return  int 
             * @todo 
            **/
            int init(const char* path);

            /**
             * @brief ·µ»ØFileManager::get_next
             *
             * @param [in/out] file_name   : const char* &
             * @return  int 
             * @todo 
            **/
            int get_next(const char*& file_name);

            /**
             * @brief add filter on FileManager::get_next
             * file name end with "data"
             * and return file size
             *
             * @param [out] file_name   : const char* &
             * @param [out] size   : int64_t& file size
             * @return  int 
             * @todo 
            **/
            int get_next(const char*& file_name, int64_t& size);

            /**
             * @brief reset index_ to 0
             *
             * @return  void 
             * @todo 
            **/
            void reset();

            /**
             * @brief for debug only
             *
             * @return  void 
             * @todo 
            **/
            void print();
            ~DataFileManager();
        };
    }
}














#endif  //__FILE_MANAGER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
