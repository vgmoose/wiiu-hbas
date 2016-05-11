#!/bin/python
# This script generates the directory of apps
# which is parsed by the client app

import os
import xml.etree.ElementTree as ET

style = """<style>td img { width: 100px; height: 37px; }
table
{
	font-weight: 300;
	padding: 10px;
	padding-top: 30px;
	width: 100%;
	color: #676767;
}
.notice
{
    text-align: center;
    font-family: Arial, sans-serif;
    color: #999;
    margin-top: 5px;
    font-size: 14px;
}
td
{
	padding: 5px;
    align: center;

}
td:nth-child(1)
{
    text-align: right;
}
a
{
    font-color: blue;
    font-family: Arial, sans-serif;
}
td:nth-child(2)
{
    font-weight: bold;
    font-family: Arial, sans-serif;
    font-size: 18px;
    width: 250px;
}
tr:nth-child(even)

{
	background-color: ghostwhite;
}

</style>
<div style="text-align: center; width=100%;">
<img src="hbas.png">
</div>
<div class="notice">You are currently viewing the web front-end to the Homebrew App Store. For more information, see the <a href=#">thread here</a>.</div>
"""

html = style + "<table>\n"
yaml = ""

for app in os.listdir("apps"):
    if app.startswith("."):
        continue
        
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

        name = tree.find("name").text
        coder = tree.find("coder").text
        desc = tree.find("short_description").text
    
    icon = "apps/%s/icon.png" % app
    if not os.path.isfile(icon):
        icon = "missing.png"
    
    dl = "zips/%s.zip" % app
        
    html += "<tr><td><img src='%s'></td><td>%s</td><td>%s</td><td>%s</td><td><a href='%s'>Download</a></td></tr>\n" % (icon, name, coder, desc, dl)
    
    yaml += "app: %s\n- %s\n- %s\n- %s\n- %s\n" % (app, name, coder, desc, binary)
    
html += "</table>"

index = open("index.html", "w+")
index.write(html)
index.close()

directory = open("directory.yaml", "w+")
directory.write(yaml)
directory.close()