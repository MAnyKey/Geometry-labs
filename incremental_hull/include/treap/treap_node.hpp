#ifndef TREAP_NODE_H
#define TREAP_NODE_H

#include <memory>
#include <iostream>

namespace treap
{
  inline void print_n_spaces(std::ostream & out, size_t count)
  {
    for(size_t i = 0; i < count; ++i)
    {
      out.put(' ');
    }
  }

  template<class T>
  struct inc
  {

    inc(T & var, size_t n)
      : ref(var)
      , count(n)
    {
      ref += count;
    }

    ~inc()
    {
      ref -= count;
    }

  private:
    T & ref;
    size_t count;
  };

  extern size_t indent;

  template<class KeyType>
  class treap_t;
  
  template<class KeyType>
  struct treap_node
  {
    typedef std::shared_ptr<treap_node> shared_treap;

    treap_node(KeyType _k, int _p, const shared_treap & _left = nullptr, const shared_treap & _right = nullptr)
      : key(_k)
      , priority(_p)
      , left_child(_left)
      , right_child(_right)
      , prev_node(nullptr)
      , next_node(nullptr)
      , min_node(this)
      , max_node(this)
    {
      link_right_child(right_child);
      link_left_child(left_child);
    }

    const KeyType    & value() const { return key;         }
    const shared_treap left()  const { return left_child;  }
    const shared_treap right() const { return right_child; }
    const treap_node * next()  const { return next_node;   }
    const treap_node * prev()  const { return prev_node;   }
    const treap_node * min()   const { return min_node;    }
    const treap_node * max()   const { return max_node;    }

    // static shared_treap add       (shared_treap & root, KeyType _k, int priority = priority_source());
    // static shared_treap remove    (shared_treap & root, KeyType _k);
    // static treap_node * find(const shared_treap & root, KeyType _k, bool return_last_node = false);
    
    // static void         split(const shared_treap & node, KeyType key, shared_treap & out_left, shared_treap & out_right);
    // static shared_treap merge(const shared_treap & left, const shared_treap & right);
    
    static shared_treap make_treap() { return nullptr; }
    
    // template<class Func>
    // static void for_each(const shared_treap & root, Func f);

    static void print_node_reference(std::ostream & out, const shared_treap & node)
    {
      if (node) {
        out << *node;
      } else {
        out << "(nil)";
      }
    }

    friend std::ostream &operator<<(std::ostream & out, const treap_node & node)
    {
      out << "#<TREAP_NODE :key " << node.key << ' ' << ":priority " << node.priority << std::endl;

      {
        inc<size_t> i(indent, 5);
        
        print_n_spaces(out, indent);
        out << "left child: ";
        print_node_reference(out, node.left_child);
        out << std::endl;
        
        print_n_spaces(out, indent);
        out << "right child: ";
        print_node_reference(out, node.right_child);
      }
      out << '>';
      return out;
    }
  
  private:

    static int priority_source() { return rand(); }
    
    // static void internal_add    (shared_treap & node, const shared_treap & ins);
    // static void internal_remove (shared_treap & node, KeyType _k);
    void link_right_child (const shared_treap & new_right);
    void link_left_child  (const shared_treap & new_left);

    void update_min_node()
    {
      if (left_child) {
        min_node = left_child->min_node;
      } else {
        min_node = this;
      }
    }

    void update_max_node()
    {
      if (right_child) {
        max_node = right_child->max_node;
      } else {
        max_node = this;
      }
    }

    void set_right_child(const shared_treap & new_right)
    {
      right_child = new_right;
      update_max_node();
    }

    void set_left_child(const shared_treap & new_left)
    {
      left_child = new_left;
      update_min_node();
    }
  
    const KeyType key;
    const int priority;

    shared_treap left_child;
    shared_treap right_child;

    treap_node * prev_node;
    treap_node * next_node;

    treap_node * min_node;
    treap_node * max_node;

    friend class treap_t<KeyType>;
    
  };

  // template<class KeyType>
  // inline void treap_node<KeyType>::internal_add(std::shared_ptr<treap_node<KeyType> > & node, const std::shared_ptr<treap_node<KeyType> > & ins)
  // {
  //   if (!node) {
  //     node = ins;
  //   } else if (ins->priority <= node->priority) {
  //     if (node->key > ins->key) {
  //       internal_add(node->left_child, ins);
  //       node->link_left_child( node->left_child);
  //     } else {
  //       internal_add(node->right_child, ins);
  //       node->link_right_child( node->right_child);
  //     }
  //   } else {
  //     split(node, ins->key, ins->left_child, ins->right_child);
  //     ins->link_left_child( ins->left_child);
  //     ins->link_right_child( ins->right_child);
  //     node = ins;
  //   }
  // }

  // template<class KeyType>
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap_node<KeyType>::add(std::shared_ptr<treap_node<KeyType> > & root, KeyType _k, int priority)
  // {
  //   if (find(root, _k) == nullptr) {
  //     auto mid = std::make_shared<treap_node>(_k, priority);
  //     internal_add(root, mid);
  //   }
  //   return root;
  //   // if (root) {
  //   //   shared_treap l, r;
  //   //   split(root, _k, l, r);
  //   //   return root = merge(merge(l,mid), r);
  //   // } else {
  //   //   return root = mid;
  //   // }
  // }

  // template<class KeyType>
  // inline void treap_node<KeyType>::internal_remove(std::shared_ptr<treap_node<KeyType> > & node, KeyType _k)
  // {
  //   if (!node) {
  //     return;
  //   } else if (node->key == _k) {
  //     node = merge(node->left_child, node->right_child);
  //   } else {
  //     if (node->key > _k) {
  //       internal_remove(node->left_child, _k);
  //       node->link_left_child( node->left_child);
  //     } else {
  //       internal_remove(node->right_child, _k);
  //       node->link_right_child( node->right_child);
  //     }
  //   }
  // }

  // template<class KeyType>
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap_node<KeyType>::remove(std::shared_ptr<treap_node<KeyType> > & root, KeyType _k)
  // {
  //   // internal_remove(root, _k);
  //   // return root;
  //   shared_treap l, r, t, mid;
  //   split(root, _k - 1, l, t);
  //   if (t) {
  //     split(t, _k, mid, r);
  //   }
  //   for_each(mid,[_k](const treap_node & node) {
  //       //std::cout << "node.key: " << node.key << " _k: " << _k << std::endl;
  //       assert(node.key == _k);
  //     });
  //   return root = merge(l, r);
  // }

  // template<class KeyType>
  // inline treap_node<KeyType> * treap_node<KeyType>::find(const std::shared_ptr<treap_node<KeyType> > & root, KeyType _k, bool returns_last_node)
  // {
  //   treap_node * node, * prev;
  //   for(node = root.get();
  //       node && (node->key != _k);
  //       prev = node, node = (node->key < _k) ? node->right_child.get() : node->left_child.get()) {}
    
  //   if (!node && returns_last_node) {
  //     return prev;
  //   }
  //   return node;
  // }


  // template<class KeyType>
  // inline void treap_node<KeyType>::split(const std::shared_ptr<treap_node<KeyType> > & node,
  //                                        KeyType split_key,
  //                                        std::shared_ptr<treap_node<KeyType> > & out_left,
  //                                        std::shared_ptr<treap_node<KeyType> > & out_right)
  // {
  //   shared_treap p;
  //   if (!node) {
  //     out_left = out_right = nullptr;
  //     return;
  //   }
    
  //   if (node->key <= split_key) {
  //     if (!node->right_child) {
  //       out_right = nullptr;
  //     } else {
  //       split(node->right_child, split_key, p, out_right);
  //     }
  //     node->link_right_child( p);
  //     out_left = node;
  //     //out_left = std::make_shared<treap_node>(this->key, priority, left_child, p);
  //   } else {
  //     if (!node->left_child) {
  //       out_left = nullptr;
  //     } else {
  //       split(node->left_child, split_key, out_left, p);
  //     }
  //     node->link_left_child( p);
  //     out_right = node;
  //     //out_right = std::make_shared<treap_node>(this->key, priority, p, right_child);
  //   }
  // }

  // template<class KeyType>
  // inline std::shared_ptr<treap_node<KeyType> >
  // treap_node<KeyType>::merge(const std::shared_ptr<treap_node<KeyType> > & left,
  //                            const std::shared_ptr<treap_node<KeyType> > & right)
  // {
  //   if (!left) {
  //     return right;
  //   }
  //   if (!right) {
  //     return left;
  //   }

  //   if (left->priority > right->priority) {
  //     auto new_right = merge(left->right_child, right);
  //     left->link_right_child(new_right);
  //     return left;
  //   } else {
  //     auto new_left = merge(left, right->left_child);
  //     right->link_left_child(new_left);
  //     return right;
  //   }
  // }

  // template<class KeyType> template<class Func>
  // void treap_node<KeyType>::for_each(const std::shared_ptr<treap_node<KeyType> > & root, Func f)
  // {
  //   if (root) {
  //     for(auto curr_node = root->min_node;curr_node; curr_node = curr_node->next_node) {
  //       f(*curr_node);
  //     }
  //   }
  // }

  

  template<class KeyType>
  inline void treap_node<KeyType>::link_right_child(const std::shared_ptr<treap_node<KeyType> > & new_right)
  {
    this->set_right_child(new_right);
    if (new_right) {
      const auto & right_min_node = new_right->min_node;
      this->next_node = right_min_node;
      right_min_node->prev_node = this;
    } else {
      this->next_node = nullptr;
    }
    // this->min_node->prev_node = this->max_node;
    // this->max_node->next_node = this->min_node;
  }

  template<class KeyType>
  inline void treap_node<KeyType>::link_left_child(const std::shared_ptr<treap_node<KeyType> > & new_left)
  {
    this->set_left_child(new_left);
    if (new_left) {
      const auto & left_max_node = new_left->max_node;
      this->prev_node = left_max_node;
      left_max_node->next_node = this;
    } else {
      this->prev_node = nullptr;
    }
    // this->min_node->prev_node = this->max_node;
    // this->max_node->next_node = this->min_node;
  }
}


#endif //TREAP_NODE_H
