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
            "CREATE TABLE USERS (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT NOT NULL); "
            "CREATE TABLE ALBUMS (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT NOT NULL, USER_ID INTEGER NOT NULL, CREATION_DATE TEXT NOT NULL, FOREIGN KEY(USER_ID) REFERENCES USERS(ID)); "
            "CREATE TABLE PICTURES (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT NOT NULL, LOCATION TEXT NOT NULL, CREATION_DATE TEXT NOT NULL, ALBUM_ID INTEGER NOT NULL, FOREIGN KEY(ALBUM_ID) REFERENCES ALBUMS(ID)); "
            "CREATE TABLE TAGS (PICTURE_ID INTEGER NOT NULL, USER_ID INTEGER NOT NULL, PRIMARY KEY(PICTURE_ID, USER_ID), FOREIGN KEY(PICTURE_ID) REFERENCES PICTURES(ID), FOREIGN KEY(USER_ID) REFERENCES USERS(ID));";

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

void DatabaseAccess::close()
{
    sqlite3_close(this->_db);
    this->_db = nullptr;
}

void DatabaseAccess::clear()
{
    this->db_albums.clear();
    this->db_users.clear();
}