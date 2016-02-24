/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

/**
 * @file sqlite_dao.cpp
 * @author xingyi01(xingyi01@baidu.com)
 * @date 2014/04/22 14:24:35
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "sqlite_dao.h"
#include "comm_log.h"
#include "comm_def.h"
#include "schema_manager.h"
#include "table_schema.h"
#include "table_manager.h"
#include "mm_allocator.h"
#include "table_index.h"
#include "row_index.h"
#include "dao_data_path.h"

namespace lbs
{
	namespace dao
	{
		int SqliteDao::query(void* data_base, const char* sql)
		{
			return sqlite3_exec((sqlite3*) data_base, sql, NULL, NULL, NULL);
		}

		int SqliteDao::get_data(db_mm::TableSchema* table_schema, int& row_num)
		{
			int ret = 0;
			int table_id = table_schema->get_table_id();
			std::string file_name = get_file_name(table_id);
			db_mm::TableIndex* table_index = table_manager_->get_table_index(table_id);

			row_num = 0;
			table_index->set_data_path(file_name);

			FILE* file = fopen(file_name.c_str(), "w+");

			do
			{
				char** res = NULL;
				int rows = 0;
				int columns = 0;

				if (SQLITE_OK != (ret = sqlite3_get_table(sqlite_, sql_, &res, &rows, &columns, NULL)))
				{
					COM_WRITELOG(COMLOG_WARNING, "sqlite get_data error [%s]", sqlite3_errmsg(sqlite_));
					break;
				}

				//row_num = rows - 1 > 0 ? rows - 1 : 0;
				row_num = rows;

				for (int i = 1; i <= rows; i++)
				{
					for (int j = 0; j < columns; j++)
					{
						int index = i * columns + j;
						//printf("i = %d j = %d res[] = %s\n", i, j, res[index]);

						if (NULL != res[index])
						{
							fwrite(res[index], strlen(res[index]) + 1, 1, file);
						}

						else
						{
							fwrite("\0", 1, 1, file);
						}

						index++;
					}

					fwrite("\n", 1, 1, file);
				}

				sqlite3_free_table(res);
			} while (0);

			if (NULL != file)
			{
				fclose(file);
			}

			return ret;
		}

		int SqliteDao::init(const char* data_file)
		{
			int ret = 0;
            bool flag = false;

			if (!is_init_)
			{
				ret = sqlite3_open(data_file, &sqlite_);
				data_base_ = sqlite_;

				if (SQLITE_OK != ret)
				{
					COM_WRITELOG(COMLOG_WARNING, "sqlite open failed error [%s]", sqlite3_errmsg(sqlite_));
				}

				else
				{
					int row_num = 0;
					db_mm::SchemaManager* schema_manager = table_manager_->get_schema_manager();
					int table_num = schema_manager->get_table_num();

					for (int i = 0; i < table_num; i++)
					{
						db_mm::TableIndex* table_index = table_manager_->get_table_index(i);
						db_mm::TableSchema* table_schema = schema_manager->get_table_schema(i);

						if (0 != (ret = select_sql(table_schema)))
						{
							COM_WRITELOG(COMLOG_WARNING, "in sqlite_dao, sql err,sql[%s], ret[%d]", sql_, ret);
                            break;
						}

						else if (0 != (ret = get_data(table_schema, row_num)))
						{
							COM_WRITELOG(COMLOG_WARNING, "in sqlite_dao get_data error, ret[%d]", ret);
                            //如果表不存在，继续
                            ret = 0;

                            continue;
						}

						else
						{
                            flag = true;
							table_index->set_row_num(row_num);
						}
					}
				}

				is_init_ = true;
			}

            if (!flag)
            {
                ret = -1;
            }

			return ret;
		}

		int SqliteDao::start_transaction()
		{
			return sqlite3_exec(sqlite_, "begin transaction", NULL, NULL, NULL);
		}

		int SqliteDao::roll_back()
		{
			return sqlite3_exec(sqlite_, "rollback", NULL, NULL, NULL);
		}

		int SqliteDao::commit()
		{
			return sqlite3_exec(sqlite_, "commit", NULL, NULL, NULL);
		}

		std::string SqliteDao::save()
		{
			int ret = 0;

			start_transaction();

            int table_num = table_manager_->get_schema_manager()->get_table_num();

            for (int i = 0; i < table_num; i++)
            {
			    db_mm::TableIndex* table_index = table_manager_->get_table_index(i);
			    db_mm::TableIndex::RowIndexIter row_iter = table_index->iter();

                if (!table_index->has_data())
                {
                    continue;
                }

			    db_mm::RowIndex* row_index = NULL;

			    while (0 == row_iter.get_next(row_index) && 0 == ret)
			    {
			    	if (db_mm::DELETE_ROW == row_index->get_row_stat())
			    	{
			    		ret = delete_data(row_index);
			    	}

			    	else if (db_mm::UPDATE_ROW == row_index->get_row_stat())
			    	{
			    		if (0 == ret && 0 == (ret = delete_data(row_index)))
			    		{
			    			ret = insert_data(row_index);
			    		}
			    	}
			    }
            }

			if (0 == ret)
			{
				commit();
			}

			else
			{
				error_msg_ = sqlite3_errmsg(sqlite_);
				roll_back();
			}

			return error_msg_;
		}

		SqliteDao::~SqliteDao()
		{
			if (NULL != sqlite_)
			{
				sqlite3_close(sqlite_);
			}
		}
	}
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
