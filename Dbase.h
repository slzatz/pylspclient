#include <sqlite3.h>
#include <string>
#include <fmt/core.h>
//#include <iostream>

//typedef int (*sq_callback)(void *, int, char **, char **); //sqlite callback type
using sq_callback = int (*)(void *, int, char **, char **);

class Sqlite {
  public:
    Sqlite(std::string db_path);

    sqlite3 *db;
};

class Query {
  public:
    //note the constructor must take a reference to db
    //if not you construct a copy and that doesn't work
    template<typename... Args>
    Query(Sqlite &db, fmt::string_view format_str, const Args & ... args) {
      fmt::format_args argspack = fmt::make_format_args(args...);
      sql = fmt::vformat(format_str, argspack);
      result = sqlite3_prepare_v2(db.db, sql.c_str(), -1, &res, 0);
    }

    ~Query() {
     sqlite3_finalize(res);
     fmt::print("Finalized {}\n", sql);
    }

    int step(void);
    std::string column_text(int);
    int column_int(int);
    bool column_bool(int);

    std::string sql;
    int result;
    sqlite3_stmt *res;
};
