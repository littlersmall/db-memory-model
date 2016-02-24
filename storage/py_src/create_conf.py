import os
from itertools import *

cd = 'create_table_cd_mif.sql'
sw = 'create_table_sw_mif.sql'

def get_table():
    tables = ''.join(open(sw).readlines())

    for i in tables.strip().split('TABLE'):
        yield i

def get_table_column():
    for i in get_table():
        table_name = i.split('\n')[0].strip()

        if ('(' in i) and (');' in i):
            s_index = i.find('(')
            e_index = i.find(');')
            yield table_name + '\n' + i[s_index+1:e_index]

def get_each_table_schema():
    for index,i in enumerate(get_table_column()):
        for j in i.split('\n'):
            if j.strip():
                yield str(index) + ' ' + j.strip()

def create_table_conf():
    for key, item in groupby(get_each_table_schema(), lambda x:x.split(' ')[0]):
        #key item
        table_file = None
        item_list = list(item)
        for index, i in enumerate(item_list):
            if 0 == index:
                table_name = i
                table_file = open('%02d.table' % int(key), 'w')
                
                table_file.write('[table]' + '\n')
                table_file.write('table_name : ' + i.split('.')[-1] + '\n')
                table_file.write('table_id : ' + str(key) + '\n')
                table_file.write('column_num : ' + str(len(item_list)) + '\n')
                table_file.write('row_key : 0' + '\n')
                table_file.write('row_num : 0' + '\n')
                table_file.write('foreign_key_num : ' + '0' + '\n')

                table_file.write('[.@column]' + '\n')
                table_file.write('column_id : ' + '0' + '\n')
                table_file.write('column_name : ' + 'ctid' + '\n')
                table_file.write('column_type : 9' + '\n')
 
            else:
                table_file.write('[.@column]' + '\n')
                table_file.write('column_id : ' + str(index) + '\n')
                table_file.write('column_name : ' + i.split(' ')[1] + '\n')
                
                if 'INTEGER' in ''.join(i):
                    table_file.write('column_type : 9' + '\n')
                elif ('DOUBLE' in ''.join(i)) or ('DECIMAL' in ''.join(i)):
                    table_file.write('column_type : 10' + '\n') 
                else:
                    table_file.write('column_type : 8' + '\n')
             
        table_file.close()    

create_table_conf()

os.system("mv *.table sw_model")
