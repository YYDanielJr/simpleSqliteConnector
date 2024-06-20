#pragma once
#include "sqlite3.h"
#include <string>
#include "Logger.h"
#include <vector>
#include <cstdio>
struct TableParam;
typedef std::vector<std::string> TableParams;
typedef std::vector<std::vector<std::string>> SelectedItems;

#define NULL 0
#define INTEGER 1
#define REAL 2
#define TEXT 3
#define BLOB 4

#define PRIMARY_KEY 1
#define NOT_NULL 2

class SqliteUnableToOpenDbFile final : public std::exception {
    const char* what() const throw() {
        return "Unable to open or create database file";
    }
};

typedef struct TableColumn {
    int dataType;
    std::string columnName;
    int param;
} TableColumn;

class SqliteConnector {
private:
    sqlite3* db;
    std::string tableName;
public:
    SqliteConnector() {}
    SqliteConnector(const std::string& dbPath);
    ~SqliteConnector();
    // 创建表格。第一个参数是表名，第二个参数是每列的数据类型、列名、列参数。
    void createTable(const std::string& tableName, const std::vector<TableColumn>& tableColumns);
    // 传入一个字符串数组
    void insertData(const TableParams& tableParams);
    // 传入一个字符串数组
    void deleteData(const TableParams& targetParams);
    // 传入两个字符串数组，前者为需要更新的目标，后者为需要更新的特征项
    void updateData(const TableParams& targetParams, const TableParams& tableParams);
    // 格式字符串的格式如下：
    // 列1,列2,...,列n;值1,值2,...,值n
    void insertData(const std::string& formatStr);
    // 格式字符串的格式如下：
    // 删除项1,删除项2,...,删除项n;值1,值2,...,值n
    void deleteData(const std::string& formatStr);
    // 格式字符串的格式如下：
    // 更新项1,更新项2,...,更新项n;值1,值2,...,值n
    void updateData(const std::string& formatStr);
    SelectedItems* selectData();
    SelectedItems* selectData(const std::string& tableName);
    SelectedItems* selectData(const TableParams& targetParams, const TableParams& tableParams);
    SelectedItems* selectData(const std::string& tableName, const TableParams& targetParams, const TableParams& tableParams);
    static void deleteSelectedItems(SelectedItems* selectedItems) {
        if(selectedItems != nullptr) {
            delete selectedItems;
            selectedItems = nullptr;
        }
    }
};
