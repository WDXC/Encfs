#ifndef _Interface_incl_
#define _Interface_incl_

#include <string>

class ConfigVar;

class Interface {
  public:
    Interface(const char* name, int Current, int Revision, int Age);
    Interface(std::string name, int Current, int Revision, int Age);
    Interface();

    bool implementation(const Interface& dst) const;

    const std::string& name() const;
    int current() const;
    int revision() const;
    int age() const;

    std::string name();
    int& current();
    int& revision();
    int& age();

    Interface& operator=(const Interface& src) = default;

  private:
    std::string _name;
    int _current;
    int _revision;
    int _age;
};

ConfigVar& operator<<(ConfigVar&, const Interface& );
const ConfigVar& operator>>(const ConfigVar& , Interface &);


bool operator<(const Interface& A, const Interface& B);
bool operator>(const Interface& A, const Interface& B);
bool operator<=(const Interface& A, const Interface& B);
bool operator>=(const Interface& A, const Interface& B);
bool operator==(const Interface& A, const Interface& B);
bool operator!=(const Interface& A, const Interface& B);

#endif
