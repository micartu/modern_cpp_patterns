#include "flyweight.h"
#include <_types/_uint32_t.h>
#include <iostream>
#include <map>
#include <ostream>
#include <string>

using nkey = uint32_t;

static std::map<std::string, nkey> names;
static std::map<nkey, std::string> keys;
static nkey seed;

struct User
{
  User(const std::string &first_name, const std::string &surname)
    : first_name(add(first_name)), surname(add(surname))
  {}

  [[nodiscard]] const std::string &get_first_name() const
  {
    return keys.find(first_name)->second;
  }

  [[nodiscard]] const std::string &get_surname() const
  {
    return keys.find(surname)->second;
  }

protected:
  nkey first_name, surname;
  static nkey add(const std::string &name)
  {
    auto it = names.find(name);
    if (it == names.end()) {
      nkey next = ++seed;
      // make bidirectional link number <-> string
      names.insert({ name, next });
      keys.insert({ next, name });
      return next;
    }
    return it->second;
  }

  friend std::ostream &operator<<(std::ostream &oss, const User &obj)
  {
    return oss << "first name: " << obj.get_first_name()
               << " last name: " << obj.get_surname();
  }
};

void run_flyweight_examples()
{
  User john_doe{ "John", "Doe" };
  User mike_doe{ "Mike", "Doe" };
  User jane_doe{ "Jane", "Doe" };

  std::cout << john_doe << std::endl
            << mike_doe << std::endl
            << jane_doe << std::endl;
}
