/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file parser.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/02/25 14:44:48
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __PARSER_H_
#define  __PARSER_H_

#include <string>

namespace lbs
{
    namespace common
    {
        /**
         * @brief 
         *
         * @param [in] str   : const char*
         * @param [out] data   : int&
         * @return  int 
         * @todo 
        **/
        int parse_str(const char* str, int& data);

        /**
         * @brief 
         *
         * @param [in] str   : const char*
         * @param [out] data   : const char* &
         * @return  int 
         * @todo 
        **/
        int parse_str(const char* str, const char*& data);

        /**
         * @brief 
         *
         * @param [in] str   : const char*
         * @param [out] data   : double&
         * @return  int 
         * @todo 
        **/
        int parse_str(const char* str, double& data);

        /**
         * @brief value to string
         *
         * @param [in] value   : const int
         * @param [out] str   : char*
         * @return  int 
         * @todo 
        **/
        int parse_value(const int value, char* str);

        /**
         * @brief 
         *
         * @param [in] value   : const char*
         * @param [out] str   : char*
         * @return  int 
         * @todo 
        **/
        int parse_value(const char* value, char* str);

        /**
         * @brief 
         *
         * @param [in] value   : const double
         * @param [out] str   : char*
         * @return  int 
         * @todo 
        **/
        int parse_value(const double value, char* str);

        int parse_value(const std::string value, std::string& str); 
    }
}
















#endif  //__PARSER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
