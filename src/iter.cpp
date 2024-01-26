#include <iostream>
#include <string>

template<typename T> struct BinaryTree;

template<typename T> struct Node
{
  T value;
  Node<T> *left = nullptr;
  Node<T> *right = nullptr;
  Node<T> *parent = nullptr;
  BinaryTree<T> *tree = nullptr;

  // constructors

  explicit Node(const T &value) : value(value) {}

  Node(const T &value, Node<T> *const left, Node<T> *const right)
    : value(value), left(left), right(right)
  {
    this->left->tree = this->right->tree = tree;
    this->left->parent = this->right->parent = this;
  }

  void set_tree(BinaryTree<T> *t)
  {
    tree = t;
    if (left) left->set_tree(t);
    if (right) right->set_tree(t);
  }
};

template<typename U> struct PreOrderIterator
{
  Node<U> *current;

  explicit PreOrderIterator(Node<U> *current) : current(current) {}

  bool operator!=(const PreOrderIterator<U> &other)
  {
    return current != other.current;
  }

  Node<U> &operator*() { return *current; }

  PreOrderIterator<U> &operator++()
  {
    if (current->right) {
      current = current->right;
      while (current->left) { current = current->left; }
    } else {
      Node<U> *p = current->parent;
      while (p && current == p->right) {
        current = p;
        p = p->parent;
      }
      current = p;
    }
    return *this;
  }
};

template<typename T> struct BinaryTree
{
  Node<T> *root = nullptr;

  explicit BinaryTree(Node<T> *const root) : root(root)
  {
    root->set_tree(this);
  }

  typedef PreOrderIterator<T> iterator;

  iterator begin()
  {
    Node<T> *n = root;

    if (n) {
      while (n->left) { n = n->left; }
    }
    return iterator{ n };
  }

  iterator end() { return iterator{ nullptr }; }

  class pre_order_traversal
  {
    BinaryTree<T> &tree;

  public:
    pre_order_traversal(BinaryTree<T> &tree) : tree{ tree } {}
    iterator begin() { return tree.begin(); }
    iterator end() { return tree.end(); }
  } pre_order { *this };
};


void run_iterator_examples()
{
  BinaryTree<std::string> family{ new Node<std::string>{ "me",
    new Node<std::string>{ "mother",
      new Node<std::string>{ "mother's mother" },
      new Node<std::string>{ "mother's father" } },
    new Node<std::string>{ "father" } } };

  std::cout << "Tree traversal with an iterator:\n";

  for (auto it = family.begin(); it != family.end(); ++it) {
    std::cout << (*it).value << "\n";
  }

  std::cout << "same with an alias:\n";

  for (const auto& it: family.pre_order) {
    std::cout << it.value << "\n";
  }
}