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
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " p: " << p << std::endl;
#endif //  NDEBUG
      while(node) {
#ifndef NDEBUG
        std::clog << __FUNCTION__ << " node: " << node->value() << std::endl;
#endif //  NDEBUG
        const auto node_cls = classify(p, node, upper_bound, lower_bound);
        const auto min_cls = classify(p, node->min(), upper_bound, lower_bound);
        const auto rot = rotation::rotation(p, node->min()->value(), node->value());

        if (node_cls & (class_support_right | class_semicollinear_prev_right)) {
          if (node->value().x == p.x) {
            return nullptr;
          } 
          return node;
        }

        if ((rotation::ROTATION_RIGHT == rot && class_concave != min_cls) ||
            (rotation::ROTATION_LEFT  == rot && class_concave == node_cls)) {
          node = node->left().get();
        } else {
          node = node->right().get();
        }
      }
      return nullptr;
    }


    const hull_builder::treap_node * hull_builder::right_search(const point_type & p, const hull_builder::treap_node * node,
                                                                const hull_builder::treap_node * upper_bound,
                                                                const hull_builder::treap_node * lower_bound)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " p: " << p << std::endl;
#endif //  NDEBUG
      while(node) {
#ifndef NDEBUG
        std::clog << __FUNCTION__ << " node: " << node->value() << std::endl;
#endif //  NDEBUG
        const auto node_cls = classify(p, node, upper_bound, lower_bound);
        const auto min_cls = classify(p, node->min(), upper_bound, lower_bound);
        const auto rot = rotation::rotation(p, node->min()->value(), node->value());

        if (node_cls & (class_support_left | class_semicollinear_next_left)) {
          if (node->value().x == p.x) {
            return nullptr;
          }
          return node;
        }

        if ((class_reflex != min_cls) && (class_reflex != node_cls) && (rotation::ROTATION_LEFT != rot)) {
          node = node->right().get();
        } else {
          node = node->left().get();
        }
      }
      return nullptr;
    }

    void hull_builder::add_point(const point_type & p)
    {
#ifndef NDEBUG
      logger<decltype(upper_hull)> up_log(upper_hull);
      logger<decltype(lower_hull)> low_log(lower_hull);
#endif 
      if (bootstrap.size() < 2) {
        if (bootstrap.size() && bootstrap.front().x == p.x) {
          upper_hull.clear();
          lower_hull.clear();
          upper_hull.insert(std::max(bootstrap.front(), p));
          lower_hull.insert(std::min(bootstrap.front(), p));
        } else {
          upper_hull.insert(p);
          lower_hull.insert(p);
        }
        bootstrap.push_back(p);
        return;
      }
      // assert(check_all());

      auto upper_point = upper_hull.find(p, true);
      auto lower_point = lower_hull.find(p, true);
      auto pos = check_position(p, upper_point, lower_point);
#ifndef NDEBUG
      std::clog << "pos: " << pos << std::endl;
#endif //  NDEBUG
      switch (pos) {

      case position_in_on:
        break;

      case position_up:
        put_point(p, upper_hull, lower_hull.min_node(), lower_hull.max_node());
        break;

      case position_down:
        put_point(p, lower_hull, upper_hull.min_node(), upper_hull.max_node());
        break;

      case position_left:
        put_point(p, upper_hull, lower_hull);
        break;

      case position_right:
        put_point(p, lower_hull, upper_hull);
        break;
      }
      assert(verify_hull());
      // if (!verify_hull()) {
      //   throw std::logic_error("Bad hull");
      // }
    }

    void hull_builder::put_point(const point_type & p, hull_treap & hull_chain,
                                 const hull_builder::treap_node * upper_bound, const hull_builder::treap_node * lower_bound)
    {
      auto left_edge = left_search(p, hull_chain.root().get(), upper_bound, lower_bound);
      auto right_edge = right_search(p, hull_chain.root().get(),upper_bound, lower_bound);

      if (left_edge && right_edge) {
        hull_chain.shrink_between(right_edge, left_edge);
      } else if (left_edge) {
        hull_chain.shrink_to(left_edge);
      } else if (right_edge) {
        hull_chain.shrink_from(right_edge);
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
        throw std::logic_error("Not all edges found. All points on one line.");
      }
      left_hull_chain.insert(p);
      right_hull_chain.insert(p);
    }

    hull_builder::vertex_class hull_builder::classify_point(const point_type & source,
                                                            const point_type & test_point,
                                                            const point_type & prev_point,
                                                            const point_type & next_point)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " source: " << source << " test_point: " << test_point
                << " prev_point: " << prev_point << " next_point: " << next_point << std::endl;
#endif // NDEBUG
      const auto next_rot = rotation::rotation(source, test_point, next_point);
      const auto prev_rot = rotation::rotation(source, test_point, prev_point);
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " next_rot: " << next_rot << " prev_rot: " << prev_rot << std::endl;
#endif // NDEBUG
      if (next_rot == prev_rot) {
        switch (next_rot) {
        case rotation::ROTATION_NONE:
          return class_collinear;
        case rotation::ROTATION_LEFT:
          return class_support_left;
        case rotation::ROTATION_RIGHT:
          return class_support_right;
        }
      }
      if (prev_rot == rotation::ROTATION_NONE) {
        return (next_rot == rotation::ROTATION_LEFT) ? class_semicollinear_prev_left : class_semicollinear_prev_right;
      } else if (next_rot == rotation::ROTATION_NONE) {
        return (prev_rot == rotation::ROTATION_LEFT) ? class_semicollinear_next_left : class_semicollinear_next_right;
      }
      const auto poly_rotation = rotation::rotation(prev_point, test_point, next_point);
      if (next_rot == poly_rotation) {
        return class_concave;
      } else {
        return class_reflex;
      }
    }

    hull_builder::position_type
    hull_builder::check_position_vertex_vertex(const point_type & p, const point_type & upper_point, const point_type & lower_point)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " p: " << p << " upper: " << upper_point << " lower: " << lower_point << std::endl;
#endif
      if (p.y > upper_point.y) {
        return position_up;
      } else if (p.y < lower_point.y) {
        return position_down;
      } else {
        return position_in_on;
      }
    }

    hull_builder::position_type
    hull_builder::check_position_vertex_edge(const point_type & p, const point_type & upper_point, const treap_node * lower_node)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " p: " << p << " upper: " << upper_point << " lower: " << lower_node->value() << std::endl;
#endif
      if (p.y > upper_point.y) {
        return position_up;
      }
      const auto & lower_point = lower_node->value();
      if (p.x > lower_point.x) {
        if (lower_node->prev()) {
          const auto & low_prev = lower_node->prev()->value();
          const auto rot = rotation::rotation(lower_point, low_prev, p);
          if (rotation::ROTATION_RIGHT == rot) {
            return position_down;
          } else {
            return position_in_on;
          }
        } else {
          std::clog << "Unused branch of " << __FUNCTION__ << std::endl;
          return position_right;
        }
      } else {
        if (lower_node->next()) {
          const auto & low_next = lower_node->next()->value();
          const auto rot = rotation::rotation(lower_point, low_next, p);
          if (rotation::ROTATION_LEFT == rot) {
            return position_down;
          } else {
            return position_in_on;
          }
        } else {
          std::clog << "Unused branch of " << __FUNCTION__ << ' ' << __FILE__  " (" << __LINE__ << ") " << std::endl;
          return position_left;
        }
      }
    }
    
    hull_builder::position_type
    hull_builder::check_position_edge_vertex(const point_type & p, const treap_node * upper_node, const point_type & lower_point)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " p: " << p << " upper: " << upper_node->value() << " lower: " << lower_point << std::endl;
#endif
      if (p.y < lower_point.y) {
        return position_down;
      }
      const auto & upper_point = upper_node->value();
      if (p.x > upper_point.x) {
        if (upper_node->next()) {
          const auto & next_upper = upper_node->next()->value();
          const auto rot = rotation::rotation(upper_point, next_upper, p);
#ifndef NDEBUG
          std::clog << "next_upper: " << next_upper << " rot: " << rot << std::endl;
#endif
          if (rotation::ROTATION_LEFT == rot) {
            return position_up;
          } else {
            return position_in_on;
          }
        } else {
#ifndef NDEBUG
          std::clog << "Unused branch of " << __FUNCTION__ << ' ' << __FILE__  " (" << __LINE__ << ") " << std::endl;
#endif
          return position_left;
        }
      } else {
        if (upper_node->prev()) {
          const auto & prev_upper = upper_node->prev()->value();
          const auto rot = rotation::rotation(upper_point, prev_upper, p);
#ifndef NDEBUG
          std::clog << "prev_upper: " << prev_upper << " rot: " << rot << std::endl;
#endif
          if (rotation::ROTATION_RIGHT == rot) {
            return position_up;
          } else {
            return position_in_on;
          }
        } else {
#ifndef NDEBUG
          std::clog << "Unused branch of " << __FUNCTION__ << ' ' << __FILE__  " (" << __LINE__ << ") " << std::endl;
#endif
          return position_left;
        }
      }
    }

    hull_builder::position_type
    hull_builder::check_position_edges(const point_type & p,
                                       const point_type & up_left, const point_type & up_right,
                                       const point_type & low_left, const point_type & low_right)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " p: " << p << " up_left: " << up_left << " up_right: " << up_right
                << " low_left: " << low_left << " low_right: " << low_right << std::endl;
#endif // NDEBUG
      if (rotation::ROTATION_LEFT == rotation::rotation(up_left, up_right, p)) {
        return position_up;
      } else if (rotation::ROTATION_RIGHT == rotation::rotation(low_left, low_right, p)) {
        return position_down;
      } else {
        return position_in_on;
      }
    }


    hull_builder::position_type
    hull_builder::check_position_edge_edge(const point_type & p, const treap_node * upper_node, const treap_node * lower_node)
    {
#ifndef NDEBUG
      std::clog << __FUNCTION__ << " p: " << p << " upper: " << upper_node->value() << " lower: " << lower_node->value() << std::endl;
#endif // NDEBUG
      point_type upper_point = upper_node->value();
      point_type lower_point = lower_node->value();
      if (p.x < upper_point.x && p.x < lower_point.x && !upper_node->prev() && !lower_node->next()) {
        return position_left;
      } else  if (p.x > upper_point.x && p.x > lower_point.x && !upper_node->next() && !lower_node->prev()) {
        return position_right;
      } else {
        point_type upper_second_point;
        point_type lower_second_point;
        if (p.x < upper_point.x) {
          assert(upper_node->prev());
          upper_second_point = upper_point;
          upper_point = upper_node->prev()->value();
        } else {
          assert(upper_node->next());
          upper_second_point = upper_node->next()->value();
        }
          
        if (p.x < lower_point.x) {
          assert(lower_node->next());
          lower_second_point = lower_point;
          lower_point = lower_node->next()->value();
        } else {
          assert(lower_node->prev());
          lower_second_point = lower_node->prev()->value();
        }
        return check_position_edges(p, upper_point, upper_second_point, lower_point, lower_second_point);
      }
    }
      
    hull_builder::position_type
    hull_builder::check_position(const point_type & p, const treap_node * upper_node, const treap_node * lower_node)
    {
      point_type upper_point = upper_node->value();
      point_type lower_point = lower_node->value();
      if (upper_point.x != p.x) {
        if (lower_point.x != p.x) {
          return check_position_edge_edge(p, upper_node, lower_node);
        } else {
          return check_position_edge_vertex(p, upper_node, lower_point);
        }
      } else {
        if (lower_point.x != p.x) {
          return check_position_vertex_edge(p, upper_point, lower_node);
        } else {
          return check_position_vertex_vertex(p, upper_point, lower_point);
        }
      }
    }
    
  }
}
