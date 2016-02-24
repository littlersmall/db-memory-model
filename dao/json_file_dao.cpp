/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file json_file_dao.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/10 15:49:31
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "json_file_dao.h"

namespace lbs
{
    namespace dao
    {
        int JsonFileDao::init(std::string file_name)
        {
            int ret = 0;

            if (!is_init_)
            {
                FILE* file = fopen(file_name.c_str(), "r");
                char line[1024*10] = {};
                std::vector<std::string> json_vec;


                while (fgets(line, sizeof(line), file))
                {
                    if (NULL != line)
                    {
                        line[strlen(line)-1] = '\0';
                        //printf("line = %s\n", line);
                        json_vec.push_back(line); 
                    }
                }

                if (NULL != file)
                {
                    fclose(file);
                }

                ret = json_dao_.init(json_vec);

                is_init_ = true;
            }

            return ret;
        }

        int JsonFileDao::save(std::string file_name)
        {
            std::vector<std::string> json_vec;

            json_dao_.save(json_vec);

            FILE* file = fopen(file_name.c_str(), "w");

            for (unsigned int i = 0; i < json_vec.size(); i++)
            {
                fwrite(json_vec[i].c_str(), json_vec[i].size(), 1, file);
            } 

            if (NULL != file)
            {
                fclose(file);
            }

            return 0;
        }
    }
}



















/* vim: set ts=4 sw=4 sts=4 tw=100 */
