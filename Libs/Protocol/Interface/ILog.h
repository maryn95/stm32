#pragma once

class ILog
{
public:
	virtual void writeLog(const char*, ...) {};
};
