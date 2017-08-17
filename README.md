Application Updater
===
Is a console application written with **Qt** in **C++**.<br/>
It's on the [**MIT License**](https://opensource.org/licenses/MIT), so you can use it however you like.

It offers you a **very simple** way to managing updates of your app. For example, if you have your project binaries on GitHub, all you have
to do, is to only change **2 lines of code** in a main file.

Here is what to do:
* [Set your app name](https://github.com/Nelnir/Application-Updater/blob/9aead0d9e4a2be85c31b2c0337c9eb3eccf45cbc/src/main.cpp#L9)
* [Set a link to latest release on GitHub api](https://github.com/Nelnir/Application-Updater/blob/9aead0d9e4a2be85c31b2c0337c9eb3eccf45cbc/src/main.cpp#L11)


Really it's that **simple**.

However you need to know how to manage naming files on GitHub. You can change naming conventions, by simply adding them via
[this method](https://github.com/Nelnir/Application-Updater/blob/9aead0d9e4a2be85c31b2c0337c9eb3eccf45cbc/src/updater.h#L57), 
or just simply leave it [default](https://github.com/Nelnir/Application-Updater/blob/9aead0d9e4a2be85c31b2c0337c9eb3eccf45cbc/src/updater.cpp#L53).

So, when you upload **file.exe** for example for **windows xp 32bit**, so you need to name it **file__WINXP_32.exe**.<br/>
You can also combine names like that **file__WIN7_64_WINXP_64.exe**, so these OS will download that file.<br/>
Or if you just want all systems to have specified file, just dont add any naming convention.<br/>
After downloading, files will have their original names, due to this separator "**__**",<br/> and older files will be automatically deleted. 

### Moving files into a folder
If you want a specific file to be moved into subdirectory you need to do some work in the description release.<br/>
All text before these 3 characters "///", will be displayed in the updater.

So you need to put after these characters something like this

**fileName.txt -> scripts**<br/>
**textures.jpg -> data\textures**<br/>

It will put the **fileName.txt**, to **scripts** directory, and **textures.jpg** to **data\textures** directory.<br/>
If these directories doesn't exist, it will automatically create them.

## Usage

You need to run updater with one **argument**, which is a local **version** of application. Or provide a way of reading version. 
