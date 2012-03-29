#ifndef HULL_H
#define HULL_H

#include <cassert>
#include <utility>
#include <algorithm>

#include "treap/treap.hpp"
#include "geom/primitives/point.h"
#include "io/point.h"
#include "geom/primitives/vector.h"
#include "geom/rotation.hpp"

namespace geom {
  namespace hull {

    using structures::point_type;
    using treap::treap_node;
    using treap::treap_t;
        
    struct hull_builder {

      typedef treap_t<point_type> hull_treap;
      typedef hull_treap::shared_node shared_treap;
      typedef typename hull_treap::node treap_node;
      
      hull_builder()
        : upper_hull([](const point_type & lh, const point_type & rh) {
            return lh.x < rh.x;})
        , lower_hull([](const point_type & lh, const point_type & rh) {
            return lh.x > rh.x;})
      {}

      void add_point(const point_type & p);
      bool verify_hull() const;

      template<class OutputIt>
      void get_current_hull(OutputIt out_iter) const;

    private:

      bool verify_min_max() const;
      bool verify_rotation() const;
      bool verify_self_intersection() const;

      enum vertex_class {
        class_reflex                   = 0x01,
        class_concave                  = 0x02,
        
        class_semicollinear_prev_left  = 0x04,
        class_semicollinear_prev_right = 0x08,
        class_semicollinear_next_left  = 0x10,
        class_semicollinear_next_right = 0x20,
        class_collinear                = 0x40,
        
        class_support_left             = 0x80,
        class_support_right            = 0x100
        
      };

      static vertex_class classify_point(const point_type & source,
                                         const point_type & test_point,
                                         const point_type & prev_point,
                                         const point_type & next_point);

      static const treap_node * get_next(const treap_node * node, const treap_node * bound)
      {
        if (node->next()) {
          return node->next();
        } else {
          if (bound->value() == node->value()) {
            assert(bound->next());
            return bound->next();
          } else {
            return bound;
          }
        }
      }
      
      static const treap_node * get_prev(const treap_node * node, const treap_node * bound)
      {
        if (node->prev()) {
          return node->prev();
        } else {
          if (bound->value() == node->value()) {
            assert(bound->prev());
            return bound->prev();
          } else {
            return bound;
          }
        }
      }
      
      static vertex_class classify(const point_type & p, const treap_node * node,
                                   const treap_node * upper_bound, const treap_node * lower_bound)
      {
        const auto next_node = get_next(node, upper_bound);
        const auto prev_node = get_prev(node, lower_bound);
        const auto cls = classify_point(p, node->value(), prev_node->value(), next_node->value());
#ifndef NDEBUG
        std::clog << __FUNCTION__ << ": cls: " << cls << std::endl;
#endif // NDEBUG
        return cls;
      }

      enum position_type {
        position_left,
        position_right,
        position_in_on,
        position_up,
        position_down
      };

      static position_type check_position(const point_type & p, const treap_node * upper_node, const treap_node * lower_node);
      static position_type check_position_vertex_vertex(const point_type & p, const point_type & upper_point, const point_type & lower_point);
      static position_type check_position_vertex_edge(const point_type & p, const point_type & upper_point, const treap_node * lower_node);
      static position_type check_position_edge_vertex(const point_type & p, const treap_node * upper_node, const point_type & lower_point);
      static position_type check_position_edge_edge(const point_type & p, const treap_node * upper_node, const treap_node * lower_node);
      static position_type check_position_edges(const point_type & p,
                                                const point_type & up_left, const point_type & up_right,
                                                const point_type & low_left, const point_type & low_right);

      static const treap_node * left_search(const point_type & p, const treap_node * node,
                                            const treap_node * upper_bound, const treap_node * lower_bound);
      static const treap_node * right_search(const point_type & p, const treap_node * node,
                                             const treap_node * upper_bound, const treap_node * lower_bound);

      void put_point(const point_type & p, hull_treap & hull_chain,
                     const treap_node * upper_bound, const treap_node * lower_bound);
      void put_point(const point_type & p, hull_treap & left_hull_chain, hull_treap & right_hull_chain);
      

      hull_treap upper_hull;
      hull_treap lower_hull;
      std::vector<point_type> bootstrap;
    
    };

    template<class InputIt, class OutputIt>
    void incremental_hull(InputIt first, InputIt last, OutputIt out_iter)
    {
      hull_builder builder;
      for (;first!=last; ++first) {
        builder.add_point(*first);
      }
      builder.get_current_hull(out_iter);
    }

    template<class OutputIt>
    void hull_builder::get_current_hull(OutputIt out_iter) const 
    {
      bool first_up = upper_hull.min_node()->value() == lower_hull.max_node()->value();
      bool first_low = lower_hull.min_node()->value() == upper_hull.max_node()->value();
      
      upper_hull.for_each([&out_iter, first_up] (const treap_node & node) mutable {
          if (first_up) {
            first_up = false;
            return;
          }
          *out_iter = node.value();
          ++out_iter;
        });
      lower_hull.for_each([&out_iter, first_low] (const treap_node & node) mutable {
          if (first_low) {
            first_low = false;
            return;
          }
          *out_iter = node.value();
          ++out_iter;
        });
    }

  }
  
}

#endif //HULL_H
