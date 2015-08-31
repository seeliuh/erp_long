#include "dbman.h"
#include <QMessageBox>
#include <QtSql/QSqlQuery>

dbman::dbman()
{

}

bool dbman::Connect(const QString &sIP, const QString &sDBName, const QString &sUser, const QString &sPasswd)
{
    m_db=QSqlDatabase::addDatabase("QODBC");
    m_db.setDatabaseName(QString("Driver={sql server};server=%1;database=%2;uid=%3;pwd=%4;")
                       .arg(sIP)
                       .arg(sDBName)
                       .arg(sUser)
                       .arg(sPasswd)
                       );
    if (!m_db.open())
    {
        return false;
    }
    return true;
}


bool dbman::Close()
{
    m_db.close();
    return true;
}

bool dbman::TryConnect()
{

    return true;
}

bool dbman::execute(const QString &sSql)
{
    QSqlQuery sql;
    sql.prepare(sSql);
    return sql.exec();
}
