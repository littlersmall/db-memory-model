/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file conf_manager.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/03/13 13:53:22
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __CONF_MANAGER_H_
#define  __CONF_MANAGER_H_

#include <vector>
#include <string>
#include <map>
#include "Configure.h"
#include "mysql.h"
#include <pthread.h>

#define GLOBAL_CONF lbs::common::ConfManager::get_conf_manager()

/* usage: GLOBAL_CONF.init(conf_path);
 *        GLOBAL_CONF.set_mysql_info(host, port, user, passwd, db_name);
 *        GLOBAL_CONF.get_table(table_name, table);
 *        GLOBAL_CONF.get_conf_file(file_name, conf);
 *
 *
 */
namespace lbs
{
    namespace common
    {
        class ConfManager
        {
            pthread_mutex_t table_mutex_;
            pthread_mutex_t conf_mutex_;

            std::vector<std::string> conf_file_array_;
            std::vector<std::string> so_file_array_;

            typedef std::vector<std::vector<std::string> > vec_table;

            //store table data
            std::vector<vec_table> table_array_;
            //table schema
            std::vector<std::map<std::string, int> > table_schema_array_;
            //table name
            std::vector<std::string> table_name_array_;

            std::vector<int> old_table_array_;

            //--- mysql info
            std::string mysql_host_;
            int mysql_port_;
            std::string mysql_user_;
            std::string mysql_passwd_;
            std::string mysql_db_name_;
            //---

            MYSQL* mysql_;

            ConfManager();

            void ls_dir(const char* path, std::vector<std::string>& array);
            int do_sql(const char* sql);
            int get_column_index(const int table_index, const std::string column_name, int& index);
            int load_new_table(const std::string table_name, int& index);

            //nested class
            //get the certain column: row["column_name"]
            class Row
            {
                friend class ConfManager;
                std::vector<std::string>& row_;
                int table_index_;

                public:

                /**
                 * @brief 
                 *
                 * @param [in] data   : std::vector<std::string>&
                 * @param [in] index   : const int
                **/
                Row(std::vector<std::string>& data, const int index);

                /**
                 * @brief 
                 *
                 * @param [in] index   : const int
                 * @return  std::string[] 
                 * @todo 
                **/
                std::string operator[] (const int index);

                /**
                 * @brief
                 *
                 * @param [in] column_name   : const char*
                 * @return  std::string[] 
                 * @todo 
                **/
                std::string operator[] (const std::string column_name);
            };

            public:

            class Table
            {
                friend class ConfManager;
                int table_index_;

                public:

                Table();

                /**
                 * @brief like a map, key is value, search the certain column(through index)
                 * if founded, return the row's index
                 *
                 *
                 * @param [in] index   : const int
                 * @param [in] value   : const char*
                 * @return  int 
                 * @todo 
                **/
                int find(const int index, const std::string value);

                /**
                 * @brief search the certain column(through column_name)
                 *
                 * @param [in] column_name   : const char*
                 * @param [in] value   : const char*
                 * @return  int 
                 * @todo 
                **/
                int find(const std::string column_name, const std::string value);

                /**
                 * @brief get row through row's index
                 *
                 * @param [in] index   : const int
                 * @return  Row[] 
                 * @todo 
                **/
                Row operator[](const int index);

                 /**
                 * @brief 
                 *
                 * @param [out] name_array   : std::vector<std::string>&
                 * @return  int 
                 * @todo 
                **/
                int get_column_names(std::vector<std::string>& name_array);               

                int size();
                int column_num();
            };

            /**
             * @brief 
             *
             * @return  ConfManager& 
             * @todo 
            **/
            static ConfManager& get_conf_manager();

            int init(const char* conf_file_path = "./conf/", 
                    const char* so_file_path = "./lib/",
                    const char* host = "cq02-map-pic-rdtest00.cq02.baidu.com", 
                    const int port = 3306, 
                    const char* user = "root",
                    const char* passwd = "root", 
                    const char* db_name = "lbs_db_dms");

            /**
             * @brief 
             *
             * @param [in] host   : const char*
             * @param [in] port   : const int
             * @param [in] user   : const char*
             * @param [in] passwd   : const char*
             * @param [in] db_name   : const char*
             * @return  void 
             * @todo 
            **/
            void set_rule_mysql_info(const char* host, const int port, const char* user,
                    const char* passwd, const char* db_name);
            /**
             * @brief 
             *
             * @param [in] file_name   : const char*
             * @param [out] conf   : comcfg::Configure&
             * @return  int 
             * @todo 
            **/
            int get_conf_file(const std::string file_name, comcfg::Configure& conf);

            /**
             * @brief 
             *
             * @param [in] file_name   : const char*
             * @param [out] full_name   : std::string&
             * @return  int 
             * @todo 
            **/
            int get_so_file(const std::string file_name, std::string& full_name);

            /**
             * @brief get table through table name
             *
             * @param [in] table_name   : const char*
             * @param [out] table   : Table&
             * @return  int 
             * @todo 
            **/
            int get_table(const std::string table_name, Table& table);

            /**
             * @brief refresh table
             *
             * @param [in] table_name   : const char*
             * @return  int 
             * @todo 
            **/
            int update_table(const std::string table_name);

            /**
             * @brief release expired array
             *
             * @return  void 
             * @todo 
            **/
            void clear();

            /**
             * @brief release all
             *
             * @return  void 
             * @todo 
            **/
            void clear_all();

            ~ConfManager();
        }; 

        typedef ConfManager::Table Table;

        //use 'Conf' for short
        typedef comcfg::Configure Conf; 
    }
}
















#endif  //__CONF_MANAGER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
