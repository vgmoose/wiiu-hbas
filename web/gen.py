#! /usr/bin/python
# This script generates the directory of apps
# which is parsed by the client app
# and creates the web interface for wiiubru(by default)

import os
import xml.etree.ElementTree as ET
import json
import zipfile
import cgi
import datetime

exedate = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

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
<ul>
<li style='float: left'><img src='http://www.wiiubru.com/forum/forum/styles/digi_red/theme/images/logo.png' alt='wiiubru' width='95px' height='45px'></img></li>
<li style='float: left'><input type='text' id='search' placeholder='&#xF002;' style='font-family:Arial, FontAwesome'></li>
  <li class="dropdown">
    <a href="#" class="dropbtn"><i class="fa fa-link" aria-hidden="true"></i> USEFUL LINKS</a>
    <div class="dropdown-content">
      <a href="https://gbatemp.net/threads/release-homebrew-app-store-for-wiiu.433275/" target='_blank'><i class="fa fa-external-link-square" aria-hidden="true"></i> HBAS GBAtemp</a>
      <a href="https://gbatemp.net/threads/www-wiiubru-com-homebrew-launcher-website-and-5-5-exploit-hosting.411691/" target='_blank'><i class="fa fa-external-link-square" aria-hidden="true"></i> WiiUbru GBAtemp</a>
      <a href="https://www.paypal.me/pwsincd" target='_blank'><i class="fa fa-paypal" aria-hidden="true"></i> DONATE</a>
      <a href="http://www.wiiubru.com/ask" target='_blank'><i class="fa fa-question" aria-hidden="true"></i> ASK</a>
      <a href="https://github.com/vgmoose/hbas" target='_blank'><i class="fa fa-github" aria-hidden="true"></i> GIT</a>
    </div>
  </li>
  <li><a href="http://www.wiiubru.com/appstore/starterkit/starter_pack.rar"><i class="fa fa-download" aria-hidden="true"></i> STARTER PACK</a></li>
  <li><a href="https://kiwiirc.com/client/irc.kiwiirc.com/wiiubru" target='_blank'><i class="fa fa-comments-o" aria-hidden="true"></i> IRC</a></li>
  <li><a href="http://forum.wiiubru.com/" target='_blank'><i class="fa fa-forumbee" aria-hidden="true"></i> FORUM</a></li>
  <li><a href="http://www.wiiubru.com/appstore/stats/index.html" target='_blank'><i class="fa fa-line-chart" aria-hidden="true"></i> STATS</a></li>
  <li><a href="http://www.wiiubru.com/appstore/index.html"><i class="fa fa-download" aria-hidden="true"></i> HBL APPS</a></li>
  <li><a href="http://www.wiiubru.com/appstore/rpx.html"><i class="fa fa-download" aria-hidden="true"></i> RPX APPS</a></li>
</ul>
</header>
<br>
<div style="text-align: center;">
<a href="https://gbatemp.net/threads/release-homebrew-app-store.433275/" target='_blank'>
<img src="images/hbasheader.png" class='logo'>
</a>
<br>
</div>
<div align="center">
<script async src="//pagead2.googlesyndication.com/pagead/js/adsbygoogle.js"></script>
<!-- wiiubru -->
<ins class="adsbygoogle"
     style="display:inline-block;width:728px;height:90px"
     data-ad-client="ca-pub-7623664678937879"
     data-ad-slot="9058831412"></ins>
<script>
(adsbygoogle = window.adsbygoogle || []).push({});
</script>
</div>
"""
search = """
<script>
// search script
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
<a href="http://www.wiiubru.com/appstore/stats/index.html" target='_blank' title='STATS'>
<i class="fa fa-line-chart fa-2x" aria-hidden="true"></i>
</a>
.
<a href="http://forum.wiiubru.com/" target='_blank' title='FORUMS'>
<i class="fa fa-forumbee fa-2x" aria-hidden="true"></i>
</a>
.
<a href="https://kiwiirc.com/client?settings=76b79dc4baeccc495e0b78d2a052f984" target='_blank' title='IRC'>
<i class="fa fa-comments-o fa-2x" aria-hidden="true"></i>
</a>
.
<a href="https://github.com/vgmoose/hbas" target='_blank' title='GITHUB'>
<i class='fa fa-github fa-2x' aria-hidden='true'></i>
</a>
.
<a href="http://www.wiiubru.com/appstore/starterkit/starter_pack.rar" target='_blank' title='STARTER PACK'>
<i class="fa fa-download fa-2x" aria-hidden="true"></i>
</a>
.
<a href="https://www.paypal.me/pwsincd" target='_blank' title='DONATE'>
<i class="fa fa-paypal fa-2x" aria-hidden="true"></i>
</a>
<a href="https://www.wiiubru.com/ask" target='_blank' title='ASK'>
<i class="fa fa-question fa-2x" aria-hidden="true"></i>
</a>
</span>
</footer>
"""

lastupdate = "<div style='color: white;'>Updated on : %s</div>" % (exedate)

html = lastupdate + style + "<table id='wiiubru' class='sortable'><thead><tr><th class='sorttable_nosort'>ICON</th><th style = 'cursor:pointer;' title = 'Click to sort.'><i class='fa fa-sort' aria-hidden='true'></i> TITLE</th><th class='sorttable_nosort'>VERSION</th><th style = 'cursor:pointer;' title = 'Click to sort.'><i class='fa fa-sort' aria-hidden='true'></i> AUTHOR</th><th style = 'cursor:pointer;' title = 'Click to sort.'><i class='fa fa-sort' aria-hidden='true'></i> DESCRIPTION</th><th class='sorttable_nosort'>DOWNLOAD</th><th class='sorttable_nosort'>SOURCE</th></tr></thead><tbody>\n"
html2 = lastupdate + style + "<table id='wiiubru' class='sortable'><thead><tr><th class='sorttable_nosort'>ICON</th><th style = 'cursor:pointer;' title = 'Click to sort.'><i class='fa fa-sort' aria-hidden='true'></i> TITLE</th><th class='sorttable_nosort'>VERSION</th><th style = 'cursor:pointer;' title = 'Click to sort.'><i class='fa fa-sort' aria-hidden='true'></i> AUTHOR</th><th style = 'cursor:pointer;' title = 'Click to sort.'><i class='fa fa-sort' aria-hidden='true'></i> DESCRIPTION</th><th class='sorttable_nosort'>DOWNLOAD</th><th class='sorttable_nosort'>SOURCE</th></tr></thead><tbody>\n"

yaml = ""
yaml2 = ""

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

appsandgames = os.listdir("apps") + os.listdir("games")
gamepoint = len(os.listdir("apps"))

count = 0
for app in appsandgames:
    if app.startswith("."):
        continue

    if count >= gamepoint:
        typee = "rpx"
        targdir = "games"
    else:
        typee = "hbl"
        targdir = "apps"

    count += 1

    print "Indexing " + app + "...<br>"
    xmlfile = targdir + "/%s/meta.xml" % app

    name = app
    coder = "???"
    desc = "???"
    long_desc = "???"
    version = '???'
    source = '????'

    binary = None

    for file in os.listdir(targdir + "/%s" % app):
        if file.endswith(".elf"):
            binary = file
        if file == "code":
            for file2 in os.listdir(targdir + "/%s/code" % app):
                if file2.endswith(".rpx"):
                    binary = file2

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

    if typee == "hbl":
        icon = targdir + "/%s/icon.png" % app
    else:
        icon = targdir + "/%s/meta/icon.png" % app
    if not os.path.isfile(icon):
        icon = "images/missing.png"

    dl = "zips/%s.zip" % app
    dlhref = "<a href='%s'>Download</a>" % dl

    src_link = "N/A"
    if source != "????":
        src_link = "<a href='%s' target='_blank'>Source</a>" % source

    d[app] = {"name": name, "author": coder, "desc": desc, "url": src_link, "binary": binary, "long_desc": long_desc,
              "type": typee}

    if typee == "hbl":
        yaml += "app: %s\n- %s\n- %s\n- %s\n- %s\n- %s\n" % (app, name, coder, desc, binary, version)
        html += "<tr><td class='tooltip' title='%s'><img src='%s' class='image' alt='%s'></td><td style='text-transform: uppercase;'>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n" % (
        long_desc, icon, desc, name, version, coder, desc, dlhref, src_link)
    else:
        html2 += "<tr><td class='tooltip' title='%s'><img src='%s' class='image' alt='%s'></td><td style='text-transform: uppercase;'>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n" % (
        long_desc, icon, desc, name, version, coder, desc, dlhref, src_link)

    yaml2 += "app: %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n" % (app, name, coder, desc, binary, version, typee)

    if dozipping:
        zipf = zipfile.ZipFile("zips/%s.zip" % app, 'w', zipfile.ZIP_DEFLATED)
        zipdir(targdir + "/%s" % app, zipf)
        zipf.close()

html += "</tbody></table><br><br>"
html2 += "</tbody></table><br><br>"
html += search
html2 += search
html += footer
html2 += footer
html += "</body></html>"
html2 += "</body></html>"

jsonstring = json.dumps(d, indent=4, separators=(',', ': '))

index = open("index.html", "w+")
index.write(html)
index.close()

index = open("rpx.html", "w+")
index.write(html2)
index.close()

directory = open("directory.yaml", "w+")
directory.write(yaml)
directory.close()

directory = open("directory12.yaml", "w+")
directory.write(yaml2)
directory.close()

jsonout = open("directory.json", "w+")
jsonout.write(jsonstring)
jsonout.close()

print"<html><br> Update Complete !!!<br><br></html>"
print "<a href='index.html'>See the Results.</a>"

# xml3 = open("directory.xml", "w+")
# xml3.write(dict2xml(d))
# xml3.close()
