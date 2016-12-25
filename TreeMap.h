#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <iostream>
namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:

  class Node
  {
    public:
    value_type node;
    Node *left, *right, *parent;

    Node(): left(nullptr), right(nullptr), parent(nullptr) {}
    Node(const key_type key, mapped_type value, Node* parent): node(key,value), left(nullptr), right(nullptr), parent(parent) {}
  };

  Node *root;
  int size; // how many elements are stored

public:

  TreeMap(): root(nullptr), size(0) {}


  TreeMap(std::initializer_list<value_type> list): TreeMap()
  {
     for (auto it = list.begin(); it != list.end(); it++)
        (*this)[it->first] = it->second;
  }

  TreeMap(const TreeMap& other) :TreeMap()
  {
    for (Iterator it = other.begin(); it != other.end(); it++)
        (*this)[it.node->node.first] = it.node->node.second;
  }

  TreeMap(TreeMap&& other)
  {
     size = other.size;
     root = other.root;
     other.root = nullptr;
     other.size = 0;

  }

  TreeMap& operator=(const TreeMap& other)
  {
      if (*this == other)
                return *this;
    clear(root);
    for (Iterator it = other.begin(); it != other.end(); it++)
        (*this)[it.node->node.first] = it.node->node.second;

            return *this;

  }

  TreeMap& operator=(TreeMap&& other)
  {

    if (*this == other)
            return *this;

     clear(root);

     size = other.size;
     root = other.root;

     other.root = nullptr;
     other.size = 0;
          return *this;

  }

  bool isEmpty() const
  {
     if (size == 0) return true;
     else return false;
  }

  mapped_type& operator[](const key_type& key)
  {
    Node *current = root;
    Node *saved_parent = nullptr;
    Node *new_node;
    while (current != nullptr)
    {
        saved_parent = current;
        if (current->node.first == key) return current->node.second;
        if (key > current->node.first) current = current->right;
        else current = current->left;
    }
    new_node = new Node(key, mapped_type{}, saved_parent);

    size++;
    if (saved_parent == nullptr)root = new_node;
    else
    {
        if (key > saved_parent->node.first)
            saved_parent->right = new_node;
        else saved_parent->left = new_node;
    }
    return new_node->node.second;

  }

  const mapped_type& valueOf(const key_type& key) const
  {
    Node *current = root;
    while (current!= nullptr)
    {
        if (current->node.first == key) return current->node.second;
        if (key > current->node.first) current = current->right;
        else current = current->left;
    }
    throw std::out_of_range("such key doesn't exist");
  }

  mapped_type& valueOf(const key_type& key)
  {
    Node *current = root;
    while (current!= nullptr)
    {
        if (current->node.first == key) return current->node.second;
        if (key > current->node.first) current = current->right;
        else current = current->left;
    }
    throw std::out_of_range("such key doesn't exist");
  }

  const_iterator find(const key_type& key) const
  {
    Node *current = root;
    while (current!= nullptr && current->node.first != key)
    {
       if (key > current->node.first) current = current->right;
        else current = current->left;
    }
   return const_iterator(this, current);
  }

  iterator find(const key_type& key)
  {
    Node *current = root;
    while (current!= nullptr && current->node.first != key)
    {
       if (key > current->node.first) current = current->right;
        else current = current->left;
    }
   return iterator(ConstIterator(this, current));
  }

  void remove(const key_type& key)
  {
    Iterator it = begin();
    Iterator Next;
    Node *current, *saved_parent = nullptr;
    for (; it != end(); it++)
    {
       current = it.node;
       if (current != root) saved_parent = current->parent;

       if ( current->node.first == key)
       {


          if (current->left == nullptr && current->right == nullptr)
          {
            if (current == root) {root = nullptr; delete current; size--; return;}
            if (saved_parent->node.first < key) saved_parent-> right = nullptr;
            else saved_parent->left = nullptr;
            size--;
            delete current;
            return;
          }

        if (current->left == nullptr && current->right != nullptr )
          {
             if (current == root) {root = current->right; delete current; size--; return;}
             if (saved_parent->node.first < key) saved_parent-> right = current->right;
             else saved_parent->left = current->right;
             size--;
             delete current;
             return;
          }

        if (current->left != nullptr && current->right == nullptr )
          {
             if (current == root) {root = current->left; delete current; size--; return;}
             if (saved_parent->node.first < key) saved_parent-> right = current->left;
             else saved_parent->left = current->left;
             size--;
             delete current;
             return;
          }

        if (current->left != nullptr && current->right != nullptr)
        {
            Next = it++;
            Node *next = Next.node;

            Node *copied = new Node (next->node.first, next->node.second, current->parent);
            if (current == root)
            {

            copied->left = root->left;
            copied->right = root->right;
            root = copied;
            copied->left->parent = root;
            copied->right->parent = root;

            delete current;
            size--;
            return;
            }
            if (saved_parent->node.first < key) saved_parent-> right = copied;
            else saved_parent->left = copied;
            copied->left = current->left;
            copied->right = current->right;

          if (next->left == nullptr && next->right != nullptr )
          {
             if (next->parent->node.first < key) next->parent-> right = next->right;
             else next->parent->left = next->right;
             delete next;
          }

        else if (next->left != nullptr && next->right == nullptr )
          {
             if (next->parent->node.first < key) next->parent-> right = next->left;
             else next->parent->left = next->left;
             delete next;
          }
        size--;
        return;
        }
       }

    }
    throw std::out_of_range("given key doesn't exist");
  }

  void remove(const const_iterator& it)
  {
    if (it == end()) throw std::out_of_range("there is no such element");
    key_type key = it.node->node.first;
    remove(key);
    return;
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator ==(const TreeMap& other) const
  {
     if (size != other.size) return false;
     Iterator it = begin();
     Iterator other_it = other.begin();
     for (; it != end(); it++, other_it++)
        if (it.node->node != other_it.node->node) return false;
    return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
   return iterator(cbegin());
  }

  iterator end()
  {
    return iterator(cend());
  }

  const_iterator cbegin() const
  {
    if (root == nullptr) return ConstIterator(this, nullptr);
    Node *current = root;
    while (current->left != nullptr)
        current = current->left;
    return ConstIterator(this, current);
  }

  const_iterator cend() const
  {
    return ConstIterator(this, nullptr);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }


        ~TreeMap() {
            clear(root);
        }


        void clear(Node* pRoot) {
            if (pRoot == nullptr)
                return;

            clear(pRoot->left);
            clear(pRoot->right);

            if (pRoot->left != nullptr) {
                --size;
                delete pRoot->left;
                pRoot->left = nullptr;
            }

            if (pRoot->right != nullptr) {
                --size;
                delete pRoot->right;
                pRoot->right = nullptr;
            }

            if (pRoot == root) {
                delete root;
                root = nullptr;
                --size;
            }
        }

};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
    friend class TreeMap;
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;
private:
    const TreeMap *tmap;
    Node *node;
public:
  explicit ConstIterator()   {}

  ConstIterator(const TreeMap *tmap, Node *node): tmap(tmap), node(node) {};

  ConstIterator(const ConstIterator& other): tmap(other.tmap), node(other.node) {};

  ConstIterator& operator++()
  {

     if (node == nullptr) throw std:: out_of_range("cannot increment end iterator");
     if (node->right != nullptr)
     {
        node = node->right;
        while (node->left != nullptr) node = node->left;

     }
     else
     {
        while (node->parent != nullptr && node->parent->right == node)
            node = node->parent;

        node = node->parent;
     }
     return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator it(*this);
    operator++();
    return it;
  }

  ConstIterator& operator--()
  {
    if(tmap->root == nullptr) throw(std::out_of_range("Attempted to decrement end iterator of empty Map"));
    if(node == nullptr)
    {
      node = tmap->root;
      while(node->right != nullptr)
      {
        node = node->right;
      }
      return *this;
    }

    if(node->left == nullptr)
    {
      while(node->parent != nullptr && node->parent->left == node)
      {
        node = node->parent;
      }
      node = node->parent;
    }
    else
    {
      node = node->left;
      while(node->right != nullptr) node = node->right;
    }
    return *this;
}
  ConstIterator operator--(int)
  {
    ConstIterator it(*this);
    operator--();
    return it;
  }

  reference operator*() const
  {
    if (node == nullptr) throw std:: out_of_range("cannot dereference end iterator");
    return node->node;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
      return ( node == other.node);
  }

  bool operator!=(const ConstIterator& other) const
  {
      return (node != other.node);}
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator()
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_MAP_H */
