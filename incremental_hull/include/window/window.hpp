#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <iterator>

#include <QColor>

#include "visualization/viewer_adapter.h"
#include "geom/primitives/point.h"
#include "geom/hull/hull.hpp"

using visualization::viewer_adapter;
using visualization::drawer_type;

using geom::structures::point_type;
using geom::hull::hull_builder;

struct incremental_hull_window : viewer_adapter {

  void draw(drawer_type & drawer) const;

  bool on_double_click(const point_type & pt);
  bool on_key(int key);

private:
  std::vector<point_type>  points;
  std::vector<point_type>  hull;
  hull_builder builder;

};

void incremental_hull_window::draw(drawer_type & drawer) const
{
  drawer.set_color(Qt::blue);
  for (const auto & p : points) {
    drawer.draw_point(p, 3);
  }
  if (hull.size()) {
    drawer.set_color(Qt::red);
    point_type prev = hull.back();
    for (const auto & p : hull) {
      drawer.draw_line(p, prev);
      prev = p;
    }
  }
}

bool incremental_hull_window::on_double_click(const point_type & pt)
{
  points.push_back(pt);
  builder.add_point(pt);
  hull.clear();
  builder.get_current_hull(back_inserter(hull));
  std::clog << "Hull: " << std::endl;
  for(const auto & p : hull) {
    std::clog << p << ' ';
  }
  std::clog << std::endl;
  return true;
}
  
bool incremental_hull_window::on_key(int key)
{
  return true;
}

#endif //WINDOW_H
