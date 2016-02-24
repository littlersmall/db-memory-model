/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file parser.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/02/25 15:03:55
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "parser.h"
#include "comm_def.h"
#include <stdio.h>

namespace lbs
{
    namespace common
    {
        int parse_str(const char* str, int& data)
        {
            int ret = 0;

            if (NULL == str)
            {
                ret = NULL_POINTER;
            }

            else
            {
                data = 0;

                while ('\0' != *str)
                {
                    data = 10*data + *(str++) - '0';
                }
            }
            
            return ret;
        }

        int parse_str(const char* str, const char*& data)
        {
            int ret = 0;

            if (NULL == str)
            {
                ret = NULL_POINTER;
            }

            else
            {
                data = str;
            }

            return ret;
        }

        int parse_str(const char* str, double& data)
        {
            int ret = 0;

            if (NULL == str)
            {
                ret = NULL_POINTER; 
            } 

            else
            {
                sscanf(str, "%lf", &data);
            }

            return ret;
        }

        int parse_value(const int value, char* str)
        {
            return 0 != snprintf(str, common::MAX_COLUMN_LENGTH, "%d", value); 
        }

        int parse_value(const char* value, char* str)
        {
            if (NULL != value)
            {
                memcpy(str, value, strlen(value) + 1);
            } 

            return 0;
        }

        int parse_value(const double value, char* str)
        {
            return 0 != snprintf(str, common::MAX_COLUMN_LENGTH, "%f", value);
        }

        int parse_value(const std::string value, std::string& str)
        {
            str = value;


            return 0;
        }
    }
}




















/* vim: set ts=4 sw=4 sts=4 tw=100 */
