#include "DatabaseAccess.h"

bool DatabaseAccess::open()
{
    // Open db
    int file_exist = _access(DB_FILE_NAME, 0);
    int res = sqlite3_open(DB_FILE_NAME, &this->_db);
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

        executeSqlQuery(sqlStatement);
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

}

void DatabaseAccess::executeSqlQuery(const char* sql)
{
    sqlite3* db;
    int res = sqlite3_open("galleryDB.sqlite", &db);
    if (res != SQLITE_OK) {
        std::cout << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    char* errMessage = nullptr;
    res = sqlite3_exec(db, sql, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cout << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage);
    }

    sqlite3_close(db);
}

void DatabaseAccess::executeSqlQueryWithCallback(const char* sql, int(*callback)(void*, int, char**, char**), void* data)
{
    sqlite3* db;
    int res = sqlite3_open("galleryDB.sqlite", &db);
    if (res != SQLITE_OK) {
        std::cout << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    char* errMessage = nullptr;
    res = sqlite3_exec(db, sql, callback, data, &errMessage);
    if (res != SQLITE_OK) {
        std::cout << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage);
    }

    sqlite3_close(db);
}


void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
    std::string deleteAlbumStatement =
        "DELETE FROM ALBUMS "
        "WHERE NAME = '" + albumName + "' "
        "AND USER_ID = " + std::to_string(userId) + ";"

        "DELETE FROM PICTURES "
        "WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "');";

    executeSqlQuery(deleteAlbumStatement.c_str());
}

Album DatabaseAccess::openAlbum(const std::string& albumName)
{
    // basically here we would like to delete the allocated memory we got from openAlbum
}

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
    std::string tagUserStatement =
        // tag user statement
        "INSERT INTO TAGS (PICTURE_ID, USER_ID) "
        "VALUES "
        "((SELECT ID FROM PICTURES WHERE NAME = '" + pictureName + "' AND ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "')), " // find picture id by using the name of the album and the name of the picture
        "'" + std::to_string(userId) + "');"; // use given user id

    executeSqlQuery(tagUserStatement.c_str());
}

void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
    std::string untagUserStatement =
        // delete certain tag statement - untag
        "DELETE FROM TAGS WHERE "
        "PICTURE_ID = (SELECT ID FROM PICTURES WHERE NAME = '" + pictureName + "' AND ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "'))" // find picture id by using the name of the album and the name of the picture
        " AND USER_ID = " + std::to_string(userId) + ";";  // use given user id


    executeSqlQuery(untagUserStatement.c_str());

}

void DatabaseAccess::createUser(User& user)
{
    std::string createUserStatement =
        "INSERT INTO USERS "
        "(ID, NAME) " 
        "VALUES('" + std::to_string(user.getId()) + "' ,'" + user.getName() + "'); ";

    executeSqlQuery(createUserStatement.c_str());

}

void DatabaseAccess::deleteUser(const User& user)
{
    std::string deleteUserStatement =
        // Delete the tags in which the user is in them
        "DELETE FROM TAGS "
        "WHERE USER_ID = " + std::to_string(user.getId()) + "; "

        // Delete statement of user's pictures
        "DELETE FROM PICTURES "
        "WHERE ALBUM_ID IN (SELECT ID FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + "); "

        // Delete statement of user's albums
        "DELETE FROM ALBUMS "
        "WHERE USER_ID = " + std::to_string(user.getId()) + "; " 

        // Delete statement of user from users
        "DELETE FROM USERS "
        "WHERE ID = " + std::to_string(user.getId()) + "; "; 

    executeSqlQuery(deleteUserStatement.c_str());

}
