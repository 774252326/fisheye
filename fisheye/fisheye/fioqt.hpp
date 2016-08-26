#pragma once

#include <QDir>
#include <QString>
static void SetFolder(const std::string &fop)
{
	QString fp(fop.c_str());
	if(!QDir(fp).exists())
		QDir().mkdir(fp);
}





