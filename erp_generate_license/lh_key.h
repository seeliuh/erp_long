#ifndef LH_KEY_H
#define LH_KEY_H

#include <string>
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

#endif // LH_KEY_H
