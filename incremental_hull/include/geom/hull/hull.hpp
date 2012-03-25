#ifndef HULL_H
#define HULL_H

#include <cassert>
#include <utility>

#include "treap/treap.hpp"
#include "geom/primitives/point.h"
#include "io/point.h"
#include "geom/primitives/vector.h"

namespace geom {
  namespace hull {

    using structures::point_type;
    using treap::treap_node;
    using treap::treap;
    
    struct hull_builder {

      hull_builder()
        : hull([this](const point_type & lh, const point_type & rh) -> bool {
            if (lh == min_point) {
              return true;
            } else if (rh == min_point) {
              return false;
            } else {
              auto rot = rotation(min_point, lh, rh);
              return rot == ROTATION_LEFT;
            }})
      {}
      

      typedef treap<point_type> hull_treap;
      typedef hull_treap::shared_node shared_treap;

      void add_point(const point_type & p);

      template<class OutputIt>
      void get_current_hull(OutputIt out_iter);

    private:

      shared_treap left_search(const point_type & p, const shared_treap & node)
      {
        assert(node);
        auto cls = classify(p, node);
        switch (cls) {
        case CLS_SUPPORT:
          return node;
        case CLS_REFLEX:
          return left_search(p, node->left());
        case CLS_CONCAVE:
          return left_search(p, node->right());
        }
        std::cerr << "Unknown classify answer: " << cls << std::endl;
        abort();
      }

      shared_treap right_search(const point_type & p, const shared_treap & node)
      {
        assert(node);
        auto cls = classify(p, node);
        switch (cls) {
        case CLS_SUPPORT:
          return node;
        case CLS_REFLEX:
          return right_search(p, node->right());
        case CLS_CONCAVE:
          return right_search(p, node->left());
        }
        std::cerr << "Unknown classify answer: " << cls << std::endl;
        abort();
      }
      
      std::pair<shared_treap, shared_treap> search_edges(const point_type & p, const shared_treap & node)
      {
        if (!node) {
          return std::make_pair(nullptr, nullptr);
        }
        
        auto node_cls = classify(p, node);
        auto min_cls = classify(p, hull.min_node());
        if ( (node_cls == CLS_CONCAVE && min_cls == CLS_CONCAVE) ||
             (node_cls == CLS_REFLEX  && min_cls == CLS_REFLEX) ) {
          auto rot = rotation::rotation(p, hull.min_node()->value(), node->value());
          if ( (node_cls == CLS_CONCAVE && rot == ROTATION_LEFT) ||
               (node_cls == CLS_REFLEX && rot == ROTATION_RIGHT) ) {
            return search_edges(p, node->right());
          } else {
            return search_edges(p, node->left());
          }
        }
        // if ( (node_cls == CLS_CONCAVE && min_cls == CLS_REFLEX) ||
        //      (node_cls == CLS_REFLEX  && min_cls == CLS_CONCAVE) ) {
        return std::make_pair(left_search(p, node), right_search(p, node));
      }

      
      hull_treap hull;
      point_type min_point;
    
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
      hull_treap::for_each(hull,[&out_iter](const hull_treap & node) {
          *out_iter = node.value();
          ++out_iter;
        });
    }

    void hull_builder::add_point(const point_type & p)
    {
      // decltype(left_search(p))  left_edge;
      // decltype(right_search(p)) right_edge;
      auto nodes = search_edges(p, hull);
      auto left_edge = nodes.first;
      auto right_edge = nodes.second;
      if (!left_edge || !right_edge) {
        if (left_edge || right_edge) {
          std::clog << "Error finded only one tangent line";
          std::clog << hull;
          abort();
        }
        // point inside hull
        return;
      }
      hull->shrink_between(left_edge, right_edge);
      if (hull->get_comp()(left_edge.value(), right_edge.value())) {
        // insert this point as new minimum
        min_point = p;
      }
      hull->insert(p);
    }

  }
}

#endif //HULL_H
