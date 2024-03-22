#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QStringList>
class OperateDB : public QObject
{
    Q_OBJECT
public:
    explicit OperateDB(QObject *parent = nullptr);
    static OperateDB& getInstance();//单例模式
    void init();
    ~OperateDB();
    bool handleRegister(const char* name,const char*password);//处理注册
    bool handleLogin(const char* name,const char*password);//处理登录
    void handleOffline(const char *name);//处理下线
    QStringList handleAllOnline();//处理查看在线用户
    int handleSearchUser(const char *name);
    int handleAddFriend(const char* friendName,const char*loginName);//处理加好友逻辑
    void AddFriend(const char* friendName,const char*loginName); //修改数据库，成为好友
    QStringList handleFlushFriend(const char*loginName);//处理刷新好友
    bool handleDelFriend(const char* loginName,const char*friendName);//处理删除好友
signals:

public slots:

private:

    QSqlDatabase m_db;//连接数据库

};

#endif // OPERATEDB_H
