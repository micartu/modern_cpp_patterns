#include "decorator.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

struct Shape
{
  virtual std::string str() const = 0;
  virtual ~Shape() = default;
};

struct Circle : Shape
{
  float radius;

  explicit Circle(const float radius) : radius(radius) {}

  void resize(float factor) { radius *= factor; }

  std::string str() const override
  {
    std::ostringstream oss;
    oss << "A circle of radius:" << radius;
    return oss.str();
  }
};

// ---- Dynamic Decorators

struct ColoredShape : Shape
{
  Shape &shape;
  std::string color;

  ColoredShape(Shape &&shape, const std::string &color) : shape(shape), color(color) {}

  std::string str() const override
  {
    std::ostringstream oss;
    oss << shape.str() << " has the color " << color;
    return oss.str();
  }
};

struct TransparentShape : Shape
{
  Shape &shape;
  uint8_t transparency;

  TransparentShape(Shape &&shape, uint8_t transparency) : shape(shape), transparency(transparency) {}

  std::string str() const override
  {
    std::ostringstream oss;
    oss << shape.str() << " has " << static_cast<float>(transparency) / 255.f * 100.f << "% transparency";
    return oss.str();
  }
};

/// ----- TEMPLATED (=Static) MAGIC:

template<typename T> struct ColoredTShape : T
{
  static_assert(std::is_base_of<Shape, T>::value, "Template argument must be a Shape");
  std::string color;

  // need this (or not!)
  ColoredTShape() = default;

  template<typename... Args>
  explicit ColoredTShape(const std::string &color, Args... args) : T(std::forward<Args>(args)...), color{ color }
  // you cannot call base class ctor here
  // b/c you have no idea what it is
  {}

  std::string str() const override
  {
    std::ostringstream oss;
    oss << T::str() << " has the color " << color;
    return oss.str();
  }
};


template<typename T> struct TransparentTShape : T
{
  uint8_t transparency;

  template<typename... Args>
  explicit TransparentTShape(const uint8_t transparency, Args... args)
    : T(std::forward<Args>(args)...), transparency{ transparency }
  {}

  std::string str() const override
  {
    std::ostringstream oss;
    oss << T::str() << " has " << static_cast<float>(transparency) / 255.f * 100.f << "% transparency";
    return oss.str();
  }
};

// need partial specialization for this to work
template<typename> struct Logger;

template<typename R, typename... Args> struct Logger<R(Args...)>
{
  Logger(std::function<R(Args...)> func, const std::string &name) : func(func), name(name) {}

  R operator()(Args... args)
  {
    std::cout << "Entering " << name << std::endl;
    R result = func(args...);
    std::cout << "Exiting " << name << std::endl;
    return result;
  }

  std::function<R(Args...)> func;
  std::string name;
};

template<typename R, typename... Args> auto make_logger(R (*func)(Args...), const std::string &name)
{
  return Logger<R(Args...)>(std::function<R(Args...)>(func), name);
}

void run_decorator_mixin_examples()
{
  ColoredTShape<TransparentTShape<Circle>> circle{ "blue", 25, 1 };
  // here we can call all method of objects combined in the composed type:
  circle.resize(10);
  circle.transparency = 255;
  std::cout << circle.str() << std::endl;
}

/// an example function to be decorated:
double add(double a, double b)
{
  std::cout << a << "+" << b << "=" << (a + b) << std::endl;
  return a + b;
}

void run_decorator_fun_examples()
{
  auto logged_add = make_logger(add, "Add");
  auto result = logged_add(3, 5);
  std::cout << "res: " << result << std::endl;
}

void run_decorator_examples()
{
  TransparentShape myCicle{ ColoredShape{ Circle(23), "green" }, 64 };
  std::cout << myCicle.str() << std::endl;
  /*
  // SEGVed
  // but need to add a lvalue copy constructor to see it!
  ColoredShape shaped{ Circle(44), "red" };
  TransparentShape anotherCicle{ shaped, 64 };
  std::cout << anotherCicle.str() << std::endl;
  */
  run_decorator_mixin_examples();
  run_decorator_fun_examples();
}
