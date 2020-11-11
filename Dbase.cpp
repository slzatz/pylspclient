#include "Dbase.h"
#include <iostream>

Sqlite::Sqlite(std::string db_path) {
  int rc = sqlite3_open(db_path.c_str(), &db);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    exit(1);
  }
}

int Query::step(void) {
  return sqlite3_step(res);
 }
std::string Query::column_text(int col) {
  return std::string(reinterpret_cast<const char *>(sqlite3_column_text(res, col)));
}

int Query::column_int(int col) {
  return sqlite3_column_int(res, col);
}

bool Query::column_bool(int col) {
  return (sqlite3_column_int(res, col) == 1) ? true : false;
}

