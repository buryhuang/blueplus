#pragma once

class CApp
{
public:
	CApp(void);
	virtual ~CApp(void);
	virtual void AppStart()=0;
};
