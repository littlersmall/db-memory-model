/*
 * row_instance.h
 *
 *  Created on: 2014年4月15日
 *      Author: chenzhixiang
 */

#ifndef ROW_INSTANCE_H_
#define ROW_INSTANCE_H_


#include "row_index.h"
#include "table_schema.h"
#include "ttf_table.h"
#include <list>
#include "comm_log.h"
#include "comm_def.h"
#include "row_index.h"
#include "comm_compute.h"
#include "table_manager.h"

namespace lbs 
{
    namespace db_mm
    {
        class TableManager;
        class RowIndex;
    }

    namespace sdk 
    {
        class RowInstance
        {
        public:
            RowInstance(){m_iTabId = -1; row_index_ = NULL;}
            RowInstance(int table_id);
            int get_table_id();

            /*
             * 根据某个表的主键获取RowInstance对象
             * */
            int getRowByKey(const char* key, RowInstance& value);

             /**
             * @获取某个表related_table_id中与该link关联的行对象
             *
             * @param [in] form   : related_table_id 关联表的编码,参考base_row.h文件中定义的sdk空间下常量
             * @return  std::list<RowInstance> 关联的对象之间的列表
             * @todo 可以根据此函数获取某个表的一行对象, 可以拿到id对象后从proxylayer中寻找相关要素
             * 比如link_id=123, 需要找到一次link为关联的所有交限: 首先找到以此link为进入线的nav_restriction表中的关联项
             * 然后找nav_restriction_detail表中的以此link为退出线的detail_id,获取restict_id
             * 然后找nav_restriction_pass表中以此link为经过线的detail_id
            **/
            int getRelatedObject(int related_table_id,std::list<RowInstance> & relatedObjs);

            db_mm::RowIndex* get_row_index()
            {
                return row_index_;
            }

            /**
             * @brief
             *
             * @param [in] row_index   : db_mm::RowIndex*
             * @return  int
             * @todo
            **/
            void set_data(db_mm::RowIndex* row_index);

            /**
             * @brief attention!! 最快的方式，但是没有任何错误检查，慎用
             *
             * @param [in] index   : const int
             * @return  const char*
             * @todo
            **/
            inline const char* get(const int index)
            {
                return (row_index_->get_row_data()
                        + row_index_->get_column_offset(index));
            }

            /**
             * @brief
             *
             * @param [in] column_name   : const char*
             * @return  const char*
             * @todo
            **/
            const char* get(const char* column_name)
            {
                int column_id = row_index_->get_column_id(column_name);

                return get(column_id);
            }

            /**
             * @brief
             *
             * @param [in] index   : const int
             * @param [out] data   : T&
             * @return  template <class T>             int
             * @todo
            **/
            template <class T>
            int get(const int index, T& data)
            {
                return common::parse_str(get(index), data);
            }

            /**
             * @brief
             *
             * @param [in] column_name   : const char*
             * @param [out] data   : T&
             * @return  int
             * @todo
            **/
            template <class T>
            int get(const char* column_name, T& data)
            {
                return common::parse_str(get(column_name), data);
            }

            /**
             * @brief
             *
             * @param [in] column_name   : const char*
             * @param [out] data   : T&
             * @return  template <class T>             int
             * @todo
            **/
            template <class T>
            int set(const char* column_name, T& data)
            {
                int ret = 0;
                int column_id = row_index_->get_column_id(column_name);

                if (common::ITERATOR_END != column_id)
                {
                  char column_value[common::MAX_COLUMN_LENGTH] = {};

                  common::parse_value(data, column_value);

                  ret = row_index_->set_column_data(column_id, column_value);
                }

                else
                {
                  ret = common::ITERATOR_END; 
                }

                return ret;
            }

            bool del()
            {
                int ret = false;

                if (row_index_)
                {
                    ret = row_index_->del();
                }

                return ret;
            }

            bool operator < (const RowInstance& other) const
            {
                bool ret = false;

                if (row_index_ && other.row_index_)
                {
                    std::string row_key_l = row_index_->get_row_data();
                    std::string row_key_r = other.row_index_->get_row_data();      

                    ret = (row_key_l < row_key_r);
                }

                return ret;
            }

            bool incomplete()
            {
                bool ret = false;

                if (row_index_)
                {
                    char incomplete_key[1024] = {};

                    snprintf(incomplete_key, sizeof(incomplete_key), 
                        "%d_%s", m_iTabId, row_index_->get_row_data());
                    ret = row_index_->get_table_manager()->incomplete(incomplete_key);
                }

                return ret;
            }

            virtual ~RowInstance();

            //protected:
             /**
            * @brief
            *
            * @param [in] index   : const int
            * @return  int
            * @todo
            **/
            void set_data(const int index, db_mm::TableManager* table_manager);

            /**
             * @brief
             *
             * @param [in] row_key   : const char*
             * @return  int
             * @todo
            **/
            void set_data(const char* row_key, db_mm::TableManager* table_manager);

        protected:
            int m_iTabId;
            db_mm::RowIndex* row_index_;
        };
    } /* namespace sdk */
} /* namespace lbs */

#endif /* ROW_INSTANCE_H_ */
