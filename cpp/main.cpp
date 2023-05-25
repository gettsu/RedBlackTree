#include <iostream>

enum class Color { Red, Black };

template <typename T> struct Node {
  Color color;
  T data;
  Node<T> *left;
  Node<T> *right;
  Node<T> *parent;

  Node(const T &value, Color c = Color::Red)
      : color(c), data(value), left(nullptr), right(nullptr), parent(nullptr) {}
};

template <typename T> class RedBlackTree {
public:
  RedBlackTree() : root(nullptr) {}
  void Insert(const T &value) {
    Node<T> *node = new Node<T>(value);
    InsertNode(node);
    FixInsert(node);
  }

  Node<T> *Search(const T &value) { return SearchNode(root, value); }

  void Delete(const T &value) {
    Node<T> *target = Search(value);
    if (target == nullptr)
      return;

    DeleteNode(target);
  }

  void out() { PrintRedBlackTree(root); }
  void PrintRedBlackTree(Node<T> *node, int indent = 0, bool is_left = false) {
    if (node == nullptr)
      return;

    for (int i = 0; i < indent; i++)
      std::cout << "  ";

    std::cout << (is_left ? "|-- " : "\\-- ");
    std::cout << node->data << " "
              << (node->color == Color::Black ? "(B)" : "(R)") << std::endl;

    PrintRedBlackTree(node->left, indent + 1, true);
    PrintRedBlackTree(node->right, indent + 1, false);
  }

private:
  Node<T> *root;

  void InsertNode(Node<T> *node) {
    Node<T> *parent = nullptr;
    Node<T> *current = root;
    while (current != nullptr) {
      parent = current;
      if (node->data < current->data)
        current = current->left;
      else
        current = current->right;
    }

    node->parent = parent;

    if (parent == nullptr)
      root = node;
    else if (node->data < parent->data)
      parent->left = node;
    else
      parent->right = node;
  }

  void FixInsert(Node<T> *node) {
    // Do if the color of node->parent is not BLACK and node is not the
    // root.
    while (node->parent != nullptr && node->parent->color == Color::Red) {
      // if parent is on the left side of parent->parent
      if (node->parent == node->parent->parent->left) {
        Node<T> *uncle = node->parent->parent->right;
        if (uncle != nullptr && uncle->color == Color::Red) {
          node->parent->color = Color::Black;
          uncle->color = Color::Black;
          node->parent->parent->color = Color::Red;
          node = node->parent->parent;
        } else {
          // if uncle has black color
          if (node == node->parent->right) {
            node = node->parent;
            // RotateRight follows, so called LRRotation
            RotateLeft(node);
          }
          node->parent->color = Color::Black;
          node->parent->parent->color = Color::Red;
          RotateRight(node->parent->parent);
        }
        // if parent is on the right side of parent->parent
      } else {
        Node<T> *uncle = node->parent->parent->left;
        if (uncle != nullptr && uncle->color == Color::Red) {
          node->parent->color = Color::Black;
          uncle->color = Color::Black;
          node->parent->parent->color = Color::Red;
          node = node->parent->parent;
        } else {
          if (node == node->parent->left) {
            node = node->parent;
            // RLRotation
            RotateRight(node);
          }
          node->parent->color = Color::Black;
          node->parent->parent->color = Color::Red;
          RotateLeft(node->parent->parent);
        }
      }
    }
    root->color = Color::Black;
  }

  Node<T> *SearchNode(Node<T> *node, const T &value) {
    if (node == nullptr || node->data == value)
      return node;

    if (value < node->data)
      return SearchNode(node->left, value);
    else
      return SearchNode(node->right, value);
  }

  void DeleteNode(Node<T> *node) {
    Node<T> *successor = nullptr;
    if (node->left == nullptr || node->right == nullptr)
      successor = node;
    else
      successor = MinimumNode(node->right);

    Node<T> *child = successor->right;

    if (child != nullptr)
      child->parent = successor->parent;

    if (successor->parent == nullptr)
      root = child;
    else if (successor == successor->parent->left)
      successor->parent->left = child;
    else
      successor->parent->right = child;

    if (successor != node)
      node->data = successor->data;

    if (successor->color == Color::Black)
      FixDelete(child);

    delete successor;
  }

  void FixDelete(Node<T> *node) {
    while (node != root && (node == nullptr || node->color == Color::Black)) {
      if (node == node->parent->left) {
        Node<T> *sibling = node->parent->right;
        if (sibling->color == Color::Red) {
          sibling->color = Color::Black;
          node->parent->color = Color::Red;
          RotateLeft(node->parent);
          sibling = node->parent->right;
        }
        if ((sibling->left == nullptr ||
             sibling->left->color == Color::Black) &&
            (sibling->right == nullptr ||
             sibling->right->color == Color::Black)) {
          sibling->color = Color::Red;
          // since new node lacks black, it should be fixed in next loop
          node = node->parent;
        } else {
          if (sibling->right == nullptr ||
              sibling->right->color == Color::Black) {
            sibling->left->color = Color::Black;
            sibling->color = Color::Red;
            RotateRight(sibling);
            sibling = node->parent->right;
          }
          sibling->color = node->parent->color;
          node->parent->color = Color::Black;
          sibling->right->color = Color::Black;
          RotateLeft(node->parent);

          node = root;
          break;
        }
      } else {
        Node<T> *sibling = node->parent->left;
        if (sibling->color == Color::Red) {
          sibling->color = Color::Black;
          node->parent->color = Color::Red;
          RotateRight(node->parent);
          sibling = node->parent->left;
        }
        if ((sibling->right == nullptr ||
             sibling->right->color == Color::Black) &&
            (sibling->left == nullptr ||
             sibling->left->color == Color::Black)) {
          sibling->color = Color::Red;
          node = node->parent;
        } else {
          if (sibling->left == nullptr ||
              sibling->left->color == Color::Black) {
            sibling->right->color = Color::Black;
            sibling->color = Color::Red;
            RotateLeft(sibling);
            sibling = node->parent->left;
          }
          sibling->color = node->parent->color;
          node->parent->color = Color::Black;
          sibling->left->color = Color::Black;
          RotateRight(node->parent);
          node = root;
          break;
        }
      }
    }

    if (node != nullptr)
      node->color = Color::Black;
  }

  Node<T> *MinimumNode(Node<T> *node) {
    if (node == nullptr)
      return nullptr;
    while (node->left != nullptr)
      node = node->left;
    return node;
  }

  void RotateLeft(Node<T> *node) {
    Node<T> *right_child = node->right;
    node->right = right_child->left;
    if (right_child->left != nullptr)
      right_child->left->parent = node;
    right_child->parent = node->parent;
    if (node->parent == nullptr)
      root = right_child;
    else if (node == node->parent->left)
      node->parent->left = right_child;
    else
      node->parent->right = right_child;
    right_child->left = node;
    node->parent = right_child;
  }

  void RotateRight(Node<T> *node) {
    Node<T> *left_child = node->left;
    node->left = left_child->right;
    if (left_child->right != nullptr)
      left_child->right->parent = node;
    left_child->parent = node->parent;
    if (node->parent == nullptr)
      root = left_child;
    else if (node == node->parent->left)
      node->parent->left = left_child;
    else
      node->parent->right = left_child;
    left_child->right = node;
    node->parent = left_child;
  }
};

int main() {
  RedBlackTree<int> tree;
  tree.Insert(10);
  tree.Insert(20);
  tree.Insert(30);
  tree.Insert(35);
  tree.Insert(25);
  tree.Delete(30);

  tree.Insert(5);
  tree.Insert(3);
  tree.Insert(8);
  tree.Insert(1);
  tree.Insert(6);
  tree.Insert(9);
  tree.Insert(2);
  tree.Insert(7);
  tree.Insert(4);
  tree.out();

  tree.Delete(6);
  tree.Delete(2);
  tree.Delete(8);

  tree.out();
}
