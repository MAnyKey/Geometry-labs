#ifndef HULL_H
#define HULL_H

#include "treap/treap_node.hpp"
#include "geom/primitives/point.h"
#include "geom/primitives/vector.h"

namespace geom {
  namespace hull {

    using structures::point_type;
    using treap::treap_node;
    
    struct hull_builder {
      

      typedef treap_node<point_type> hull_treap;

      void add_point(const point_type & p);

      template<class OutputIt>
      void get_current_hull(OutputIt out_iter);

    private:

      hull_treap::shared_treap hull;
    
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
         
    }

  }
}

#endif //HULL_H
