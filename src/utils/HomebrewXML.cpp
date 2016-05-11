#include <gctypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinyxml.h"

#include "HomebrewXML.h"

#define ENTRIE_SIZE	 8192

/* qparam filename Filepath of the XML file */
bool HomebrewXML::LoadHomebrewXMLData(const char* filename)
{
	Name.clear();
	Coder.clear();
	Version.clear();
	ShortDescription.clear();
	LongDescription.clear();
	Releasedate.clear();

	TiXmlDocument xmlDoc(filename);
	if(!xmlDoc.LoadFile())
		return false;

	TiXmlElement *appNode =  xmlDoc.FirstChildElement("app");
	if(!appNode)
		return false;

	TiXmlElement *node = NULL;

	node = appNode->FirstChildElement("name");
	if(node && node->FirstChild() && node->FirstChild()->Value())
		Name = node->FirstChild()->Value();

	node = appNode->FirstChildElement("coder");
	if(node && node->FirstChild() && node->FirstChild()->Value())
		Coder = node->FirstChild()->Value();

	node = appNode->FirstChildElement("version");
	if(node && node->FirstChild() && node->FirstChild()->Value())
		Version = node->FirstChild()->Value();

	node = appNode->FirstChildElement("short_description");
	if(node && node->FirstChild() && node->FirstChild()->Value())
		ShortDescription = node->FirstChild()->Value();

	node = appNode->FirstChildElement("long_description");
	if(node && node->FirstChild() && node->FirstChild()->Value())
		LongDescription = node->FirstChild()->Value();

	char ReleaseText[200];
	memset(ReleaseText, 0, sizeof(ReleaseText));

	node = appNode->FirstChildElement("release_date");
	if(node && node->FirstChild() && node->FirstChild()->Value())
		snprintf(ReleaseText, sizeof(ReleaseText), node->FirstChild()->Value());

	int len = (strlen(ReleaseText) - 6); //length of the date string without the 200000 at the end
	if (len == 8)
		snprintf(ReleaseText, sizeof(ReleaseText), "%c%c/%c%c/%c%c%c%c", ReleaseText[4], ReleaseText[5], ReleaseText[6], ReleaseText[7], ReleaseText[0], ReleaseText[1], ReleaseText[2], ReleaseText[3]);
	else if (len == 6)
		snprintf(ReleaseText, sizeof(ReleaseText), "%c%c/%c%c%c%c", ReleaseText[4], ReleaseText[5], ReleaseText[0], ReleaseText[1], ReleaseText[2], ReleaseText[3]);
	else
		snprintf(ReleaseText, sizeof(ReleaseText), "%s", ReleaseText);

	Releasedate = ReleaseText;

	node = appNode->FirstChildElement("arguments");
	if(!node)
		return true;

	TiXmlElement *argNode = node->FirstChildElement("arg");

	while(argNode)
	{
		if(argNode->FirstChild() && argNode->FirstChild()->Value())
			Arguments.push_back(std::string(argNode->FirstChild()->Value()));

		argNode = argNode->NextSiblingElement();
	}

	return true;
}

/* Set argument */
void HomebrewXML::SetArgument(const char* argument)
{
	// Crop value from argument, if present
	char argName[strlen(argument)+1];
	strcpy(argName, argument);
	char *ptr = strrchr(argName, '=');
	if(ptr) *(ptr+1) = 0;

	// Check if argument already exists and edit it
	bool found = false;
	for(u8 i=0; i < Arguments.size(); i++)
	{
		size_t pos = Arguments[i].find(argName);
		if(pos != std::string::npos)
		{
			Arguments[i] = argument;
			found = true;
			break;
		}
	}

	// if it doesn't exist, add the new argument.
	if(!found)
		Arguments.push_back(argument);
}

/* Get name */
const char * HomebrewXML::GetName() const
{
	return Name.c_str();
}

/* Set Name */
void HomebrewXML::SetName(char * newName)
{
	Name = newName;
}

/* Get coder */
const char * HomebrewXML::GetCoder() const
{
	return Coder.c_str();
}

/* Get version */
const char * HomebrewXML::GetVersion() const
{
	return Version.c_str();
}

/* Set version */
void HomebrewXML::SetVersion(const char * newVer)
{
	Version = newVer;
}

/* Get releasedate */
const char * HomebrewXML::GetReleasedate() const
{
	return Releasedate.c_str();
}

/* Get shortdescription */
const char * HomebrewXML::GetShortDescription() const
{
	return ShortDescription.c_str();
}

/* Get longdescription */
const char * HomebrewXML::GetLongDescription() const
{
	return LongDescription.c_str();
}
