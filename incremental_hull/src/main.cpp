#include <QApplication>

#include "window/window.hpp"
#include "geom/hull/hull.hpp"
#include "treap/treap.hpp"

//using namespace treap;
using namespace geom::hull;
using geom::structures::point_type;
using treap::treap_t;

hull_builder hb;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  incremental_hull_window window;
  visualization::run_viewer(&window, "ABC");
  // point_type p;
  // treap_t<point_type> tr([](const point_type & lh, const point_type & rh) {
  //     return lh.x < rh.x;
  //   });
  // while(std::cin >> p) {
  //   tr.insert(p);
  //   std::cout << tr << std::endl;
  // }
}
  
