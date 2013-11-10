#ifndef TDW_DATABASE_H
#define TDW_DATABASE_H

#include "libsqlitewrapped.h"
#include "sqlite3.h"

enum DatabaseTypes
{
    DT_MEDIA = 0,
    MAX_DT
};

static const char* databaseFilenames[] = {
    {"media.db"}
};

class DatabaseHandler
{
    public:
        static Query* GetTableContents(DatabaseTypes db, const char* table);
        static void CleanupQuery(Query* qry);

    private:
        //
};

#endif
