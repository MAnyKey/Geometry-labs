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
      std::clog << __FUNCTION__ << " boundary: " << boundary->value(), << std::endl;
      if (boundary->prev()) {
        shared_node junk, t;
        std::clog << "bounday prev(): " << boundary->prev()->value() << std::endl;
        split(root_node, boundary->prev()->value(), junk, t);

        std::clog << "t: ";
        node::print_node_reference(std::clog, t);
        std::clog << std::endl;

        std::clog << "junk: ";
        node::print_node_reference(std::clog, junk);
        std::clog << std::endl;
        root_node = t;
      }
    }
      
    void shrink_from(const node * boundary)
    {
      std::clog << __FUNCTION__ << " boundary: " << boundary->value() << std::endl;
      shared_node junk, t;
      split(root_node, boundary->value(), t, junk);

      std::clog << "t: ";
      node::print_node_reference(std::clog, t);
      std::clog << std::endl;

      std::clog << "junk: ";
      node::print_node_reference(std::clog, junk);
      std::clog << std::endl;
      root_node = t;
    }
      
    void shrink_between(const node * left, const node * right)
    {
      std::clog << __FUNCTION__ << " left: " << left->value() << " right: " << right->value() << std::endl;
      // if (comp(left->value(), right->value())) {
      // [begin... left ... right end]
      assert(comp(right->value(), left->value()));
      shared_node tleft, tright, t, junk;
      std::clog << "left->prev() " << left->prev()->value() << std::endl;
      split(root_node, left->prev()->value(), t, tleft);
      
      std::clog << "tleft: ";
      node::print_node_reference(std::clog, tleft);
      std::clog << std::endl;
        
      std::clog << "t: ";
      node::print_node_reference(std::clog, t);
      std::clog << std::endl;

      split(t, right->value(), tright, junk);

      std::clog << "tright: ";
      node::print_node_reference(std::clog, tright);
      std::clog << std::endl;
      
      std::clog << "junk: ";
      node::print_node_reference(std::clog, junk);
      std::clog << std::endl;
      
      root_node = merge(tright, tleft);
      
      // split(root_node, left->value(), tleft, t);

      // std::clog << "tleft: ";
      // node::print_node_reference(std::clog, tleft);
      // std::clog << std::endl;
        
      // std::clog << "t: ";
      // node::print_node_reference(std::clog, t);
      // std::clog << std::endl;
        
      // if (auto rs = right->prev()) {
      //   shared_node junk;
      //   split(t, rs->value(), junk, tright);
          
      //   std::clog << "junk: ";
      //   node::print_node_reference(std::clog, junk);
      //   std::clog << std::endl;

      //   std::clog << "tright: ";
      //   node::print_node_reference(std::clog, tright);
      //   std::clog << std::endl;
      // }
      // root_node = merge(tleft, tright);
      // } else {
      //   // [begin... right ... left end]
      //   std::clog << "[begin... right ... left end]\n";
      //   shared_node t;
      //   {
      //     shared_node junk;
      //     split(root_node, left->value(), t, junk);
      //     std::clog << "t: ";
      //     node::print_node_reference(std::clog, t);
      //     std::clog << std::endl;
      //     std::clog << "junk: ";
      //     node::print_node_reference(std::clog, junk);
      //     std::clog << std::endl;
      //   }
      //   if (auto rs = right->prev()) {
      //     shared_node junk;
      //     shared_node tright;
      //     split(t, rs->value(), junk, tright);
      //     t = tright;
      //     std::clog << "junk: ";
      //     node::print_node_reference(std::clog, junk);
      //     std::clog << std::endl;
          
      //     std::clog << "tright: ";
      //     node::print_node_reference(std::clog, tright);
      //     std::clog << std::endl;
      //   }
      //   root_node = t;
      // }
    }
    void shrink_between(const shared_node & left, const shared_node & right)
    {
      shrink_between(left.get(), right.get());
    }

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
    void for_each(Func f);

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
  //inline void treap_t<KeyType>::internal_add(std::shared_ptr<treap_node<KeyType> > & node, const std::shared_ptr<treap_node<KeyType> > & ins)
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
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap_t<KeyType>::add(std::shared_ptr<treap_node<KeyType> > & root, KeyType _k, int priority)
  {
    if (find(root, _k) == nullptr) {
      auto mid = std::make_shared<node>(_k, priority);
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
  inline void treap_t<KeyType>::internal_remove(treap_t<KeyType>::shared_node & node, KeyType _k)
  //inline void treap_t<KeyType>::internal_remove(std::shared_ptr<treap_node<KeyType> > & node, KeyType _k)
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
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap_t<KeyType>::remove(std::shared_ptr<treap_node<KeyType> > & root, KeyType _k)
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
  inline typename treap_t<KeyType>::node * treap_t<KeyType>::find(const shared_node & root, const KeyType & _k, bool returns_last_node)
  // inline treap_node<KeyType> * treap_t<KeyType>::find(const std::shared_ptr<treap_node<KeyType> > & root, KeyType _k, bool returns_last_node)
  {
    node * node, * prev;
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
  // inline void treap_t<KeyType>::split(const std::shared_ptr<treap_node<KeyType> > & node,
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
  inline typename treap_t<KeyType>::shared_node treap_t<KeyType>::merge(const shared_node & left, const shared_node & right)
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap_t<KeyType>::merge(const std::shared_ptr<treap_node<KeyType> > & left,
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
  void treap_t<KeyType>::for_each(Func f)
  // void treap_t<KeyType>::for_each(const std::shared_ptr<treap_node<KeyType> > & root, Func f)
  {
    if (root_node) {
      // auto curr_node = root_node->min_node;
      // do {
      //   f(*curr_node);
      //   curr_node = curr_node->next_node;
      // } while (curr_node != root_node->min_node);
      for(auto curr_node = root_node->min_node;curr_node; curr_node = curr_node->next_node) {
        f(*curr_node);
      }
    }
  }

  
}


#endif //TREAP_H
