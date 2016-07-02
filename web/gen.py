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

style = """<head>
<!-- javascript and css styling for the icon tooltip and js for sorting-->
    <link rel="stylesheet" type="text/css" href="css/tooltipster.bundle.min.css" />
    <script type="text/javascript" src="http://code.jquery.com/jquery-1.10.0.min.js"></script>
    <script type="text/javascript" src="js/tooltipster.bundle.min.js"></script>
    <script type="text/javascript" src="js/sorttable.js"></script>
<style>
/*table style*/
#wiiubru {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
    color: white;
}
#wiiubru td, #wiiubru th {
    text-align: center;
    padding: 8px;
    border-radius: 5px;
}
#wiiubru tr
{ 
   -webkit-transition: -webkit-transform .2s ease-in-out;
    -webkit-transform: scale(0.9, 0.77);
}

#wiiubru tr:nth-child(even){background-color:rgba(63, 63, 63, 0.6)}
#wiiubru tr:nth-child(odd){background-color:rgba(42, 42, 42, 0.6);}
a:link {color: #b30000;}
a:visited {color: #b30000;}
a:hover {color: white;}

#wiiubru tr:hover {
   background-color:rgba(11, 10, 10, 0.6);
   -webkit-transform: scale(0.9, 1.1); 
}

#wiiubru th {
    padding-top: 12px;
    padding-bottom: 12px;
    background-color:rgba(59, 0, 0, 0.4); 

}
/*icon stretch and grey*/
.image 
{
    -webkit-filter: grayscale(75%);
    filter: grayscale(75%);
   -webkit-transition: -webkit-transform .2s ease-in-out;
    height: 50%;
    width: 50%;
}
.image:hover
{
    -webkit-filter: grayscale(0);
    filter: grayscale(0);
   -webkit-transition: -webkit-transform .2s ease-in-out;
    -webkit-transform: scale(1.5, 1.5);
}
body
{
    background-image: url("images/bg.png");
/*table sort icons .. to be continued*/
}
table.sortable th::after, th.sorttable_sorted::after, th.sorttable_sorted_reverse::after {
  content: " ";
  display: inline-block;
  width: 24px;
  height: 24px;
}
th.sorttable_sorted::after {
  background: url(images/my-sorted-icon.png);
  background-size: contain;
}
th.sorttable_sorted_reverse::after {
  background: url(images/my-sorted-reversed-icon.png);
  background-size: cover;
}
/*search box styling*/
input[type=text]
{
    float: left;
    width: 120px;
    border: 1px solid #cacaca;
    border-radius: 4px;
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    font-size: 16px;
    color: white;
    background-color: rgba(42, 42, 42, 0.4);
    -webkit-transition: width 0.4s ease-in-out;
    transition: width 0.4s ease-in-out;
}

input[type=text]:focus { 
    padding-left: 15px;
    float: left;
    width: 500;
    outline: none;
}
input[type=text]:hover {
  background-color: rgba(42, 42, 42, 0.8);  
}
.pull-left {
    margin-left: 80px;
    float: left;
}
.pull-right {
    margin-right: 80px;
    float: right;
}
.logo
{
    width : 1000;
    height: 160;
    max-width: 100%;
    text-decoration: none;
}
.icon
{
    width: 24px;
    height: 24px;
    -webkit-transition: -webkit-transform .2s ease-in-out;
    -webkit-transform: scale(1.0, 1.0);
    text-decoration: none;
}
.icon:hover
{
       -webkit-transform: scale(1.4, 1.4); 
}


</style>
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
<!-- website header logo ,nav and search box -->
<body>
<div style="text-align: center; width=100%;">
<a href="#">
<img src="images/hbasheader.png" class='logo'>
</a>
<br>
</div>
<div style='font-family: "Trebuchet MS", Arial, Helvetica, sans-serif; text-align: center; width=100%; color: white;'>

<span class='pull-left'><input type="text" id="search" placeholder="  Search..."></span>
<span class='pull-right'>
<a href="https://github.com/vgmoose/hbas">
<img src="images/github.png" class='icon' title='Github'>
</a>
<a href="https://kiwiirc.com/client?settings=76b79dc4baeccc495e0b78d2a052f984">
<img src="images/chat.png" class='icon' title='IRC'>
</a>
</span>
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

    html += "<tr><td class='tooltip' title='%s'><img src='%s' class='image'></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n" % (long_desc, icon, name, version, coder, desc, dlhref, src_link)
    
    yaml += "app: %s\n- %s\n- %s\n- %s\n- %s\n- %s\n" % (app, name, coder, desc, binary, version)
    
    if dozipping:
        zipf = zipfile.ZipFile("zips/%s.zip" % app, 'w', zipfile.ZIP_DEFLATED)
        zipdir("apps/%s" % app, zipf)
        zipf.close()


html += "</tbody></table></body>"
html += search

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

