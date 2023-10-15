#pragma once

/*
   SOLID is an acronym which stands for the following design principles
   (and their abbreviations):
   • Single Responsibility Principle (SRP)
   • Open-Closed Principle (OCP)
   • Liskov Substitution Principle (LSP)
   • Interface Segregation Principle (ISP)
   • Dependency Inversion Principle (DIP)
*/
#include <array>
#include <functional>
#include <string>
#include <vector>

// Single Responsibility Principle (SRP) -
// each class has only one responsibility,
// and therefore has only one reason to change.
// is self explanatory, so no examples


// Open-Closed Principle (OCP):
// -----------------------------

enum class Color { Red, Green, Blue };
enum class Size { Small, Medium, Large };

struct Product
{
  std::string name;
  Color color;
  Size size;

  // Product(string& _name, Color _color, Size _size) : name(_name), color(_color), size(_size) {}
};

/// general class which solves the filtering problem by using a function
/// as a criterium for adding an item to the final result
template<typename T> struct LambdaFilter
{
  static std::vector<T> filter(std::vector<T> items, std::function<bool(T)> fun)
  {
    std::vector<T> result;
    for (auto &item : items) {
      if (fun(item)) { result.push_back(item); }
    }
    return result;
  };
};

// ---- solution proposed by the book - Modern C++ Patterns ---->

template<typename T> struct AndSpecification;

template<typename T> struct OrSpecification;

template<typename T> struct Specification
{
  virtual bool is_satisfied(T item) = 0;
  virtual ~Specification() = default;

  AndSpecification<T> operator&&(Specification &other) { return AndSpecification<T>(*this, other); }

  OrSpecification<T> operator||(Specification &other) { return OrSpecification<T>(*this, other); }
};

template<typename T> struct Filter
{
  virtual std::vector<T> filter(std::vector<T> items, Specification<T> &spec) = 0;

  virtual ~Filter() = default;
};

struct BetterFilter : Filter<Product>
{
  std::vector<Product> filter(std::vector<Product> items, Specification<Product> &spec) override
  {
    std::vector<Product> result;
    for (auto &prd : items) {
      if (spec.is_satisfied(prd)) { result.push_back(prd); }
    }
    return result;
  }
};

struct ColorSpecification : Specification<Product>
{
  Color color;

  explicit ColorSpecification(const Color color) : color{ color } {}

  bool is_satisfied(Product item) override { return item.color == color; }
};

struct SizeSpecification : Specification<Product>
{
  Size sz;

  explicit SizeSpecification(const Size sz) : sz{ sz } {}

  bool is_satisfied(Product item) override { return item.size == sz; }
};

template<typename T> struct AndSpecification : Specification<T>
{
  Specification<T> &first;
  Specification<T> &second;

  AndSpecification(Specification<T> &first, Specification<T> &second) : first{ first }, second{ second } {}

  bool is_satisfied(T item) override { return first.is_satisfied(item) && second.is_satisfied(item); }
};

template<typename T> struct OrSpecification : Specification<T>
{
  Specification<T> &first;
  Specification<T> &second;

  OrSpecification(Specification<T> &first, Specification<T> &second) : first{ first }, second{ second } {}

  bool is_satisfied(T item) override { return first.is_satisfied(item) || second.is_satisfied(item); }
};

// So let’s recap what OCP principle is and how the preceding example enforces it.
// Basically, OCP states that you shouldn’t need to go back to code you’ve already
// written and tested and change it.

// Liskov Substitution Principle (LSP):
// The Liskov Substitution Principle, named after Barbara Liskov,
// states that if an interface takes an object of type Parent,
// it should equally take an object of type Child without anything breaking.
// suppose we have type class Rectangle with area and other functions which set a and b of that rect
// now suppose we make a special kind of Rectangle called a Square.
// This object overrides the setters to set both width and height
// if we use function like void process(Rectangle& r) where
// would change height then Square would have one side not equal to other!
//
// The takeaway from this example (which I admit is a little contrived)
// is that process() breaks the LSP by being thoroughly unable to take a
// derived type Square instead of the base type Rectangle. If you feed it
// a Rectangle, everything is fine, so it might take some time before
// the problem shows up in your tests (or in production—hopefully not!).
// -----------------------------
//
// Interface Segregation Principle
// make an interface for every standalone function you support
// do not make god objects without interfaces!
//
// So, just to recap, the idea here is to segregate parts of a complicated interface
// into separate interfaces so as to avoid forcing implementors to implement functionality
// that they do not really need. Anytime you write
// a plug-in for some complicated application and you’re given an interface with 20 confusing
// functions to implement with various no-ops and return nullptr, more likely than not the API
// authors have violated the ISP.
// -----------------------------
//
// Dependency Inversion Principle
// High-level modules should not depend on low-level modules.
// Both should depend on abstractions.
// -----------------------------
// C++ has many implementations of such a library. As an example we can bring up the Boost.DI lib
// https://github.com/boost-ext/di
// Now, you’re probably expecting to see make_unique/make_shared calls as we initialize the Car.
// But we won’t do any of that. Instead, we’ll use Boost.DI.
// First of all, we’ll define a binding that binds ILogger to ConsoleLogger;
// what this means is, basically, “any time someone asks for an ILogger give them a ConsoleLogger”:
// auto injector = di::make_injector(
//      di::bind<ILogger>().to<ConsoleLogger>()
// );

void run_solid_examples();
