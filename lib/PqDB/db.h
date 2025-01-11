#pragma once
#ifndef MySDL_DB
#define MySDL_DB

#include "lib.h"

#define SQL_SELECT      0
#define SQL_INSERT      1
#define SQL_CREATE      2
#define SQL_UPDATE      3
#define SQL_ALTER       4
#define SQL_DROP        5
#define SQL_DELETE      6
#define SQL_TRUNCATE    7
#define SQL_GRANT       8
#define SQL_REVOKE      9
#define SQL_VACUUM      10



struct Statement {
    string name;
    string command;
    int nParams;
    int type;
    bool prepared;

    Statement(
        const string& name,
        const string& command,
        const int numOfParams,
        const int commandType
    ):  name(name), 
        command(command), 
        nParams(numOfParams), 
        type(commandType), 
        prepared(false) 
    {}
};

inline ostream& operator<<(ostream& os, const Statement& s){
    os << "Statement(";
        os << s.name;
        os << ", \"";
        os << s.command;
        os << "\", Type: ";
        os << s.type;
        os << ", Prepared: ";
        os << s.prepared;
        os << ", Params: ";
        os << s.nParams;
    os << ")";
    return os;
}


class DBResult {
    friend class DB;
private:
    PGresult* result;

public:
    DBResult(PGresult* res = nullptr) : result(res) {}
    ~DBResult() {
        if (result) PQclear(result);
    }



    /**
     * Returns if the result pointer is valid, not nullptr.
     * 
     * @return True if valid, False if nullptr
     */
    bool isValid() const {
        return result != nullptr;
    }



    /**
     * Returns the number of rows in the result.
     * It doesnt check if the result is valid so do it manually
     * before calling this function.
     * 
     * @return Number of rows
     */
    int rowCount() const {
        return PQntuples(result);
    }



    /**
     * Returns the number of columns in the result.
     * It doesnt check if the result is valid so do it manually
     * before calling this function.
     * 
     * @return Number of columns
     */
    int columnCount() const {
        return PQnfields(result);
    }



    /**
     * Gets the value of the specific cell using
     * provided row and column and saves it into
     * the string param value.
     * 
     * @param row The cell's row
     * @param column The cell's column
     * @param value Where the value of the cell will be saved
     * @return 0 on sucess and positive on error 
     */
    int getValue(int row, int column, string& value) const {
        if (!isValid()){
            return DB_INVALID_RESULT;
        }

        if (row >= rowCount() || column >= columnCount()){
            return DB_INVALID_ROW_COLUMN;
        }

        if (PQgetisnull(result, row, column) == 0) {
            return DB_INVALID_RES_VALUE;
        }

        value = PQgetvalue(result, row, column);
        return NO_ERROR;
    }

    ExecStatusType status() const {
        return PQresultStatus(result);
    }

    const char* errorMessage() const {
        return PQresultErrorMessage(result);
    }
};






class DB{
    private:
    static inline string dbName;
    static inline string dbUser;
    static inline string dbPass;
    static inline string dbAddr;
    static inline int    dbPort;

    static inline PGconn* dbConn = nullptr;

    public:
    static int init(
        const string& dbName,
        const string& username = "postgres",
        const string& password = "password",
        const string& hostAddr = "127.0.0.1",
        const int& port = 5432
    );

    static int prepareStatement(Statement& statement);
    static int execPrepared(Statement& statement, const char* params, DBResult& result);

    private:
    static bool checkResult(const PGresult* s, const int type);
};


#endif
// Creator: @AndrijaRD