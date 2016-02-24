#if 0
#include <string>
#include <iostream>
#include <map>
#include "schema_manager.h"
#include "table_manager.h"
#include "table_schema.h"
#include "table_index.h"
#include "row_index.h"
#include "comm_compute.h"
#include "comm_log.h"
#include "cmongodb.h"

namespace lbs
{
	namespace dao
	{
		class MongoDao
		{
		public:
            MongoDao(db_mm::TableManager* table_manager) : table_manager_(table_manager) {}
			int write_mongo_to_file(std::string mongo_host, int port, std::string mongo_base, std::string mongo_table,
			            std::string file_name, const db_mm::TableSchema* table_schema);
			int init(const char* host, const char* db_name, int port);
			int save();
			int parse_row_to_mongo(db_mm::RowIndex* row_index, std::vector<common::basedata_t> & value_vec);

		private:
            db_mm::TableManager* table_manager_;
			common::CMongoDB cmongodb;
			std::string db_name;
			std::string table_name;
		};
	}
}


#endif
