#pragma once

#include "ItemNotFoundException.h"

#include "IDataAccess.h"
#include "Album.h"
#include "User.h"
#include "sqlite3.h"

#include <iostream>
#include <io.h>
#include <string>
#include <list>
#include <set>

#define DB_FILE_NAME "galleryDB.sqlite"

// Tables names
#define ALBUMS_TABLE "Albums"
#define PICS_TABLE "Pictures"
#define USERS_TABLE "Users"
#define TAGS_TABLE "Tags"

// Columns names
#define NAME_COLUMN "NAME"
#define ID_COLUMN "ID"
#define LOCATION_COLUMN "LOCATION"
#define CREATION_DATE_COLUMN "CREATION_DATE"
#define PICTURE_ID_COLUMN "PICTURE_ID"
#define ALBUM_ID_COLUMN "ALBUM_ID"
#define USER_ID_COLUMN "USER_ID"


class DatabaseAccess : public IDataAccess
{
public:
	DatabaseAccess();
	~DatabaseAccess();

	// album related
	const Album getAlbumByName(const std::string& albumName);
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;

	void createAlbum(const Album& album) override;
	void deleteAlbum(const std::string& albumName, int userId) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;

	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) override;

	void printAlbums() override;

	// picture related
	Picture getPictureFromAlbum(const std::string& albumName, const std::string& pictureName);
	std::list<Picture> getPicturesFromAlbum(int albumId);

	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;

	void removeTagsOfPicture(const std::string& albumName, const std::string& pictureName);

	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;

	std::set<int> getTagsByPic(const Picture& pic);

	// user related
	void printUsers() override;
	void createUser(User& user) override;
	void deleteUser(const User& user) override;
	bool doesUserExists(int userId) override;
	User getUser(int userId) override;
	std::list<User> getUsers();

	// user statistics
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// queries
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	bool open() override;
	void close() override;
	void clear() override;

private:
	// Sql callback functions
	static int getAlbumsCallback(void* voidAlbum, int columnCount, char** data, char** columnName);
	static int getUsersCallback(void* voidUser, int columnCount, char** data, char** columnName);
	static int getPicturesCallback(void* voidPicture, int columnCount, char** data, char** columnName);
	static int getIntCallback(void* voidInt, int columnCount, char** data, char** columnName);
	static int getTagsCallback(void* voidTags, int len, char** data, char** columnName);

	// Execute queries - use those functions instead of repeating the same lines
	void executeSqlQuery(const char* sql);
	void executeSqlQueryWithCallback(const char* sql, int(*callback)(void*, int, char**, char**), void* data);

	sqlite3* _db;
};