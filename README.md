## 使用QT6开发的网盘软件
数据库：sqlite3
软件:qtcreator
版本:qt6
* 功能：添加好友，私聊，群发，上传文件，下载文件等

该项目有很多bug，比如私聊部分，和一个好友聊天可以正常显示没问题，但是再选择其他好友，聊天信息会在同一个窗口显示，而不是开多个聊天窗口

* 创建文件夹功能
要求在用户注册成功时创建一个该用户名的根目录
客户端点击创建按钮，发送用户名，目录信息，以及新建文件夹名字。 

*关于进入文件夹和返回上一级
     在双击目录函数里添加了一行代码更新当前所在路径， 
    TcpClient::getInstance().setStrCurPath(strNewPath);
    可以合并两个功能
![image](https://github.com/ruomio123/Qt-Pan/assets/133988378/2889ad36-c009-4782-8d1e-479dd88dd174)
![image](https://github.com/ruomio123/Qt-Pan/assets/133988378/aec67e2c-27bc-4258-b09b-fc40008b89aa)

