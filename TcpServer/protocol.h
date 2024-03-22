﻿#ifndef PROTOCOL_H
#define PROTOCOL_H
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
typedef unsigned int uint;

#define REGISTER_OK "register ok"
#define REGISTER_FAILED "register failed:name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed:name or pwd error or has logined"

#define SEARCH_USER_NO "the user dont exist"
#define SEARCH_USER_ONLINE "the user is online"
#define SEARCH_USER_OFFLINE "the user is offline"

#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "friend offline"
#define ADD_FRIEND_NOTEXIST "user not exist"
#define DEL_FRIEND_OK "delete friend ok"

#define DIR_NO_EXIST "dir not exist"
#define FILE_NAME_EXIST "file name exist"
#define CREATE_DIR_OK "create dir is ok"

#define DEL_DIR_OK "delete dir ok"
#define DEL_DIR_FAILED "delete is failed"

#define RENAME_FILE_OK "rename ok"
#define RENAME_FILE_FAILED "rename failed"

#define ENTER_DIR_FAILED "not a dir"

#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAILED "upload failed"
enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGISTER_REQUEST,//注册请求
    ENUM_MSG_TYPE_REGISTER_RESPOND,//注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST,//登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,//登录回复
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,//查看在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,//查看在线用户回复
    ENUM_MSG_TYPE_SEARCH_USER_REQUEST,//搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPOND,//搜索用户回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,//添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,//同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,//拒绝添加好友
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUST,//刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,//刷新好友回复
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,//删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,//删除好友回复
    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,//私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,//私聊回复
    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,//群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,//群聊回复
    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,//创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,//创建文件夹回复
    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,//刷新文件请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,//刷新文件回复
    ENUM_MSG_TYPE_DEL_DIR_REQUEST,//删除目录请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,//删除目录回复

    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,//重命名请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,//重命名回复
    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,//进入文件夹请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,//进入文件夹回复
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,//上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,//上传文件回复
    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,//下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,//下载文件回复
    ENUM_MSG_TYPE_MAX=0x00ffffff,
};

struct FileInfo
{
    char caFileName[32];//文件名字
    int iFileType;//文件类型
};

struct PDU
{
    uint uiPDULen; //总的协议数据单元大小
    uint uiMsgType; //消息类型
    char caData[64];
    uint uiMsgLen;//实际消息长度
    int caMsg[];//实际消息

};

PDU *mkPDU(uint uiMsgLen);
#endif // PROTOCOL_H
