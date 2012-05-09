#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>
#include <ctime>
#include <QApplication>

#include "window/window.hpp"
#include "geom/hull/hull.hpp"
#include "treap/treap.hpp"

using namespace geom::hull;
using geom::structures::point_type;
using treap::treap_t;

struct random_point_iter {

  random_point_iter(size_t max_points = 0)
    :points(max_points)
  {}

  random_point_iter & operator++()
  {
    return *this;
  }
  
  point_type operator*()
  {
    assert(points > 0);
    point_type p(get_coord(), get_coord());
    --points;
    return p;
  }

  bool operator==(const random_point_iter & other) const
  {
    return points == other.points;
  }
  
private:
  
  static int get_coord()
  {
    static const int max_coord = 1 << 25;
    return (rand() % (max_coord * 2)) - max_coord;
  }

  size_t points;

};

bool operator!=(const random_point_iter & lh, const random_point_iter & rh)
{
  return !(lh == rh);
}

void batch(bool fake)
{
  const size_t max_points = 1000000;
  // if (fake) {
  //   auto last = random_point_iter();
  //   for (auto first = random_point_iter(max_points); first != last; ++first) {
  //     auto p = *first;
  //   }
  //   return;
  // }
  hull_builder hb;

  auto start = std::chrono::high_resolution_clock::now();
  auto last = random_point_iter();
  for (auto first = random_point_iter(max_points); first != last; ++first) {
    hb.add_point(*first);
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "points: " << max_points << std::endl
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms\n";
  if (hb.verify_hull()) {
    std::cout << "Ok!\n";
  } else {
    std::cout << "Bad :(\n";
    exit(1);
  }
}

void test_builder()
{
  // srand(time(0));
  srand(239);
  for(size_t i = 0; i < 100; i++) {
    std::cout << "Test " << (i + 1) << ": start" << std::endl;
    batch(false);
  }
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  auto args = QCoreApplication::arguments();
  if (std::find(args.begin(), args.end(), "--test") != args.end()) {
    test_builder();
    return 0;
  }
  incremental_hull_window window;
  visualization::run_viewer(&window, "Incremental Hull");
}
  
