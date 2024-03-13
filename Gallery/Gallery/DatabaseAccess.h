#pragma once

#include "IDataAccess.h"

class DatabaseAccess : public IDataAccess
{
	virtual bool open() override;
	virtual void close() override;
	virtual void clear() override;
};