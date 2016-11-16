#! /usr/bin/python
# Version 2
# This script generates the directory of apps
# which is parsed by the client app
# and creates the web interface for wiiubru(by default)

# in particiular, this script will generate homebrew zips,
# icon zips, and a directory json

import os, json, zipfile, datetime, time
import xml.etree.ElementTree as ET

# This function zips the incoming path into the file in ziph
def zipdir(path, ziph):
	for root, dirs, files in os.walk(path):
		for file in files:
			ziph.write(os.path.join(root, file))
			
# This function parses out several attributes from xml
def xml_read(incoming):
	outgoing = []
	for key in incoming:
		try:
			outgoing.append(tree.find(key).text)
		except:
			outgoing.append("N/A")
	return tuple(outgoing)

# create the zips directory
try:
	os.mkdir("zips")
except:
	pass

# the resulting json output
out = {}
out["apps"] = []

# read lasts updated info from a "cache.txt" file
cache = {}
try:
	contents = open("cache.txt", "r")
	for line in contents:
		line = line.replace("\n", "").split("\t")
		name = line[1]
		updated = line[0]
		cache[name] = updated
except:
	pass

apps = os.listdir("apps")

for app in apps:
	# ignore dotfiles
	if app.startswith("."):
		continue
		
	targdir = "apps"
	
	# get meta.xml file from HBL
	xmlfile = targdir + "/%s/meta.xml" % app

	# create some default fields
	name = coder = desc = long_desc = version = source = updated = category = src_link = "N/A"
	typee = "hbl"

	# find a binary in this app folder
	binary = None
	for file in os.listdir(targdir + "/%s" % app):
		if file.endswith(".elf") or file.endswith(".rpx"):
			binary = file
			updated = time.ctime(os.path.getmtime(targdir + "/%s" % app))

	# if there's no binary found, continue
	if not binary:
		continue

	# make sure rpxes are categorized as such
	if binary.endswith(".rpx"):
		typee = "rpx"

	# get fields out of xml
	if os.path.isfile(xmlfile):
		# parse the xml file
		tree = ET.parse(xmlfile)

		# pull out those attributes
		name, coder, desc, source, long_desc, version, category = xml_read(["name", "coder", "short_description", "url", "long_description", "version", "category"])
		
	# sanitize long_desc for json output
	long_desc = long_desc.replace("\n", "\\n").replace("\t", "\\t")
	
	# get icon path
	icon = targdir + "/%s/icon.png" % app

	# append to output json
	out["apps"].append({"updated": updated, "directory": app, "name": name, "author": coder, "desc": desc, "url": source, "binary": binary, "long_desc": long_desc, "type": typee, "cat": category})
	
			
	# if there's no update according to the cache
	if app in cache and cache[app] == updated:
		continue
			
	print "Compressing " + app + "..."

	# zip up this app
	zipf = zipfile.ZipFile("zips/%s.zip" % app, 'w', zipfile.ZIP_DEFLATED)
	zipdir(targdir + "/%s" % app, zipf)
	zipf.close()

# last updated date
out["updated"] = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

# json string formatting
jsonstring = json.dumps(out, indent=4, separators=(',', ': '))

# write it to file
jsonout = open("directory.json", "w+")
jsonout.write(jsonstring)
jsonout.close()

# compress the directory.json and app icons
# to make downloading faster
ziph = zipfile.ZipFile("directory.zip", 'w', zipfile.ZIP_DEFLATED)
ziph.write("directory.json")
for cur in out["apps"]:
	ziph.write("apps/"+cur["directory"]+"/icon.png", cur["directory"]+".png")
ziph.close()

# write out the update times to a cache file
cache_out = open("cache.txt", "w")
for cur in out["apps"]:	cache_out.write(cur["updated"]+"\t"+cur["directory"]+"\n")
cache_out.close()

# print done and run legacy scripts
print "Updated directory.json !!!"
print "Running v1_gen.py for hbas 1.0..."
os.system("python v1_gen.py")
