#include "DatabaseAccess.h"

int DatabaseAccess::getAlbumsCallback(void* voidAlbum, int columnCount, char** data, char** columnName)
{
    auto albums = static_cast<std::list<Album>*>(voidAlbum); // Revert back to album list from void
    Album album;

    for (int i = 0; i < columnCount; ++i)
    {
        if (std::string(columnName[i]) == ID_COLUMN)
        {
            album.setId(atoi(data[i]));
        }
        else if (std::string(columnName[i]) == NAME_COLUMN)
        {
            album.setName(data[i]);
        }
        else if (std::string(columnName[i]) == CREATION_DATE_COLUMN)
        {
            album.setCreationDate(data[i]);
        }
        else if (std::string(columnName[i]) == USER_ID_COLUMN)
        {
            album.setOwner(atoi(data[i]));
        }
    }

    albums->push_back(album);

    return 0;
}

int DatabaseAccess::getUsersCallback(void* voidUser, int columnCount, char** data, char** columnName)
{
    auto users = static_cast<std::list<User>*>(voidUser);
    User user;

    for (int i = 0; i < columnCount; ++i)
    {
        if (std::string(columnName[i]) == ID_COLUMN)
        {
            user.setId(atoi(data[i]));
        }
        else if (std::string(columnName[i]) == NAME_COLUMN)
        {
            user.setName(data[i]);
        }
    }

    users->push_back(user);
    return 0;

}

int DatabaseAccess::getPicturesCallback(void* voidPicture, int columnCount, char** data, char** columnName)
{
    auto pics = static_cast<std::list<Picture>*>(voidPicture);
    Picture pic;

    for (int i = 0; i < columnCount; ++i)
    {
        if (std::string(columnName[i]) == ID_COLUMN)
        {
            pic.setId(atoi(data[i]));
        }
        else if (std::string(columnName[i]) == NAME_COLUMN)
        {
            pic.setName(data[i]);
        }
        else if (std::string(columnName[i]) == LOCATION_COLUMN)
        {
            pic.setPath(data[i]);
        }
        else if (std::string(columnName[i]) == CREATION_DATE_COLUMN)
        {
            pic.setCreationDate(data[i]);
        }
    }

    pics->push_back(pic);
    return 0;
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


//QUERIES METHODS:

User DatabaseAccess::getTopTaggedUser()
{
    std::list<User> users;
    std::string sqlGetTopTaggedUser =
        "SELECT USERS.* FROM USERS "
        "INNER JOIN TAGS "
        "ON USERS.ID = TAGS.USER_ID "
        "GROUP BY USERS.ID "
        "ORDER BY COUNT(*) DESC "
        "LIMIT 1;";

    executeSqlQueryWithCallback(sqlGetTopTaggedUser.c_str(), getUsersCallback, &users);

    if (users.empty())
    {
        throw MyException("No user found");
        return User();
    }
    else
    {
        return users.front();
    }
}

Picture DatabaseAccess::getTopTaggedPicture()
{
    std::list<Picture> pictures;
    std::string sqlGetTopTaggedPicture =
        "SELECT PICTURES.* FROM PICTURES "
        "INNER JOIN TAGS "
        "ON PICTURES.ID = TAGS.PICTURE_ID "
        "GROUP BY PICTURES.ID "
        "ORDER BY COUNT(*) DESC "
        "LIMIT 1;";

    executeSqlQueryWithCallback(sqlGetTopTaggedPicture.c_str(), getPicturesCallback, &pictures);

    if (pictures.empty())
    {
        throw MyException("No picture found");
        return Picture();
    }
    else
    {
        return pictures.front();
    }
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
    std::list<Picture> pictures;
    std::string sqlGetTaggedPicturesOfUser =
        "SELECT PICTURES.* FROM PICTURES "
        "INNER JOIN TAGS "
        "ON PICTURES.ID = TAGS.PICTURE_ID "
        "WHERE TAGS.USER_ID = " + std::to_string(user.getId()) + ";";

    executeSqlQueryWithCallback(sqlGetTaggedPicturesOfUser.c_str(), getPicturesCallback, &pictures);

    if (pictures.empty())
    {
        throw MyException("No pictures found");
    }
    return pictures;
}

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

//ALBUM METHODS :

const Album DatabaseAccess::getAlbumByName(const std::string& albumName)
{
    std::list<Album> albums = getAlbums();

    for (const auto& album : albums) {
        if (album.getName() == albumName) // Assume the first album with the name wanted is the desired Album
        {
            return album;
        }
    }

    // Didn't find a matching album
    throw ItemNotFoundException("Could not find the album", albumName);
}



const std::list<Album> DatabaseAccess::getAlbums()
{
    std::list<Album> albums;
    std::string sqlGetAlbums = "SELECT * FROM ALBUMS;";

    executeSqlQueryWithCallback(sqlGetAlbums.c_str(), getAlbumsCallback, &albums);

    return albums;
}

const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
    std::list<Album> albums;
    std::string sqlGetAlbumsOfUser = 
        "SELECT * FROM ALBUMS "  
        " WHERE USER_ID = " + std::to_string(user.getId()) + ";";

    executeSqlQueryWithCallback(sqlGetAlbumsOfUser.c_str(), getAlbumsCallback, &albums);

    return albums;
}

void DatabaseAccess::createAlbum(const Album& album)
{
    std::string createAlbumStatement =
        "INSERT INTO ALBUMS "
        "(NAME, USER_ID, CREATION_DATE) "
        "VALUES "
        "('" + album.getName() + "', " + std::to_string(album.getOwnerId()) + ", '" + album.getCreationDate() + "');";

    executeSqlQuery(createAlbumStatement.c_str());
}


void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
    std::string sqlCommands;
    try
    {
        sqlCommands += "BEGIN TRANSACTION; ";

        // Construct SQL statements for deleting the album and its pictures
        std::string deleteAlbumStatement =
            "DELETE FROM ALBUMS "
            "WHERE NAME = '" + albumName + "' "
            "AND USER_ID = " + std::to_string(userId) + ";";

        std::string deletePicturesStatement =
            "DELETE FROM PICTURES "
            "WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "');";

        // Add SQL statements to the command
        sqlCommands += deleteAlbumStatement;
        sqlCommands += deletePicturesStatement;

        // Commit transaction
        sqlCommands += "COMMIT; ";

        // Execute the SQL commands
        executeSqlQuery(sqlCommands.c_str());
    }
    catch (const std::exception& e)
    {
        // If an exception occurs, rollback the transaction
        executeSqlQuery("ROLLBACK;");
        throw e;
    }
}

bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
    std::list<Album> albums;
    std::string sqlGetAlbumExists =
        "SELECT * FROM ALBUMS "
        " WHERE USER_ID = " + std::to_string(userId) +
        " AND NAME = '" + albumName + "';";

    executeSqlQueryWithCallback(sqlGetAlbumExists.c_str(), getAlbumsCallback, &albums);

    return !albums.empty();
}


Album DatabaseAccess::openAlbum(const std::string& albumName)
{
    getAlbumByName(albumName); // CHECK LATER
}

void DatabaseAccess::closeAlbum(Album& pAlbum)
{
    // NOTHING TO DO!
}

void DatabaseAccess::printAlbums()
{
    std::list<Album> albums = getAlbums();

    if (albums.empty()) {
        throw MyException("There are no existing albums.");
    }
    std::cout << "Album list:" << std::endl;
    std::cout << "-----------" << std::endl;
    for (const Album& album : albums) {
        std::cout << std::setw(5) << "* " << album;
    }
}

//PICTURE METHODS :

Picture DatabaseAccess::getPictureFromAlbum(const std::string& albumName, const std::string& pictureName)
{
    try
    {
        Album album = getAlbumByName(albumName);

        std::list<Picture> pics;
        std::string sqlGetPic =
            "SELECT * FROM PICTURES "
            " WHERE ALBUM_ID = " + std::to_string(album.getId()) +
            " AND NAME = '" + pictureName + "';";

        executeSqlQueryWithCallback(sqlGetPic.c_str(), getPicturesCallback, &pics);

        if (pics.empty())
        {
            throw ItemNotFoundException("Picture with the following name not found", pictureName);
        }
        else
        {
            return pics.front();
        }
    }
    catch (const ItemNotFoundException& e)
    {
        std::cerr << e.what() << std::endl;
        return Picture();
    }
}

void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
    try {
        Album album = getAlbumByName(albumName);

        std::string addPictureStatement =
            "INSERT INTO PICTURES (NAME, LOCATION, CREATION_DATE, ALBUM_ID) "
            "VALUES ("
            "'" + picture.getName() + "', "
            "'" + picture.getPath() + "', "
            "'" + picture.getCreationDate() + "', "
            + std::to_string(album.getId()) +
            ");";

        executeSqlQuery(addPictureStatement.c_str());
    }
    catch (const ItemNotFoundException& e) {
        std::cerr << e.what() << std::endl;
        // Handle case where album is not found
    }
}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
    std::string sql;

    try
    {
        // Start transaction
        executeSqlQuery("BEGIN;");

        // Find the user ID based on the album name
        Album album = getAlbumByName(albumName);
        int userId = album.getOwnerId();

        sql = "DELETE FROM PICTURES WHERE "
            " ALBUM_ID = " + std::to_string(album.getId()) + " AND "
            " NAME = '" + pictureName + "';";

        executeSqlQuery(sql.c_str());

        removeTagsOfPicture(albumName, pictureName);

        executeSqlQuery("END;");
    }
    catch (const ItemNotFoundException& e) {
        std::cerr << e.what() << std::endl;
    }

}

void DatabaseAccess::removeTagsOfPicture(const std::string& albumName, const std::string& pictureName)
{
    std::string untagPicStatement =
        // delete certain tag statement - untag
        "DELETE FROM TAGS WHERE "
        "PICTURE_ID = (SELECT ID FROM PICTURES WHERE NAME = '" + pictureName + 
        "' AND ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "'));"; 
        // find picture id by using the name of the album and the name of the picture

    executeSqlQuery(untagPicStatement.c_str());
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

//USER METHODS : 

void DatabaseAccess::printUsers()
{
    std::cout << "Users list:" << std::endl;
    std::cout << "-----------" << std::endl;

    std::list<User> users = getUsers();

    for (const auto& user : users){
        std::cout << user << std::endl;
    }
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
    // Begin transaction
    executeSqlQuery("BEGIN TRANSACTION;");

    // Construct SQL statement
    std::string deleteUserStatement =
        "DELETE FROM TAGS "
        "WHERE USER_ID = " + std::to_string(user.getId()) + "; "
        "DELETE FROM PICTURES "
        "WHERE ALBUM_ID IN (SELECT ID FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + "); "
        "DELETE FROM ALBUMS "
        "WHERE USER_ID = " + std::to_string(user.getId()) + "; "
        "DELETE FROM USERS "
        "WHERE ID = " + std::to_string(user.getId()) + "; ";

    // Execute SQL statement
    executeSqlQuery(deleteUserStatement.c_str());

    // Commit transaction
    executeSqlQuery("COMMIT;");
}

bool DatabaseAccess::doesUserExists(int userId)
{
    std::list<User> users;
    std::string sqlGetUserExists =
        "SELECT * FROM USERS "
        " WHERE ID = " + std::to_string(userId) + ";";

    executeSqlQueryWithCallback(sqlGetUserExists.c_str(), getUsersCallback, &users);

    return !users.empty();
}

User DatabaseAccess::getUser(int userId)
{
    std::list<User> users;
    std::string sqlGetUser =
        "SELECT * FROM USERS "
        " WHERE ID = " + std::to_string(userId) + ";";

    executeSqlQueryWithCallback(sqlGetUser.c_str(), getUsersCallback, &users);

    if (users.empty())
    {
        throw ItemNotFoundException("User with the following ID not found", userId);
        return User();
    }
    else
    {
        return users.front();
    }
}

std::list<User> DatabaseAccess::getUsers() 
{
    std::list<User> users;
    std::string sqlGetUsers =
        "SELECT * FROM USERS;";

    executeSqlQueryWithCallback(sqlGetUsers.c_str(), getUsersCallback, &users);

    if (users.empty())
    {
        throw MyException("No users exist");
    }
    return users;
}
