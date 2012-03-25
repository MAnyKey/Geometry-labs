#ifndef TREAP_H
#define TREAP_H

#include <functional>

#include "treap/treap_node.hpp"

namespace treap
{
  template<class KeyType>
  struct treap {
    typedef treap_node<KeyType> node;
    typedef typename node::shared_treap shared_node;
    typedef std::function<bool (const KeyType &, const KeyType &)> Comp;
    
    explicit treap(const Comp & _comp = std::less<KeyType>())
      : root_node(node::make_treap())
      , comp(_comp)
    {}

        
    void insert(const KeyType & key);

    void shrink_between(const shared_node & left, const shared_node & right);

    void set_comp(const Comp & _comp);
    void get_comp() const { return comp; }

    node * min_node() const;
    const shared_node & root() const { return root_node; }

    friend std::ostream &operator<<(std::ostream & out, const treap & treap)
    {
      node::print_node_reference(out, treap.root());
      return out;
    }

  private:

    shared_node   add       (shared_node & root, KeyType _k, int priority = priority_source());
    shared_node   remove    (shared_node & root, KeyType _k);
           node * find(const shared_node & root, KeyType _k, bool return_last_node = false);
    
    void         split(const shared_node & node, KeyType key, shared_node & out_left, shared_node & out_right);
    shared_node  merge(const shared_node & left, const shared_node & right);
    
    template<class Func>
    void for_each(const shared_node & root, Func f);

    void internal_add    (shared_node & node, const shared_node & ins);
    void internal_remove (shared_node & node, KeyType _k);

    
    
    shared_node root_node;
    Comp comp;
  };

  template<class KeyType>
  inline void treap<KeyType>::internal_add(treap<KeyType>::shared_node & node, const shared_node & ins)
  //inline void treap<KeyType>::internal_add(std::shared_ptr<treap_node<KeyType> > & node, const std::shared_ptr<treap_node<KeyType> > & ins)
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
  inline typename treap<KeyType>::shared_node treap<KeyType>::add(treap<KeyType>::shared_node & root, KeyType _k, int priority)
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap<KeyType>::add(std::shared_ptr<treap_node<KeyType> > & root, KeyType _k, int priority)
  {
    if (find(root, _k) == nullptr) {
      auto mid = std::make_shared<treap_node>(_k, priority);
      internal_add(root, mid);
    }
    return root;
    // if (root) {
    //   shared_treap l, r;
    //   split(root, _k, l, r);
    //   return root = merge(merge(l,mid), r);
    // } else {
    //   return root = mid;
    // }
  }

  template<class KeyType>
  inline void treap<KeyType>::internal_remove(treap<KeyType>::shared_node & node, KeyType _k)
  //inline void treap<KeyType>::internal_remove(std::shared_ptr<treap_node<KeyType> > & node, KeyType _k)
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
  inline typename treap<KeyType>::shared_node treap<KeyType>::remove(treap<KeyType>::shared_node & root, KeyType _k)
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap<KeyType>::remove(std::shared_ptr<treap_node<KeyType> > & root, KeyType _k)
  {
    internal_remove(root, _k);
    return root;
    // shared_treap l, r, t, mid;
    // split(root, _k - 1, l, t);
    // if (t) {
    //   split(t, _k, mid, r);
    // }
    // for_each(mid,[_k](const treap_node & node) {
    //     //std::cout << "node.key: " << node.key << " _k: " << _k << std::endl;
    //     assert(node.key == _k);
    //   });
    // return root = merge(l, r);
  }

  template<class KeyType>
  inline typename treap<KeyType>::node * treap<KeyType>::find(const shared_node & root, KeyType _k, bool returns_last_node)
  // inline treap_node<KeyType> * treap<KeyType>::find(const std::shared_ptr<treap_node<KeyType> > & root, KeyType _k, bool returns_last_node)
  {
    node * node, * prev;
    for(node = root.get();
        node && (node->key != _k);
        prev = node, node = comp(node->key, _k) ? node->right_child.get() : node->left_child.get()) {}
    
    if (!node && returns_last_node) {
      return prev;
    }
    return node;
  }


  template<class KeyType>
  inline void treap<KeyType>::split(const shared_node & node,
                                    KeyType split_key,
                                    treap<KeyType>::shared_node & out_left,
                                    treap<KeyType>::shared_node & out_right)
  // inline void treap<KeyType>::split(const std::shared_ptr<treap_node<KeyType> > & node,
  //                                        KeyType split_key,
  //                                        std::shared_ptr<treap_node<KeyType> > & out_left,
  //                                        std::shared_ptr<treap_node<KeyType> > & out_right)
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
      //out_left = std::make_shared<treap_node>(this->key, priority, left_child, p);
    } else {
      if (!node->left_child) {
        out_left = nullptr;
      } else {
        split(node->left_child, split_key, out_left, p);
      }
      node->link_left_child( p);
      out_right = node;
      //out_right = std::make_shared<treap_node>(this->key, priority, p, right_child);
    }
  }

  template<class KeyType>
  inline typename treap<KeyType>::shared_node treap<KeyType>::merge(const shared_node & left, const shared_node & right)
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap<KeyType>::merge(const std::shared_ptr<treap_node<KeyType> > & left,
  //                       const std::shared_ptr<treap_node<KeyType> > & right)
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

  //template<class KeyType> template<class Func>
  template<class KeyType> template<class Func>
  void treap<KeyType>::for_each(const shared_node & root, Func f)
  // void treap<KeyType>::for_each(const std::shared_ptr<treap_node<KeyType> > & root, Func f)
  {
    if (root) {
      for(auto curr_node = root->min_node;curr_node; curr_node = curr_node->next_node) {
        f(*curr_node);
      }
    }
  }

  
}


#endif //TREAP_H
