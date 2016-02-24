# db-memory-model
微型内存数据库


一 内存模型整体结构图
![](http://upload-images.jianshu.io/upload_images/1397675-455d9a449b3315d1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
二 sdk和strorage
![](http://upload-images.jianshu.io/upload_images/1397675-776c78e084e9b6ef.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
三 dao和storage
![](http://upload-images.jianshu.io/upload_images/1397675-663464e08e342037.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
四 DAO
1 Database dao
![](http://upload-images.jianshu.io/upload_images/1397675-573f2f8a7a24aa3b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
注：其中的init和save函数因参数和返回值不同，因此无法合并到SqlDb中。
2 Json Dao
![](http://upload-images.jianshu.io/upload_images/1397675-4bce63ef64af6f86.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240) 
五 Storage
![](http://upload-images.jianshu.io/upload_images/1397675-75b20d8e1fcadc10.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
1 mm_allocator
内存分配器。
(1)集中分配内存，每次64M
(2)集中释放
(3)标记使用方
(4)打印内存使用情况
2 comm_def
(1) 定义错误码
(2) 定义公用函数
3 table_manager
相当于一个数据库
(1) 载入数据
(2) 设置反向索引
(3) 释放所有内存
4 schema_manager
一个数据库的所有的表结构
(1) 载入所有table的配置文件(配置文件描述了表的结构和主键外键)
(2) 生成每个表的结构，包括列名，类型，外键关系，反向外键关系
5 table_schema
对应于一个表的表结构
(1) 生成每个表的主键，列，外键等关系
(2) 根据每列的顺序，维护一个列名到列id的哈希索引
(3) 并维护每个表的数据的具体位置
6 file_manager
(1) 管理文件名
(2) 遍历一个目录按条件获取文件名，并按照文件名排序
(3) 提供关于文件名的遍历操作
7 table_index
对于于数据库的一个表
(1) 维护一个表关于主键的哈希索引
(2) 提供添加一行数据的操作
(3) 将固定格式的文件数据通过mmap的方式载入内存
8 row_index
对应于数据库的一行
(1) 维护每行数据的真实地址指针
(2) 维护每行的当前状态(正常，被更新，已删除)
(3) 维护每行数据中，每一列数据位置的索引
(4) 维护每行数据的反向索引
(5) 提供修改列数据的接口
