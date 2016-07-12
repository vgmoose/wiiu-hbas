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

style = """<!DOCTYPE html>
<html lang="en">
<head>
<link rel='icon' href='http://appstore.wiiubru.com/icon.ico' type='image/x-icon'>
<link rel='shortcut icon' href='http://appstore.wiiubru.com/icon.ico' type='image/x-icon'>
<!-- javascript and css styling for the icon tooltip and js for sorting-->
    <link rel="stylesheet" type="text/css" href="css/wiiubru.css" />
    <link rel="stylesheet" type="text/css" href="css/tooltipster.bundle.min.css" />
    <link rel="stylesheet" type="text/css" href="css/font-awesome.min.css" />
    <script type="text/javascript" src="http://code.jquery.com/jquery-1.10.0.min.js"></script>
    <script type="text/javascript" src="js/tooltipster.bundle.min.js"></script>
    <script type="text/javascript" src="js/sorttable.js"></script>

<!-- tooltip script to display the homebrews long_description when mousing over the icon table cell -->
    <script>
        $(document).ready(function() {
            $('.tooltip').tooltipster({
                 animation: 'grow',
                 delay: 200,
                 side: 'right',
                 maxWidth: 700,
                 contentAsHTML: 'true' 
            });
        });
    </script>

</head>
<body>
<header>
<span class='pull-left'><input type='text' id='search' placeholder='&#xF002;' style='font-family:Arial, FontAwesome'></span>
<span class='pull-right'>
<a href="https://github.com/vgmoose/hbas" target='_blank'>
<i class='fa fa-github fa-2x' aria-hidden="true"></i>
</a>

<a href="https://kiwiirc.com/client?settings=76b79dc4baeccc495e0b78d2a052f984" target='_blank'>
<i class="fa fa-comments-o fa-2x" aria-hidden="true"></i>
</a>

</span>
</header>

<br>
<div style="text-align: center;">
<a href="https://gbatemp.net/threads/release-homebrew-app-store.433275/" target='_blank'>
<img src="images/hbasheader.png" class='logo'>
</a>
<br>
</div>

"""
search = """
<!-- search script -->
<script>
var $rows = $('#wiiubru tbody tr');
$('#search').keyup(function() {
    var val = $.trim($(this).val()).replace(/ +/g, ' ').toLowerCase();
    
    $rows.show().filter(function() {
        var text = $(this).text().replace(/\s+/g, ' ').toLowerCase();
        return !~text.indexOf(val);
    }).hide();
});
</script>
"""

footer = """
<footer>
<br>
<span class='pull-right' style='color: white'>
You are currently viewing the web front-end to the Homebrew App Store.
</span>
<span class='pull-left'>
<a href="https://github.com/vgmoose/hbas" target='_blank'>
<i class='fa fa-github fa-2x' aria-hidden='true'></i>
</a>

<a href="https://kiwiirc.com/client?settings=76b79dc4baeccc495e0b78d2a052f984" target='_blank'>
<i class="fa fa-comments-o fa-2x" aria-hidden="true"></i>
</a>

</span>
</footer>
"""

html = style + "<table id='wiiubru' class='sortable'><thead><tr><th class='sorttable_nosort'>ICON</th><th style = 'cursor:pointer;' title = 'Click to sort.'>TITLE</th><th class='sorttable_nosort'>VERSION</th><th style = 'cursor:pointer;' title = 'Click to sort.'>AUTHOR</th><th style = 'cursor:pointer;' title = 'Click to sort.'>DESCRIPTION</th><th class='sorttable_nosort'>DOWNLOAD</th><th class='sorttable_nosort'>SOURCE</th></tr></thead><tbody>\n"
yaml = ""
    
try:
    os.mkdir("zips")
except:
    pass

d = {}
    
dozipping = False
    
if "dozips" in form:
    dozipping = True
    print "Generating new zip files as well<br><br>"
else:
    print "Not generating new zip files, to do that go <a href='gen.py?dozips=1'>here</a> (takes a while)<br><br>"

for app in os.listdir("apps"):
    if app.startswith("."):
        continue
    print "Indexing " + app + "...<br>"
    xmlfile = "apps/%s/meta.xml" % app
    
    name = app
    coder = "???"
    desc = "???"
    long_desc = "???"
    version = '???'
    source = '????'
    
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
            long_desc = long_desc.replace("'", "!") 
        except:
            pass
        
        try:
            version = tree.find("version").text
        except:
            pass
    
    icon = "apps/%s/icon.png" % app
    if not os.path.isfile(icon):
        icon = "images/missing.png"
    
    dl = "zips/%s.zip" % app
    dlhref = "<a href='%s'>Download</a>" % dl

    src_link = "N/A"
    if source != "????":
        src_link = "<a href='%s' target='_blank'>Source</a>" % source

    d[app] = {"name": name, "author": coder, "desc": desc, "url": src_link, "binary": binary, "long_desc": long_desc}

    html += "<tr><td class='tooltip' title='%s'><img src='%s' class='image' alt='%s'></td><td style='text-transform: uppercase;'>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n" % (long_desc, icon, desc, name, version, coder, desc, dlhref, src_link)
    
    yaml += "app: %s\n- %s\n- %s\n- %s\n- %s\n- %s\n" % (app, name, coder, desc, binary, version)
    
    if dozipping:
        zipf = zipfile.ZipFile("zips/%s.zip" % app, 'w', zipfile.ZIP_DEFLATED)
        zipdir("apps/%s" % app, zipf)
        zipf.close()


html += "</tbody></table><br><br>"
html += search
html += footer
html += "</body>"
html += "</html>"


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
print "<a href='index.html'>See the Results.</a>"

#xml3 = open("directory.xml", "w+")
#xml3.write(dict2xml(d))
#xml3.close()

