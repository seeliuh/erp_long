#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"
#include <qlogging.h>
#include <QtSql/QSqlDatabase>
#include <QStringList>
#include <QDebug>
#include "dbman.h"
#include <fstream>


#define B0(a) (a & 0xFF)
#define B1(a) (a >> 8 & 0xFF)
#define B2(a) (a >> 16 & 0xFF)
#define B3(a) (a >> 24 & 0xFF)

__inline char GetB64Char(int index)
{
    const char szBase64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (index >= 0 && index < 64)
        return szBase64Table[index];
    return '=';
}

__inline int GetB64Index(char ch)
{
    int index = -1;
    if (ch >= 'A' && ch <= 'Z')
    {
        index = ch - 'A';
    }
    else if (ch >= 'a' && ch <= 'z')
    {
        index = ch - 'a' + 26;
    }
    else if (ch >= '0' && ch <= '9')
    {
        index = ch - '0' + 52;
    }
    else if (ch == '+')
    {
        index = 62;
    }
    else if (ch == '/')
    {
        index = 63;
    }

    return index;
}

int Base64Encode(char * base64code, const char * src, int src_len)
{
    if (src_len == 0)
        src_len = strlen(src);

    int len = 0;
    unsigned char* psrc = (unsigned char*)src;
    char * p64 = base64code;
    int i;
    for (i = 0; i < src_len - 3; i += 3)
    {
        unsigned long ulTmp = *(unsigned long*)psrc;
        register int b0 = GetB64Char((B0(ulTmp) >> 2) & 0x3F);
        register int b1 = GetB64Char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
        register int b2 = GetB64Char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F);
        register int b3 = GetB64Char((B2(ulTmp) << 2 >> 2) & 0x3F);
        *((unsigned long*)p64) = b0 | b1 << 8 | b2 << 16 | b3 << 24;
        len += 4;
        p64  += 4;
        psrc += 3;
    }

    // 处理最后余下的不足3字节的饿数据
    if (i < src_len)
    {
        int rest = src_len - i;
        unsigned long ulTmp = 0;
        for (int j = 0; j < rest; ++j)
        {
            *(((unsigned char*)&ulTmp) + j) = *psrc++;
        }

        p64[0] = GetB64Char((B0(ulTmp) >> 2) & 0x3F);
        p64[1] = GetB64Char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
        p64[2] = rest > 1 ? GetB64Char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F) : '=';
        p64[3] = rest > 2 ? GetB64Char((B2(ulTmp) << 2 >> 2) & 0x3F) : '=';
        p64 += 4;
        len += 4;
    }

    *p64 = '\0';

    return len;
}
int Base64Decode(char * buf, const char * base64code, int src_len)
{
    if (src_len == 0)
        src_len = strlen(base64code);

    int len = 0;
    unsigned char* psrc = (unsigned char*)base64code;
    char * pbuf = buf;
    int i;
    for (i = 0; i < src_len - 4; i += 4)
    {
        unsigned long ulTmp = *(unsigned long*)psrc;

        register int b0 = (GetB64Index((char)B0(ulTmp)) << 2 | GetB64Index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
        register int b1 = (GetB64Index((char)B1(ulTmp)) << 4 | GetB64Index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
        register int b2 = (GetB64Index((char)B2(ulTmp)) << 6 | GetB64Index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;

        *((unsigned long*)pbuf) = b0 | b1 << 8 | b2 << 16;
        psrc  += 4;
        pbuf += 3;
        len += 3;
    }

    // 处理最后余下的不足4字节的饿数据
    if (i < src_len)
    {
        int rest = src_len - i;
        unsigned long ulTmp = 0;
        for (int j = 0; j < rest; ++j)
        {
            *(((unsigned char*)&ulTmp) + j) = *psrc++;
        }

        register int b0 = (GetB64Index((char)B0(ulTmp)) << 2 | GetB64Index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
        *pbuf++ = b0;
        len  ++;

        if ('=' != B1(ulTmp) && '=' != B2(ulTmp))
        {
            register int b1 = (GetB64Index((char)B1(ulTmp)) << 4 | GetB64Index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
            *pbuf++ = b1;
            len  ++;
        }

        if ('=' != B2(ulTmp) && '=' != B3(ulTmp))
        {
            register int b2 = (GetB64Index((char)B2(ulTmp)) << 6 | GetB64Index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;
            *pbuf++ = b2;
            len  ++;
        }

    }

    *pbuf = '\0';

    return len;
}

LHKeyGenerator::LHKeyGenerator(const std::string &sLimitTime, const std::string &sCompanyName):
    m_sLimitTime(sLimitTime),
    m_sCompanyName(sCompanyName)
{

}

bool LHKeyGenerator::Check()
{
    if(m_sCompanyName.length() <=0 || m_sCompanyName.length() > 256 || m_sLimitTime.length()<=0 || m_sLimitTime.length()> 32)
    {
        return false;
    }
    return true;
}

std::string LHKeyGenerator::Generate()
{
    std::string sTotal = m_sLimitTime + std::string(";") + m_sCompanyName;
    char *pBase64 = new char[sTotal.length()*1.5];
    Base64Encode(pBase64, sTotal.c_str(), sTotal.length());
    std::string res(pBase64);
    delete []pBase64;
    return res;
}


LHKeyParser::LHKeyParser(const std::string &key):
    m_sKey(key)
{

}

bool LHKeyParser::Parse(std::string &date, std::string &company)
{
    if(m_sKey.length()<10)
        return 0;
    char *pDecoded = new char[company.length()*1.5];
    Base64Decode(pDecoded, m_sKey.c_str(), m_sKey.length());
    std::string sTotal(pDecoded);
    int pos = sTotal.find_first_of(";");
    if(pos == std::string::npos)
        return false;
    date = sTotal.substr(0, pos);
    company = sTotal.substr(pos+1, sTotal.length()-pos);

    if(company.length() <=0 || company.length() > 256 || date.length()<=0 || date.length()> 32)
    {
        return false;
    }
    return true;
}


void lh_key_test()
{
    QString company(QString::fromLocal8Bit("老see工作室"));
    QString date("2015-09-03");
    LHKeyGenerator gen(date.toStdString(), company.toStdString());
    if(!gen.Check())
    {
        qDebug() << "error ,check return false";
    }
    else{
        qDebug() << "check return true";
    }
    std::string key = gen.Generate();
    qDebug() << "key:" << key.c_str();

    LHKeyParser parse(key);
    std::string sDate, sCompany;
    parse.Parse(sDate, sCompany);
    qDebug() << "date=" << sDate.c_str() << ", company=" << sCompany.c_str();

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_editIP = findChild<QLineEdit*>("ctl_le_ip");
    m_editDBName = findChild<QLineEdit*>("ctl_le_dbname");
    m_editUser = findChild<QLineEdit*>("ctl_le_user");
    m_editPasswd = findChild<QLineEdit*>("ctl_le_passwd");
    m_editKey = findChild<QLineEdit*>("ctl_le_key");
    m_editIP->setText("192.168.1.153");
    m_editDBName->setText("tds_dserp");
    m_editUser->setText("sa");
    m_editPasswd->setText("sa");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void EnumSqlDriver()
{
    qDebug() << "Available drivers:";
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver, drivers)
    qDebug() << "\t" << driver;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    qDebug() << "ODBC driver valid?" << db.isValid();
}

void MainWindow::on_pushButton_2_clicked()
{
#if 1
    if(m_db.Connect(m_editIP->text(), m_editDBName->text(), m_editUser->text(), m_editPasswd->text()))
    {
    QMessageBox::information(this, "Info", QString::fromLocal8Bit("连接成功"), QMessageBox::Yes);
    }
    else
    {
    QMessageBox::information(this, "Info", QString::fromLocal8Bit("连接失败"), QMessageBox::Yes);
    }
#else
    lh_key_test();
#endif
}

void MainWindow::on_register_2_clicked()
{
    QString sKey = m_editKey->text();
    LHKeyParser parse(sKey.toStdString());
    std::string sCompanyName, sDate;
    if(!parse.Parse(sDate, sCompanyName))
    {
        QMessageBox::information(this, "Error", QString::fromLocal8Bit("输入参数有误"), QMessageBox::Yes);
        return;
    }
    qDebug() << QString::fromLocal8Bit("update company name:");
    qDebug() << QString("update sysi90 set ccsz = '%1' where ccsdm='CGSMC'").arg(sCompanyName.c_str()).toStdString().c_str();
    m_db.execute(QString("update sysi90 set ccsz = '%1' where ccsdm='CGSMC'").arg(sCompanyName.c_str()));
    qDebug() << QString::fromLocal8Bit("drop procedure");

    m_db.execute("DROP PROCEDURE Pr_Get_Systime");
    qDebug() << "create procedure:";

    QString sql = QString::fromLocal8Bit("CREATE PROCEDURE Pr_Get_Systime WITH  ENCRYPTION AS \
                                                 SELECT GETDATE() \
                                                 if (select  ccsz from sysi90 where cssxt = 'sys' and ccsdm = 'cgsmc')<>'%1' \
                                                 update sysi90 set ccsz ='公司未注册，请注册后使用，数据库崩溃不可使用' where cssxt = 'sys' and ccsdm = 'cgsmc' \
                    if (select  ccsz from sysi90 where cssxt = 'sys' and ccsdm = 'cgsmc')<>'%2' \
                    UPDATE   SYSB91  SET CCDBT='数据库崩溃请注册后使用',cckmc='请注册后使用' WHERE  czbdm='11' or  czbdm='12' \
                    if DATEDIFF(day, GETDATE(), '%3')< 1 \
                    update sysi90 set ccsz ='试用期已结束，请注册后使用，数据库崩溃不可使用' where cssxt = 'sys' and ccsdm = 'cgsmc' \
                    UPDATE   SYSB91  SET CCDBT='数据库崩溃请注册后使用',cckmc='请注册后使用' WHERE  czbdm='11' or  czbdm='12'").
                    arg(sCompanyName.c_str()).
                    arg(sCompanyName.c_str()).
                    arg(sDate.c_str());
    m_db.execute(sql);
    QMessageBox::information(this, "Info", QString::fromLocal8Bit("恭喜您，注册成功"), QMessageBox::Yes);
    //std::ofstream tmpf("./1.sql");
    //tmpf << sql.toStdString();
}

void MainWindow::on_MainWindow_destroyed()
{
   m_db.Close();
}
