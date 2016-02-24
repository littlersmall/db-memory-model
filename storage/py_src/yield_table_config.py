#!/usr/bin/env python

import os
from itertools import *

def get_data():
    for i in  os.popen('sh get_data.sh').readlines():
        yield i.strip().split('\t')


def group_by():

    index = 0

    for i in groupby(get_data(), lambda x:x[0]):
        #print [i[0], index, [group for group in i[1]]]
        if ('nav' in i[0] or 'poi' in i[0]):
            yield [i[0], index, [group for group in i[1]]]
            index += 1

table_info = [i for i in group_by()]

for i in table_info:
    f = open('%02d' % i[1] + '.table', 'w')

    f.write('[table]' + '\n')
    f.write('table_name : ' + i[0] + '\n')
    f.write('table_id : ' + str(i[1]) + '\n')
    f.write('column_num : ' + str(len(i[2])) + '\n')
    f.write('row_key : 0' + '\n')
    f.write('row_num : 0' + '\n')

    foreign_key_num = 0
    #foreign_key
    for j in i[2]:
        if 'F' == j[3]:
            foreign_key_num += 1

    f.write('foreign_key_num : ' + str(foreign_key_num) + '\n')

    foreign_index = 0

    for j in i[2]:
        if 'F' == j[3]:
            f.write('[.@foreign_key]' + '\n')
            table_name = j[4]
            column_name = j[5]

            table_id = 0

            for k in table_info:
                if k[0] == table_name:
                    table_id = k[1]
                    
                    f.write('table_id : ' + str(table_id) + '\n')
                    f.write('column_id : ' + str(foreign_index) + '\n')

                    break

        foreign_index += 1

    for index, j in enumerate(i[2]):
        f.write('[.@column]' + '\n')
        f.write('column_id : ' + str(index) + '\n')
        f.write('column_name : ' + j[1] + '\n')

        if (j[2] == 'double'):
            f.write('column_type : 10' + '\n')
        elif (j[2] == 'int'):
            f.write('column_type : 8' + '\n')
        else:
            f.write('column_type : 9' + '\n')

        
    f.close() 

#os.system("mv *.table ../conf/ttf_model/table_conf/")
#os.system("mv ../conf/ttf_model/table_conf/45.table ../conf/ttf_model/table_poi_conf/")

        

