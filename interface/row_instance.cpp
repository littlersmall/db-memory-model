/*
 * row_instance.cpp
 *
 *  Created on: 2014年4月15日
 *      Author: xingyi01(xingyi01@baidu.com)
 */

#include "row_instance.h"
#include "table_schema.h"
#include "table_index.h"
#include "row_index.h"
#include "comm_def.h"
#include "table_manager.h"

namespace lbs {
namespace sdk {

RowInstance::RowInstance(int table_id) :m_iTabId(table_id)
{
	// TODO Auto-generated constructor stub
	row_index_ = NULL;
}



int RowInstance::get_table_id()
{
	return m_iTabId;
}

int RowInstance::getRowByKey(const char* key, RowInstance& value)
{
	if(!this->row_index_) return -1;
	/**
	db_mm::TableIndex* table_index;
	db_mm::RowIndex* row_index;
	 */
	db_mm::TableManager* pTableMgr=row_index_->get_table_manager();
	value.set_data(key,pTableMgr);
	return 0;
}

int RowInstance::getRelatedObject(int related_table_id,std::list<RowInstance> & relatedObjs)
{
	if(!this->row_index_) return -1;
	db_mm::IndexList* pList=this->row_index_->get_reverse_index();
	relatedObjs.clear();

	for(; pList; pList=pList->next)
	{
	   if(pList->row_index->get_table_id()==related_table_id)
	   {
		   RowInstance ri(related_table_id);
		   ri.set_data(pList->row_index);
	   }
	}
	return 0;
}


void RowInstance::set_data(const int index, db_mm::TableManager* table_manager)
{
   row_index_ = table_manager->get_table_index(get_table_id())->get_row_index(index);
}

void RowInstance::set_data(db_mm::RowIndex* row_index)
{
   if (NULL == (row_index_ = row_index))
   {
	   COM_WRITELOG(COMLOG_WARNING, "set_data error, row_index is NULL");
   }
   m_iTabId = row_index_->get_table_id();
}

void RowInstance::set_data(const char* row_key, db_mm::TableManager* table_manager)
{
   if (NULL == row_key)
   {
	   COM_WRITELOG(COMLOG_WARNING, "set_node error, node_id is NULL");
   }

   else
   {
	   row_index_ = table_manager->get_table_index(get_table_id())->get_row_index(row_key);
   }
}



RowInstance::~RowInstance() {
	// TODO Auto-generated destructor stub
}

} /* namespace sdk */
} /* namespace lbs */
