#include "tinyxml2.h"
using namespace tinyxml2;

int createXML(const char* xmlPath,const char* title)
{
	XMLDocument doc;
	doc.Parse("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
	XMLDeclaration* declaration =doc.NewDeclaration(
		"<!DOCTYPE score-partwise PUBLIC \" -//Recordare//DTD MusicXML 2.0 Partwise//EN\" \"/musicxml20/partwise.dtd\">");
	doc.InsertFirstChild(declaration);
	XMLElement* root = doc.NewElement("score-partwise");
	doc.InsertEndChild(root);

	XMLElement* work = doc.NewElement("work");
	XMLElement* workTitle = doc.NewElement("work-title");
	XMLText* titleText = doc.NewText(title);
	work->InsertEndChild(titleText);
	root->InsertEndChild(work);


	return doc.SaveFile(xmlPath);
}