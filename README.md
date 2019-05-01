# TC1_A2
斐讯TC1智能排插私人固件，在学校无聊搞了一下，移植于A1版，请先查阅(https://github.com/a2633063/zTC1)

此版本使用的是汉枫的LPT-230wifi模块

> 固件基于官方SDK进行的二次开发
## 使用方法

### 1.接线：

把第10号引脚（复位引脚）接跟线出来，串口接第5（tx）和第6引脚(rx)，电源供电引线最好接5V（测试点）

通过串口工具连接到电脑，打开secureCRT软件，设置参数230400,8,none,1 ，连接上模块。

<img src="https://github.com/linlyv/TC1_A2/blob/master/doc/lpt230.jpg" width="540" >
> 图1 要接线的点（用到的点）


### 2.烧录固件

按住板上的按键，另一只手把复位引线碰一下GND(测试点)，接着在1秒内按电脑上的空格键（鼠标光标要在接收窗口上）。

看到有打印出Bootloader的信息，接着按照输入‘S’，再输入‘Y’,  点击菜单栏的"传输"(Transfer)，选择“发送Xmodem”，然后就选择XXX_gcc.bin文件，打开。

等待传输完成即可。

<img src="https://github.com/linlyv/TC1_A2/blob/master/doc/step1.png" width="540">
<img src="https://github.com/linlyv/TC1_A2/blob/master/doc/step2.png" width="540">
<img src="https://github.com/linlyv/TC1_A2/blob/master/doc/step3.png" width="540">

### 3.配网

初次使用可以通过网页去配网：手机连接连接名为HFLPT-230的AP,然后用浏览器访问10.10.100.254  这个地址，需要登录，用户名和密码都是admin.

在网页上输入要连接的WiFi SSID和密码,保存等待重启完成，模块就会自动连接你刚刚设置的WiFi网络。

<img src="https://github.com/linlyv/TC1_A2/blob/master/doc/web.png" width="540">

### 4.app控制
使用A1版的APP，发现局域网设备的方式添加设备[SmartControl_Android_MQTT](https://github.com/a2633063/SmartControl_Android_MQTT)。

### 5.固件升级

串口升级和web网页升级

只需要访问模块的ip地址+iweb.html(例如：http://192.168.123.184/iweb.html),就可以打开升级页面，选择第一项Upgrade application，浏览文件选择“XXX_upgrade.bin”文件，点击Upload
即可，传输成功会返回：Update successful !


> 没有意外的应该是可以控制的，实时控制是可以的，但由于是UDP连接，有时APP控制不是很顺畅。
> 还不能保存app设置的参数，像定时任务，插口名称都会掉电后丢失。
> 我现在没有测试的设备了，因为一次OTA升级过程中卡死了，导致模块不能进BootLoader了！！！，暂时就只有局域网的APP控制了，可能问题也比较多。

> 本人不承担他人使用本项目所造成的后果，所以请谨慎使用.
