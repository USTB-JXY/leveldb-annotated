#include <iostream>
#include <string>
#include <assert.h>
#include "leveldb/db.h"

using namespace std;

int main(void)
{
    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());

    delete db;
    
    return 0;
}