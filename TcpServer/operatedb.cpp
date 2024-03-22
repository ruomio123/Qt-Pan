#include "operatedb.h"
#include<QMessageBox>
#include<QDebug>
OperateDB::OperateDB(QObject *parent)
    : QObject{parent}
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");
}

OperateDB &OperateDB::getInstance()
{
    //每次调用都返回同一个对象
    static OperateDB instance;
    return instance;
}

void OperateDB::init()
{
    //数据库初始化
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\QTProject\\Pan\\TcpServer\\cloud.db");

    if(m_db.open())
    {
        QSqlQuery query;

        query.exec("select *from userInfo");
        qDebug()<<"我打开了数据库";
        while(query.next())
        {
            QString data=QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug()<<data;

        }
    }
    else
    {
        QMessageBox::critical(NULL,"h","打开数据库失败");
    }
}

OperateDB::~OperateDB()
{
    m_db.close();
}

bool OperateDB::handleRegister(const char *name, const char *password)
{

    if(name==nullptr||password==nullptr)
    {
        qDebug()<<"name or pwd is null";
        return false;
    }
    QString data=QString("insert into userInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(password);//字符串拼接
    qDebug()<<data;
    QSqlQuery query;
    return query.exec(data);//如果重名，直接执行失败
}

bool OperateDB::handleLogin(const char *name, const char *password)
{
    if(name==nullptr||password==nullptr)
    {
        qDebug()<<"name or pwd is null";
        return false;
    }

    QString data=QString("select *from userInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(password);//字符串拼接
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    //return query.next();//只有一条记录，要么next获得这条记录返回true，要么next没有这条记录返回false

    if(query.next())
    {
        QString data=QString("update userInfo set online =1 where name=\'%1\' and pwd=\'%2\'").arg(name).arg(password);//字符串拼接
        qDebug()<<data;
        QSqlQuery query;
        query.exec(data);
        return true;
    }
    else return false;
}

void OperateDB::handleOffline(const char *name)
{

    if(name==NULL)
    {
        return;
    }
    QString data=QString("update userInfo set online =0 where name=\'%1\'").arg(name);//字符串拼接
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
}

QStringList OperateDB::handleAllOnline()
{
    QString data=QString("select name from userInfo where online=1");//字符串拼接
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();
    while(query.next())
    {
        result.append(query.value(0).toString());
        qDebug()<<"name--"<<result;
    }
    return result;
}

int OperateDB::handleSearchUser(const char *name)
{
  //如果为空，返回-1，
    if(name==NULL)return -1;
    QString data=QString("select online from userInfo where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next())//存在
    {
        int state=query.value(0).toInt(); //表示在线状态的变量
        if(state==1)
        {
            return 1;//存在且在线
        }
        else if(state==0)
        {
            return 0;//存在不在线
        }
    }
    else
    {
        return -1;//不存在
    }
}

int OperateDB::handleAddFriend(const char *friendName, const char *loginName)
{
    //名字为空返回-1，
    if(friendName==NULL||loginName==NULL)
    {
        qDebug()<<"添加好友：名字为空";
        return -1;
    }
    QString data=QString("select *from friend where (id=(select id from userInfo where name=\'%1\') and friendId=(select id from userInfo where name=\'%2\')) "
   " or (id=(select id from userInfo where name=\'%3\') and friendId=(select id from userInfo where name=\'%4\'))").arg(loginName).arg(friendName).arg(friendName).arg(loginName);
    qDebug()<<"****"<<data;
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        return 0;//双方已经是好友
    }
    else
    {
        //
        qDebug()<<"要添加的好友是"<<friendName;
        QString data=QString("select online from userInfo where name=\'%1\'").arg(friendName);
        QSqlQuery query;
        query.exec(data);
        if(query.next())//存在
        {
            int state=query.value(0).toInt(); //表示在线状态的变量
            if(state==1)
            {
                return 1;//在线但不是好友
            }
            else if(state==0)
            {
                return 2;//不在线
            }
        }
        else
        {
            return 3;//不存在
        }
    }
}

void OperateDB::AddFriend(const char *friendName, const char *loginName)
{
    /*
     *先在数据库里找这两个人的id
     */
    QString data1=QString("select id from userInfo where name=\'%1\'").arg(loginName);
    QString data2=QString("select id from userInfo where name=\'%1\'").arg(friendName);
    QSqlQuery query1,query2;
    query1.exec(data1);
    query2.exec(data2);
    int id1=0,id2=0;//先定义两个变量
    if(query1.next()) id1=query1.value(0).toInt();
    if(query2.next())id2=query2.value(0).toInt();
    qDebug()<<"加好友的两个人id是"<<id1<<" "<<id2;

    QSqlQuery query;
    QString data=QString("insert into friend(id,friendId) values(\'%1\',\'%2\'),(\'%3\',\'%4\')").arg(id1).arg(id2).arg(id2).arg(id1);//字符串拼接
    qDebug()<<data;
    query.exec(data);

//   以下为测试用
//    QSqlQuery query;
//    QString data=QString("SELECT id from userInfo where name=\'%1\'").arg(loginName);
//    query.exec(data);
//    while (query.next()) {
//       int country = query.value(0).toInt();
//        qDebug()<<"****id是"<<country;
//    }

}

QStringList OperateDB::handleFlushFriend(const char *loginName)
{
    QStringList strFriendList;
    strFriendList.clear();//先清空
    if(loginName==NULL)return strFriendList;//直接返回空的列表

    QString data=QString("select name from userInfo where id in (select id from friend where friendId=(select id from userInfo where name=\'%1\')) or id in (select friendId from friend where id=(select id from userInfo where name=\'%1\'))").arg(loginName);
    QSqlQuery query;
    query.exec(data);
    while(query.next())
    {
        strFriendList.append(query.value(0).toString());
        qDebug()<<"打印一下名字"<<query.value(0).toString();
        //我这里和视频写的不一样，我刷新显示的是所有的好友，不光是在线的。
    }


    return strFriendList;

}

bool OperateDB::handleDelFriend(const char *loginName, const char *friendName)
{
    if(loginName==NULL||friendName==NULL)
    {
        return false;
    }
    qDebug()<<"进入了处理删除函数"<<loginName<<"要删"<<friendName;
    QString data1=QString("delete from friend where id=(select id from userInfo where name=\'%1\') and friendId=(select id from userInfo where name=\'%2\') ").arg(loginName).arg(friendName);
    QSqlQuery query;
    query.exec(data1);

    QString data2=QString("delete from friend where id=(select id from userInfo where name=\'%1\') and friendId=(select id from userInfo where name=\'%2\') ").arg(friendName).arg(loginName);
    query.exec(data2);

    return true;
}


