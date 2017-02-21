/* 
 * HomebrewManager.cpp
 * 
 * Installs/Updates homebrew from Zip files (parse manifest) and 
 * deletes homebrew from a manifest file.
 *
 * The functions need the homebrew short_name, which is used for .install files
 * https://gist.github.com/vgmoose/90f48949c95927c8e92c990bd6985b38
 *
*/

#pragma once

#include "utils/Zip.h"
#include <string>


class HomebrewManager {
	public:		
		HomebrewManager(std::string & shortName, ProgressWindow * progress = NULL);
		void Install();
		void Delete();
		~HomebrewManager();
	private:
		std::string downloadZip();
		void installZip(std::string & ZipPath);
	
		std::string ShortName;
		int UseProgressBar;
		ProgressWindow * Progress;
};