#include <QApplication>

#include "window/window.hpp"
#include "geom/hull/hull.hpp"
//#include "treap/treap_node.hpp"

//using namespace treap;
using namespace geom::hull;
hull_builder hb;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  incremental_hull_window window;
  visualization::run_viewer(&window, "ABC");
}
  
