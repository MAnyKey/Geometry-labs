#include "geom/hull/hull.hpp"

namespace geom {
  namespace hull {

    bool hull_builder::verify_hull() const
    {
      const bool min_max = verify_min_max();
      if (!min_max) {
        std::cout << __FUNCTION__ << ": verify min_max has failed\n";
      }
      const bool rotation = verify_rotation();
      if (!rotation) {
        std::cout << __FUNCTION__ << ": verify min_max has failed\n";
      }
      const bool self_intersect = verify_self_intersection();
      if (!self_intersect) {
        std::cout << __FUNCTION__ << ": verify self intersect has failed\n";
      }
      return (min_max && rotation && self_intersect);
    }

    bool hull_builder::verify_min_max() const
    {
      if (bootstrap.size() > 0) {
        const auto & up_min = upper_hull.min_node()->value();
        const auto & up_max = upper_hull.max_node()->value();

        const auto & low_min = lower_hull.min_node()->value();
        const auto & low_max = lower_hull.max_node()->value();

        bool ch_left = (up_min == low_max) || (up_min.x == low_max.x);
        bool ch_right = (up_max == low_min) || (up_max.x == low_min.x);
        return ch_left && ch_right;
      }
      return true;
    }

    bool hull_builder::verify_rotation() const
    {
      bool chk = true;
      upper_hull.for_each([this, &chk](const treap_node & node) {
          const auto rot = rotation::rotation(get_prev(&node, lower_hull.max_node())->value(),
                                              node.value(),
                                              get_next(&node, lower_hull.min_node())->value());
          if (chk) {
            chk = (rotation::ROTATION_LEFT != rot);
          }
        });
      lower_hull.for_each([this, &chk](const treap_node & node) {
          const auto rot = rotation::rotation(get_prev(&node, upper_hull.max_node())->value(),
                                              node.value(),
                                              get_next(&node, upper_hull.min_node())->value());
          if (chk) {
            chk = (rotation::ROTATION_LEFT != rot);
          }
        });
      return chk;
    }

    enum line_cross_class {

      crosses,
      none,
      touches
    };

    line_cross_class cross(const point_type & pbegin1, const point_type & pend1, const point_type & pbegin2, const point_type & pend2)
    {
      const rotation::rotation_t rotations[] = { rotation::rotation(pbegin1, pend1, pbegin2),
                                                 rotation::rotation(pbegin1, pend1, pend2),
                                                 rotation::rotation(pbegin2, pend2, pbegin1),
                                                 rotation::rotation(pbegin2, pend2, pend1) };
      const auto rend = rotations + (sizeof(rotations)/sizeof(*rotations));
      if (std::find(rotations, rend, rotation::ROTATION_NONE) != rend) {
        return touches;
      }
      if ((rotations[0] != rotations[1]) && (rotations[2] != rotations[3])) {
        return crosses;
      }
      return none;
    }

    template<class Func>
    void for_each_edges(const std::vector<point_type> & points, Func f)
    {
      if (points.size() > 1) {
        point_type prev = points.back();
        for(const auto & p : points) {
          f(prev, p);
          prev = p;
        }
      }
    }

    bool hull_builder::verify_self_intersection() const
    {
      bool chk = true;
      std::vector<point_type> v;
      get_current_hull(back_inserter(v));
      for_each_edges(v, [&](const point_type & pbegin, const point_type & pend) {
          for_each_edges(v, [&](const point_type & nbegin, const point_type & nend) {
              const auto c = cross(pbegin, pend, nbegin, nend);
              switch (c) {

              case crosses:
                chk = false;
                break;

              case none:
                break;

              case touches:
                if ((pbegin != nbegin) && (pbegin != nend) && (pend != nbegin) && (pend != nend)) {
                  chk = false;
                } 
                break;  
              }
            });
        });
      return chk;
    }

  }
}
