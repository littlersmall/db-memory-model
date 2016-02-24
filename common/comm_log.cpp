/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file comm_log.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/03/22 13:47:19
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "comm_log.h"
#include "conf_manager.h"
#include "Configure.h"

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/uio.h>
#include <sys/time.h>

namespace lbs
{
    namespace common
    {
        const char* ComLog::err_str_[] = {"FATAL", "WARNING", "NOTICE", "DEBUG"};

        ComLog::ComLog()
        {
            day_ = "NULL";
            real_time_ = true;
            fd_ = 2;
            real_fd_ = 2;

            //默认不开 debug
            level_ = COMLOG_NOTICE;
        }

        ComLog& ComLog::get_com_log()
		{
			static ComLog com_log;

			return com_log;
		}

        void ComLog::set_task_id(const int task_id, bool real_time)
        {
            real_time_ = real_time;

            char day[1024] = {};
            struct timeval tv; 

            gettimeofday(&tv, NULL);

            struct tm tm; 
            char log_file_name[1024] = {};

            ::localtime_r((const time_t*)&tv.tv_sec, &tm);

            snprintf(day, sizeof(day), "%04d-%02d-%02d", 
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

            if (day_ != day)
            {
                day_ = day;

                Conf conf;
                char log_full_path[1024] = {};

                GLOBAL_CONF.get_conf_file("process_config.ini",  conf);
 
                snprintf(log_full_path, sizeof(log_full_path), "%s/%s/", 
                        conf["system"]["log_path"].to_cstr(),
                        day_.c_str());

                log_path_ = log_full_path;

                if (NULL == opendir(log_full_path))
                {
                    mkdir(log_full_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                }

                if (2 != fd_)
                {
                    close(fd_);
                    fd_ = 2;
                }

                snprintf(log_file_name, sizeof(log_file_name), 
                        "%s/task_real.log", log_path_.c_str()); 
                
                real_fd_ = open(log_file_name, 
                        O_RDWR | O_CREAT | O_APPEND | O_LARGEFILE, 0644);  
            }

            if (!real_time)
            {
                if (2 != fd_)
                {
                    close(fd_);
                }

                snprintf(log_file_name, sizeof(log_file_name), 
                        "%s/task_%d.log", log_path_.c_str(), task_id); 

                fd_ = open(log_file_name, O_RDWR | O_CREAT | O_APPEND | O_LARGEFILE, 0644);  
            }
        }

        // real
        void ComLog::init()
        {
            set_task_id(-1, true);
        }

        void ComLog::init(const int task_id)
        {
            set_task_id(task_id, false); 
        }

        void ComLog::write_log(int level, const char* file, 
                int line, const char* function, pthread_t tid, const char* fmt, ...)
        {
            struct timeval tv;
            
            gettimeofday(&tv, NULL);
            struct tm tm; 
            ::localtime_r((const time_t*)&tv.tv_sec, &tm);

            char data1[4000];
            char head[128];

            va_list args;
            
            va_start(args, fmt);

            int data_size = vsnprintf(data1, 4000, fmt, args);

            va_end(args);

            if (data_size >= 4000)
            {   
              data_size = 3999;
            }   

            // remove trailing '\n'
            while (data1[data_size-1] == '\n') 
            {
                data_size --; 
            }

            data1[data_size] = '\0';

            int head_size = 
                snprintf(head,128,"[%04d-%02d-%02d %02d:%02d:%02d.%06ld] %-5s %s (%s:%d) [%ld] ",
                                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                                tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec,
                                err_str_[level], function, file, line, tid);

            struct iovec vec[3];

            vec[0].iov_base = head;
            vec[0].iov_len = head_size;
            vec[1].iov_base = data1;
            vec[1].iov_len = data_size;

            static char NEWLINE[1] = {'\n'};

            vec[2].iov_base = NEWLINE;
            vec[2].iov_len = sizeof(NEWLINE);

            if (data_size > 0)
            {   
                if (real_time_)
                {
                    ::writev(real_fd_, vec, 3); 
                }

                else
                {
                    ::writev(fd_, vec, 3); 
                }
            }    
        }

        ComLog::~ComLog()
        {
            if (2 != fd_)
            {
                close(fd_);
            }

            if (2 != real_fd_)
            {
                close(real_fd_);
            }
        }

        void ComLog::set_log_level(const int level)
        {
            level_ = level;
        }
    }
}



















/* vim: set ts=4 sw=4 sts=4 tw=100 */
