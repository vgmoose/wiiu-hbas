#! /usr/bin/python
# This script generates the directory of apps
# which is parsed by the client app

import os
import xml.etree.ElementTree as ET
import json
import zipfile
import cgi

form = cgi.FieldStorage()


def zipdir(path, ziph):
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file))
            
print "Content-type: text/html\n\n"
print "<html>Updating Appstore......<br><br></html>"

style = """<style>

#wiiubru {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
}
#wiiubru table
{
    background-image: url("bg.jpg");
}

#wiiubru td, #wiiubru th {
    text-align: left;
    padding: 8px;
}
#wiiubru tr
{ 
   -webkit-transition: -webkit-transform .2s ease-in-out;
    -webkit-transform: scale(0.77, 0.77);
}

#wiiubru tr:nth-child(even){background-color: #f2f2f2}

#wiiubru tr:hover {
   background-color: #ddd;
   -webkit-transform: scale(1, 1); 
}

#wiiubru th {
    padding-top: 12px;
    padding-bottom: 12px;

}
.notice
{
    text-align: center;
    font-family: Arial, sans-serif;
    color: #999;
    margin-top: 5px;
    font-size: 14px;
    }

</style>
<div style="text-align: center; width=100%;">
<img src="hbas.png">
</div>
<div class="notice">You are currently viewing the web front-end to the Homebrew App Store. For more information, see the <a href=#">thread here</a>.</div>
"""

html = style + "<table id='wiiubru'><th><td>TITLE</td><td>AUTHOR</td><td>DESCRIPTION</td><td>DOWNLOAD</td><td>SOURCE</td></th>\n"
yaml = ""
    
try:
    os.mkdir("zips")
except:
    pass

d = {}
    
dozipping = False
    
if "dozips" in form:
    dozipping = True
    print "Generating new zip files as well<br>"
else:
    print "Not generating new zip files, to do that go <a href='gen.py?dozips=1'>here</a> (takes a while)<br>"

for app in os.listdir("apps"):
    if app.startswith("."):
        continue
    print "Indexing " + app + "...<br>"
    xmlfile = "apps/%s/meta.xml" % app
    
    name = app
    coder = "???"
    desc = "???"
    
    binary = None
    
    for file in os.listdir("apps/%s" % app):
        if file.endswith(".elf"):
            binary = file
    
    if not binary:
        continue
    
    if os.path.isfile(xmlfile):
        tree = ET.parse(xmlfile)

        name = coder = version = long_desc = desc = source = permissions = "????"
        try:
            name = tree.find("name").text
        except:
            pass
        try:
            coder = tree.find("coder").text
        except:
            pass
        try:
            desc = tree.find("short_description").text
        except:
            pass
        try:
            source = tree.find("url").text
        except:
            pass
        try:
            permissions = tree.find("permissions").text
        except:
            pass
        try:
            long_desc = tree.find("long_description").text
            long_desc = long_desc.replace("\n", "<br>")
        except:
            pass
        
        try:
            version = tree.find("version").text
        except:
            pass
    
    icon = "apps/%s/icon.png" % app
    if not os.path.isfile(icon):
        icon = "missing.png"
    
    dl = "zips/%s.zip" % app
    dlhref = "<a href='%s'>Download</a>" % dl

    src_link = "N/A"
    if source != "????":
        src_link = "<a href='%s'>Source</a>" % source

    d[app] = {"name": name, "author": coder, "desc": desc, "url": src_link, "binary": binary, "long_desc": long_desc}

    html += "<tr><td><img src='%s'></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n" % (icon, name, coder, desc, dlhref, src_link)
    
    yaml += "app: %s\n- %s\n- %s\n- %s\n- %s\n- %s\n" % (app, name, coder, desc, binary, version)
    
    if dozipping:
        zipf = zipfile.ZipFile("zips/%s.zip" % app, 'w', zipfile.ZIP_DEFLATED)
        zipdir("apps/%s" % app, zipf)
        zipf.close()


html += "</table>"

jsonstring = json.dumps(d, indent=4, separators=(',', ': '))

index = open("index.html", "w+")
index.write(html)
index.close()

directory = open("directory.yaml", "w+")
directory.write(yaml)
directory.close()

jsonout = open("directory.json", "w+")
jsonout.write(jsonstring)
jsonout.close()

print"<html><br> Update Complete !!!<br><br></html>"


#xml3 = open("directory.xml", "w+")
#xml3.write(dict2xml(d))
#xml3.close()
