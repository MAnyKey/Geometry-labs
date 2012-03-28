#include <stdexcept>

#include "geom/hull/hull.hpp"


template<class T>
struct logger {

  logger(T & t)
    :ref(t)
  {
    std::clog << "(before): " << ref << std::endl;
  }

  logger(const logger &) = delete;
  logger(logger &&) = delete;
  logger & operator=(const logger &) = delete;
  logger & operator=(logger &&) = delete;

  ~logger()
  {
    std::clog << "(after): " << ref << std::endl;
  }

  T & ref;
};


namespace geom {
  namespace hull {
    
    const hull_builder::treap_node * hull_builder::left_search(const point_type & p, const hull_builder::treap_node * node,
                                                 const hull_builder::treap_node * upper_bound, const hull_builder::treap_node * lower_bound)
    {
      std::cout << "bebebe!" << std::endl;
      while (true) {
        auto cls = classify(p, node, upper_bound, lower_bound);
        switch (cls) {
        case class_semicollinear_prev:
        case class_support_right:
          return node;
        case class_reflex:
        case class_semicollinear_next:
        case class_collinear:
          {
            const auto & ref = node->right();
            if (!ref) {
              return nullptr;
            }
            node = ref.get();
          }
          break;
        case class_support_left:
          if (rotation::ROTATION_NONE != rotation::rotation(get_prev(node, lower_bound)->value(),
                                                            node->value(),
                                                            get_next(node, upper_bound)->value())) {
            const auto & ref = node->right();
            if (!ref) {
              return nullptr;
            }
            node = ref.get();
          } else {
            if (node->right()) {
              node = node->right().get();
            } else if (node->left()) {
              node = node->left().get();
            } else {
              assert(false);
            }
          }
          break;

        case class_concave:
          {
            const auto & ref = node->left();
            if (!ref) {
              return nullptr;
            }
            node = ref.get();
          }
          break;
        }
      }
      // case CLS_SUPPORT:
      //   return node;
      // case CLS_REFLEX:
      //   return left_search(p, node->left() ? node->left().get() : node->prev());
      // case CLS_CONCAVE:
      //   return left_search(p, node->right() ? node->right().get() : node->next());
      // }
    }

    const hull_builder::treap_node * hull_builder::right_search(const point_type & p, const hull_builder::treap_node * node,
                                                  const hull_builder::treap_node * upper_bound, const hull_builder::treap_node * lower_bound)
    {
      std::cout << "bebebe!" << std::endl;
      while (true) {
        auto cls = classify(p, node, upper_bound, lower_bound);
        switch (cls) {
        case class_support_left:
        case class_semicollinear_next:
          return node;
        case class_reflex:
        case class_semicollinear_prev:
        case class_collinear:
          {
            const auto & ref = node->left();
            if (!ref) {
              return nullptr;
            }
            node = ref.get();
          }
          break;
        case class_support_right:
          if (rotation::ROTATION_NONE != rotation::rotation(get_prev(node, lower_bound)->value(),
                                                            node->value(),
                                                            get_next(node, upper_bound)->value())) {
            const auto & ref = node->left();
            if (!ref) {
              return nullptr;
            }
            node = ref.get();
          } else {
            if (node->left()) {
              node = node->left().get();
            } else if (node->right()) {
              node = node->right().get();
            } else {
              assert(false);
            }
          }
          break;
          // return right_search(p, node->left(), upper_bound, lower_bound);
        case class_concave:
          {
            const auto & ref = node->right();
            if (!ref) {
              return nullptr;
            }
            node = ref.get();
          }
          break;
          // return right_search(p, node->right(), upper_bound, lower_bound);
        }
      }
    }
      
// std::pair<const treap_node *, const treap_node *> search_edges(const point_type & p, const shared_treap & node)
// {
//   if (!node) {
//     return std::make_pair(nullptr, nullptr);
//   }
        
//   auto node_cls = classify(p, node.get());
//   auto min_cls = classify(p, hull.min_node());
//   if ( (node_cls == CLS_CONCAVE && min_cls == CLS_CONCAVE) ||
//        (node_cls == CLS_REFLEX  && min_cls == CLS_REFLEX) ) {
//     auto rot = rotation::rotation(p, hull.min_node()->value(), node->value());
//     if ( (node_cls == CLS_CONCAVE && rot == rotation::ROTATION_LEFT) ||
//          (node_cls == CLS_REFLEX && rot == rotation::ROTATION_RIGHT) ) {
//       return search_edges(p, node->right());
//     } else {
//       return search_edges(p, node->left());
//     }
//   }
//   // if ( (node_cls == CLS_CONCAVE && min_cls == CLS_REFLEX) ||
//   //      (node_cls == CLS_REFLEX  && min_cls == CLS_CONCAVE) ) {
//   return std::make_pair(left_search(p, node.get()), right_search(p, node.get()));
// }


    void hull_builder::add_point(const point_type & p)
    {
      logger<decltype(upper_hull)> up_log(upper_hull);
      logger<decltype(lower_hull)> low_log(lower_hull);
      if (bootstrap.size() < 2) {
        bootstrap.push_back(p);
        upper_hull.insert(p);
        lower_hull.insert(p);
        return;
      } // else if (bootstrap.size() == 2) {
      //   bootstrap.push_back(p);
      //   min_point = *std::min_element(bootstrap.begin(), bootstrap.end());
      //   for (const auto pt : bootstrap) {
      //     hull.insert(pt);
      //   }
      //   return;
      // }

      auto upper_point = upper_hull.find(p, true);
      auto lower_point = lower_hull.find(p, true);
      auto pos = check_position(p, upper_point, lower_point);
      std::clog << "pos: " << pos;
      switch (pos) {

      case position_in_on:
        return;

      case position_up:
        put_point(p, upper_hull, lower_hull.min_node(), lower_hull.max_node());
        return;

      case position_down:
        put_point(p, lower_hull, upper_hull.min_node(), upper_hull.max_node());
        return;

      case position_left:
        put_point(p, upper_hull, lower_hull);
        return;

      case position_right:
        put_point(p, lower_hull, upper_hull);
        return;
        
      }
      
      
      
      // // decltype(left_search(p))  left_edge;
      // // decltype(right_search(p)) right_edge;
      // auto nodes = search_edges(p, hull.root());
      // auto left_edge = nodes.first;
      // auto right_edge = nodes.second;
      // if (!left_edge || !right_edge) {
      //   if (left_edge || right_edge) {
      //     std::clog << "Error finded only one tangent line\n";
      //     std::clog << hull;
      //     abort();
      //   }
      //   // point inside hull
      //   std::clog << "Inside hull\n";
      //   return;
      // }
      // std::clog << "left edge: " << left_edge->value() << std::endl;
      // std::clog << "right edge: " << right_edge->value() << std::endl;
      // hull.shrink_between(left_edge, right_edge);
      // if (hull.get_comp()(left_edge->value(), right_edge->value())) {
      //   // insert this point as new minimum
      //   min_point = p;
      // }
      // hull.insert(p);
      
    }

    void hull_builder::put_point(const point_type & p, hull_treap & hull_chain,
                                 const hull_builder::treap_node * upper_bound, const hull_builder::treap_node * lower_bound)
    {
      auto left_edge = left_search(p, hull_chain.root().get(), upper_bound, lower_bound);
      auto right_edge = right_search(p, hull_chain.root().get(),upper_bound, lower_bound);

      if (left_edge && right_edge) {
        hull_chain.shrink_between(left_edge, right_edge);
      } else if (left_edge) {
        hull_chain.shrink_from(left_edge);
      } else if (right_edge) {
        hull_chain.shrink_to(right_edge);
      } else {
        throw std::logic_error("No edges found. All points on one line");
      }
      hull_chain.insert(p);
    }
      

    void hull_builder::put_point(const point_type & p, hull_builder::hull_treap & left_hull_chain, hull_builder::hull_treap & right_hull_chain)
    {
      auto left_root = left_hull_chain.root().get();
      auto right_root = right_hull_chain.root().get();

      auto left_min  = left_hull_chain.min_node();
      auto left_max = left_hull_chain.max_node();
      auto right_min = right_hull_chain.min_node();
      auto right_max = right_hull_chain.max_node();
      
      const treap_node * left_edge = left_search(p, left_root, right_min , right_max);
      const treap_node * right_edge = right_search(p, right_root, left_min, left_max);

      if (left_edge && right_edge) {
        left_hull_chain.shrink_to(left_edge);
        right_hull_chain.shrink_from(right_edge);
      } else {
        throw std::logic_error("Not all edges found. All points on one line (maybe).");
      }
      left_hull_chain.insert(p);
      right_hull_chain.insert(p);
    }
  }
}