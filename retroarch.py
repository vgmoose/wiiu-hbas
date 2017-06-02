#!/bin/python
# Retroarch Updater v2

import urllib2, time, cgi, shutil, os, sys
from datetime import date, timedelta


# the target of the retroarch 7z file
target = "https://buildbot.libretro.com/nightly/nintendo/wiiu/%s_RetroArch_rpx.7z"

# the current date 
cur_time = time.strftime("%Y:%m:d")

# yesterday's date (in case times are off)
yesterday = (date.today() - timedelta(1)).strftime("%Y-%m-%d")

# any environment variables from the user
form = cgi.FieldStorage()

# if target date specified
if "target" in form:

	# set current time to target date 
	cur_time = form["target"]

	# using a specific target means don't fallback to yesterday
	yesterday = None

try:
	# try to fetch the appropriate download (7z file)
	response = urllib2.urlopen(target % cur_time)

except:
	# if yesterday's date is set
	if yesterday:
		try:	
			# retry the download of the 7z file
			response = urllib2.urlopen(target % yesterday)

			# update the current time to yesterday for later in the script
			cur_time = yesterday

		except:
			# if it's still not found...
			print("No retroarch downloads found for %s or %s, try a manual date" % (cur_time, yesterday))
			exit()

	else:
		print("The URL at %s wasn't found (bad date?)" % (target % cur_time))
		exit()

# the filename we got
filename = "%s_RetroArch_rpx.7z" % cur_time

print("Downloading Retroarch cores...")

# write downloaded contents to disk
# with open(filename, 'wb') as output:
# 	output.write(response.read())

print("Successfully downloaded file: %s" % filename)

try:
    # try to remove the staging directory
    shutil.rmtree("staging")
except:
    # doesn't exist, it's okay
    pass

# make the staging directory
os.mkdir("staging")

# try to extract it to staging directory
# thos uses an external binary
os.system("7za x %s -ostaging" % filename)

# TODO: copy over and touch up metas
