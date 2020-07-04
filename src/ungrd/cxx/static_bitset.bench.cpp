#include <benchmark/benchmark.h>

#include <bitset>

#include "static_bitset.hpp"

#define GENERATE_BENCHMARKS(BM_)                                               \
  BENCHMARK_TEMPLATE(BM_, 16)->Range(1, 16);                                   \
  BENCHMARK_TEMPLATE(BM_, 32)->Range(1, 32);                                   \
  BENCHMARK_TEMPLATE(BM_, 64)->Range(1, 64);                                   \
  BENCHMARK_TEMPLATE(BM_, 128)->Range(1, 128);                                 \
  BENCHMARK_TEMPLATE(BM_, 256)->Range(1, 256);                                 \
  BENCHMARK_TEMPLATE(BM_, 4096)->Range(1, 4096);

// Any_FirstBitSet

template <size_t NBits>
static void BM_StdBitset_Any_OneBitSet(benchmark::State &state) {
  std::bitset<NBits> bits;

  bits.set(state.range(0) - 1);

  for (auto _ : state) {
    for (size_t count = 0; count < 1000; ++count)
      benchmark::DoNotOptimize(bits.any());
  }
}
GENERATE_BENCHMARKS(BM_StdBitset_Any_OneBitSet)

template <size_t NBits>
static void BM_StaticBitset_Any_OneBitSet(benchmark::State &state) {
  ungrd::static_bitset<NBits> bits;

  bits.set(state.range(0) - 1);

  for (auto _ : state) {
    for (size_t count = 0; count < 1000; ++count)
      benchmark::DoNotOptimize(bits.any());
  }
}
GENERATE_BENCHMARKS(BM_StaticBitset_Any_OneBitSet)

// SetAndGet

template <size_t NBits>
static void BM_StdBitset_SetAndGet(benchmark::State &state) {
  std::bitset<NBits> bits;

  for (auto _ : state) {
    for (size_t index = 0; index < state.range(0); ++index) {
      bits.set(0);
      benchmark::DoNotOptimize(std::as_const(bits)[0]);
    }
  }
}
GENERATE_BENCHMARKS(BM_StdBitset_SetAndGet)

template <size_t NBits>
static void BM_StaticBitset_SetAndGet(benchmark::State &state) {
  ungrd::static_bitset<NBits> bits;

  for (auto _ : state) {
    for (size_t index = 0; index < state.range(0); ++index) {
      bits.set(0);
      benchmark::DoNotOptimize(bits.get(0));
    }
  }
}
GENERATE_BENCHMARKS(BM_StaticBitset_SetAndGet)
