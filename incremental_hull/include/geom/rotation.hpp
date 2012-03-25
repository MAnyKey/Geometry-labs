#ifndef ROTATION_H
#define ROTATION_H

#include "vector.hpp"

namespace geom
{
  namespace rotation
  {
    using namespace primitives;

    
    enum rotation_t
    {
      ROTATION_LEFT,
      ROTATION_RIGHT,
      ROTATION_NONE
    };

    inline rotation_t rotation(const vector_t & lh, const vector_t & rh)
    {
      int64_t cross = cross_product(lh, rh);
      if (0 < cross)
        return ROTATION_LEFT;
      else if (0 > cross)
        return ROTATION_RIGHT;
      return ROTATION_NONE;
    }

    inline rotation_t rotation(const point_t & p1, const point_t & p2, const point_t & p3)
    {
      return rotation(vector_t(p1, p2),vector_t(p2, p3));
    }

    inline bool left_rotation(const vector_t & lh, const vector_t & rh)
    {
      return rotation(lh, rh) == ROTATION_LEFT;
    }

    inline bool right_rotation(const vector_t & lh, const vector_t & rh)
    {
      return rotation(lh, rh) == ROTATION_RIGHT;
    }

    
  }
}
  

#endif //ROTATION_H
