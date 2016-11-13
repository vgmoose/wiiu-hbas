<?php
error_reporting(E_ALL | E_STRICT);
ini_set('display_errors', 1);


$display = isset($_GET['display']);

function display_array($a) {
    global $display;
    if ($display) {
        echo '<ul>';
        foreach($a as $k=>$v) {
            echo '<li>['.$k.'] ';
            if (is_array($v)) {
                display_array($v);
            } elseif (is_bool($v)) {
                echo ($v?'TRUE':'FALSE');
            } else {
                echo $v;
            }
            echo '</li>';
        }
        echo '</ul>';
    }
}
function display_array_whtml($a) {
    global $display;
    if ($display) {
        echo '<ul>';
        foreach($a as $k=>$v) {
            echo '<li>['.$k.'] ';
            if (is_array($v)) {
                display_array($v);
            } elseif (is_bool($v)) {
                echo ($v?'TRUE':'FALSE');
            } else {
                echo str_replace(PHP_EOL,'<br/>',htmlentities($v));
            }
            echo '</li>';
        }
        echo '</ul>';
    }
}

function display($s) {
    global $display;
    if ($display)
        if (is_array($s)) {
            // display_array($s);
            echo '<pre>';
            print_r($s);
            echo '</pre>';
        } else
            echo $s;
}

display('
<!DOCTYPE html>
<html lang ="en">
<head>
    <meta charset="utf-8"/>
    <title>Refreshing repository (beta)</title>
    <style>
        ul {
            margin: 0;
        }
        
        .processing {
            border-radius: 5px;
            margin: 5px auto;
            padding: 5px;
            border: 1px solid blue;
            max-width: 700px;
        }
        
        .switchable-item {
            display: none;
            border-radius: 5px;
            margin: 5px auto;
            padding: 5px;
            border: 1px solid black;
            word-wrap: break-word;
            overflow: auto;
        }
        
        pre .line-number {
            /* Ukuran line-height antara teks di dalam tag <code> dan <span class="line-number"> harus sama! */
            display: block;
            float:left;
            border-right:1px solid gray;
            color: gray;
            text-align:right;
            padding-right: 3px;
            margin-right: 3px
        }

        pre .line-number span {
            display:block;
        }

        pre .cl {
            display:block;
            clear:both;
        }
    </style>
    
    <script src="js/jquery-1.12.4.min.js"></script>
    <script>
    $(document).ready(function(){
        $(":button").click(function(){
            $(this).parent().find(".switchable-item").toggle();
        });
        
        var pre = document.getElementsByTagName(\'pre\'),
            pl = pre.length;
        for (var i = 0; i < pl; i++) {
            pre[i].innerHTML = \'<span class="line-number"></span>\' + pre[i].innerHTML + \'<span class="cl"></span>\';
            var num = pre[i].innerHTML.split(/\\n/).length;
            for (var j = 0; j < num; j++) {
                var line_num = pre[i].getElementsByTagName(\'span\')[0];
                line_num.innerHTML += \'<span>\' + (j + 1) + \'</span>\';
            }
        }
    });
    </script>
</head>
<body>');

/*******************************
    Define some constants
********************************/

define("BOOLT", "boolean"     );
define("INTT", "integer"     );
// define("FLT", "double"      );
define("STRT", "string"      );
define("ARRT", "array"       );
// define("OBJ", "object"      );
// define("RSRC", "resource"    );
// define("", "NULL"        );
// define("UNKT", "unknown type");

define("KEY","_key_");
define("VAL","_val_");
define("DEPTH_SEPARATOR",":");

define("NO_ORDER"     ,0);
define("NAME_ORDER"   ,1);
// define("NAMER_ORDER"  ,1);
define("AUTHOR_ORDER"  ,2);
// define("CODERR_ORDER" ,3);
define("DESC_ORDER"   ,3);
// define("DESCR_ORDER"  ,5);
define("CUSTOM_ORDER" ,4);
// define("CUSTOMR_ORDER",7);

$order = ['none','name','author','desc','custom'];

define("DIR_SEPARATOR","/");


/*******************************
    Relevant paths
********************************/

// default values for store-repo-info.json

// First, define what dirs will be used
$info = array();
$info['appsdir']      = 'apps';
$info['xtradir']      = 'apps_extra';
$info['zipsdir']      = 'zips'; // $zipdir.'/'.$ext.'/'.$app.'-'.$ext.'.zip'

$info['ext_sd_root']  = 'hbl';      // files used by app outside of its own dir
$info['ext_icons']    = 'icons';    // alternate icons
$info['ext_hosting']  = 'www';      // files for self-hosting
$info['ext_pc_tools'] = 'pc_tools'; // files meant to be used on pc (server, elf editor, etc.)

// Path to config file with options regarding how to update the repo
$info['appstoreconf'] = 'store-repo-conf.json';

// Files used by app or end-user
$info['appstorepage'] = 'appstore_beta.html';
$info['appstorefile'] = 'directory_beta.yaml';
$info['appstorejson'] = 'directory_beta.json';

$infofile = 'store-repo-info.json';
$infokeys = array_keys($info);

/*******************************
    Default configuration
********************************/

$ignore_list = array('.','..');

$apps_info = array();

$config_type = array();
$config_type[''] = [ARRT];
$config_type[DEPTH_SEPARATOR.KEY] = [STRT];
$config = array();
$config_type[DEPTH_SEPARATOR.'ignore'] = [ARRT];
$config['ignore'] = array();
$config_type[DEPTH_SEPARATOR.'ignore'.DEPTH_SEPARATOR.KEY] = [INTT];
$config_type[DEPTH_SEPARATOR.'ignore'.DEPTH_SEPARATOR.VAL] = [STRT];
$config_type[DEPTH_SEPARATOR.'order'] = [ARRT];
$config['order'] = array();
$config_type[DEPTH_SEPARATOR.'order'.DEPTH_SEPARATOR.KEY] = [STRT];
$config_type[DEPTH_SEPARATOR.'order'.DEPTH_SEPARATOR.'type'] = [STRT,INTT];
$config['order']['type'] = NO_ORDER;
$config_type[DEPTH_SEPARATOR.'order'.DEPTH_SEPARATOR.'reverse'] = [BOOLT];
$config['order']['reverse'] = false;
$config_type[DEPTH_SEPARATOR.'order'.DEPTH_SEPARATOR.'custom_order'] = [ARRT];
$config['order']['custom_order'] = array();
$config_type[DEPTH_SEPARATOR.'order'.DEPTH_SEPARATOR.'custom_order'.DEPTH_SEPARATOR.KEY] = [INTT];
$config_type[DEPTH_SEPARATOR.'order'.DEPTH_SEPARATOR.'custom_order'.DEPTH_SEPARATOR.VAL] = [STRT];
$config_type[DEPTH_SEPARATOR.'yamlfields'] = [ARRT];
$config['yamlfields'] = ['name','author','desc','binary','version'];
$config_type[DEPTH_SEPARATOR.'yamlfields'.DEPTH_SEPARATOR.KEY] = [INTT];
$config_type[DEPTH_SEPARATOR.'yamlfields'.DEPTH_SEPARATOR.VAL] = [STRT];
$config_type[DEPTH_SEPARATOR.'jsonfields'] = [ARRT];
$config['jsonfields'] = ['name','author','desc','url','binary','version','long_desc','icons'];
$config_type[DEPTH_SEPARATOR.'jsonfields'.DEPTH_SEPARATOR.KEY] = [INTT];
$config_type[DEPTH_SEPARATOR.'jsonfields'.DEPTH_SEPARATOR.VAL] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'] = [ARRT];
$config['default_values'] = array();
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.KEY] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'force'] = [BOOLT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'] = [ARRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.KEY] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL] = [ARRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.KEY] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'name'] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'author'] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'desc'] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'long_desc'] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'binary'] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'version'] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'url'] = [ARRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'url'.DEPTH_SEPARATOR.KEY] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'url'.DEPTH_SEPARATOR.'named'] = [ARRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'url'.DEPTH_SEPARATOR.'named'.DEPTH_SEPARATOR.KEY] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'url'.DEPTH_SEPARATOR.'named'.DEPTH_SEPARATOR.VAL] = [STRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'url'.DEPTH_SEPARATOR.'unnamed'] = [ARRT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'url'.DEPTH_SEPARATOR.'unnamed'.DEPTH_SEPARATOR.KEY] = [INTT];
$config_type[DEPTH_SEPARATOR.'default_values'.DEPTH_SEPARATOR.'apps'.DEPTH_SEPARATOR.VAL.DEPTH_SEPARATOR.'url'.DEPTH_SEPARATOR.'unnamed'.DEPTH_SEPARATOR.VAL] = [STRT];

$config_type_keys = array_keys($config_type);

function valid_config_type($config, $prefix='') {
    global $config_type;
    global $config_type_keys;
    // display('Validity from prefix "'.$prefix.'" (type: '.gettype($config).')<br/>'.PHP_EOL);
    if (!array_key_exists($prefix,$config_type)) {
        // display('Prefix "'.$prefix.'": valid!<br/>'.PHP_EOL);
        return true;
    } else {
        if (in_array(gettype($config),$config_type[$prefix])) {
            if (is_array($config)) {
                $still_valid = true;
                foreach($config as $key=>$val) {
                    if (!$still_valid)
                        break;
                    $still_valid = valid_config_type($key,$prefix.DEPTH_SEPARATOR.KEY);
                    if (!$still_valid)
                        break;
                    if (in_array($prefix.DEPTH_SEPARATOR.$key,$config_type_keys)) {
                        $still_valid = valid_config_type($val,$prefix.DEPTH_SEPARATOR.$key);
                    } else {
                        $still_valid = valid_config_type($val,$prefix.DEPTH_SEPARATOR.VAL);
                    }
                }
                // display('Prefix "'.$prefix.'": '.($still_valid?'':'in').'valid!<br/>'.PHP_EOL);
                return $still_valid;
            } else {
                // display('Prefix "'.$prefix.'": valid!<br/>'.PHP_EOL);
                return true;
            }
        } else {
            // display('Prefix "'.$prefix.'": invalid! (expecting something in ['.$config_type[$prefix][0].','.$config_type[$prefix][1].']<br/>'.PHP_EOL);
            return false;
        }
    }
}

/*******************************
    Reading configuration
********************************/

// general info
display('<div class="processing"><strong>General information</strong><br/>'.PHP_EOL);
if (is_file($infofile)) {
    display('General information file found.'.PHP_EOL);
    if (($infostring = file_get_contents($infofile)) !== false) {
        display('<div class="switchable"><button type="button" style="font-family:Monospace">'.$infofile.'</button>'.PHP_EOL.'<pre class="switchable-item">'.htmlentities($infostring).'</pre></div>'.PHP_EOL);
        display('<div class="switchable"><button type="button">Configuration parsing</button>'.PHP_EOL.'<div class="switchable-item">');
        $infoconfig = json_decode($infostring,true);
        if (json_last_error() === JSON_ERROR_NONE) {
            display('Applying configuration:'.PHP_EOL);
            display('<ul>'.PHP_EOL);
            foreach ($infokeys as $infokey) {
                if (array_key_exists($infokey,$infoconfig)) {
                    display('<li>'.$infokey.': '.$info[$infokey].' → '.$infoconfig[$infokey].'</li>'.PHP_EOL);
                    $info[$infokey] = $infoconfig[$infokey];
                }
            }
            display('</ul>'.PHP_EOL);
        } else {
            display('Error while decoding: '.json_last_error_msg().PHP_EOL.'<br/>Standard settings will be applied.'.PHP_EOL);
        }
        display('</div></div>'.PHP_EOL);
    } else {
        display('Error opening <span style="font-family: Monospace">'.$infofile.'</span>.'.PHP_EOL.'<br/>Standard settings will be applied.'.PHP_EOL);
    }
} else {
    display('<span style="font-family: Monospace">'.$infofile.'</span> not found, going with default configuration.');
}
display('<div class="switchable"><button type="button">Configuration</button>'.PHP_EOL.'<div class="switchable-item"><ul>');
foreach ($info as $key=>$val) {
    display('<li>'.$key.': '.$val.'</li>'.PHP_EOL);
}
display('</ul></div></div>');
display('</div>'.PHP_EOL);

// zipping!
$zipping = isset($_GET['zip']);
display('<div class="processing"><strong>Zipping</strong><br/>'.($zipping?'Enabled':'Disabled').'.</div>'.PHP_EOL);

// configuration
display('<div class="processing"><strong>Configuration</strong><br/>'.PHP_EOL);
if (is_file($info['appstoreconf'])) {
    display('Configuration file found.'.PHP_EOL);
    if (($configstring = file_get_contents($info['appstoreconf'])) !== false) {
        display('<div class="switchable"><button type="button" style="font-family:Monospace">'.$info['appstoreconf'].'</button>'.PHP_EOL.'<pre class="switchable-item">'.htmlentities($configstring).'</pre></div>'.PHP_EOL);
        display('<div class="switchable"><button type="button">Configuration parsing</button>'.PHP_EOL.'<div class="switchable-item">');
        $config_parsed = json_decode($configstring,true);
        if (json_last_error() === JSON_ERROR_NONE) {
            if (valid_config_type($config_parsed)) {
                display('<ul>'.PHP_EOL);
                foreach($config_parsed as $key=>$figuration) {
                    display('<li>Applying settings for '.$key.'</li>'.PHP_EOL);
                    $config[$key] = $figuration;
                }
                display('</ul>'.PHP_EOL);
            } else {
                display('Type error while parsing. Standard settings will be applied'.PHP_EOL);
            }
        } else {
            display('Error while decoding: '.json_last_error_msg().PHP_EOL.'<br/>Standard settings will be applied.'.PHP_EOL);
        }
        display('</div></div>'.PHP_EOL);
    } else {
        display('Error opening <span style="font-family: Monospace">'.$info['appstoreconf'].'</span>.'.PHP_EOL.'<br/>Standard settings will be applied.'.PHP_EOL);
    }
} else {
    display('<span style="font-family: Monospace">'.$info['appstoreconf'].'</span> not found, going with default configuration.');
}
display('<div class="switchable"><button type="button">Configuration</button>'.PHP_EOL.'<div class="switchable-item">');
if (is_string($config['order']['type'])) {
    if (in_array($config['order']['type'],$order)) {
        $config['order']['type'] = array_search($config['order']['type'],$order);
    } else {
        $config['order']['type'] = NO_ORDER;
    }
} else {
    if (!key_exists($config['order']['type'],$orders)) {
        $config['order']['type'] = NO_ORDER;
    }
}
$ignore_list = array_merge($ignore_list,$config['ignore']);
display_array($config);
display('</div></div>');

display('</div>'.PHP_EOL);


/*******************************
    Browsing apps dir
********************************/

function scandir_rec($dir) {
    $fullcontent = array();
    $dir_content = scandir($dir);
    foreach ($dir_content as $key => $content) {
        $path = $dir.DIR_SEPARATOR.$content;
        if (is_dir($path)) {
            if ($content != '.' && $content != '..')
                $fullcontent[$content] = scandir_rec($path);
        } else
            $fullcontent[] = $content;
    }
    return $fullcontent;
}

function scantolist($fullcontent,$prefix_path='') {
    $filelist = array();
    foreach ($fullcontent as $keyordir => $content) {
        if (is_array($content)) {
            $new_prefix = $prefix_path.$keyordir.DIR_SEPARATOR;
            $filelist[] = $new_prefix;
            $filelist = array_merge($filelist,scantolist($content, $new_prefix));
        } else
            $filelist[] = $prefix_path.$content;
    }
    return $filelist;
}


function listfiles($dir) {
    return scantolist(scandir_rec($dir));
}

display('<div class="processing"><strong>Browsing application folder</strong><br/>'.PHP_EOL);
if (is_dir($info['appsdir'])) {
    foreach(scandir($info['appsdir']) as $app) {
        if (!is_dir($info['appsdir'].DIR_SEPARATOR.$app))
            continue;
        $is_app = false;
        if (in_array($app,$ignore_list)) {
            display('Ignoring '.$app.'<br/>'.PHP_EOL);
            continue;
        }
        $app_content = scandir_rec($info['appsdir'].DIR_SEPARATOR.$app);
        foreach($app_content as $file) {
            if (is_string($file) && strtolower((new SplFileInfo($info['appsdir'].DIR_SEPARATOR.$app.DIR_SEPARATOR.$file))->getExtension()) == 'elf') {
                $is_app = true;
                $apps_info[$app]["binary"] = $file;
                break;
            }
        }
        if (!$is_app) {
            display('Not an app: '.$app.'<br/>'.PHP_EOL);
            continue;
        }
        
        display('<div class="switchable"><button type="button">'.$app.'</button>'.PHP_EOL.'<div class="switchable-item">');
        
        $apps_info[$app]["files"]     = scantolist($app_content);
        $apps_info[$app]["name"]      = '';
        $apps_info[$app]["version"]   = '';
        $apps_info[$app]["author"]    = '';
        $apps_info[$app]["desc"]      = '';
        $apps_info[$app]["long_desc"] = '';
        
        $xmlpath = $info['appsdir'].DIR_SEPARATOR.$app.DIR_SEPARATOR.'meta.xml';
        if (is_file($xmlpath) && ($meta = simplexml_load_file($xmlpath)) !== false) {
            // echo '<!--'; print_r($meta); echo '-->';
            $apps_info[$app]["name"]      = (string) trim($meta->name);
            $apps_info[$app]["version"]   = (string) trim($meta->version);
            $apps_info[$app]["author"]    = (string) trim($meta->coder);
            // $apps_info[$app]["desc"]      = htmlentities((string) trim($meta->short_description));
            $apps_info[$app]["desc"]      = (string) trim($meta->short_description);
            // $apps_info[$app]["long_desc"] = htmlentities((string) trim($meta->long_description));
            $apps_info[$app]["long_desc"] = (string) trim($meta->long_description);
            $appurlnamed = array();
            $appurlunnamed = array();
            foreach ($meta->url as $url) {
                if ((string) $url['name'] !== "")
                    $appurlnamed[(string) $url["name"]] = (string) trim($url);
                else
                    $appurlunnamed[] = (string) trim($url);
            }
            $apps_info[$app]["url"] = ["named"=>$appurlnamed,"unnamed"=>$appurlunnamed];
        } else
            display($xmlpath.' is not good or doesn\'t exist.<br/>'.PHP_EOL);
        
        // $apps_info[$app]["long_desc"] = str_replace(PHP_EOL,'<br/>',$apps_info[$app]["long_desc"]);
        // $apps_info[$app]["long_desc"] = preg_replace('#(https?://\S+)#','<a href="$1">$1</a>',$apps_info[$app]["long_desc"]);
        
        $extrapath = $info['xtradir'].DIR_SEPARATOR.$app.DIR_SEPARATOR;
        if (is_dir($extrapath.$info['ext_sd_root']))
            $apps_info[$app]["extra_sd_files"] = listfiles($extrapath.$info['ext_sd_root']);
        if (is_dir($extrapath.$info['ext_hosting']))
            $apps_info[$app]["www"] = listfiles($extrapath.$info['ext_hosting']);
        if (is_dir($extrapath.$info['ext_icons']))
            $apps_info[$app]["icons"] = listfiles($extrapath.$info['ext_icons']);
        if (is_dir($extrapath.$info['ext_pc_tools']))
            $apps_info[$app]["pc_tools"] = listfiles($extrapath.$info['ext_pc_tools']);
        
        // custom order
        if ($config['order']['type'] == CUSTOM_ORDER) {
            if (($id = array_search($app, $config['order']['custom_order'])) !== false)
                $apps_info[$app][$order[CUSTOM_ORDER]] = $id;
            else
                $apps_info[$app][$order[CUSTOM_ORDER]] = $config['order']['reverse']?-1:PHP_INT_MAX;
        }
        
        // defaults
        if (key_exists($app,$config['default_values']['apps'])) {
            foreach($config['default_values']['apps'][$app] as $field=>$val) {
                if ($config['default_values']['force'] || !key_exists($field,$apps_info[$app])) {
                    $apps_info[$app][$field] = $val;
                }
            }
        }
        display_array_whtml($apps_info[$app]);
        display('</div></div>'.PHP_EOL);
    }
}
display('</div>'.PHP_EOL);

/*******************************
    Zipping
********************************/

function zipping($zippath,$filelist,$prefix_server,$prefix_zip='',$zipflag=ZipArchive::CREATE | ZipArchive::OVERWRITE) {
    $zip = new ZipArchive;
    // display($zippath.'<br/>'.PHP_EOL);
    if (($res=$zip->open($zippath,$zipflag)) === true) {
        foreach($filelist as $content) {
            // display('Adding '.$content.' as '.$prefix_zip.$content.'<br/>'.PHP_EOL);
            if (substr($content,-1) == DIR_SEPARATOR)
                $zip->addEmptyDir($prefix_zip.$content);
            else
                $zip->addFile($prefix_server.$content,$prefix_zip.$content);
        }
        $zip->close();
    } else {
        display('Error opening '.$zippath.': '.$res.PHP_EOL);
    }
}

if ($zipping) {
    display('<div class="processing"><strong>Making zip files</strong><br/>'.PHP_EOL);
    foreach($apps_info as $app => $appinfo) {
        display('<div class="switchable"><button type="button">'.$app.'</button>'.PHP_EOL.'<div class="switchable-item"><ul>');
        // main app files
        zipping($info['zipsdir'].DIR_SEPARATOR.$info['ext_sd_root'].DIR_SEPARATOR.$app.'-'.$info['ext_sd_root'].'.zip',
                $appinfo['files'],
                $info['appsdir'].DIR_SEPARATOR.$app.DIR_SEPARATOR,
                'wiiu/apps/'.$app.DIR_SEPARATOR);
        
        $extrapath = $info['xtradir'].DIR_SEPARATOR.$app.DIR_SEPARATOR;
        
        // adding extra files
        if (is_dir($extrapath.$info['ext_sd_root'])) {
            zipping($info['zipsdir'].DIR_SEPARATOR.$info['ext_sd_root'].DIR_SEPARATOR.$app.'-'.$info['ext_sd_root'].'.zip',
                    $appinfo['extra_sd_files'],
                    $extrapath.$info['ext_sd_root'].DIR_SEPARATOR,
                    '',
                    ZipArchive::CREATE);
        }
        display('<li><a href="'.$info['zipsdir'].DIR_SEPARATOR.$info['ext_sd_root'].DIR_SEPARATOR.$app.'-'.$info['ext_sd_root'].'.zip'.'">Files for SD</a></li>');
        
        // hosting
        if (is_dir($extrapath.$info['ext_hosting'])) {
            zipping($info['zipsdir'].DIR_SEPARATOR.$info['ext_hosting'].DIR_SEPARATOR.$app.'-'.$info['ext_hosting'].'.zip',
                    $appinfo['www'],
                    $extrapath.$info['ext_hosting'].DIR_SEPARATOR);
            display('<li><a href="'.$info['zipsdir'].DIR_SEPARATOR.$info['ext_hosting'].DIR_SEPARATOR.$app.'-'.$info['ext_hosting'].'.zip'.'">Files for hosting</a></li>');
        }
        
        // icons
        if (is_dir($extrapath.$info['ext_icons'])) {
            zipping($info['zipsdir'].DIR_SEPARATOR.$info['ext_icons'].DIR_SEPARATOR.$app.'-'.$info['ext_icons'].'.zip',
                    $appinfo['icons'],
                    $extrapath.$info['ext_icons'].DIR_SEPARATOR);
            display('<li><a href="'.$info['zipsdir'].DIR_SEPARATOR.$info['ext_icons'].DIR_SEPARATOR.$app.'-'.$info['ext_icons'].'.zip'.'">Extra icons</a></li>');
        }
        
        // pc tools
        if (is_dir($extrapath.$info['ext_pc_tools'])) {
            zipping($info['zipsdir'].DIR_SEPARATOR.$info['ext_pc_tools'].DIR_SEPARATOR.$app.'-'.$info['ext_pc_tools'].'.zip',
                    $appinfo['pc_tools'],
                    $extrapath.$info['ext_pc_tools'].DIR_SEPARATOR);
            display('<li><a href="'.$info['zipsdir'].DIR_SEPARATOR.$info['ext_pc_tools'].DIR_SEPARATOR.$app.'-'.$info['ext_pc_tools'].'.zip'.'">Related PC tools</a></li>');
        }
        display('</ul></div></div>'.PHP_EOL);
    }
    display('</div>'.PHP_EOL);
}

/*******************************
    Sorting apps
********************************/

function cmp_apps($a, $b) {
    global $config, $order;
    $cmp = strcmp($a[$order[$config['order']['type']]], $b[$order[$config['order']['type']]]);
    // echo $cmp.'<br/>';
    return $config['order']['reverse']?-$cmp:$cmp;
}
if ($config['order']['type'] != NO_ORDER)
    uasort($apps_info, 'cmp_apps');

display('<div class="processing"><strong>Sorting</strong><br/>Complete.</div>'.PHP_EOL);

/*******************************
    Generating yaml, json
********************************/

$appsyaml = array();
$appsjson = array();

foreach($apps_info as $app=>$appinfo) {
    $appsyaml[$app] = array();
    foreach($config['yamlfields'] as $field) {
        if (key_exists($field,$appinfo))
            $appsyaml[$app][] = $appinfo[$field];
    }
    $appsjson[$app] = array();
    foreach($config['jsonfields'] as $field) {
        if (key_exists($field,$appinfo))
            $appsjson[$app][$field] = $appinfo[$field];
    }
}

$yamlstring = '';
foreach($appsyaml as $app=>$appinfo) {
    $yamlstring .= 'app: '.$app.PHP_EOL;
    foreach($appinfo as $infoyaml) {
        $yamlstring .= '- '.$infoyaml.PHP_EOL;
    }
}

$jsonstring = json_encode($appsjson,JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);

display('<div class="processing"><strong>directory for app store</strong><br/>'.PHP_EOL);
display('<div class="switchable"><button type="button" style="font-family: Monospace">'.$info['appstorefile'].'</button>'.PHP_EOL.'<pre class="switchable-item">'.$yamlstring.'</pre></div>'.PHP_EOL);
file_put_contents($info['appstorefile'],$yamlstring);
display('<div class="switchable"><button type="button" style="font-family: Monospace">'.$info['appstorejson'].'</button>'.PHP_EOL.'<pre class="switchable-item">'.htmlentities($jsonstring).'</pre></div>'.PHP_EOL);
file_put_contents($info['appstorejson'],$jsonstring);
display('</div>'.PHP_EOL);

/*******************************
    Files with list of files
********************************/

display('<div class="processing"><strong>List of files for apps</strong><br/>(these files are in the apps dir, which is currently '.$info['appsdir'].')'.PHP_EOL);
foreach($apps_info as $app=>$appinfo) {
    $appjson = array();
    $appjson['files'] = $appinfo['files'];
    if (key_exists('extra_sd_files',$appinfo))
        $appjson['extra_sd_files'] = $appinfo['extra_sd_files'];
    $appjsonstring = json_encode($appjson,JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);
    display('<div class="switchable"><button type="button" style="font-family: Monospace">'.$app.'.json</button>'.PHP_EOL.'<pre class="switchable-item">'.htmlentities($appjsonstring).'</pre></div>'.PHP_EOL);
    file_put_contents($info['appsdir'].DIR_SEPARATOR.$app.'.json',$appjsonstring);
}
display('</div>'.PHP_EOL);

display('
</body>
</html>');
?>