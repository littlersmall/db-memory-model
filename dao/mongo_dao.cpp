#if 0
#include <vector>
#include <string>
#include <map>
#include <bsl/deque.h>
#include <fstream>
#include <iostream>
#include "mongo_dao.h"
#include "batchnormal.h"
#include "dao_data_path.h"

namespace lbs
{
    namespace dao
    {
        int MongoDao::write_mongo_to_file(std::string mongo_host, int port, std::string mongo_base, std::string mongo_table,
                std::string file_name, const db_mm::TableSchema* table_schema)
        {
        	int reuslt_count = 0;
        	std::ofstream out_file(file_name.c_str());
			if(!out_file)
			{
				std::cout << "Unable to open out_file"<<std::endl;
				return -1;
			}

			int iRet = cmongodb.open_db(mongo_host, port);
			if (iRet < 0)
			{
				out_file.close();
				printf("open_db failed, ret %d\n!", iRet);
				return iRet;
			}

			std::string query_str = "";
			std::string field_query = "";
			std::map<std::string, common::basedata_t> qurey_values;

			iRet = cmongodb.query_field(mongo_base, mongo_table, query_str, field_query);
			if (iRet < 0)
			{
				out_file.close();
				std::cout << "mongodb check_duplicate query data Error" << std::endl;
				return -1;
			}
			int count = 0;
			while (cmongodb.next_poi_sql_map(qurey_values))
			{
				count++;

				std::string mid = qurey_values["mid"].value;

				if(mid.empty())
				{
					continue;
				}

				int column_num = 0;

				column_num = table_schema->get_column_num();

				for (int i = 0; i < column_num; i++)
				{
					std::string str_end(1,'\0');
					std::string column_value;
					const char* column_name = NULL;

					column_name = table_schema->get_column_name(i);

					std::map<std::string, common::basedata_t>::iterator it = qurey_values.find(column_name);

					if(it != qurey_values.end())
					{
						out_file<<(it->second.value);
					}
					out_file<<str_end;
				}

				out_file<<std::endl;

				reuslt_count ++;

				if(count % 10000 == 0)
				{
					std::cout<<"success done "<<count<<" records."<<std::endl;
				}
			}

			std::cout<<"total success "<<count<<std::endl;

			cmongodb.close_db();
			out_file.close();

			return reuslt_count;
        }


        int MongoDao::init(const char* host, const char* db_name, int port)
        {
            int ret = 0;
            int table_id = 45;

            db_mm::SchemaManager* schema_manager = table_manager_->get_schema_manager();
            db_mm::TableSchema* table_schema = schema_manager->get_table_schema(table_id);
            std::string file_name = get_file_name(table_id);

            this->db_name = db_name;
            table_name = table_schema->get_table_name();

            int count = write_mongo_to_file(host, port, db_name, table_name, file_name, table_schema);

            if(count < 0)
            {
            	return -1;
            }

            table_schema->set_row_num(count);
            table_schema->set_has_data();
            table_schema->set_data_path(file_name.c_str());

            return ret;
        }

        int MongoDao::parse_row_to_mongo(db_mm::RowIndex* row_index, std::vector<common::basedata_t> & value_vec)
		{
			const db_mm::TableSchema* table_schema = NULL;
			int column_num = 0;
			const char* data = NULL;

			table_schema = row_index->get_table_schema();
			data = row_index->get_row_data();
			column_num = table_schema->get_column_num();


			for (int i = 0; i < column_num; i++)
			{
				common::basedata_t basedate;

				db_mm::TYPE data_type;
				const char* column_name = NULL;

				column_name = table_schema->get_column_name(i);
				data_type = table_schema->get_column_type(i);

				int offset = 0;

				offset = row_index->get_column_offset(i);

				switch (data_type)
				{
					case db_mm::INT_TYPE:
						basedate.key = column_name;
						basedate.value = data + offset;
						basedate.type = common::CONVERT_INT;
						break;
					case db_mm::DOUBLE_TYPE:
						basedate.key = column_name;
						basedate.value = data + offset;
						basedate.type = common::CONVERT_DOUBLE;
						break;
					case db_mm::VCHAR_TYPE:
					default:
						basedate.key = column_name;
						basedate.value = data + offset;
						basedate.type = common::CONVERT_STRING;
						break;
				}
				value_vec.push_back(basedate);
			}

			return 0;
		}

        int MongoDao::save()
        {
            std::string base_table = db_name + "." + table_name;

            bsl::deque<db_mm::RowIndex *> & update_queue = table_manager_->get_update_data();

            for(unsigned int i = 0; i < update_queue.size(); ++i)
            {
                if (45 != update_queue[i]->get_table_id())
                {
                    continue;
                }

            	std::vector<common::basedata_t> value_vec;
            	parse_row_to_mongo(update_queue[i], value_vec);
            	cmongodb.update_values(base_table, value_vec);
            }

            bsl::deque<db_mm::RowIndex *> & insert_queue = table_manager_->get_insert_data();

            for(unsigned int i = 0; i < insert_queue.size(); ++i)
			{
                if (45 != insert_queue[i]->get_table_id())
                {
                    continue;
                }

				std::vector<common::basedata_t> value_vec;
				parse_row_to_mongo(insert_queue[i], value_vec);
				cmongodb.insert_values(base_table, value_vec);
			}

            return 0;
        }
    }
}
#endif




