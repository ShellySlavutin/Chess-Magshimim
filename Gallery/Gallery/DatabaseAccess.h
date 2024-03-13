#pragma once

#include "IDataAccess.h"
#include "Album.h"
#include "User.h"
#include "sqlite3.h"

#include <iostream>
#include <io.h>
#include <string>
#include <list>

class DatabaseAccess : public IDataAccess
{
public:
	virtual bool open() override;
	virtual void close() override;
	virtual void clear() override;

private:
	sqlite3* _db;

	std::list<Album> db_albums;
	std::list<User> db_users;
};