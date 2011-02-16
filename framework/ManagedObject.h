/** \addtogroup framework
 *  @{
 */

#pragma once

#include <string>
using namespace std;

class CManagedObject
{
public:
	CManagedObject(void);
	CManagedObject(wstring name):m_name(name){}
	virtual ~CManagedObject(void);
	wstring GetName()
	{
		return m_name;
	}

protected:
	wstring m_name;
};

/** @}*/
