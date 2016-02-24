/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file define_class.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/07/21 11:51:29
 * @version $Revision$ 
 * @brief 
 *  
 **/

/*
 * usage: NAV_CLASS(NavLink, NAV_LINK);
 *
 */

#ifndef  __DEFINE_CLASS_H_
#define  __DEFINE_CLASS_H_

#include "row_instance.h"

#define X_CLASS(class_name, table_id) \
class class_name : public RowInstance \
{\
    public: \
    class_name() : RowInstance(table_id) {}\
    int get_table_id() \
    {\
        return table_id;\
    }\
};
















#endif  //__DEFINE_CLASS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
