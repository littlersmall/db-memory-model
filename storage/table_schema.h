/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file table_schema.h
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/15 11:05:27
 * @brief 
 *  
 **/




#ifndef  __ROW_DESRC_H_
#define  __ROW_DESRC_H_

#include "comm_def.h"
#include "Configure.h"
#include "hash_char.h"
#include <vector>
#include <string>
#include <bsl/containers/hash/bsl_hashmap.h>

namespace lbs
{
    namespace db_mm
    {
        class TableManager;

        //data type
        enum TYPE
        {
            INT_TYPE = 8,
            //vchar
            VCHAR_TYPE,
            //double
            DOUBLE_TYPE,
            //text
            TEXT_TYPE
        };

        class TableSchema
        {
            typedef struct RowKey
            {
                // row_key id
                int column_id;
                // reserve
                int row_key_length;
                // the num of ref by other tables
                int reverse_index_num;

                typedef struct TableAndColumnId
                {
                    int table_id;
                    int column_id;

                    TableAndColumnId()
                    {
                        memset(this, '\0', sizeof(*this));
                    }

                    TableAndColumnId(const int t_id, const int c_id)
                        : table_id(t_id), column_id(c_id)
                    {}
                }TableAndColumnId;
                
                std::vector<TableAndColumnId> index_table_array;

                RowKey()
                {
                    memset(this, '\0', sizeof(*this));
                }

                void print()
                {
                    printf("column_id = %d\n", column_id);
                    printf("reverse_index_num = %d\n\n", reverse_index_num);

                    for (int i = 0; i < reverse_index_num; i++)
                    {
                        printf("table_id = %d\n", index_table_array[i].table_id);
                        printf("column_id = %d\n\n", index_table_array[i].column_id);
                    }
                }
            }RowKey;

            typedef struct ForeignKey
            {
                //foreign key num
                int foreign_key_num;
                //foreign key array
                int* foreign_key_array;
                //the referenced table id array
                int* table_id_array;

                ForeignKey()
                {
                    memset(this, '\0', sizeof(*this));
                }

                void print()
                {
                    printf("foreign_key_num = %d\n\n", foreign_key_num);

                    for (int i = 0; i < foreign_key_num; i++)
                    {
                        printf("column_id = %d\n", foreign_key_array[i]);
                        printf("table_id = %d\n\n", table_id_array[i]);
                    }
                }

                ~ForeignKey()
                {
                    if (NULL != foreign_key_array)
                    {
                        delete[] foreign_key_array;

                        foreign_key_array = NULL;
                    }

                    if (NULL != table_id_array)
                    {
                        delete[] table_id_array;

                        table_id_array = NULL;
                    }
                }
            }ForeignKey;

            typedef struct Cell
            {
                char column_name[common::COLUMN_NAME_LENGTH];
                int column_id;
                TYPE data_type;
                
                Cell()
                {
                    memset(this, '\0', sizeof(*this));
                }

                void print()
                {
                    printf("column_name = %s\n", column_name);
                    printf("column_id = %d\n", column_id);
                    printf("data_type = %d\n", data_type);
                }
            }Cell;

            char table_name_[common::TABLE_NAME_LENGTH];
            //start from 0
            int table_id_;
            //column
            int column_num_;

            //row_key
            RowKey row_key_;
            ForeignKey foreign_key_; 
            //cell(column property)
            Cell* cell_array_;
            bsl::hashmap<char*, int, common::HashFun, common::EqualFun> column_hash_;

            bool has_row_key_;

            public:
            TableSchema();

            /**
             * @brief
             *
             * @param [in] conf   : const comcfg::Configure&
             * @return  int 
             * @todo 
            **/
            int init(const comcfg::Configure& conf);

            /**
             * @brief add reverse index
             *
             * @param [in] t_id   : const int
             * @param [in] c_id   : const int
             * @return
             * @todo 
            **/
            void add_reverse_index(const int t_id, const int c_id);
 
            /**
             * @brief get foreign table_id
             *
             * @param [out] table_num   : int&
             * @param [out] table_id_array   : int* &
             * @return  int 
             * @todo 
            **/
            int get_foreign_table_id(const int index) const;
            /**
             * @brief
             *
             * @param [out] t_id   : int&
             * @return
             * @todo 
            **/
            int get_table_id() const;

            /**
             * @brief get foreign column_id
             *
             * @param [in] index   : const int
             * @param [out] column_id   : int&
             * @return  int 
             * @todo 
            **/
            int get_foreign_key_id(const int index) const;

            /**
             * @brief
             *
             * @param [out] foreign_key_num   : int&
             * @return
             * @todo 
            **/
            int get_foreign_key_num() const;

            /**
             * @brief 
             *
             * @param [out] reverse_index_num   : int&
             * @return
             * @todo 
            **/
            int get_reverse_index_num() const;

            /**
             * @brief get reverse index info
             *
             * @param [in] index   : const int
             * @param [out] table_id   : int&
             * @param [out] column_id   : int&
             * @return  int 
             * @todo 
            **/
            int get_reverse_table_info(const int index, 
                    int& table_id, int& column_id) const;

            /**
             * @brief
             *
             * @param [out] row_num   : int&
             * @return
             * @todo 
            **/
            int get_row_num() const;

            /**
             * @brief
             *
             * @param [out] column_num   : int&
             * @return
             * @todo 
            **/
            int get_column_num() const;

            /**
             * @brief data type
             *
             * @param [in] index   : const int
             * @param [out] type   : TYPE&
             * @return  int 
             * @todo 
            **/
            TYPE get_column_type(const int index) const;

            /**
             * @brief
             *
             * @param [in] index   : const int
             * @param [out] column_name   : const char* &
             * @return  int 
             * @todo 
            **/
            const char* get_column_name(const int index) const;

            /**
             * @brief
             *
             * @param [out] table_name   : const char* &
             * @return int
             * @todo 
            **/
            const char* get_table_name() const;

            /**
             * @brief 
             *
             * @param [in] column_name   : const char*
             * @param [out] column_id   : int&
             * @return  int 
             * @todo 
            **/
            int get_column_id(const char* column_name) const;

            /**
             * @brief 
             *
             * @return  bool 
             * @todo 
            **/
            bool has_data() const;

            /**
             * @brief 
             *
             * @return  void 
             * @todo 
            **/
            void set_has_data();

            /**
             * @brief 
             *
             * @param [in] data_path   : const char*
             * @return  void 
             * @todo 
            **/
            void set_data_path(const char* data_path);

            /**
             * @brief 
             *
             * @param [out] data_path   : const char* &
             * @return  void 
             * @todo 
            **/
            const char* get_data_path() const;

            /**
             * @brief 
             *
             * @return  bool 
             * @todo 
            **/
            bool has_row_key() const;

            /**
             * @brief for debug only
             *
             * @return  void 
             * @todo 
            **/
            void print();

            ~TableSchema();
        };
    }
}















#endif  //__ROW_DESRC_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
