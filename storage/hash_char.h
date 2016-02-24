/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file hash_char.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/01/24 20:18:10
 * @version $Revision$ 
 * @brief specialization char*
 *  
 **/


#ifndef  __HASH_CHAR_H_
#define  __HASH_CHAR_H_

namespace lbs
{
    namespace common
    {
        typedef char* key;

        struct HashFun
        {
            //bkdr hash
            //use bkdr algorithm to compute (char*)'s hash value
            unsigned int operator() (const key& my_key) const
            {
                unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
                unsigned int hash = 0;
                int index = 0;

                while (*(my_key + index))
                {
                    hash = hash * seed + *(my_key + index);
                    index++;
                }

                return (hash & 0x7FFFFFFF);
            }    
        };

        struct EqualFun
        {
            bool operator() (const key& left, const key& right) const
            {
                int left_len = strlen(left);
                int right_len = strlen(right);

                return ((left_len == right_len) 
                        && (0 == memcmp(left, right, left_len)));
            }
        };
    }
}
















#endif  //__HASH_CHAR_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
