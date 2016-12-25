#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
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
    Node *next;
    Node *prev;

    Node(): next(nullptr), prev(nullptr) {}
    Node(const KeyType key, mapped_type value): node (key,value), next(nullptr), prev(nullptr) {}
};

Node **table;
int size;
int bucket_count;

int hash(const key_type key) const
{
   return std::hash<key_type>{}(key) % bucket_count;
}


public:


  HashMap(int buckets_number = 10): size(0), bucket_count(buckets_number)
  {
    table = new Node* [buckets_number]();
  }

  HashMap(std::initializer_list<value_type> list): HashMap()
  {
    for (auto it = list.begin(); it != list.end(); it++)
        (*this)[it->first] = it->second;
  }

  HashMap(const HashMap& other): HashMap(other.bucket_count)
  {
    for (iterator it = other.begin(); it != other.end(); it++)
        (*this)[it->first] = it->second;
  }

  HashMap(HashMap&& other): HashMap(other.bucket_count)
  {
     for (iterator it = other.begin(); it != other.end(); it++)
        (*this)[it->first] = it->second;

    other.table = nullptr;
    other.size = 0;
    other.bucket_count = 0;
  }

   void delete_all()
   {
    Node *current, *next;
    size = 0;
    for (int bucket_id = 0; bucket_id < bucket_count; bucket_id++)
    {
        current = table[bucket_id];
        while (current != nullptr)
        {
            next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] table;
   }

  ~HashMap()
  {
    delete_all();
  }

  HashMap& operator=(const HashMap& other)
  {
    if (*this == other) return *this;
    delete_all();
    table = new Node* [other.bucket_count]();
    for (iterator it = other.begin(); it != other.end(); it++)
        (*this)[it->first] = it->second;
    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if (*this != other)
    {
        delete_all();
        table = new Node* [other.bucket_count]();
        for (iterator it = other.begin(); it != other.end(); it++)
            (*this)[it->first] = it->second;
    }
    other.table = nullptr;
    other.size = 0;
    other.bucket_count = 0;
    return *this;
  }

  bool isEmpty() const
  {
    if (size == 0) return true;
    else return false;
  }

  mapped_type& operator[](const key_type& key)
  {
        int bucket_id = hash(key);
        Node *current = table [bucket_id];
        Node *prev = nullptr;
        while (current != nullptr && current->node.first != key)
        {
            prev = current;
            current = current->next;
        }
        if (current != nullptr) return current->node.second; //current->node.first == key

        size++;
        Node *new_node = new Node (key, mapped_type{});
        if (prev == nullptr) //bucket was empty
        {
            table [bucket_id] = new_node;
        }
        else
        {
            prev->next = new_node;
            new_node->prev = prev;
        }
        return new_node->node.second;

  }

  const mapped_type& valueOf(const key_type& key) const
  {
    const iterator it = find (key);
    if (it == end()) throw std:: out_of_range("such key doesn't exist");
    return it->second;
   }

  mapped_type& valueOf(const key_type& key)
  {

    iterator it = find (key);
    if (it == end()) throw std:: out_of_range("such key doesn't exist");
    return it->second;

  }



  const_iterator find(const key_type& key) const
  {
    int bucket_id = hash(key);
    Node* current = table [bucket_id];
    while (current != nullptr && current->node.first != key)
        current = current->next;

    return ConstIterator(this, bucket_id, current);
  }

  iterator find(const key_type& key)
  {
    int bucket_id = hash(key);
    Node* current = table [bucket_id];
    while (current != nullptr && current->node.first != key)
        current = current->next;

    return Iterator(ConstIterator(this, bucket_id, current));
  }

  void remove(const key_type& key)
  {
    int bucket_id = hash(key);
    Node *current = table[bucket_id];
    while (current != nullptr && current->node.first != key)
        current = current->next;

    if (current == nullptr) throw std::out_of_range("such key doesn't exist");
    if(current->next == nullptr) /*deleting last element in bucket */
    {
        if (current->prev != nullptr) current->prev->next = nullptr; // there is still an element in bucket
        else table[bucket_id] = nullptr; //now bucket is empty
        delete current;
    }
    else  //removing from middle
    {
        current->prev-> next = current->next;
        current->next->prev = current->prev;
        delete current;
    }
    size--;
  }

  void remove(const const_iterator& it)
  {
    if (it == cend()) throw std::out_of_range("cannot erase end");
    if(it.node->next == nullptr) /*deleting last element in bucket */
    {
        if (it.node->prev != nullptr) it.node->prev->next = nullptr; // there is still an element in bucket
        else table[it.bucket_id] = nullptr; //now bucket is empty
        delete it.node;
    }
    else  //removing from middle
    {
        it.node->prev-> next = it.node->next;
        it.node->next->prev = it.node->prev;
        delete it.node;
    }
    size--;
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const HashMap& other) const
  {
    if (size != other.size) return false;
    Iterator search_key;
    for (Iterator it = begin(); it != end();  it++)
    {
        search_key = other.find (it->first);
        if (search_key == end() || search_key->second != it->second)
            return false;
    }

    return true;

  }

  bool operator!=(const HashMap& other) const
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
    if (size == 0) return cend();
    int bucket_id = 0;
    while (table[bucket_id] == nullptr) bucket_id++;
    return ConstIterator(this, bucket_id,table[bucket_id] );
  }

  const_iterator cend() const
  {
      return const_iterator(this, bucket_count, nullptr);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
friend class HashMap;
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

private:
   const HashMap *hashmap;
   int bucket_id;
   Node *node;
public:

  explicit ConstIterator() : hashmap (nullptr), bucket_id(0), node (nullptr)   {}
  ConstIterator(const HashMap *hashmap, int bucket_id, Node *node) : hashmap (hashmap), bucket_id(bucket_id), node (node) {}
  ConstIterator(const ConstIterator& other)
  {
     hashmap = other.hashmap;
     bucket_id = other.bucket_id;
     node = other.node;
  }

  ConstIterator& operator++()
  {
   if (node == nullptr) throw std::out_of_range("cannot increment end iterator");
   if (node->next != nullptr) node = node->next;
   else
   {
        bucket_id++;
        while (bucket_id < hashmap->bucket_count && hashmap->table[bucket_id] == nullptr)
           bucket_id++;
        if (bucket_id == hashmap->bucket_count) node = nullptr;
        else node = hashmap->table [bucket_id];

   }
   return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator c(*this);
    operator++();
    return c;
  }

  ConstIterator& operator--()
  {
    if (node == nullptr) //end
    {
        bucket_id = hashmap->bucket_count -1;
    }
    else if (node->prev != nullptr)
    {
        node = node->prev;
        return *this;
    }
    else
    {
        bucket_id -- ;
        node =  nullptr;
    }

    while (bucket_id != -1 && hashmap->table[bucket_id] == nullptr)
           bucket_id--;
    if (bucket_id == -1) throw std::out_of_range("cannot decrement begin iterator");
    node = hashmap->table [bucket_id];
    while (node->next != nullptr) node = node->next;
     return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator c(*this);
    operator--();
    return c;
  }

  reference operator*() const
  {
    if (node == nullptr)throw std:: out_of_range("cannot dereference end iterator");
    return node->node;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    if (node == other.node) return true;
    else return false;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

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

#endif /* AISDI_MAPS_HASHMAP_H */
