/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file dao.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/09 16:13:25
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __DAO_H_
#define  __DAO_H_

namespace lbs
{
    namespace db_mm
    {
        //interface
        class Dao
        {
            protected:
            void* input_;
            void* output_;

            public:
            Dao() : input_(NULL), output_(NULL)
            {}

            virtual int init(void* input, void* output, const char* data_path) = 0;
            virtual int save() = 0; 
        };
    }
}
















#endif  //__DAO_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
