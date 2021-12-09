#include "aoc/helpers.h"

#include <vector>
#include <set>
#include <map>

namespace {

using Segment = std::pair<char, int64_t>;
using Path = std::vector<Segment>;
using Point = std::pair<int64_t, int64_t>;
using Grid = std::set<Point>;
using DistanceGrid = std::map<Point, int64_t>;

static std::map<char, Point> dirs = {
    { 'R', {1,0} },
    { 'L', {-1,0} },
    { 'U', {0,1} },
    { 'D', {0,-1} },
};

class Wire
{
public:
    Wire(const std::string& line) {
        std::stringstream ss(line);

        std::string segment;
        while (std::getline(ss, segment, ',')) {
            std::string_view s(segment.data(), segment.length());
            const auto num = s.substr(1);
            int64_t v = 0;
            for (size_t i = 0; i < num.length(); i++) {
                const auto c = num[i];
                assert(c >= '0' && c <= '9');
                v *= 10;
                v += (c - '0');
            }
            path.emplace_back(s[0], v);
        }
    }

    Grid walk_path() {
        Grid g;
        int64_t x = 0;
        int64_t y = 0;
        for (const auto p : path) {
            // Get the direction we will be walking
            const auto d = dirs[p.first];

            for (auto i = p.second; i > 0; i--) {
                x += d.first;
                y += d.second;
                g.emplace(x, y);
            }
        }

        return g;
    }

    DistanceGrid walk_path_with_distance() {
        DistanceGrid g;
        int64_t x = 0;
        int64_t y = 0;
        int64_t steps = 0;
        for (const auto p : path) {
            // Get the direction we will be walking
            const auto d = dirs[p.first];

            for (auto i = p.second; i > 0; i--) {
                x += d.first;
                y += d.second;
                g.insert({{x, y}, ++steps});
            }
        }

        return g;
    }

    int64_t intersect_path(const Grid& g) {
        int64_t x = 0;
        int64_t y = 0;
        int64_t min = INT64_MAX;
        for (const auto p : path) {
            // Get the direction we will be walking
            Point d = dirs[p.first];

            for (auto i = p.second; i > 0; i--) {
                x += d.first;
                y += d.second;
                Point pt = {x, y};
                if (g.find(pt) != g.end()) {
                    min = std::min(min, ::abs(x) + ::abs(y));
                }
            }
        }

        return min;
    }

    int64_t intersect_path(const DistanceGrid& g) {
        int64_t x = 0;
        int64_t y = 0;
        int64_t min = INT64_MAX;
        int64_t steps = 0;
        for (const auto p : path) {
            // Get the direction we will be walking
            Point d = dirs[p.first];

            for (auto i = p.second; i > 0; i--) {
                x += d.first;
                y += d.second;
                Point pt = {x, y};
                steps++;
                const auto opt = g.find(pt);
                if (opt != g.end()) {
                    min = std::min(min, opt->second + steps);
                }
            }
        }

        return min;
    }
private:

    Path path;
};

};

int main(int argc, char **argv) {

    aoc::AutoTimer t;

    auto in = aoc::open_argv_1(argc, argv);

    std::string wire;

    std::getline(in, wire);
    Wire w1(wire);
    std::getline(in, wire);
    Wire w2(wire);

    auto g = w1.walk_path();
    auto r = w2.intersect_path(g);

    std::cout << "Part 1: " << r << std::endl;

    auto d = w1.walk_path_with_distance();
    r = w2.intersect_path(d);

    std::cout << "Part 2: " << r << std::endl;

    return 0;
}
