#ifndef TREAP_H
#define TREAP_H

#include <functional>

#include "treap/treap_node.hpp"

namespace treap
{
  template<class KeyType>
  struct treap_t {
    typedef treap_node<KeyType> node;
    typedef typename node::shared_treap shared_node;
    typedef std::function<bool (const KeyType &, const KeyType &)> Comp;
    
    explicit treap_t(const Comp & _comp = std::less<KeyType>())
      : root_node(node::make_treap())
      , comp(_comp)
    {}

        
    void insert(const KeyType & key)
    {
      add(root_node, key);
    }
    
    node * find(const KeyType & _k, bool returns_last_node = false)
    {
      return find(root_node, _k, returns_last_node);
    }

    void shrink_to(const node * boundary)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " boundary: " << boundary->value() << std::endl;
#endif // NDEBUG
      if (boundary->prev()) {
        shared_node junk, t;
#ifndef NDEBUG
        std::clog << "bounday prev(): " << boundary->prev()->value() << std::endl;
#endif // NDEBUG
        
        split(root_node, boundary->prev()->value(), junk, t);
        
#ifndef NDEBUG
        std::clog << "t: ";
        node::print_node_reference(std::clog, t);
        std::clog << std::endl;

        std::clog << "junk: ";
        node::print_node_reference(std::clog, junk);
        std::clog << std::endl;
#endif // NDEBUG
        root_node = t;
      }
    }
      
    void shrink_from(const node * boundary)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " boundary: " << boundary->value() << std::endl;
#endif // NDEBUG
      shared_node junk, t;
      split(root_node, boundary->value(), t, junk);
#ifndef NDEBUG
      std::clog << "t: ";
      node::print_node_reference(std::clog, t);
      std::clog << std::endl;
      
      std::clog << "junk: ";
      node::print_node_reference(std::clog, junk);
      std::clog << std::endl;
#endif // NDEBUG
      root_node = t;
    }
      
    void shrink_between(const node * left, const node * right)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " left: " << left->value() << " right: " << right->value() << std::endl;
#endif // NDEBUG
      
      // [begin... left ... right end]
      assert(comp(left->value(), right->value()));
      shared_node tleft, tright, t, junk;
      
#ifndef NDEBUG
      std::clog << "right->prev() " << right->prev()->value() << std::endl;
#endif // NDEBUG
      
      split(root_node, right->prev()->value(), t, tright);
      
#ifndef NDEBUG
      std::clog << "tright: ";
      node::print_node_reference(std::clog, tright);
      std::clog << std::endl;
      
      std::clog << "t: ";
      node::print_node_reference(std::clog, t);
      std::clog << std::endl;
#endif // NDEBUG

      split(t, left->value(), tleft, junk);
      
#ifndef NDEBUG
      std::clog << "tleft: ";
      node::print_node_reference(std::clog, tleft);
      std::clog << std::endl;
      
      std::clog << "junk: ";
      node::print_node_reference(std::clog, junk);
      std::clog << std::endl;
#endif // NDEBUG
      
      root_node = merge(tleft, tright);
    }
    void shrink_between(const shared_node & left, const shared_node & right)
    {
      shrink_between(left.get(), right.get());
    }

    void clear() { root_node = nullptr; }

    void set_comp(const Comp & _comp) { comp = _comp; }
    const Comp & get_comp() const { return comp; }

    const node * min_node() const { return (root_node) ? root_node->min() : nullptr; }
    const node * max_node() const { return (root_node) ? root_node->max() : nullptr; }
    const shared_node & root() const { return root_node; }

    friend std::ostream &operator<<(std::ostream & out, const treap_t & treap)
    {
      node::print_node_reference(out, treap.root());
      return out;
    }

    template<class Func>
    void for_each(Func f) const;

  private:

    static int priority_source() {
      return rand();
    }

    shared_node   add       (shared_node & root, const KeyType & _k, int priority = priority_source());
    shared_node   remove    (shared_node & root, const KeyType & _k);
           node * find(const shared_node & root, const KeyType & _k, bool returns_last_node = false);
    
    void         split(const shared_node & node, KeyType key, shared_node & out_left, shared_node & out_right);
    shared_node  merge(const shared_node & left, const shared_node & right);
    
    

    void internal_add    (shared_node & node, const shared_node & ins);
    void internal_remove (shared_node & node, KeyType _k);

    shared_node root_node;
    Comp comp;
  };

  template<class KeyType>
  inline void treap_t<KeyType>::internal_add(treap_t<KeyType>::shared_node & node, const shared_node & ins)
  {
    if (!node) {
      node = ins;
    } else if (ins->priority <= node->priority) {
      //if (node->key > ins->key) {
      if (comp(ins->key, node->key)) {
        internal_add(node->left_child, ins);
        node->link_left_child( node->left_child);
      } else {
        internal_add(node->right_child, ins);
        node->link_right_child( node->right_child);
      }
    } else {
      split(node, ins->key, ins->left_child, ins->right_child);
      ins->link_left_child( ins->left_child);
      ins->link_right_child( ins->right_child);
      node = ins;
    }
  }

  template<class KeyType>
  inline typename treap_t<KeyType>::shared_node treap_t<KeyType>::add(treap_t<KeyType>::shared_node & root, const KeyType & _k, int priority)
  {
    if (find(root, _k) == nullptr) {
      auto mid = std::make_shared<node>(_k, priority);
      internal_add(root, mid);
    }
    return root;
  }

  template<class KeyType>
  inline void treap_t<KeyType>::internal_remove(treap_t<KeyType>::shared_node & node, KeyType _k)
  {
    if (!node) {
      return;
    } else if (node->key == _k) {
      node = merge(node->left_child, node->right_child);
    } else {
      //if (node->key > _k) {
      if (comp(_k, node->key)) {
        internal_remove(node->left_child, _k);
        node->link_left_child( node->left_child);
      } else {
        internal_remove(node->right_child, _k);
        node->link_right_child( node->right_child);
      }
    }
  }

  template<class KeyType>
  inline typename treap_t<KeyType>::shared_node treap_t<KeyType>::remove(treap_t<KeyType>::shared_node & root, const KeyType & _k)
  {
    internal_remove(root, _k);
    return root;
  }

  template<class KeyType>
  inline typename treap_t<KeyType>::node * treap_t<KeyType>::find(const shared_node & root, const KeyType & _k, bool returns_last_node)
  {
    node * node, * prev = nullptr;
    // !comp(node->key, _k) && !comp(_k, node->key);
    for(node = root.get();
        node && (comp(node->key, _k) || comp(_k, node->key)); // (node->key != _k);
        prev = node, node = comp(node->key, _k) ? node->right_child.get() : node->left_child.get()) {}
    
    if (!node && returns_last_node) {
      return prev;
    }
    return node;
  }


  template<class KeyType>
  inline void treap_t<KeyType>::split(const shared_node & node,
                                      KeyType split_key,
                                      treap_t<KeyType>::shared_node & out_left,
                                      treap_t<KeyType>::shared_node & out_right)

  {
    shared_node p;
    if (!node) {
      out_left = out_right = nullptr;
      return;
    }
    
    //if (node->key <= split_key) {
    if (!comp(split_key, node->key)) {
      if (!node->right_child) {
        out_right = nullptr;
      } else {
        split(node->right_child, split_key, p, out_right);
      }
      node->link_right_child( p);
      out_left = node;
    } else {
      if (!node->left_child) {
        out_left = nullptr;
      } else {
        split(node->left_child, split_key, out_left, p);
      }
      node->link_left_child( p);
      out_right = node;
    }
  }

  template<class KeyType>
  inline typename treap_t<KeyType>::shared_node treap_t<KeyType>::merge(const shared_node & left, const shared_node & right)
  {
    if (!left) {
      return right;
    }
    if (!right) {
      return left;
    }

    if (left->priority > right->priority) {
      auto new_right = merge(left->right_child, right);
      left->link_right_child(new_right);
      return left;
    } else {
      auto new_left = merge(left, right->left_child);
      right->link_left_child(new_left);
      return right;
    }
  }

  template<class KeyType> template<class Func>
  void treap_t<KeyType>::for_each(Func f) const
  {
    if (root_node) {
      for(auto curr_node = root_node->min_node;curr_node; curr_node = curr_node->next_node) {
        f(*curr_node);
      }
    }
  }

  
}


#endif //TREAP_H
