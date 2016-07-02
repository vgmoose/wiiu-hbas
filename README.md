**Work in progress! The actual client application (HBAS) [is in beta!](http://github.com/vgmoose/hbas/releases)**

![icon](https://raw.githubusercontent.com/vgmoose/hbas/master/meta/icon.png)

Known Homebrew app store repos:
- [My apps - hbas.vgmoose.com](http://hbas.vgmoose.com)
- [WiiUBru - appstore.wiiubru.com](http://appstore.wiiubru.com)

### Usage
Copy the release to /wiiu/apps on the SD card, and launch it from the homebrew launcher. You can then launch this app to install and manage downloaded software for the HBL onto your SD card from any HBAS server. 

### Customizing the server
You can put a text file in the /wiiu/apps/hbas folder called "server.txt". If this file is detected then HBAS will use the contents of this file as the specified server. It will expect a [yaml file](https://github.com/vgmoose/hbas/blob/gh-pages/directory.yaml) at a GET request to /directory.yaml . If this file is not in the app bundle, it will default to [hbas.vgmoose.com](http://hbas.vgmoose.com/directory.yaml).

By using the configuration file, you can distribute (freely!) your own custom copy of HBAS that only manages apps from your server. Since my site is a USA server, this would be especially useful to increase download speeds for people outside of the US.

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
