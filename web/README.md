## Homebrew App Store Generator
The script in this directory is intended to be used with the new hbas2 web interface.
See [this repo](https://github.com/pwsincd/hbas2) for more information on how to run the web interface.

## How to run
Place HBL app folders in the "apps" folder, and then run the generator script
```
python gen.py
```

This will generate the following:

- directory.json file, to be used to show information about the apps
- directory.zip file, which contains the json and all app icons
- several zips of every app in the "zips" folder, which will be created if it doesn't exist
- cache.txt, which will tell the script whether or not to update an app's zip next run

Running the script will also run the v1_gen.py file, which generates the legacy directory.yaml file for v1.0 of HBAS.
To clear the cache, the cache.txt file can be deleted, which will result in all zip files being regenerated next run.

A .htaccess file is also provided for Apache web hosts

