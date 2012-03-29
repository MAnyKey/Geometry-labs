#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <iterator>
#include <fstream>

#include <QColor>

#include "visualization/viewer_adapter.h"
#include "geom/primitives/point.h"
#include "geom/hull/hull.hpp"

using visualization::viewer_adapter;
using visualization::drawer_type;

using geom::structures::point_type;
using geom::hull::hull_builder;

struct incremental_hull_window : viewer_adapter {

  incremental_hull_window()
  {
    auto args = QCoreApplication::arguments();
    
    for (int i = 0; i < args.size(); ++i) {
      if (args[i] == "-f") {
        ++i;
        std::ifstream fin(args[i].toStdString(), std::ifstream::in);
        if (fin) {
          point_type p;
          while (fin >> p) {
            points.push_back(p);
            builder.add_point(p);
          }
          builder.get_current_hull(std::back_inserter(hull));
          std::cout << "Hell World!\n";
        }
      }
    }
  }

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
    drawer.draw_point(p, 4);
  }
  if (hull.size()) {
    
    point_type prev = hull.back();
    for (const auto & p : hull) {
      drawer.set_color(Qt::red);
      drawer.draw_line(p, prev);
      drawer.set_color(Qt::yellow);
      drawer.draw_point(p, 4);
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
  #ifndef NDEBUG
std::clog << "Hull: " << std::endl;
#endif // NDEBUG
  for(const auto & p : hull) {
    #ifndef NDEBUG
std::clog << p << ' ';
#endif // NDEBUG
  }
  #ifndef NDEBUG
std::clog << std::endl;
#endif // NDEBUG
  return true;
}
  
bool incremental_hull_window::on_key(int key)
{
  return true;
}

#endif //WINDOW_H
