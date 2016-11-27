#! /usr/bin/python
# Version 2
# This script generates the directory of apps
# which is parsed by the client app
# and creates the json directory for wiiubru's web interface(by default)

# in particiular, this script will generate homebrew zips,
# icon zips, and a directory json

import os, json, zipfile, datetime, time, imghdr, traceback, sys, ast
from pprint import pformat
import xml.etree.ElementTree as ET

print "Content-type: text/html\n\n"

# try and catch ANY error so web has output
try:

	# list of featured apps, these will be moved to the top of the json file
	featured = ["asturoids", "spacegame", "flappy_bird", "homebrew_launcher", "loadiine_gx2", "retro_launcher", "pong", "pacman", "snake", "mgba", "CHIP8", "PokeMiniU", "saviine", "ftpiiu", "cfwbooter", "appstore", "geckiine", "u-paint", "hid_keyboard_monitor", "LiveSynthesisU", "keyboard_example", "Snes9x2010"]

	# This function zips the incoming path into the file in ziph
	def zipdir(path, ziph):
		for root, dirs, files in os.walk(path):
			for file in files:
				ziph.write(os.path.join(root, file))

	# This function parses out several attributes from xml
	def xml_read(incoming, tree, app):
		outgoing = []
		for key in incoming:
			try:
				outgoing.append(tree.find(key).text)
			except:
				print "%s: Missing &lt;%s&gt; in meta.xml<br>" % (app, key)
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
	
	stats = {}
	# open up a stats file, if one is available
	try:
		report_file = open("stats/Report.csv")
		target_app = ""
		for line in report_file:
			# we hit a line with stat data
			if line.startswith("General Statistics for "):
				# update the target app
				target_app = line.split("/")[3]
				stats[target_app] = 0
				
			# we hit a line with nonempty hit data
			if line.startswith("Hits") and line.split(",")[1].rstrip() != "":
				stats[target_app] += int(line.split(",")[1].rstrip())
	except:
		print("Encountered an error parsing stats/Report.csv<br>")
		pass

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

		# get icon.png file from HBL
		iconfile = targdir + "/%s/icon.png" % app

		if imghdr.what(iconfile) != "png":
			print "Skipping %s as its icon.png isn't a png file" % app
			continue

		# create some default fields
		name = coder = desc = long_desc = version = source = updated = filesize = category = src_link = "N/A"
		typee = "hbl"

		# find a binary in this app folder
		binary = None
		for file in os.listdir(targdir + "/%s" % app):
			if file.endswith(".elf") or file.endswith(".rpx"):
				binary = file
				updated = time.strftime('%d/%m/%Y', time.gmtime(os.path.getmtime(targdir + "/%s" % app)))
				# find filesize of binary in Kbs
				filesize = os.path.getsize(targdir + "/%s/%s" % (app,binary))/1024

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
			name, coder, desc, source, long_desc, version, category = xml_read(["name", "coder", "short_description", "url", "long_description", "version", "category"], tree, app)

		# sanitize long_desc for json output
		long_desc = long_desc.replace("\n", "\\n").replace("\t", "\\t")

		# get icon path
		icon = targdir + "/%s/icon.png" % app
		
		# try to load stats if available
		cur_stats = 0
		if app in stats:
			cur_stats = stats[app]

		# append to output json
		out["apps"].append({"filesize": filesize, "version": version, "updated": updated, "directory": app, "name": name, "author": coder, "desc": desc, "url": source, "binary": binary, "long_desc": long_desc, "type": typee, "cat": category, "hits": cur_stats})


		# if there's no update according to the cache
		if app in cache and cache[app] == updated:
			continue

		print "Compressing " + app + "...<br>"

		# zip up this app
		zipf = zipfile.ZipFile("zips/%s.zip" % app, 'w', zipfile.ZIP_DEFLATED)
		zipdir(targdir + "/%s" % app, zipf)
		zipf.close()

	# last updated date
	out["updated"] = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

	# move "featured" apps to the front if available
	apps = out["apps"]
	for app in featured[::-1]:
		try:
			for rapp in apps:
				if rapp["directory"] == app:
					apps.insert(0, apps.pop(apps.index(rapp)))
					continue
		except:
			pass

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
	print "Updated directory.json !!!<br>"
	
	print "Running v1_gen.py for hbas 1.0...<br>"
	os.system("python v1_gen.py")

except Exception, err:
	exc_type, exc_value, exc_tb = sys.exc_info()
	lines = pformat(traceback.format_exception(exc_type, exc_value, exc_tb))
	
	print "<font color='red'><h2>500 Error</h2>"
	print "".join(ast.literal_eval(lines)).replace("\n", "<br>\n").replace("\\n", "<br>\n")
	
	