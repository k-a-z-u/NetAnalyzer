/*
 * File.cpp
 *
 *  Created on: 02.09.2012
 *      Author: kazu
 */

#include "File.h"
#include "MyString.h"
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>

#include <stdio.h>
#ifdef __MINGW32__
    #include <direct.h>
    #define getCurrentDir _getcwd
	#define NEEDED_SLASH '\\'
	#define OTHER_SLASH '/'
#else
    #include <unistd.h>
    #define getCurrentDir getcwd
	#define NEEDED_SLASH '/'
	#define OTHER_SLASH '\\'
#endif

/** ctor */
File::File() {
	this->fileName = getWorkingDir();
	unify(this->fileName);
}

/** ctor */
File::File(const File& file) {
	this->fileName = file.getAbsolutePath();
	unify(this->fileName);
}

/** ctor */
File::File(const std::string& fileName) {
	if (fileName[0] == NEEDED_SLASH || fileName[1] == ':') {
		this->fileName = fileName;
	} else {
		this->fileName = getWorkingDir() + NEEDED_SLASH + fileName;
	}
	unify(this->fileName);
}

/** ctor */
File::File(const File& parent, const std::string& fileName) {
	this->fileName = parent.getAbsolutePath() + NEEDED_SLASH + fileName;
	unify(this->fileName);
}

/** dtor */
File::~File() {}


/** get the parent (folder) of this file/folder */
File File::getParent() const {
	size_t pos = fileName.find_last_of(NEEDED_SLASH);
	if (pos == std::string::npos) {return fileName;}		// fixme OK?
	std::string newFileName = fileName.substr(0, pos);
	return File(newFileName);
}

/** get the working-directory */
std::string File::getWorkingDir() {
	char cwd[FILENAME_MAX] = {0};
	if (!getCurrentDir(cwd, sizeof(cwd))) {return std::string();}
	std::string dir = std::string(cwd);
	File::unify(dir);
	return dir;
}

/** get the absolute path of this file */
std::string File::getAbsolutePath() const {
	if (fileName[fileName.size()-1] == NEEDED_SLASH) {
		return fileName.substr(0, fileName.size()-1);
	} else {
		return fileName;
	}
}

/** convert slashes etc. */
void File::unify(std::string& file) {
	strReplaceChar(file, OTHER_SLASH, NEEDED_SLASH);
}


bool File::exists() const {
	struct stat buf;
	int retCode = stat(getAbsolutePath().c_str(), &buf);
	return (retCode != -1) ? true : false;
}


bool File::isFile() const {
	struct stat buf;
	int retCode = stat(getAbsolutePath().c_str(), &buf);
	if (retCode == -1) {return false;}
	return !S_ISDIR(buf.st_mode);
}

bool File::isFolder() const {
	struct stat buf;
	int retCode = stat(getAbsolutePath().c_str(), &buf);
	if (retCode == -1) {return false;}
	return S_ISDIR(buf.st_mode);
}

void File::listFiles(std::vector<File>& lst) {

	DIR* pDIR;
	pDIR = opendir(getAbsolutePath().c_str());
	if( pDIR == NULL ) {return;}

	struct dirent* entry;
	while( (entry = readdir(pDIR)) != NULL ){
		if (strcmp(entry->d_name, ".") == 0) {continue;}
		if (strcmp(entry->d_name, "..") == 0) {continue;}
		File f(*this, std::string(entry->d_name));
		lst.push_back(f);
	}

	// cleanup
	closedir(pDIR);

}

void File::mkdir() {

	::mkdir(getAbsolutePath().c_str(), 0755);

}

void File::mkdirs() {

	// recursivly create directory tree
	if (!exists()) {
		getParent().mkdirs();
		mkdir();
	}

}
