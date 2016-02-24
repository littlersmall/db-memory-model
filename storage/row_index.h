/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file row_index.h
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/21 16:58:44
 * @brief 
 *  
 **/

#ifndef  __ROW_INDEX_H_
#define  __ROW_INDEX_H_

//#include <bsl/deque.h>
#include <stdint.h>
#include "table_schema.h"

namespace lbs
{
    namespace db_mm
    {
        class RowIndex;
        class TableIndex;

        typedef struct IndexList
        {
            //被引用的字段id
            int column_id;
            RowIndex* row_index;
            struct IndexList* next;

            IndexList();
            ~IndexList();
        }IndexList;

        enum ROW_STAT
        {
            NORMAL_ROW,
            UPDATE_ROW,
            DELETE_ROW 
        };

        class RowIndex
        {
            //point to row's data
            const char* row_data_; 
            //the offset of each column (start from the second column)
            int16_t* column_array_;
            TableIndex* table_index_;
            IndexList* reverse_index_list_; 

            ROW_STAT row_stat_;

            /**
             * @brief push it to index_list_
             *
             * @param [in] column_id   : const int
             * @param [in] row_index   : RowIndex*
             * @return  void 
             * @todo 
            **/
            void push_back(const int column_id, RowIndex* row_index);

            void del_reverse_index(RowIndex* row_index);

            public:
            RowIndex();

            /**
             * @brief new column_array_
             *
             * @param [in] table_schema   : const TableSchema*
             * @return  int 
             * @todo 
            **/
            int init(TableIndex* table_index); 
                    //const bool reverse_index_on);

            /**
             * @brief set each column's offset, and return end offset of the row
             *
             * @param [in] row_data   : const char*
             * @param [out] row_end_offset   : int&
             * @return  int 
             * @todo 
            **/
            int set_offset(const char* row_data, 
                    int& row_end_offset);

            /**
             * @brief get row_data_
             *
             * @param [out] row_data_   : const char* &
             * @return  int 
             * @todo 
            **/
            const char* get_row_data() const;

            /**
             * @brief get column's offset
             *
             * @param [in] column_id   : const int
             * @param [out] offset   : int&
             * @return  int 
             * @todo 
            **/
            int get_column_offset(const int column_id) const;

            /**
             * @brief 
             *
             * @param [in] column_name   : const char*
             * @param [out] column_id   : int&
             * @return  int 
             * @todo 
            **/
            int get_column_id(const char* column_name);

            /**
             * @brief add reverse index
             *
             * @param [in] column_id   : const int
             * @param [in] row_index   : RowIndex*
             * @return  int 
             * @todo 
            **/
            int set_reverse_index(const int column_id, RowIndex* row_index);

            /**
             * @brief 
             *
             * @param [out] table_schema   : const TableSchema* &
             * @return  void 
             * @todo 
            **/
            const TableSchema* get_table_schema() const;

            /**
             * @brief 
             *
             * @param [out] index_list   : IndexList* &
             * @return
             * @todo 
            **/
            IndexList* get_reverse_index();

            /**
             * @brief 
             *
             * @param [out] table_id   : int&
             * @return  void 
             * @todo 
            **/
            int get_table_id();

            /**
             * @brief 
             *
             * @param [out] table_name   : const char* &
             * @return  int 
             * @todo 
            **/
            const char* get_table_name();

            /**
             * @brief 
             *
             * @param [in] index   : const int
             * @param [out] column_name   : const char* &
             * @return  int 
             * @todo 
            **/
            const char* get_column_name(const int index);

            /**
             * @brief 
             *
             * @param [out] column_num   : int&
             * @return  void 
             * @todo 
            **/
            int get_column_num();

            /**
             * @brief 
             *
             * @param [in] column_id   : const int
             * @param [in] value   : const char*
             * @return  int 
             * @todo 
            **/
            int set_column_data(const int column_id, const char* value);

            /**
             * @brief 获得row当前状态
             * 1. DELETE_ROW 已被删除
             * 2. UPDATE_ROW 有更新
             * 3. NORMAL_ROW 没有过修改
             *
             * @return  ROW_STAT 
             * @todo 
            **/
            ROW_STAT get_row_stat()
            {
                return row_stat_;
            }

            /**
             * @brief 
             *
             * @param [in] stat   : ROW_STAT
             * @return  void 
             * @todo 
            **/
            void set_row_stat(ROW_STAT stat)
            {
                row_stat_ = stat; 
            }

            /**
             * @brief 删除自身，如果有反向索引，则不删除
             *
             * @return  bool 
             * @todo 
            **/
            bool del();

            /**
             * @brief 
             *
             * @return  TableManager* 
             * @todo 
            **/
            TableManager* get_table_manager();

            /**
             * @brief for debug only
             * print each row's row_key, reverse_index, column offset
             *
             * @return  void 
             * @todo 
            **/
            void print();

            ~RowIndex();
        };
    }
}














#endif  //__ROW_INDEX_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
