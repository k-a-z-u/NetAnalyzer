/*
 * File.h
 *
 *  Created on: 02.09.2012
 *      Author: kazu
 */

#ifndef FILE_H_
#define FILE_H_

#include <string>
#include <vector>






class File {

public:

	/** ctor (current directory) */
	File();

	/** copy ctor */
	File(const File& file);

	/** ctor from given filename/folder */
	File(const std::string& fileName);

	/** ctor from given file and sub-filename/folder */
	File(const File& parent, const std::string& fileName);

	/** dtor */
	virtual ~File();



	/** get the parent (folder) of this file/folder */
	File getParent() const;

	/** get the working-directory */
	static std::string getWorkingDir();

	/** get the absolute path of this file */
	std::string getAbsolutePath() const;


	/** returns true if this file exists */
	bool exists() const;

	/** returns true if this is a folder */
	bool isFolder() const;

	/** returns true if this is a file */
	bool isFile() const;


	/** create whole directory tree */
	void mkdirs();

	/** create current directory */
	void mkdir();


	/** get all files/folders within this folder */
	void listFiles(std::vector<File>& lst);


private:

	/** convert slashes etc. */
	static void unify(std::string& file);

	/* attributes */
	std::string fileName;

};

#endif /* FILE_H_ */
