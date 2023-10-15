#include "composite.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

struct GraphicObject
{
  virtual ~GraphicObject() = default;
  virtual void draw() = 0;
};

struct Circle : GraphicObject
{
  void draw() override { std::cout << "Circle" << std::endl; }
};

struct Group : GraphicObject
{
  std::string name;

  explicit Group(const std::string &name) : name{ name } {}

  void draw() override
  {
    std::cout << "Group " << name.c_str() << " contains:" << std::endl;
    for (auto &&o : objects) o->draw();
  }

  std::vector<GraphicObject *> objects;
};

inline void graphics()
{
  Group root("root");
  Circle c1, c2;
  root.objects.push_back(&c1);

  Group subgroup("sub");
  subgroup.objects.push_back(&c2);

  root.objects.push_back(&subgroup);

  root.draw();
}

// acuumulators
// 2 + (3+4)
struct Expression
{
  virtual double eval() = 0;
  virtual void collect(vector<double> &v) = 0;
};

struct Literal : Expression
{
  double value;

  explicit Literal(const double value) : value{ value } {}

  double eval() override { return value; }

  void collect(vector<double> &v) override { v.push_back(value); }
};

struct AdditionExpression : Expression
{
  shared_ptr<Expression> left, right;

  AdditionExpression(const shared_ptr<Expression> &expression, const shared_ptr<Expression> &expression1)
    : left{ expression }, right{ expression1 }
  {}

  double eval() override { return left->eval() + right->eval(); }

  void collect(vector<double> &v) override
  {
    left->collect(v);
    right->collect(v);
  }
};

void run_composite_examples()
{
  graphics();
  AdditionExpression sum{ make_shared<Literal>(2),
    make_shared<AdditionExpression>(make_shared<Literal>(3), make_shared<Literal>(4)) };
  cout << "2+(3+4) = " << sum.eval() << endl;

  vector<double> v;
  sum.collect(v);
  for (auto x : v) cout << x << "\t";
  cout << endl;

  vector<double> values{ 1, 2, 3, 4 };
  double s = 0;
  for (auto x : values) s += x;
  cout << "average is " << (s / values.size()) << endl;
}
