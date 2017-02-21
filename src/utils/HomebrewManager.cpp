#include "menu/HomebrewLaunchWindow.h"
#include "common/common.h"
#include "fs/DirList.h"
#include "fs/fs_utils.h"
#include "utils/HomebrewXML.h"
#include "Application.h"
#include "dynamic_libs/sys_functions.h"
#include "network/FileDownloader.h"
#include "utils/Zip.h"
#include <algorithm>
#include <sstream>
#include "HomebrewManager.h"

HomebrewManager::HomebrewManager(std::string & shortName, ProgressWindow * progress)
{
	log_printf("-> HomebrewManager: Constructor");
	if(progress == NULL)
	{
		log_printf("No ProgressWindow * passed, not using progress");
		UseProgressBar = 0;
	}
	else
	{
		UseProgressBar = 1;
		Progress = progress;
	}
	log_printf("HomebrewManager: shortName is %s", shortName.c_str());
	ShortName = shortName;
}

HomebrewManager::~HomebrewManager()
{
	//delete &ShortName;
}

void HomebrewManager::Install()
{
	/* Download and install an Homebrew from the current repo */
	log_printf("-> HomebrewManager::Install");
	
	//! First download the Zip File
	log_printf("Downloading Zip file");
    std::string ZipFilePath = downloadZip();
	
	//! Now extract the file and parse the manifest
	log_printf("Installing Zip File");
	installZip(ZipFilePath);
}

void HomebrewManager::Delete()
{
	log_printf("-> HomebrewManager::Delete");
	
	//! (try to) Open the Manifest
	std::string ManifestPath = "sd:/wiiu/apps/hbas/.manifest/" + ShortName + ".install";
	
	struct stat sbuff;
	if (stat(ManifestPath.c_str(), &sbuff) != 0) //! There's no manifest
	{
		
		//! Use the normal delete method
		log_printf("No manifest file found, using default delete");
		
		std::string removePath = "sd:/wiiu/apps/" + ShortName;
		log_printf("Deleting folder %s", removePath.c_str());
		
		if (removePath.length() < 15) // Do not delete "sd:/wiiu/apps/"
			return;
		
		//! Remove the files in the directory
		DirList dirList(removePath, 0, DirList::Files | DirList::CheckSubfolders);
		for (int x = 0; x < dirList.GetFilecount(); x++)
			remove(dirList.GetFilepath(x));
		
		//! Remove the directory
		rmdir(removePath.c_str());
		return;
	}
	
	//! Open the manifest normally
	CFile * ManifestFile = new CFile(ManifestPath, CFile::ReadOnly);
	char * Manifest_cstr = (char *)malloc(ManifestFile->size());
	ManifestFile->read((u8*)Manifest_cstr, ManifestFile->size());	
	std::stringstream Manifest(Manifest_cstr);
	
	//! Parse the manifest
	log_printf("Parsing the Manifest");
	
	std::string CurrentLine;
	while(std::getline(Manifest, CurrentLine))
	{		
		char Mode = CurrentLine.at(0);
		std::string DeletePath = "sd:/" + CurrentLine.substr(3);
		
		switch(Mode)
		{
			case 'U':
				log_printf("%s : UPDATE", DeletePath.c_str());
				log_printf("Removing %s", DeletePath.c_str());
				std::remove(DeletePath.c_str());
				break;
			case 'G':
				log_printf("%s : GET", DeletePath.c_str());
				log_printf("Removing %s", DeletePath.c_str());
				std::remove(DeletePath.c_str());
				break;
			case 'L':
				log_printf("%s : LOCAL", DeletePath.c_str());
				log_printf("Removing %s", DeletePath.c_str());
				std::remove(DeletePath.c_str());
				break;
			default:
				break;
		}
	}
	
	log_printf("Removing manifest...");
	
	ManifestFile->close();
	delete ManifestFile;
	
	std::remove(ManifestPath.c_str());
	
	
	log_printf("Homebrew removed");
}

std::string HomebrewManager::downloadZip()
{
	log_printf("-> HomebrewManager::downloadZip");

	//! Set the progress bar to 0%
	if(UseProgressBar)
		Progress->setProgress(0);
	
    std::string mRepoUrl	= std::string(repoUrl);	
    std::string tmpFilePath	= "sd:/hbas_zips";		//! Temporary path for zip files
    CreateSubfolder(tmpFilePath.c_str());			//! Make sure temp folder is here 
	
    //! Generate zip filename, url and path
    std::string zipFileName = ShortName + ".zip";
    std::string zibUrl      = mRepoUrl + "/zips/" + zipFileName;
	std::string zipPath     = tmpFilePath  +  "/" + zipFileName;
	
		
    log_printf("Homebrew ZIP Variables:");
    log_printf("zipFileName = %s", zipFileName.c_str());
    log_printf("zipUrl      = %s", zibUrl.c_str());
    log_printf("zipPath     = %s", zipPath.c_str());

    //! Download the zip file to the temp folder
    Progress->setTitle("Downloading " + zipFileName + "...");
	if(UseProgressBar)
		FileDownloader::getFileToSd(zibUrl, zipPath, &updateProgress);
    else
		FileDownloader::getFileToSd(zibUrl, zipPath);
	
	log_printf("Downloaded %s", zipFileName.c_str());
	
	return zipPath;
}

void HomebrewManager::installZip(std::string & ZipPath)
{
	log_printf("-> HomebrewManager::installZip");
	Progress->setTitle("Installing Homebrew to SDCard...");
	
	//! Open the Zip file
	UnZip * HomebrewZip = new UnZip(ZipPath.c_str());
	
	//! First extract the Manifest	
	std::string ManifestPathInternal = "wiiu/apps/hbas/.manifest/" + ShortName + ".install";
	std::string ManifestPath = "sd:/" + ManifestPathInternal;
	HomebrewZip->ExtractFile(ManifestPathInternal.c_str(), ManifestPath.c_str());
	
	//! Open the Manifest
	CFile * ManifestFile = new CFile(ManifestPath, CFile::ReadOnly);
	char * Manifest_cstr = (char *)malloc(ManifestFile->size());
	ManifestFile->read((u8*)Manifest_cstr, ManifestFile->size());	
	std::stringstream Manifest(Manifest_cstr);
	
	//! Parse the manifest
	log_printf("Parsing the Manifest");
	
	std::string CurrentLine;
	while(std::getline(Manifest, CurrentLine))
	{		
		char Mode = CurrentLine.at(0);
		std::string Path = CurrentLine.substr(3);
		std::string ExtractPath = "sd:/" + Path;
		
		switch(Mode)
		{
			case 'E':
				//! Simply Extract, with no checks or anything, won't be deleted upon removal
				log_printf("%s : EXTRACT", Path.c_str());
				HomebrewZip->ExtractFile(Path.c_str(), ExtractPath.c_str());
				break;
			case 'U':
				log_printf("%s : UPDATE", Path.c_str());
				HomebrewZip->ExtractFile(Path.c_str(), ExtractPath.c_str());
				break;
			case 'G':
				log_printf("%s : GET", Path.c_str());
				struct stat sbuff;
				if (stat(ExtractPath.c_str(), &sbuff) != 0) //! File doesn't exist, extract
					HomebrewZip->ExtractFile(Path.c_str(), ExtractPath.c_str());
				else
					log_printf("File already exists, skipping...");
				break;
			default:
				break;
		}
	}
}