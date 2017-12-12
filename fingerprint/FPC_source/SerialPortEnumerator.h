// SerialPortEnumerator.h: interface for the CSerialPortEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALPORTENUMERATOR_H__61A7DA78_EF64_49AB_8CA5_ED1A24C6B46D__INCLUDED_)
#define AFX_SERIALPORTENUMERATOR_H__61A7DA78_EF64_49AB_8CA5_ED1A24C6B46D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>

using namespace std;
typedef list<string> SerialPortList;
typedef list<string>::iterator SerialPortListIterator;

class CSerialPortEnumerator  
{
public:
	CSerialPortEnumerator();
	virtual ~CSerialPortEnumerator();

	SerialPortList Enumerate();
};

#endif // !defined(AFX_SERIALPORTENUMERATOR_H__61A7DA78_EF64_49AB_8CA5_ED1A24C6B46D__INCLUDED_)
