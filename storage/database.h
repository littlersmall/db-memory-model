/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

/**
 * @file database.h
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/24 16:00:14
 * @version $Revision$ 
 * @brief 
 *  
 **/

#ifndef  __DATABASE_H_
#define  __DATABASE_H_

#include "schema_manager.h"

#define GLOBAL_DB lbs::db_mm::DataBase::get_db()

/*
 * usage: GLOBAL_DB.get_ttf_schema()
 */

namespace lbs
{
	namespace db_mm
	{
		static const char* TTF_CONF_PATH = "./conf/memmodel/ttf_model/";

		class DataBase
		{
			SchemaManager ttf_schema_manager_;

		public:

			int init()
			{
				return ttf_schema_manager_.init(TTF_CONF_PATH);
			}

			/**
			 * @brief  new 一个 schema_manager, 并返回
			 * 记得释放指针
			 *
			 * @return  SchemaManager*
			 * @todo
			 **/
			SchemaManager* get_ttf_schema()
			{
				return &ttf_schema_manager_;
			}

			static DataBase& get_db()
			{
				static DataBase db;

				return db;
			}
		};
	}
}

#endif  //__DATABASE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
