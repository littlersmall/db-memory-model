/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file comm_def.h
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/15 11:44:23
 * @brief define variable size, error number and memroy flag
 *  
 **/


#include <iostream>
#include <vector>

#ifndef  __COMM_DEF_H_
#define  __COMM_DEF_H_

namespace lbs
{
    namespace common
    {
        //variable size
        static const int MAX_FILE_NUM = 1024;
        static const int FILE_NAME_LENGTH = 256;
        static const int COLUMN_NAME_LENGTH = 256;
        static const int TABLE_NAME_LENGTH = 256;
        static const int PATH_LENGTH = 256;
        static const int MAX_COLUMN_LENGTH = 1024;
        static const int SQL_LENGTH = 2048;
        static const int MAX_FORM_NUM = 128;
        static const int MAX_FILTER_NUM = 128;

        //error
        static const int SUCCESS = 0;
        static const int ITERATOR_END = -1;
        static const int NULL_POINTER = -2;
        static const int OUT_OF_INDEX = -3;
        static const int SIZE_ZERO = -4;
        static const int ROW_ERROR = -5;
        static const int TABLE_NUM_ERROR = -6;
        static const int READ_FILE_ERROR = -7;
        static const int HASH_NOT_EXIST = -8;
        static const int NO_MEMORY = -9;
        static const int MYSQL_CONNECT_FAILED = -10;
        static const int MMAP_ERROR = -11;
        static const int PARA_ERROR = -12;

        //memory flag
        enum MM_FLAG
        {
            FILE_MANAGER = 0,
            SCHEMA_MANAGER,
            DATA_FILE,
            TABLE_INDEX,
            ROW_INDEX,
            REVERSE_INDEX,
            UPDATE_DATA,
            OTHER,

            FLAG_MAX
        };

        template <class T>
        class Iter
        {
            public:
            virtual int get_next(T& data) = 0;
            virtual ~Iter() 
            {}
        };

        template <class T>
        void clear_vector(std::vector<T>& vec)
        {   
            std::vector<T> tmp_vec;

            tmp_vec.swap(vec);
        }
    }
}













#endif  //__COMM_DEF_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
