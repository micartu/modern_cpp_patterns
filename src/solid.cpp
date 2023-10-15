#include "solid.h"
#include <iostream>

std::vector<Product> source()
{
  Product apple{ "Apple", Color::Green, Size::Small };
  Product tree{ "Tree", Color::Green, Size::Large };
  Product house{ "House", Color::Blue, Size::Large };
  Product shit{ "Shit", Color::Green, Size::Medium };
  return { apple, tree, house, shit };
}

void run_general_solid_filter()
{
  std::cout << "run_general_solid_filter:" << std::endl;
  auto all = source();

  auto res =
    LambdaFilter<Product>::filter(all, [](auto prd) { return prd.color == Color::Green && prd.size == Size::Large; });

  std::cout << "green and large:" << std::endl;

  for (auto &prd : res) { std::cout << prd.name << " is green and large" << std::endl; }

  res = LambdaFilter<Product>::filter(all, [](auto prd) { return prd.color == Color::Green; });

  std::cout << "green:" << std::endl;

  for (auto &prd : res) { std::cout << prd.name << " is green" << std::endl; }

  std::cout << "green and large or green and small:" << std::endl;

  res = LambdaFilter<Product>::filter(all, [](auto prd) {
    return (prd.color == Color::Green && prd.size == Size::Large)
           || (prd.color == Color::Green && prd.size == Size::Small);
  });

  for (auto &prd : res) { std::cout << prd.name << "" << std::endl; }
}

void run_solid_from_book_examples()
{
  std::cout << "run_solid_from_book_examples:" << std::endl;
  auto all = source();

  BetterFilter btrflt;
  ColorSpecification green(Color::Green);
  SizeSpecification small(Size::Small);
  SizeSpecification large(Size::Large);
  // auto largeAndGreen = AndSpecification<Product>(green, large);
  auto largeAndGreen = green && large;
  // auto smallAndGreen = AndSpecification<Product>(green, small);
  auto smallAndGreen = green && small;

  auto green_things = btrflt.filter(all, green);
  for (auto &x : green_things) { std::cout << x.name << " is green" << std::endl; }

  auto green_and_big = btrflt.filter(all, largeAndGreen);
  for (auto &x : green_and_big) { std::cout << x.name << " is green and big" << std::endl; }

  std::cout << "green and large or green and small:" << std::endl;
  auto greens = largeAndGreen || smallAndGreen;
  auto greeny = btrflt.filter(all, greens);
  for (auto &x : greeny) { std::cout << x.name << "" << std::endl; }
}

// all functions combined
void run_solid_examples()
{
  run_general_solid_filter();
  run_solid_from_book_examples();
}
