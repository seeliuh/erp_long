#include "lh_key.h"

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
    char *pDecoded = new char[company.length()*1.5];
    Base64Decode(pDecoded, m_sKey.c_str(), m_sKey.length());
    std::string sTotal(pDecoded);
    int pos = sTotal.find_first_of(";");
    date = sTotal.substr(0, pos);
    company = sTotal.substr(pos+1, sTotal.length()-pos);
    return true;
}
