#include "SqliteConnector.h"
Logger logger;

const std::string dataTypeMap[] = {
    "NULL",
    "INTEGER",
    "REAL",
    "TEXT",
    "BLOB"
};

const std::string tableParamMap[] = {
    "",
    "PRIMARY KEY",
    "NOT NULL"
};

SqliteConnector::SqliteConnector(const std::string& dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        logger.error("Something wrong happened when trying to open or create database file. The program will wxit now due to this error. ");
        sqlite3_close(db);
        throw SqliteUnableToOpenDbFile();
    }
    logger.info("Database initialized.");
}

SqliteConnector::~SqliteConnector() {
    if (db) {
        sqlite3_close(db);
    }
}

void SqliteConnector::createTable(const std::string &tableName,
    const std::vector<TableColumn> &tableColumns) {
    this->tableName = tableName;
    std::string sql = "create table if not exists " + tableName + " (";
    for(auto i = tableColumns.begin(); i != tableColumns.end(); i++) {
        std::string temp;
        temp.append(i->columnName + " " + dataTypeMap[i->dataType] + " " + tableParamMap[i->param]);
        if(++i != tableColumns.end()) {
            temp.append(", ");
        }
        --i;
        sql.append(temp);
        temp.clear();
    }
    sql.append(");");
    char* errMsg;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        logger.error(errMsg);
        sqlite3_free(errMsg);
    } else {
        logger.info("Create table completed. ");
    }
}

void SqliteConnector::insertData(const TableParams &tableParams) {
    int paramNum = tableParams.size() / 2;
    std::string sql = "insert into " + tableName + " (";
    for(int i = 0; i < paramNum; i++) {
        sql.append(tableParams.at(i));
        if(i < paramNum - 1) {
            sql.append(",");
        }
    }
    sql.append(") values (");
    for(int i = 0, j = paramNum; i < paramNum; i++, j++) {
        sql.append(tableParams.at(j));
        if(i < paramNum - 1) {
            sql.append(",");
        }
    }
    sql.append(");");
    char* errMsg;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);
    if(rc != SQLITE_OK) {
        logger.error(errMsg);
        sqlite3_free(errMsg);
    }
    else {
        logger.info("Value inserted successfully.");
    }
}

void SqliteConnector::deleteData(const TableParams &targetParams) {
    int paramNum = targetParams.size() / 2;
    std::string sql = "delete from " + tableName;
    if(paramNum != 0) {
        sql.append(" where ");
        for(int i = 0, j = paramNum; i < paramNum; i++, j++) {
            sql.append(targetParams.at(i) + "=" + targetParams.at(j));
            if(i < paramNum - 1) {
                sql.append(" AND ");
            }
        }
    }
    sql.append(";");
    char* errMsg;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);
    if(rc != SQLITE_OK) {
        logger.error(errMsg);
        sqlite3_free(errMsg);
    }
    else {
        logger.info("Value deleted successfully.");
    }
}

void SqliteConnector::updateData(const TableParams& targetParams, const TableParams& tableParams) {
    int paramNum = tableParams.size() / 2;
    int targetNum = targetParams.size() / 2;
    std::string sql = "update " + tableName + " set ";
    for(int i = 0, j = targetNum; i < targetNum; i++, j++) {
        sql.append(targetParams.at(i) + "=" + targetParams.at(j));
        if(i < targetNum - 1) {
            sql.append(",");
        }
    }
    if(paramNum != 0) {
        sql.append(" where ");
        for(int i = 0, j = paramNum; i < paramNum; i++, j++) {
            sql.append(targetParams.at(i) + "=" + targetParams.at(j));
            if(i < paramNum - 1) {
                sql.append(" AND ");
            }
        }
    }
    sql.append(";");
    char* errMsg;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errMsg);
    if(rc != SQLITE_OK) {
        logger.error(errMsg);
        sqlite3_free(errMsg);
    }
    else {
        logger.info("Value updated successfully.");
    }
}

void SqliteConnector::insertData(const std::string &formatStr) {
}

void SqliteConnector::deleteData(const std::string &formatStr) {
}

void SqliteConnector::updateData(const std::string &formatStr) {
}

SelectedItems* SqliteConnector::selectData() {
    SelectedItems* ret = new SelectedItems;
    std::string sql = "select * from " + tableName + ";";
    sqlite3_stmt* stmt = nullptr;
    const char* zTail;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &zTail) == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> temp;
            for(int i = 0; i < sqlite3_column_count(stmt); i++) {
                char str[400] = {0};
                sprintf(str, "%s", sqlite3_column_text(stmt, i));
                temp.push_back(str);
            }
            ret->push_back(temp);
        }
    }
    return ret;
}

SelectedItems* SqliteConnector::selectData(const std::string& tableName) {
    SelectedItems* ret = new SelectedItems;
    std::string sql = "select * from " + tableName + ";";
    sqlite3_stmt* stmt = nullptr;
    const char* zTail;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &zTail) == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> temp;
            for(int i = 0; i < sqlite3_column_count(stmt); i++) {
                char str[400] = {0};
                sprintf(str, "%s", sqlite3_column_text(stmt, i));
                temp.push_back(str);
            }
            ret->push_back(temp);
        }
    }
    return ret;
}

SelectedItems * SqliteConnector::selectData(const TableParams &targetParams, const TableParams &tableParams) {
    SelectedItems* ret = new SelectedItems;
    std::string sql = "select ";
    for(int i = 0; i < targetParams.size(); i++) {
        sql.append(targetParams.at(i));
        if(i < targetParams.size() - 1) {
            sql.append(",");
        }
    }
    sql.append(" from " + tableName);
    if(tableParams.size() == 0) {
        sql.append(";");
    }
    else {
        int paramNum = tableParams.size() / 2;
        sql.append(" where ");
        for(int i = 0, j = paramNum; i < paramNum; i++, j++) {
            sql.append(targetParams.at(i) + "=" + targetParams.at(j));
            if(i != paramNum - 2) {
                sql.append(" AND ");
            }
        }
    }
    sqlite3_stmt* stmt = nullptr;
    const char* zTail;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &zTail) == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> temp;
            for(int i = 0; i < sqlite3_column_count(stmt); i++) {
                char str[400] = {0};
                sprintf(str, "%s", sqlite3_column_text(stmt, i));
                temp.push_back(str);
            }
            ret->push_back(temp);
        }
    }
    return ret;
}

SelectedItems * SqliteConnector::selectData(const std::string &tableName, const TableParams &targetParams,
    const TableParams &tableParams) {
    SelectedItems* ret = new SelectedItems;
    std::string sql = "select ";
    for(int i = 0; i < targetParams.size(); i++) {
        sql.append(targetParams.at(i));
        if(i < targetParams.size() - 1) {
            sql.append(",");
        }
    }
    sql.append(" from " + tableName);
    if(tableParams.size() == 0) {
        sql.append(";");
    }
    else {
        int paramNum = tableParams.size() / 2;
        sql.append(" where ");
        for(int i = 0, j = paramNum; i < paramNum; i++, j++) {
            sql.append(targetParams.at(i) + "=" + targetParams.at(j));
            if(i != paramNum - 2) {
                sql.append(" AND ");
            }
        }
    }
    sqlite3_stmt* stmt = nullptr;
    const char* zTail;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &zTail) == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> temp;
            for(int i = 0; i < sqlite3_column_count(stmt); i++) {
                char str[400] = {0};
                sprintf(str, "%s", sqlite3_column_text(stmt, i));
                temp.push_back(str);
            }
            ret->push_back(temp);
        }
    }
    return ret;
}

