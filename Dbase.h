#include <sqlite3.h>
#include <string>
#include <fmt/core.h>
#include <iostream>

//typedef int (*sq_callback)(void *, int, char **, char **); //sqlite callback type
using sq_callback = int (*)(void *, int, char **, char **);

class Sqlite {
  public:
    Sqlite(std:: string db_path);

    template<typename... Args>
    void query(fmt::string_view format_str, const Args & ... args) {
      fmt::format_args argspack = fmt::make_format_args(args...);
      sql = fmt::vformat(format_str, argspack);
    }

    template<typename... Args>
    int query2(fmt::string_view format_str, const Args & ... args) {
      fmt::format_args argspack = fmt::make_format_args(args...);
      sql = fmt::vformat(format_str, argspack);
      return sqlite3_prepare_v2(db, sql.c_str(), -1, &res, 0);
    }

    bool run(void);
    int prepare(void);
    void params(sq_callback, void *);
    int step(void);
    std::string column_text(int);
    int column_int(int);
    bool column_bool(int);
    void finalize(void);

    sqlite3 *db;
    sq_callback callback;
    void *pArg;
    char **errmsg;
    std::string sql;
    sqlite3_stmt *res;
};

class Query {
  public:
    //note the constructor must take a reference to db
    //if not you construct a copy and that doesn't work
    template<typename... Args>
    Query(Sqlite &db, fmt::string_view format_str, const Args & ... args) {
      fmt::format_args argspack = fmt::make_format_args(args...);
      sql = fmt::vformat(format_str, argspack);
      d = &db;
      //result = sqlite3_prepare_v2(db.db, sql.c_str(), -1, &db.res, 0);
      result = sqlite3_prepare_v2(db.db, sql.c_str(), -1, &res, 0);
    }

    ~Query() {
     std::cout << "About to finalize" << std::endl; 
     //sqlite3_finalize(d->res);
     sqlite3_finalize(res);
     std::cout << "Finalized" << std::endl; 
    }

    int step(void);
    std::string column_text(int);
    int column_int(int);
    bool column_bool(int);
    std::string sql;
    Sqlite * d;
    int result;
    sqlite3_stmt *res;
};
