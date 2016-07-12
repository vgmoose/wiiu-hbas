**The 1.0 version of HBAS [is released!](http://github.com/vgmoose/hbas/releases)** See [the post here](https://gbatemp.net/threads/release-homebrew-app-store-for-wiiu.433275) for more info.

![icon](https://raw.githubusercontent.com/vgmoose/hbas/master/meta/icon.png)

Known Homebrew app store repos:
- [WiiUBru - wiiubru.com/appstore](http://wiiubru.com/appstore) (default)
- [Utools - coc4tm.github.io/Utools/web](http://coc4tm.github.io/Utools/web)
- [Useful Utilities - wiiu.3utilities.com](http://wiiu.3utilities.com/appstore.html)
- [My apps - hbas.vgmoose.com](http://hbas.vgmoose.com)

At this time, only one repository at a time is supported. See below on how to use a custom repository.

### Usage
Copy the release to /wiiu/apps on the SD card, and launch it from the homebrew launcher. You can then launch this app to install and manage downloaded software for the HBL onto your SD card from any HBAS server. 

### Customizing the server
You can put a text file in the /wiiu/apps/appstore/ folder called "repository.txt". If this file is detected then HBAS will use the contents of this file as the specified server. It will expect a [yaml file](https://github.com/vgmoose/hbas/blob/gh-pages/directory.yaml) at a GET request to /directory.yaml . If this file is not in the app bundle, it will default to [hbas.vgmoose.com](http://hbas.vgmoose.com/directory.yaml).

By using the configuration file, you can distribute (freely!) your own custom copy of HBAS that only manages apps from your server. To make the file, make sure that only the contents of the URL (with no trailing slash) are in the repository.txt file, with no new line at the end either. This file should be placed inside of the "appstore" folder, when redistributing the appstore.

### Adding your app
If you are a developer and would like to add an app to HBAS, you will need to find the repository maintainer and ask them to add it. You should also supply a compiled app bundle for your application. The web frontend provides a contact link in the form of a chat icon in the upper right corner, which can be used to contact the maintainer if they are using the web template.

### Credits

Code

- dimok - lots of code, and the main framework used here is copied from [Homebrew Launcher](https://gbatemp.net/threads/homebrew-launcher-for-wiiu.416905/)
- pwsincd - writing and optimizing web template code
- brienj - extremely helpful debugging
- marionumber1 - for begrudgingly being a 5.5 enabler

Resources

- Music by [(T-T)b](http://t-tb.bandcamp.com)
- Cloud icon by [Lyolya](http://www.flaticon.com/authors/lyolya)

Misc Support

- quarktheawesome
- rw-r-r-0644
- ryuutseku85
- creepermario
- datalogger
- dylon99

*And everybody else in the developing Wii U homebrew community!*
