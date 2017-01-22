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

chan_zips = []

try:
	# get list of zip files for channels
	file_list = os.listdir("chan_zips")

	for czip in file_list:
		# strip the .zip ending, and add it to the chan_zips array
		chan_zips.append(czip.rstrip(".zip"))
except:
	pass

def cgiprint(inp):
	# print and flush, to update web right away
	print(inp)
	sys.stdout.flush()
	
def run(cmd):
	# run a unix command, and suppress the output
	return os.system(cmd + " 2> /dev/null")

# try and catch ANY error so web has output
try:

	# list of featured apps, these will be moved to the top of the json file
	featured = ["asturoids", "spacegame", "flappy_bird", "homebrew_launcher", "loadiine_gx2", "retro_launcher", "pong", "pacman", "snake", "mgba_libretro", "snes9x2010_libretro",  "gambatte_libretro", "CHIP8", "quicknes_libretro", "mame2003_libretro", "genesis_plus_gx_libretro", "appstore", "cfwbooter", "mocha", "haxchi", "hidtovpad", "geckiine", "wuphax", "saviine", "ftpiiu", "wudump", "u-paint", "cbhc", "hid_keyboard_monitor", "LiveSynthesisU", "keyboard_example"]

	# This function zips the incoming path into the file in ziph
	def zipdir(path, ziph):
		for root, dirs, files in os.walk(path):
			for file in files:
				ppath = os.path.join(root, file)
				ziph.write(ppath, ppath.lstrip("sdroot/"))

	# This function parses out several attributes from xml
	def xml_read(incoming, tree, app):
		try:
			outgoing = []
			for key in incoming:
				try:
					outgoing.append(tree.find(key).text)
				except:
					cgiprint("%s: Missing &lt;%s&gt; in meta.xml<br>" % (app, key))
					outgoing.append("N/A")
			return tuple(outgoing)
		except:
			return False

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
				target_app = line.split("/")[3].rstrip(",\r\n")
				stats[target_app] = 0
				
			# we hit a line with nonempty hit data
			if line.startswith("Hits") and line.split(",")[1].rstrip() != "":
				stats[target_app] += int(line.split(",")[1].rstrip())
	except:
		cgiprint("Encountered an error parsing stats/Report.csv<br>")
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

		if not os.path.exists(iconfile) or imghdr.what(iconfile) != "png":
			cgiprint("Skipping %s as its icon.png isn't a png file or doesn't exist<br>" % app)
			continue

		# create some default fields
		name = coder = desc = long_desc = version = source = updated = filesize = category = src_link = "N/A"
		typee = "elf"

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
			resp = xml_read(["name", "coder", "short_description", "url", "long_description", "version", "category"], tree, app)
			
			# skip app if invalid response from meta parsing
			if not resp:
				cgiprint("Error in meta.xml for %s, skipping...<br>" % app)
				continue
				
			# set attributes
			name, coder, desc, source, long_desc, version, category = resp

		# sanitize long_desc for json output
		long_desc = long_desc.replace("\n", "\\n").replace("\t", "\\t")

		# get icon path
		icon = targdir + "/%s/icon.png" % app
		
		# try to load stats if available
		cur_stats = cur_stats_web = 0
		if app in stats:
			cur_stats = stats[app]
		if app+".zip" in stats:
			cur_stats_web = stats[app+".zip"]

		hasChannel = "_"
		# does this app have a channel?
		if app in chan_zips:
			hasChannel = "Channel"

		# append to output json
		out["apps"].append({"filesize": filesize, "version": version, "updated": updated, "directory": app, "name": name, "author": coder, "desc": desc, "url": source, "binary": binary, "long_desc": long_desc, "type": typee, "cat": category, "app_hits": cur_stats, "web_hits": cur_stats_web, "channel": hasChannel})

		# if the .deletetoupdate file exists, don't compress
		# also if the zip doesn't exist already
		if os.path.exists("apps/%s/.deletetoupdate" % app) and os.path.exists("zips/%s.zip" % app):
			continue
		
		cgiprint("Compressing " + app + "...<br>")
		
		# move the app to a staging directory, to create a good structue
		# with which to zip it with
		# using unix commands cause python has weird syntax for this
		run("rm -rf sdroot")
		run("mkdir -p sdroot/wiiu/apps")
		
		# copy over the app folder
		run("cp -rf apps/%s sdroot/wiiu/apps" % app)
		
		# remove some junk files
		run("find sdroot -name .deletetoupdate -delete -o -name .DS_Store -delete")
		
		# move the other sd files into place (if they exist)
		run("mv sdroot/wiiu/apps/%s/sd/* sdroot" % app)
		run("rmdir sdroot/wiiu/apps/%s/sd" % app)

		# zip up this whole sdroot package
		zipf = zipfile.ZipFile("zips/%s.zip" % app, 'w', zipfile.ZIP_DEFLATED)
		zipdir("sdroot/", zipf)
		zipf.close()
		
		# create the .deletetoupdate file
		open("apps/%s/.deletetoupdate" % app, 'a').close()

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

	# print done and run legacy scripts
	cgiprint("Updated directory.json !!!<br>")
	
	cgiprint("Running v1_gen.py for hbas 1.0...<br>")
	os.system("python v1_gen.py")

except Exception, err:
	exc_type, exc_value, exc_tb = sys.exc_info()
	lines = pformat(traceback.format_exception(exc_type, exc_value, exc_tb))
	
	print "<font color='red'><h2>500 Error</h2>"
	print "".join(ast.literal_eval(lines)).replace("\n", "<br>\n").replace("\\n", "<br>\n")
	
	
