#include <sqlite3.h>
#include <string>
#include <fmt/core.h>

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

    bool run(void);
    int prepare(void);
    void params(sq_callback, void *);
    int step(void);
    std::string column_text(int);
    int column_int(int);
    bool column_bool(int);

 // private:
    sqlite3 *db;
    sq_callback callback;
    void *pArg;
    char **errmsg;
    std::string sql;
    sqlite3_stmt *res;
};
