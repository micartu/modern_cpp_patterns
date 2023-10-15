#include "creational.h"
#include <iostream>
#include <new>
#include <string>
#include <utility>
#include <vector>

void run_builder_examples();

void run_creational_examples() { run_builder_examples(); }

// simple builder
struct HtmlElement
{
  std::string name;
  std::string text;
  std::vector<HtmlElement> elements;

  HtmlElement() = default;
  HtmlElement(std::string name, std::string text) : name(std::move(name)), text(std::move(text)) {}

  void str(int indent = 0) const
  {
    for (int i = 0; i < indent; ++i) { std::cout << " "; }
    std::cout << "<" << name << ">";
    if (text.empty()) {
      std::cout << std::endl;
    } else {
      std::cout << text;
    }
    for (const auto &element : elements) { element.str(indent); }
    std::cout << "</" << name << ">" << std::endl;
  }
};

struct SimpleHtmlBuilder
{
  HtmlElement root;
  explicit SimpleHtmlBuilder(std::string root_name) { root.name = std::move(root_name); }

  // allows call add_child in a chain
  SimpleHtmlBuilder &add_child(std::string child_name, std::string child_text)
  {
    HtmlElement element{ std::move(child_name), std::move(child_text) };
    root.elements.emplace_back(element);
    return *this;
  }

  void str() const { root.str(); }
};


// DSL styled
struct Tag
{
  std::string name;
  std::string text;
  std::vector<Tag> children;
  std::vector<std::pair<std::string, std::string>> attributes;

  friend std::ostream &operator<<(std::ostream &ost, const Tag &tag)
  {
    ost << "<" << tag.name;
    if (!tag.attributes.empty()) {
      ost << " ";
      for (const auto &pair : tag.attributes) { ost << pair.first << "=\"" << pair.second << "\""; }
    }
    ost << ">";
    if (tag.text.empty()) {
      ost << std::endl;
    } else {
      ost << tag.text;
    }
    for (const auto &element : tag.children) { ost << element; }
    std::cout << "</" << tag.name << ">" << std::endl;
    return ost;
  }

protected:
  Tag(const std::string &name, const std::string &text) : name(name), text(text) {}
  Tag(const std::string &name, const std::vector<Tag> &children) : name(name), children(children) {}
};

struct P : Tag
{
  explicit P(const std::string &text) : Tag("P", text) {}

  P(std::initializer_list<Tag> children) : Tag("P", children) {}
};

struct IMG : Tag
{
  explicit IMG(const std::string &url) : Tag("IMG", "") { attributes.emplace_back("src", url); }
};

// ----- composite builder ------
//
class PersonBuilder;
class PersonAddressBuilder;
class PersonJobBuilder;

class Person
{
  friend class PersonBuilder;
  friend class PersonAddressBuilder;
  friend class PersonJobBuilder;
  // address
  std::string street_address, post_code, city;

  // employment
  std::string company_name, position;
  int annual_income = 0;

  Person() = default;

  friend std::ostream &operator<<(std::ostream &ost, const Person &per)
  {
    ost << "street: " << per.street_address;
    ost << ", ";
    ost << "position: " << per.position;
    ost << ", ";
    ost << "city: " << per.city;
    ost << ", ";
    ost << "company_name: " << per.company_name;
    ost << ", ";
    ost << "annual_income: " << per.annual_income;
    return ost;
  }

public:
  static PersonBuilder create();
};


class PersonBuilderBase
{
protected:
  Person &person;

  explicit PersonBuilderBase(Person &person) : person(person) {}

public:
  operator Person() { return std::move(person); }

  PersonAddressBuilder lives() const;
  PersonJobBuilder works() const;
};

class PersonBuilder : public PersonBuilderBase
{
  Person p;

public:
  PersonBuilder() : PersonBuilderBase(p) {}
};

class PersonAddressBuilder : public PersonBuilderBase
{
  using self = PersonAddressBuilder;

public:
  explicit PersonAddressBuilder(Person &person) : PersonBuilderBase(person) {}

  self &at(std::string street_address)
  {
    person.street_address = std::move(street_address);
    return *this;
  }

  self &with(std::string post_code)
  {
    person.post_code = std::move(post_code);
    return *this;
  }

  self &in(std::string city)
  {
    person.city = std::move(city);
    return *this;
  }
};

class PersonJobBuilder : public PersonBuilderBase
{
  using self = PersonJobBuilder;

public:
  explicit PersonJobBuilder(Person &person) : PersonBuilderBase(person) {}

  self &at(std::string company_name)
  {
    person.company_name = std::move(company_name);
    return *this;
  }

  self &as_a(std::string position)
  {
    person.position = std::move(position);
    return *this;
  }

  self &earning(int salary)
  {
    person.annual_income = salary;
    return *this;
  }
};

PersonBuilder Person::create() { return {}; }

PersonJobBuilder PersonBuilderBase::works() const { return PersonJobBuilder(person); }
PersonAddressBuilder PersonBuilderBase::lives() const { return PersonAddressBuilder(person); }

void run_builder_examples()
{
  SimpleHtmlBuilder builder{ "ul" };
  builder.add_child("li", "hello").add_child("li", "world");
  builder.str();

  std::cout << P{ IMG{ "http://pokemon.com/pikachu.png" } } << std::endl;

  Person per = Person::create()
                 .lives()
                 .at("123 London Road")
                 .with("SW1 1GB")
                 .in("London")
                 .works()
                 .at("PragmaSoft")
                 .as_a("Consultant")
                 .earning(10e6);

  std::cout << per << std::endl;
}
