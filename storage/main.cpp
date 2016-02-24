/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file main.cpp
 * @author xingyi01(com@baidu.com)
 * @date 2014/01/17 16:39:45
 * @brief 
 *  
 **/

#include "table_schema.h"
#include "table_manager.h"
#include "schema_manager.h"
#include "row_index.h"
#include "table_index.h"
#include "mm_allocator.h"
#include "json_file_dao.h"
#include <iostream>
#include "comm_log.h"
#include "Configure.h"
#include "mysql_dao.h"
#include "sqlite_dao.h"

using namespace std;
using namespace lbs::db_mm;
using namespace lbs::common;
using namespace lbs::dao;
using namespace lbs;

int main()
{
    //log
    //com_device_t dev[1];
    //snprintf(dev[0].file, sizeof(dev[0].file), 
    //        "/home/users/xingyi01/memmodel/log/db_mm.log");
    //snprintf(dev[0].type, sizeof(dev[0].type), "FILE");

    //COMLOG_SETSYSLOG(dev[0]);
    //com_openlog("db_memory_model", dev, 1, NULL);  

    COM_WRITELOG(COMLOG_NOTICE, "START");

    //start 

    //std::vector<std::string> table_name;

    //SchemaManager schema_manager(46);
    //schema_manager.init("./storage/conf/");
    //schema_manager.print();

    //JsonFileDao dao(&schema_manager);

    //dao.init("./test.json", "./");

    
    /*
    table_name.push_back("nav_node");
    table_name.push_back("nav_link");
    */

    //table_manager.init("./config/", "./data/", false, false);
    SchemaManager* schema_manager = new SchemaManager;
    schema_manager->init("storage/conf/ttf_model/table_poi_conf", 1);
    TableManager* table_manager = new TableManager(schema_manager);
    
    SqliteDao sqlite_dao(table_manager);

    sqlite_dao.init();
    table_manager->init_data();

    TableIndex* table_index = table_manager->get_table_index(45);
    RowIndex* row_index = table_index->add();

    row_index->set_column_data(0, "123456");
    row_index = table_index->get_row_index(0);

    row_index->set_column_data(1, "3456");
    row_index->set_column_data(0, "1111111");
    //row_index->print();

    //row_index = table_index->get_row_index("9dfb9e22-b5e7-11e3-b15d-40f2e90bae14");
    //row_index->del();

    cout << sqlite_dao.save() << endl;

    /*
    TableSchema* table_schema = table_manager->get_schema_manager()->get_table_schema(45);
    table_schema->set_data_path("./storage/data/45.data");
    table_schema->set_has_data();
    table_schema->set_row_num(24372680);
    */

    /*
    MysqlDao mysql_dao(table_manager);
    mysql_dao.init();
    table_manager->init_data();

    TableIndex* table_index = table_manager->get_table_index(1);
    
    RowIndex* row_index = table_index->add();
    row_index->set_column_data(0, "123456");
    row_index->set_column_data(1, "5");

    row_index = table_manager->get_table_index(0)->get_row_index(0);
    row_index->set_column_data(1, "123");
    row_index->set_column_data(18, "0");
    row_index->set_column_data(18, "1");
    row_index->set_column_data(18, "2");
    row_index->set_column_data(18, "2");

    row_index->del();

    row_index = table_manager->get_table_index(0)->add();
    row_index->set_column_data(0, "654321");
    cout << "set_data return" << row_index->set_column_data(0, "xxxx") << endl;


    cout << mysql_dao.save() << endl;
    */

    table_manager->get_allocator()->print();
    

    delete(table_manager);
    /*
    TableIndex* table_index = NULL;
    TableManager::get_table_manager()->get_table_index(0, table_index);

    RowIndex* row_index = NULL;

    table_index->get_row_index("2", row_index);
    row_index->set_column_data(2, "aaaaa");

    TableManager::get_table_manager()->update_data();
    */

    //table_manager.print();

    //ConnectiveCheck connective_check(table_manager);
    //connective_check.do_check(7, 0);

    //RoadTest road_test(table_manager);
    //road_test.do_check(0, 1);

    /*
    RoadTestSdk road_test_sdk;
    road_test_sdk.do_check();
    road_test_sdk.do_check_2();
    */

    //TableManager::get_table_manager()->destroy();

    
    //MMAllocator::get_allocator()->delete_mm();

    COM_WRITELOG(COMLOG_NOTICE, "END");
    //com_closelog(6000);

    return 0;
}





















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
