**Work in progress! The actual client application (HBAS) does not fully behave as described below yet!**

![icon](https://raw.githubusercontent.com/vgmoose/hbas/master/meta/icon.png)

[Visit the web frontend to the Homebrew app store here](http://hbas.vgmoose.com)

### Usage
Copy the release to /wiiu/apps on the SD card, and launch it from the homebrew launcher. You can then use this app to install and manage downloaded software for the HBL. 

### Adding your app
If you are a developer and would like to add an app to the HBL, either create a [new issue](https://github.com/vgmoose/hbas/issues) or make a pull request directly against the gh-pages branch of this repo. You'll also need to supply a compiled app bundle for your application.

### Customizing the server
You can put a text file in the /wiiu/apps/hbas folder called "server.txt". If this file is detected then HBAS will use the specified server. It will expect a [yaml file](https://github.com/vgmoose/hbas/blob/gh-pages/directory.yaml) at a GET request to /directory.yaml . If this file is not in the app bundle, it will default to [hbas.vgmoose.com](http://hbas.vgmoose.com/directory.yaml).

By using the configuration file, you can distribute (freely!) your own custom copy of HBAS that only manages apps from your server. Since my site is a USA server, this would be especially useful to increase download speeds for people outside of the US.


### Credits

- dimok - lots of code, and the main framework used here is copied from [Homebrew Launcher](https://gbatemp.net/threads/homebrew-launcher-for-wiiu.416905/)
- brienj - for making high quality standalone HBL apps that can be featured on HBAS!
- marionumber1 - for begrudgingly being a 5.5 enabler
- pwsincd - for hopefully agreeing to host a UK HBAS server 

*And everybody else in the developing Wii U homebrew community!*
