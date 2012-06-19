// $Id: XMLAttributeToChild.cpp 2378 2009-04-22 13:34:04Z hyojin.lee $

#include "Stdafx.h"
#include "XMLAttributeToChild.h"

S32
XATC::DoMain(int argc, TCHAR* argv[])
{
	const WChar*	pTest = NULL;
	//const WChar*	pTest = _W("xml/players.xml");

	if (argc < 2)		return -1;

	const WChar*	pArg = (pTest == NULL) ? argv[1] : pTest;

	J9::XMLNode	cXML;
	cXML.Load(pArg);

	if (cXML.IsNull())	return -2;

	J9::XMLNode	cConverted;
	cConverted.Create(cXML.GetName().c_str());
	cXML.Attribute2Child(cConverted);

	J9::String	s = pArg;
	s += _W(".bak");

	::MoveFile(pArg, s.c_str());

	cConverted.Save(pArg);

	return 0;
}
