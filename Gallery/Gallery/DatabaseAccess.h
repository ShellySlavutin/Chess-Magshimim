#pragma once

#include "IDataAccess.h"
#include "sqlite3.h"

#include <iostream>
#include <io.h>
#include <string>

class DatabaseAccess : public IDataAccess
{
public:
	virtual bool open() override;
	virtual void close() override;
	virtual void clear() override;

private:
	sqlite3* _db;
};