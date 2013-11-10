#include <Global.h>
#include <FileDatabase.h>
#include <Helpers.h>
#include <Paths.h>

Database* OpenDatabase(const char* filename)
{
    Database* pDB = new Database(MakeFilePath(DB_PATH, filename));
    return pDB;
}

void DatabaseQuery(Query* q, const char* query, ... )
{
    char tmp[2048];

    va_list argList;
    va_start(argList, query);
    vsprintf(tmp,query,argList);
    va_end(argList);

    q->get_result(tmp);
}

Query* DatabaseHandler::GetTableContents(DatabaseTypes db, const char* table)
{
    if (db == MAX_DT)
        return NULL;

    Database* pDB = OpenDatabase(databaseFilenames[db]);
    Query *qry = new Query(*pDB);
    DatabaseQuery(qry, "SELECT * FROM %s", table);

    if (qry->num_rows() == 0)
        return NULL;

    return qry;
}

void DatabaseHandler::CleanupQuery(Query* qry)
{
    qry->free_result();
    delete qry;
}
