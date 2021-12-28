#include "aoc/helpers.h"
#include <vector>

namespace {
  using FFT = std::vector<char>;
  const std::vector<int> Base{ 0, 1, 0, -1 };
  
  const auto apply_phase = [](const FFT& in, FFT& out) {
    size_t op = 0;
    for (size_t o = 1; o <= in.size(); o++, op++) {
      size_t b = 1;
      size_t bi = 0;

      if (o == b) {
        b = 0;
        bi = (bi + 1) & 3;
      }

      int el = 0;
      for (size_t i = 0; i < in.size(); ++i) {
          el += in[i] * Base[bi];

          ++b;
          if (b == o) {
              b = 0;
              bi = (bi + 1) & 0x3;
          }
      }

      el = abs(el);
      out[op] = el % 10;
    }
  };

  const auto apply_phases = [](size_t ncycles, size_t nrepeats, size_t offset, const FFT& in) {

    const size_t wavelength = in.size();
    const size_t total_size = nrepeats * wavelength;

    // we will reverse the input, so we are interested in only the last part
    const size_t sub_len = total_size - offset;
    FFT out;

    while (out.size() + wavelength < sub_len) {
      std::copy(in.rbegin(), in.rend(), std::back_inserter(out));
    }
    for (auto rit = in.rbegin(); out.size() < sub_len; rit++) {
      out.push_back(*rit);
    }

    FFT next(sub_len, 0);
    for (size_t n = 0; n < ncycles; n++) {
      next.clear();

      auto it = out.begin();
      next.push_back(*(it++));
      for (; it < out.end(); it++) {
        next.push_back((next.back() + *it) % 10);
      }

      out.swap(next);
    }

    std::string result;
    for (size_t op = out.size() - 1; result.size() < 8; op--) {
      const auto c = out[op];
      result.append(1, c + '0');
    }
    return result;
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);
  std::string line;

  FFT fft;
  aoc::getline(f, line);
  fft.reserve(line.size());
  for (const auto& c : line) {
    fft.push_back(c - '0');
  }
  f.close();

  constexpr int ncycles = 100;

  size_t offset = 0;
  for (size_t i = 0; i < 7; i++) {
    offset *= 10;
    offset += fft[i];
  }

  const auto part2 = apply_phases(ncycles, 10000, offset, fft);

  // Part1 algorithm mutates the input, so do it last
  std::string part1;
  {
    FFT out;
    out.resize(fft.size());
    for (int i = 0; i < ncycles; i++) {
      apply_phase(fft, out);
      ::memcpy(&fft[0], &out[0], fft.size());
    }

    for (const auto& c : fft) {
      part1.append(1, c + '0');
      if (part1.size() == 8) { break; }    
    }
  }

  aoc::print_results(part1, part2);

  return 0;
}

