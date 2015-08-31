#ifndef DBMAN_H
#define DBMAN_H
#include <string>
#include <QtSql/QSqlDatabase>
class LHKeyGenerator
{
public:
    LHKeyGenerator(const std::string &sLimitTime, const std::string &sCompanyName);
    ~LHKeyGenerator(){

    }

private:
    LHKeyGenerator(){

    }
public:
    bool Check();
    std::string Generate();
private:
    std::string m_sLimitTime;
    std::string m_sCompanyName;
};

class LHKeyParser
{
public:
    LHKeyParser(const std::string &key);
    bool Parse(std::string &date, std::string &company);
private:
    std::string m_sKey;
};

class dbman
{
public:
    dbman();
    bool Connect(const QString &sIP, const QString &sDBName, const QString &sUser,const QString &sPasswd);
    bool Close();
    bool TryConnect();
    bool execute(const QString &sSql);
private:
    QSqlDatabase m_db;
};

#endif // DBMAN_H
