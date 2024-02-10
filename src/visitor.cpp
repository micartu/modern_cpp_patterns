#include "visitor.h"

#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <stdio.h>
#include <functional>

#include <boost/optional.hpp>

using namespace std;

struct SubtractionExpression;
struct DoubleExpression;
struct AdditionExpression;

struct ExpressionVisitor
{
  virtual void visit(DoubleExpression &de) = 0;
  virtual void visit(AdditionExpression &ae) = 0;
  virtual void visit(SubtractionExpression &se) = 0;
};

struct ExpressionPrinter : ExpressionVisitor
{
  ostringstream oss;
  string str() const { return oss.str(); }
  void visit(DoubleExpression &de) override;
  void visit(AdditionExpression &ae) override;
  void visit(SubtractionExpression &se) override;
};

struct ExpressionEvaluator : ExpressionVisitor
{
  double result;
  void visit(DoubleExpression &de) override;
  void visit(AdditionExpression &ae) override;
  void visit(SubtractionExpression &se) override;
};

struct Expression
{
  virtual void accept(ExpressionVisitor &visitor) = 0;
};

struct DoubleExpression : Expression
{
  double value;
  explicit DoubleExpression(const double value) : value{ value } {}

  void accept(ExpressionVisitor &visitor) override { visitor.visit(*this); }
};

struct AdditionExpression : Expression
{
  Expression &left, &right;

  AdditionExpression(Expression &left, Expression &right)
    : left{ left }, right{ right }
  {}

  void accept(ExpressionVisitor &visitor) override { visitor.visit(*this); }
};

struct SubtractionExpression : Expression
{
  Expression &left, &right;

  SubtractionExpression(Expression &left, Expression &right)
    : left{ left }, right{ right }
  {}

  void accept(ExpressionVisitor &visitor) override { visitor.visit(*this); }
};

void ExpressionPrinter::visit(DoubleExpression &de) { oss << de.value; }

inline static bool with_braces(Expression &e) {
    // we need to show braces for these expressions: 
    // * subtraction
  return dynamic_cast<SubtractionExpression *>(&e);
}

void ExpressionPrinter::visit(AdditionExpression &e)
{
  bool need_braces = with_braces(e);
  e.left.accept(*this);
  oss << "+";
  if (need_braces) oss << "(";
  e.right.accept(*this);
  if (need_braces) oss << ")";
}

void ExpressionPrinter::visit(SubtractionExpression &se)
{
  bool need_braces = with_braces(se);
  if (need_braces) oss << "(";
  se.left.accept(*this);
  oss << "-";
  se.right.accept(*this);
  if (need_braces) oss << ")";
}

void ExpressionEvaluator::visit(DoubleExpression &de) { result = de.value; }

void ExpressionEvaluator::visit(AdditionExpression &ae)
{
  ae.left.accept(*this);
  auto temp = result;
  ae.right.accept(*this);
  result += temp;
}

void ExpressionEvaluator::visit(SubtractionExpression &se)
{
  se.left.accept(*this);
  auto temp = result;
  se.right.accept(*this);
  result = temp - result;
}


// monads

struct Address {
  string* house_name = nullptr;
};

struct Person {
  Address* address = nullptr;
};

template <typename T> struct Maybe;
template <typename T> Maybe<T> maybe(T* context)
{
  return Maybe<T>(context);
}

template <typename T>
struct Maybe {
  T* context;

  Maybe(T *context) : context(context) { }

  template <typename TFunc>
  auto with(TFunc evaluator)
  {
    /*if (context == nullptr)
      return Maybe<typename remove_pointer<decltype(evaluator(context))>::type>(nullptr);
    return maybe(evaluator(context));*/
    return context != nullptr ? maybe(evaluator(context)) : nullptr;
  };

  template <typename TFunc>
  auto Do(TFunc action)
  {
    if (context != nullptr) action(context);
    return *this;
  }
};

void print_house_name(Person* p)
{
  //    if (p != nullptr && p->address != nullptr && p->address->house_name != nullptr)
  //        cout << *p->address->house_name << endl;
  auto z = maybe(p)
    .with([](auto x) { return x->address; })
    .with([](auto x) { return x->house_name; })
    .Do([](auto x) { cout << *x << endl; });
}

void run_visitor_examples()
{
  auto d1 = DoubleExpression{ 1 };
  auto d2 = DoubleExpression{ 2 };
  auto d3 = DoubleExpression{ 3 };
  auto a1 = AdditionExpression{ d1, d3 };
  auto s1 = SubtractionExpression{ d2, d3 };  // FIXME: catching a SEGV here if I use a1 instead of d3 in exp down below!
  auto s2 = SubtractionExpression{ a1, s1 };
  auto e = AdditionExpression{ d1, s1 }; 

  ostringstream oss;
  ExpressionPrinter printer;
  ExpressionEvaluator evaluator;
  printer.visit(e);
  evaluator.visit(e);
  cout << printer.str() << " = " << evaluator.result << endl;

  // monads

  Person p;
  p.address = new Address;
  p.address->house_name = new string("Wonderful house");
  print_house_name(&p);

  delete p.address->house_name;
  delete p.address;
}