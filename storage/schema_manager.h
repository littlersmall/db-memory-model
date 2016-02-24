/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file schema_manager.h
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/15 11:53:34
 * @brief 
 *  
 **/

#ifndef  __SCHEMA_MANAGER_H_
#define  __SCHEMA_MANAGER_H_

#include <vector>
#include <string>

namespace lbs
{
    namespace db_mm
    {
        class TableSchema;
        class FileManager;

        class SchemaManager
        {
            int table_num_;
            //table_schema array
            TableSchema* table_schema_array_;
            //file_manager, manage table conf file
            FileManager* file_manager_;
            //set foreign_key's reference
            int set_foreign_key_ref();

            public:
            /**
             * @brief
             *
             * @param [in] table_num   : const int
            **/
            SchemaManager();

            /**
             * @brief
             *
             * @param [in] table_name_vec   : std::vector<std::string>&
             * @param [in] path   : const char*
             * @return  int 
             * @todo 
            **/
            int init(const char* path, int table_num = 45);

            /**
             * @brief 
             *
             * @param [in] index   : const int
             * @param [out] table_schema   : TableSchema* &
             * @return  int 
             * @todo 
            **/
            TableSchema* get_table_schema(const int index) const;

            /**
             * @brief
             *
             * @param [out] table_num   : int&
             * @return  int 
             * @todo 
            **/
            int get_table_num() const;

            /**
             * @brief for debug only
             *
             * @return  void 
             * @todo 
            **/
            void print();
            ~SchemaManager();
       };
    }
}














#endif  //__SCHEMA_MANAGER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
