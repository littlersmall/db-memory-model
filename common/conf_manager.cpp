/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
/**
 * @file conf_manager.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/03/13 15:18:40
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "conf_manager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include "comm_def.h"

namespace lbs
{
    namespace common
    {
        ConfManager::Row::Row(std::vector<std::string>& data, const int index) : 
            row_(data), table_index_(index)
        {}

        std::string ConfManager::Row::operator[] (const int index)
        {
            return row_[index];     
        }

        std::string ConfManager::Row::operator[] (const std::string column_name)
        {
            int index = ITERATOR_END;
            std::string ret;

            get_conf_manager().get_column_index(table_index_, column_name, index); 

            if (ITERATOR_END == index)
            {
                printf ("[]: index error\n"); 
            }
            
            else
            {
                ret = (*this)[index]; 
            }

            return ret;
        }

        ConfManager::Table::Table()
        {
            table_index_ = 0;
        } 

        int ConfManager::Table::find(const int index, const std::string value)
        {
            int ret = ITERATOR_END;

            //printf("%d\n", (int) get_conf_manager().table_array_[table_index_].size());
            //printf("%s\n", value);

            if (0 > index || column_num() <= index)
            {
                printf ("find: index error\n"); 
            }

            else
            {
                for (unsigned int i = 0; i < get_conf_manager().table_array_[table_index_].size(); i++)
                {
                    if (value == get_conf_manager().table_array_[table_index_][i][index])
                    {
                        ret = i;
                        break;
                    }

                    //printf("%s\n", get_conf_manager().table_array_[index_][i][index].c_str());
                }
            }

            return ret;
        }

        int ConfManager::Table::find(const std::string column_name, const std::string value)
        {
            int index = ITERATOR_END;

            get_conf_manager().get_column_index(table_index_, column_name, index); 

            return find(index, value);
        }

        ConfManager::Row ConfManager::Table::operator[](const int index)
        {
            return Row(get_conf_manager().table_array_[table_index_][index], table_index_);
        }

        int ConfManager::Table::get_column_names(std::vector<std::string>& name_array)
        {
            clear_vector(name_array);

            std::map<std::string, int>::iterator it = 
                get_conf_manager().table_schema_array_[table_index_].begin();
            std::map<std::string, int>::iterator end = 
                get_conf_manager().table_schema_array_[table_index_].end();

            for (; end != it; it++)
            {
                name_array.push_back(it->first);
            }

            return 0; 
        }

        int ConfManager::Table::size()
        {
            return (int) get_conf_manager().table_array_[table_index_].size();
        }

        int ConfManager::Table::column_num()
        {
            return (int) get_conf_manager().table_array_[table_index_][0].size();
        }

        ConfManager::ConfManager()
        {
            pthread_mutex_init(&table_mutex_, NULL);
            pthread_mutex_init(&conf_mutex_, NULL);
        }

        ConfManager& ConfManager::get_conf_manager()
        {
            static ConfManager conf_manager;

            return conf_manager;
        }

        void ConfManager::ls_dir(const char* path, std::vector<std::string>& array)
        {   
            DIR* dir = NULL;
            struct dirent* ent = NULL;
                
            if (common::MAX_FILE_NUM <= (int) array.size())
            {   
                return; 
            }   

            else if (NULL == (dir = opendir(path)))
            {
                return;
            }

            char child_path[common::PATH_LENGTH] = {'\0'};
    
            while (NULL != (ent = readdir(dir)))
            {   
                //if directory, recursion
                if (ent->d_type & DT_DIR)
                {   
                    if (0 == strcmp(ent->d_name, ".") || 0 == strcmp(ent->d_name, ".."))
                    {   
                        continue;
                    }   

                    snprintf(child_path, sizeof(child_path), "%s/%s", path, ent->d_name);

                    //printf("%s\n", child_path);
                    ls_dir(child_path, array);
                }   

                else
                {   
                    array.push_back(std::string(path) + std::string("/") + std::string(ent->d_name));

                    if (common::MAX_FILE_NUM <= (int) array.size())
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

        int ConfManager::do_sql(const char* sql)
        {
            int ret = 0;
            std::vector<std::vector<std::string> > table;

            do
            {
                if (0 != (ret = mysql_query(mysql_, sql)))
                {   
                    break;
                }   

                if (mysql_field_count(mysql_) > 0)
                {   
                    MYSQL_RES *res;
                    MYSQL_ROW row = NULL;
                    int fields_num;
                
                    if (NULL == (res = mysql_store_result(mysql_)))
                    {   
                        break;
                    }   

                    fields_num = mysql_num_fields(res);
                    std::map<std::string, int> column_map; 

                    for (int i = 0; i < fields_num; i++)
                    {
                        MYSQL_FIELD* field = NULL;

                        field = mysql_fetch_field_direct(res, i);

                        column_map[std::string(field->name)] = i;
                    }

                    table_schema_array_.push_back(column_map);

                    while (row = mysql_fetch_row(res))
                    {   
                        std::vector<std::string> row_vec;
                        int index = 0;

                        //printf("row_num = %d\n", row_num);
                        while (index < fields_num)
                        {   
                            if (NULL == row[index])
                            {
                                row_vec.push_back(std::string()); 
                                index++;
                            }

                            else
                            {
                                row_vec.push_back(row[index++]);
                            }
                        }   
                        
                        table.push_back(row_vec);
                    }   

                    //release memory
                    mysql_free_result(res);
                }   
            }while (0);

            table_array_.push_back(table);

            return ret;       
        }

        int ConfManager::get_column_index(const int table_index, const std::string column_name, int& index)
        {
            int ret = ITERATOR_END;
            std::map<std::string, int>::iterator it = table_schema_array_[table_index].find(column_name);

            if (table_schema_array_[table_index].end() != it)
            {
                index = it->second; 
                ret = 0;
            }

            return ret; 
        }

        int ConfManager::load_new_table(const std::string table_name, int& index)
        {
            int ret = 0;
            if (NULL != mysql_real_connect(mysql_, 
                        mysql_host_.c_str(), mysql_user_.c_str(), 
                        mysql_passwd_.c_str(), mysql_db_name_.c_str(), mysql_port_, NULL, 0))
            {
                mysql_set_character_set(mysql_, "utf8");
                table_name_array_.push_back(table_name); 

                char sql[SQL_LENGTH] = {};

                snprintf(sql, sizeof(sql), "select * from %s", table_name.c_str());

                ret = do_sql(sql);

                if (0 == ret)
                {
                    index = table_array_.size() - 1;
                } 
            } 

            else
            {
                ret = MYSQL_CONNECT_FAILED;
                printf("Connect to mysql server fail:%s\n", mysql_error(mysql_));
            }
            
            return ret;
        }

        int ConfManager::init(const char* conf_file_path, 
            const char* so_file_path, const char* host, const int port,
            const char* user, const char* passwd, const char* db_name)
        {
            int ret = 0;

            if (NULL != conf_file_path 
                    && NULL != so_file_path && NULL != host 
                    && NULL != user && NULL != passwd
                    && NULL != db_name)
            {
                ls_dir(conf_file_path, conf_file_array_);
                ls_dir(so_file_path, so_file_array_);

                mysql_host_ = std::string(host);
                mysql_port_ = port;
                mysql_user_ = std::string(user);
                mysql_passwd_ = std::string(passwd);
                mysql_db_name_ = std::string(db_name);
                
                mysql_ = mysql_init(NULL);
            }

            else
            {
                ret = PARA_ERROR;
            }

            return ret;
        }

        void ConfManager::set_rule_mysql_info(const char* host, 
            const int port, const char* user, const char* passwd,
            const char* db_name)
        {
            mysql_host_ = std::string(host);
            mysql_port_ = port;
            mysql_user_ = std::string(user);
            mysql_passwd_ = std::string(passwd);
            mysql_db_name_ = std::string(db_name);
        }
 
        int ConfManager::get_conf_file(const std::string file_name, comcfg::Configure& conf)
        {
            int ret = ITERATOR_END;
            std::string full_name;

            pthread_mutex_lock(&conf_mutex_);

            for (unsigned int i = 0; i < conf_file_array_.size(); i++) 
            {
                if (std::string::npos != conf_file_array_[i].find(file_name))
                {
                    full_name = conf_file_array_[i];
                    ret = 0;

                    break;
                }
            }

            if (0 == ret)
            {
                ret = conf.load("", full_name.c_str());
            }

            pthread_mutex_unlock(&conf_mutex_);

            return ret;
        }

        int ConfManager::get_so_file(const std::string file_name, std::string& full_name)
        {
            int ret = ITERATOR_END;

            for (unsigned int i = 0; i < so_file_array_.size(); i++) 
            {
                if (std::string::npos != so_file_array_[i].find(file_name))
                {
                    full_name = so_file_array_[i];
                    ret = 0;

                    break;
                }
            }

            return ret;
        }

        // 1. if table has been loaded return loaded table
        // 2. if not, load table
        int ConfManager::get_table(const std::string table_name, Table& table)
        {
            int ret = 0;
            int index = ITERATOR_END;

            pthread_mutex_lock(&table_mutex_);
            // 1
            for (unsigned int i = 0; i < table_name_array_.size(); i++)
            {
                if (table_name == table_name_array_[i])
                {
                    index = i;

                    break;
                }
            }

            // 2
            if (ITERATOR_END == index)
            {
                ret = load_new_table(table_name, index);
            }

            if (0 <= index)
            {
                table.table_index_ = index;
            }

            pthread_mutex_unlock(&table_mutex_);

            return ret;
        }

        // 1. add old table to old_table_array_
        // 2. load table as a new table
        // 3. rename old table's name with NULL
        int ConfManager::update_table(const std::string table_name)
        {
            int ret = 0;
            int index = ITERATOR_END;

            for (unsigned int i = 0; i < table_name_array_.size(); i++)
            {
                if (table_name == table_name_array_[i])
                {
                    index = i;

                    break;
                }
            }

            if (ITERATOR_END != index)
            {
                // 1
                old_table_array_.push_back(index);
                
                int tmp_index = ITERATOR_END;

                // 2
                if (0 == (ret = load_new_table(table_name, tmp_index)))
                {
                    // 3
                    table_name_array_[index] = std::string(""); 
                }
            } 

            return ret;
        }

        void ConfManager::clear()
        {
            printf("%d\n", (int) old_table_array_.size());

            for (unsigned int i = 0; i < old_table_array_.size(); i++)
            {
                //printf("size = %d\n", (int) table_array_[old_table_array_[i]].size());
                clear_vector(table_array_[old_table_array_[i]]);
                //printf("size = %d\n", (int) table_array_[old_table_array_[i]].size());
            }

            clear_vector(old_table_array_);
        }

        void ConfManager::clear_all()
        {
            clear_vector(conf_file_array_);
            clear_vector(so_file_array_);
            clear_vector(table_array_);
            clear_vector(table_schema_array_); 
            clear_vector(table_name_array_);
            clear_vector(old_table_array_);
        }

        ConfManager::~ConfManager()
        {
            if (NULL != mysql_) 
            {   
                mysql_close(mysql_);
                mysql_ = NULL;
                mysql_library_end();
            }          

            pthread_mutex_destroy(&table_mutex_);
            pthread_mutex_destroy(&conf_mutex_);
        }
    }
}

/*
#include <iostream>

int main()
{
    using namespace lbs::common;
    using namespace std;

    GLOBAL_CONF.init();
    Table table;

    string file_name;
    //ConfManager::get_conf_manager().get_conf_file("test.conf", file_name);
    ConfManager::get_conf_manager().get_so_file("test.so", file_name);
    ConfManager::get_conf_manager().get_table("conf_country_name", table);

    cout << table[table.find("B", "埃及")]["B"] << endl;
    
    ConfManager::get_conf_manager().update_table("conf_country_name");

    ConfManager::get_conf_manager().get_table("conf_country_name", table);
    cout << table[table.find("B", "埃及")]["B"] << endl;

    ConfManager::get_conf_manager().update_table("conf_country_name");


    ConfManager::get_conf_manager().clear();

    ConfManager::get_conf_manager().get_table("conf_country_name", table);
    cout << table[table.find("B", "埃及")]["B"] << endl;

    ConfManager::get_conf_manager().update_table("conf_country_name");


    ConfManager::get_conf_manager().clear_all();
    ConfManager::get_conf_manager().get_table("conf_country_name", table);
    cout << table[table.find("B", "埃及")]["B"] << endl;
    ConfManager::get_conf_manager().update_table("conf_country_name");
    ConfManager::get_conf_manager().clear();
    cout << file_name << endl;
}
*/




















/* vim: set ts=4 sw=4 sts=4 tw=100 */
