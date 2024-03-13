#include "DatabaseAccess.h"

bool DatabaseAccess::open()
{
    std::string dbFileName = "galleryDB.sqlite";

    // Open db
    int file_exist = _access(dbFileName.c_str(), 0);
    int res = sqlite3_open(dbFileName.c_str(), &this->_db);
    if (res != SQLITE_OK) {
        this->_db = nullptr;
        std::cout << "Failed to open DB" << std::endl;
        return false;
    }
    if (file_exist != 0) {
        const char* sqlStatement =
            "CREATE TABLE IF NOT EXISTS Users ("
            "ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            "NAME TEXT"
            ");"

            "CREATE TABLE IF NOT EXISTS Albums ("
            "ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            "NAME TEXT, "
            "CREATION_DATE DATE, "
            "USER_ID INTEGER, "
            "FOREIGN KEY(USER_ID) REFERENCES Users(ID)"
            ");"

            "CREATE TABLE IF NOT EXISTS Pictures ("
            "ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            "NAME TEXT, "
            "LOCATION TEXT, "
            "CREATION_DATE DATE, "
            "ALBUM_ID INTEGER, "
            "FOREIGN KEY(ALBUM_ID) REFERENCES Albums(ID)"
            ");"

            "CREATE TABLE IF NOT EXISTS Tags ("
            "ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            "PICTURE_ID INTEGER, "
            "USER_ID INTEGER, "
            "FOREIGN KEY(PICTURE_ID) REFERENCES Pictures(ID), "
            "FOREIGN KEY(USER_ID) REFERENCES Users(ID)"
            ");";

        char* errMessage = nullptr;
        res = sqlite3_exec(this->_db, sqlStatement, nullptr, nullptr, &errMessage);
        if (res != SQLITE_OK) {
            std::cout << "Failed to execute SQL statement: " << errMessage << std::endl;
            sqlite3_free(errMessage);
            return false;
        }
    }

    return true;
}
