#include "aoc/helpers.h"
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <memory>

namespace {
  class Node;
  using SharedNode = std::shared_ptr<Node>;
  using Children = std::vector<SharedNode>;
  using NodeMap = std::map<std::string, SharedNode, std::less<>>;

  using NodeSet = std::set<SharedNode>;
  using NodeQueue = std::deque<SharedNode>;
  using NodeDistanceMap = std::map<SharedNode, size_t>;

  class Node {
    public:
      Node(const std::string_view n)
        :name(n.data(), n.size())
         { }

      Children children;

      const std::string& get_name() const {
        return name;
      }

    friend std::ostream& operator<<(std::ostream& os, const Node& comp) {
      os << comp.name;
      
      if (!comp.children.empty()) {
        os << " [ ";
        for (const auto c : comp.children) {
          os << *c;
        }
        os << " ] ";
      }

      return os;
    }
    
    protected:
      const std::string name;
  };

  size_t count_orbits(const SharedNode& root, size_t d) {
    size_t count = d;

    for (const auto& n : root->children) {
      count += count_orbits(n, d + 1);
    }

    return count;
  }

  size_t get_distance(SharedNode from, SharedNode to) {
    NodeSet visited;
    NodeDistanceMap distance_map;
    NodeQueue search_queue;

    distance_map.insert(std::pair(from, 0)); // From our self to our self is 0
    search_queue.push_back(from);
    visited.insert(from);

    while (!search_queue.empty()) {
      auto n = search_queue.front();
      search_queue.pop_front();

      DEBUG(std::cout << "Checking " << n->get_name() << std::endl);
      const auto n_dist = distance_map[n];
      if (n == to) {
        // Tada!
        return n_dist;
      }

      for (const auto c : n->children) {
        auto r = visited.insert(c);
        // Already seen
        if (!r.second) {
          continue;
        }

        distance_map.insert(std::pair(c, n_dist + 1));
        DEBUG(std::cout << "Pushing " << c->get_name() << std::endl);
        search_queue.push_back(c);
      }
    }

    throw std::runtime_error("Node not found");
  }

  constexpr std::string_view COM_NODE("COM");
  constexpr std::string_view YOU("YOU");
  constexpr std::string_view SAN("SAN");
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  NodeMap m;
  NodeMap m2;
  std::string s;
  while (aoc::getline(f, s)) {
    const std::string_view sv(s);
    const auto p = sv.find(')');
    assert(p != std::string_view::npos);
    const auto com = sv.substr(0, p);
    const auto satelite = sv.substr(p + 1);

    SharedNode com_node = std::make_shared<Node>(com);
    SharedNode com_node2 = std::make_shared<Node>(com);
    auto r = m.emplace(com, com_node);
    if (!r.second) {
      com_node = r.first->second;
    }
    r = m2.emplace(com, com_node2);
    if (!r.second) {
      com_node2 = r.first->second;
    }

    SharedNode satelite_node = std::make_shared<Node>(satelite);
    SharedNode satelite_node2 = std::make_shared<Node>(satelite);
    r = m.emplace(satelite, satelite_node);
    if (!r.second) {
      satelite_node = r.first->second;
    }
    r = m2.emplace(satelite, satelite_node2);
    if (!r.second) {
      satelite_node2 = r.first->second;
    }
    
    com_node->children.push_back(satelite_node);

    com_node2->children.push_back(satelite_node2);
    satelite_node2->children.push_back(com_node2);
  }

  const auto com_root = m.find(COM_NODE);
  assert (com_root != m.end());
  std::cout << "Part 1: " << count_orbits(com_root->second, 0) << std::endl;

  const auto you = m2.find(YOU);
  assert (you != m2.end());
  const auto san = m2.find(SAN);
  assert (san != m2.end());
  std::cout << "Part 2: " << get_distance(you->second, san->second) - 2 << std::endl;

  return 0;
}

