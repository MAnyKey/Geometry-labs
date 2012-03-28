#ifndef ROTATION_H
#define ROTATION_H

#include "geom/primitives/vector.h"

namespace geom
{
  namespace rotation
  {
    using namespace structures;

    
    enum rotation_t
    {
      ROTATION_RIGHT = -1,
      ROTATION_NONE = 0, 
      ROTATION_LEFT = 1
    };

    inline rotation_t invert_rotation(const rotation_t & r)
    {
      switch (r) {
      case ROTATION_RIGHT:
        return ROTATION_LEFT;
      case ROTATION_LEFT:
        return ROTATION_RIGHT;
      default:
        return r;
      }
    }

    inline rotation_t rotation(const vector_type & lh, const vector_type & rh)
    {
      //int64_t cross = cross_product(lh, rh);
      int64_t cross = lh ^ rh;
      if (0 < cross)
        return ROTATION_LEFT;
      else if (0 > cross)
        return ROTATION_RIGHT;
      return ROTATION_NONE;
    }

    inline rotation_t rotation(const point_type & p1, const point_type & p2, const point_type & p3)
    {
      // return rotation(vector_t(p1, p2),vector_t(p2, p3));
      return rotation(p2 - p1, p3 - p2);
    }

    inline bool left_rotation(const vector_type & lh, const vector_type & rh)
    {
      return rotation(lh, rh) == ROTATION_LEFT;
    }

    inline bool right_rotation(const vector_type & lh, const vector_type & rh)
    {
      return rotation(lh, rh) == ROTATION_RIGHT;
    }

    
  }
}
  

#endif //ROTATION_H
