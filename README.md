# Sqlite Connector && Logger日志记录器 API文档
## 使用方式：<br>
1.创建自己的C++项目（**请注意是C++而不是C项目**）；<br>
2.将`SqliteConnector.h`，`SqliteConnector.cpp`，`Logger.h`，`Logger.cpp`四个文件添加进自己的项目，可以通过`CMakeLists.txt`添加，也可以通过全部同时编译的方式；<br>
3.include两个头文件。<br>
## Sqlite Connector API文档：
### 1.创建一个SqliteConnector对象
```c++
SqliteConnector() {}
SqliteConnector(const std::string& dbPath);
```
该类有默认的无参构造对象，但我更建议在构造对象时传入一个字符串，该字符串代表数据库的存储路径。
### 2.使用该对象进行增删改查的操作
#### a.创建表格
调用`createTable`函数来创建一张新表格。该函数原型如下：
```c++
typedef struct TableColumn {
    int dataType;
    std::string columnName;
    int param;
} TableColumn;

void createTable(const std::string& tableName, const std::vector<TableColumn>& tableColumns);
```
该函数需要传入两个参数，第一个参数是表名，第二个参数是一个结构体数组，每个数组元素由数据类型、列名、参数组成。<br>
数据类型和参数有默认的宏定义：
```c++
// 数据类型的宏定义
#define NULL 0
#define INTEGER 1
#define REAL 2
#define TEXT 3
#define BLOB 4

// 列参数的宏定义
#define NULL 0  // 即无参数
#define PRIMARY_KEY 1
#define NOT_NULL 2
```

如果创建表格过程正确，会输出`Create table completed.`。<br>
例，可以用这样的方式创建一个名为dyy的表，这张表有两列，第一列叫c1，为整数类型，第二列为c2，为文本类型。
```c++
SqliteConnector* connector = new SqliteConnector("dyy.db");
std::vector<TableColumn> table = {{1, "c1", 1}, {3, "c2", 0}};
connector->createTable("dyy", table);
```
#### b.插入数据
调用`insertData`函数插入新行。函数原型如下：
```c++
typedef std::vector<std::string> TableParams;

void insertData(const TableParams& tableParams);
```
该函数接受一个字符串数组作为参数。这个字符串数组是插入列名和插入数据的集合。
该数组的前半部分是列名，后半部分是插入的数据。两部分的个数应该完全相等，因此tableParams包含的字符串个数应为偶数。
请注意，插入数据如果为字符串，需要自己添加引号。<br>
例，向上述表格内插入一行数据，为{114514, "henghengheng"}。
```c++
std::vector<std::string> insert = {"c1", "c2", "114514", "\"henghengheng\""};
connector->insertData(insert);
```
#### c.查询数据
调用selectData函数来查询该表内的数据。<br>
selectData函数有以下的重载：
```c++
typedef std::vector<std::vector<std::string>> SelectedItems;

SelectedItems* selectData();
SelectedItems* selectData(const std::string& tableName);
SelectedItems* selectData(const TableParams& targetParams, const TableParams& tableParams);
SelectedItems* selectData(const std::string& tableName, const TableParams& targetParams, const TableParams& tableParams);
```
第一个函数原型无参数，默认查询当前表内的所有数据；<br>
第二个函数参数为表名，查询给定表内的所有数据；<br>
第三个函数第一个参数是需要查询的列名，为字符串数组，第二个参数是限定条件，即“WHERE”语句后的限定条件。该函数查询当前表的相关数据。<br>
第四个函数在第三个函数之前再传入表名。该函数传入给定表内的相关数据。<br>
后两个函数如果没有限定条件，则传入一个空TableParams对象。<br>
<br>
四个函数均返回一个字符串二维数组，为二维的表内容。<br>
例，表内存在这样的数据：
| - | - |
| 111 | aaa |
| 222 | bbb |
| 333 | ccc |
执行查询语句：
```c++
SelectedItems* tableP = connector->selectData();
for(auto i = tableP->begin(); i != tableP->end(); i++) {
    for(auto j = i->begin(); j != i->end(); j++) {
        std::cout << *j << "\t";
    }
    std::cout << std::endl;
}
```
得到输出如下：
```c++
111	aaa	
222	bbb	
333	ccc
```

## Logger API文档
在SqliteConnector类内已经创建了Logger对象。在你的代码内开头加入如下代码：
```c++
extern Logger logger;
```
可以直接使用该对象。
### 一般消息日志
```c++
logger.info("这是一般消息");
```
### 错误消息日志
```c++
logger.error("这是错误消息");
```