#include "Dbase.h"
#include <iostream>

//typedef int (*sq_callback)(void *, int, char **, char **); //sqlite callback type
//using sq_callback = int (*)(void *, int, char **, char **);

Sqlite::Sqlite(std::string db_path) {
  int rc = sqlite3_open(db_path.c_str(), &db);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    exit(1);
  }
}

/*
bool Sqlite::run() {
   int rc = sqlite3_exec(db, sql.c_str(), callback, pArg, errmsg);
   if (rc != SQLITE_OK) {
     //outlineShowMessage("SQL error in %s: %s", func, errmsg);
     sqlite3_free(errmsg);
     return false;
   }
   pArg = nullptr;
   callback = nullptr;
   return true;
}

void Sqlite::params(sq_callback cb, void * arg) {
  callback = cb;
  pArg = arg;
}

int Sqlite::prepare(void) {
  return sqlite3_prepare_v2(db, sql.c_str(), -1, &res, 0);
}

int Sqlite::step(void) {
  return sqlite3_step(res);
 }

void Sqlite::finalize(void) {
  sqlite3_finalize(res);
}

std::string Sqlite::column_text(int col) {
  return std::string(reinterpret_cast<const char *>(sqlite3_column_text(res, col)));
}

int Sqlite::column_int(int col) {
  return sqlite3_column_int(res, col);
}

bool Sqlite::column_bool(int col) {
  return (sqlite3_column_int(res, col) == 1) ? true : false;
}
*/

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
/*
std::string Sqlite::column_text(sqlite3_stmt *r, int col) {
  return std::string(reinterpret_cast<const char *>(sqlite3_column_text(r, col)));
}

int Sqlite::column_int(sqlite3_stmt *r, int col) {
  return sqlite3_column_int(r, col);
}

bool Sqlite::column_bool(sqlite3_stmt *r, int col) {
  return (sqlite3_column_int(r, col) == 1) ? true : false;
}
*/
/*
Sqlite lm_db(LM_DB);
Sqlite fts_db(FTS_DB);

void do_something() {
  lm_db.query("fjalkdsf {} lkdsjflskdjflds {} lksdjfldsj {}", x, y, z);
  lm_db.run_query()
*/  
