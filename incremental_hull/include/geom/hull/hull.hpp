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
    //using namespace geom::rotation;
        
    struct hull_builder {

      hull_builder()
        // : hull([this](const point_type & lh, const point_type & rh) -> bool {
        //     if (lh == min_point) {
        //       return true;
        //     } else if (rh == min_point) {
        //       return false;
        //     } else {
        //       auto rot = rotation::rotation(min_point, lh, rh);
        //       switch (rot) {
        //       case rotation::ROTATION_RIGHT:
        //         return false;
        //       case rotation::ROTATION_LEFT:
        //         return true;
        //       case rotation::ROTATION_NONE:
        //         return square_abs(lh - min_point) < square_abs(rh - min_point);
        //       }
        //       // return rot == rotation::ROTATION_LEFT;
        //     }})
        : upper_hull([](const point_type & lh, const point_type & rh) {
            return lh.x < rh.x;})
        , lower_hull([](const point_type & lh, const point_type & rh) {
            return lh.x > rh.x;})
      {}
      

      typedef treap_t<point_type> hull_treap;
      typedef hull_treap::shared_node shared_treap;
      typedef typename hull_treap::node treap_node;

      void add_point(const point_type & p);

      template<class OutputIt>
      void get_current_hull(OutputIt out_iter);

    private:

      // enum VERTEX_CLASS {
      //   CLS_SUPPORT,
      //   CLS_REFLEX,
      //   CLS_CONCAVE
      // };

      

      

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
                                  const point_type & next_point)
      {
        std::clog << __FUNCTION__ << " source: " << source << " test_point: " << test_point
                  << " prev_point: " << prev_point << " next_point: " << next_point << std::endl;
        const auto next_rot = rotation::rotation(source, test_point, next_point);
        const auto prev_rot = rotation::rotation(source, test_point, prev_point);
        std::clog << __FUNCTION__ << " next_rot: " << next_rot << " prev_rot: " << prev_rot << std::endl;
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
        
        // if (rotation::ROTATION_LEFT == next_rot) {
        //   if (rotation::ROTATION_RIGHT == prev_rot) {
        //     return class_reflex;
        // }
        // if ((rotation::ROTATION_RIGHT == next_rot) && (rotation::ROTATION_LEFT == prev_rot)) {
        //   return class_reflex;
        // } else if ((rotation::ROTATION_LEFT == next_rot) && (rotation::ROTATION_RIGHT == prev_rot)) {
        //   return class_concave;
        // } else if 
      }

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
        // const auto next_node = (node->next()) ? node->next() : upper_bound;
        // const auto prev_node = (node->prev()) ? node->prev() : lower_bound;
        const auto next_node = get_next(node, upper_bound);
        const auto prev_node = get_prev(node, lower_bound);
        const auto cls = classify_point(p, node->value(), prev_node->value(), next_node->value());
        std::clog << __FUNCTION__ << ": cls: " << cls << std::endl;
        return cls;
      }

      enum position_type {
        position_left,
        position_right,
        position_in_on,
        position_up,
        position_down
      };

      static position_type check_position_vertex_vertex(const point_type & p, const point_type & upper_point, const point_type & lower_point)
      {
        std::clog << __FUNCTION__ << " p: " << p << " upper: " << upper_point << " lower: " << lower_point << std::endl;
        if (p.y > upper_point.y) {
          return position_up;
        } else if (p.y < lower_point.y) {
          return position_down;
        } else {
          return position_in_on;
        }
      }

      static position_type check_position_vertex_edge(const point_type & p, const point_type & upper_point, const treap_node * lower_node)
      {
        std::clog << __FUNCTION__ << " p: " << p << " upper: " << upper_point << " lower: " << lower_node->value() << std::endl;
        if (p.y > upper_point.y) {
          return position_up;
        }
        const auto & lower_point = lower_node->value();
        if (p.x > lower_point.x) {
          if (lower_node->prev()) {
            if (rotation::ROTATION_RIGHT == rotation::rotation(lower_point, lower_node->prev()->value(), p)) {
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
            if (rotation::ROTATION_LEFT == rotation::rotation(lower_point, lower_node->next()->value(), p)) {
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
      static position_type check_position_edge_vertex(const point_type & p, const treap_node * upper_node, const point_type & lower_point)
      {
        std::clog << __FUNCTION__ << " p: " << p << " upper: " << upper_node->value() << " lower: " << lower_point << std::endl;
        if (p.y < lower_point.y) {
          return position_down;
        }
        const auto & upper_point = upper_node->value();
        if (p.x > upper_point.x) {
          if (upper_node->next()) {
            if (rotation::ROTATION_LEFT == rotation::rotation(lower_point, upper_node->next()->value(), p)) {
              return position_up;
            } else {
              return position_in_on;
            }
          } else {
            std::clog << "Unused branch of " << __FUNCTION__ << ' ' << __FILE__  " (" << __LINE__ << ") " << std::endl;
            return position_left;
          }
        } else {
          if (upper_node->prev()) {
            if (rotation::ROTATION_RIGHT == rotation::rotation(lower_point, upper_node->prev()->value(), p)) {
              return position_up;
            } else {
              return position_in_on;
            }
          } else {
            std::clog << "Unused branch of " << __FUNCTION__ << ' ' << __FILE__  " (" << __LINE__ << ") " << std::endl;
            return position_left;
          }
        }
      }

      static position_type check_position_edges(const point_type & p,
                                          const point_type & up_left, const point_type & up_right,
                                          const point_type & low_left, const point_type & low_right)
      {
        std::clog << __FUNCTION__ << " p: " << p << " up_left: " << up_left << " up_right: " << up_right
                  << " low_left: " << low_left << " low_right: " << low_right << std::endl;
        if (rotation::ROTATION_LEFT == rotation::rotation(up_left, up_right, p)) {
          return position_up;
        } else if (rotation::ROTATION_RIGHT == rotation::rotation(low_left, low_right, p)) {
          return position_down;
        } else {
          return position_in_on;
        }
      }


      static position_type check_position_edge_edge(const point_type & p, const treap_node * upper_node, const treap_node * lower_node)
      {
        std::clog << __FUNCTION__ << " p: " << p << " upper: " << upper_node->value() << " lower: " << lower_node->value() << std::endl;
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
      
      // static position_type check_position_edge_edge(const point_type & p, const treap_node * upper_node, const treap_node * lower_node)
      // {
      //   const auto & upper_point = upper_node->value();
      //   const auto & lower_point = lower_node->value();
      //   if (p.x > upper_point.x) {
      //     if (upper_node->next()) {
            
      //       if (p.x > lower_point.x) {
      //         if (lower_node->prev()) {
      //           return check_position(p, upper_point, upper_node->next()->value, lower_point, lower_node->prev()->value());
      //         } else {
      //           return position_right;
      //         }
      //       } else { // p.x < lower_point.x
      //         assert(lower_node->next());
      //         return check_position(p, upper_point, upper_node->next()->value(), lower_node->next()->value(), lower_point);
      //         // if (lower_node->next()) {
      //         //   return check_position(p, upper_point, upper_node->next()->value(), lower_node->next()->value(), lower_point);
      //         // } else {
      //         //   return position_left;
      //         // }
      //       }
      //     } else { // ! upper_node->next()
      //       return position_right;
      //     }
      //   } else { // p.x < upper_point.x 
      //     if (upper_node->prev()) {
      //       if (p.x < lower_point.x) {
      //         if (lower_node->next()) {
      //           return check_position(p, upper_node->prev()->value(), upper_point, lower_node->next()->value(), lower_point);
      //         } else {
      //           return position_left;
      //         }
      //       } else { // p.x > lower_point.x
      //         assert(lower_node->prev());
      //         return check_position(p, upper_node->prev()->value(), upper_point, lower_point, lower_node->prev()->value());
      //       }
      //     } else {
      //       return position_left;
      //     }
      //   }
      // }
      

      static position_type check_position(const point_type & p, const treap_node * upper_node, const treap_node * lower_node)
      {
        point_type upper_point = upper_node->value();
        point_type lower_point = lower_node->value();
        if (upper_point != p) {
          if (lower_point != p) {
            return check_position_edge_edge(p, upper_node, lower_node);
          } else {
            return check_position_edge_vertex(p, upper_node, lower_point);
          }
        } else {
          if (lower_point != p) {
            return check_position_vertex_edge(p, upper_point, lower_node);
          } else {
            return check_position_vertex_vertex(p, upper_point, lower_point);
          }
        }
        //edge_type upper_edge(upper_point);
        //edge_type lower_edge(lower_point);
        // if (upper_node->value() == p || lower_node->value() == p) {
        //   return position_in_on;
        // }
      }

      static const treap_node * left_search(const point_type & p, const treap_node * node,
                                            const treap_node * upper_bound, const treap_node * lower_bound);
      
      static const treap_node * right_search(const point_type & p, const treap_node * node,
                                             const treap_node * upper_bound, const treap_node * lower_bound);

      void put_point(const point_type & p,
                     hull_treap & hull_chain,
                     const treap_node * upper_bound, const treap_node * lower_bound);

      void put_point(const point_type & p, hull_treap & left_hull_chain, hull_treap & right_hull_chain);
      

      hull_treap upper_hull;
      hull_treap lower_hull;
      // point_type min_point;
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
    void hull_builder::get_current_hull(OutputIt out_iter)
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
