#include <benchmark/benchmark.h>

#include "object_pool.hpp"

#include <array>
#include <numeric>

#define GENERATE_BENCHMARKS(BM_)                                               \
  BENCHMARK_TEMPLATE(BM_, 1)->Range(1, 1);                                     \
  BENCHMARK_TEMPLATE(BM_, 2)->Range(1, 2);                                     \
  BENCHMARK_TEMPLATE(BM_, 3)->Range(1, 3);                                     \
  BENCHMARK_TEMPLATE(BM_, 4)->Range(1, 4);                                     \
  BENCHMARK_TEMPLATE(BM_, 8)->Range(1, 8);                                     \
  BENCHMARK_TEMPLATE(BM_, 1024)->Range(1, 16);                                 \
  BENCHMARK_TEMPLATE(BM_, 4096)->Range(1, 4096);                               \
  BENCHMARK_TEMPLATE(BM_, 8192)->Range(1, 8192);

template <typename T, size_t N>
struct iota_array {
  iota_array() { std::iota(data.begin(), data.end(), 0); }

  std::array<T, N> data;
};

// AcquireOneReleaseOne

template <size_t NBytes>
static void BM_ObjectPool_NewOneDeleteOne(benchmark::State &state) {
  using object_type = iota_array<char, NBytes>;

  for (auto _ : state) {
    auto *object = new object_type;
    benchmark::DoNotOptimize(object->data);
    delete object;
  }
}
GENERATE_BENCHMARKS(BM_ObjectPool_NewOneDeleteOne)

template <size_t NBytes>
static void BM_ObjectPool_AcquireOneReleaseOne(benchmark::State &state) {
  using object_type = iota_array<char, NBytes>;

  {
    auto &pool = ungrd::get_object_pool<object_type>();
    pool.clear();
    state.counters["cb"] = pool.count_chunks();
    state.counters["acb"] = pool.count_available_chunks();
    state.counters["aob"] = pool.count_available_objects();
  }

  for (auto _ : state) {
    auto *object = ungrd::acquire_object<object_type>();
    new (object) object_type;
    benchmark::DoNotOptimize(object->data);
    object->~object_type();
    ungrd::release_object<object_type>(object);
  }

  {
    auto &pool = ungrd::get_object_pool<object_type>();
    state.counters["ca"] = pool.count_chunks();
    state.counters["aca"] = pool.count_available_chunks();
    state.counters["aoa"] = pool.count_available_objects();
  }
}
GENERATE_BENCHMARKS(BM_ObjectPool_AcquireOneReleaseOne)

// AcquireManyReleaseMany

template <size_t NBytes>
static void BM_ObjectPool_NewManyDeleteMany(benchmark::State &state) {
  using object_type = iota_array<char, NBytes>;

  std::vector<object_type *> objects(state.range(0));

  for (auto _ : state) {
    for (size_t index = 0; index < objects.size(); ++index)
      objects[index] = new object_type;

    benchmark::DoNotOptimize(objects);

    for (size_t index = 0; index < objects.size(); ++index)
      delete objects[index];
  }
}
GENERATE_BENCHMARKS(BM_ObjectPool_NewManyDeleteMany)

template <size_t NBytes>
static void BM_ObjectPool_AcquireManyReleaseMany(benchmark::State &state) {
  using object_type = iota_array<char, NBytes>;

  {
    auto &pool = ungrd::get_object_pool<object_type>();
    pool.clear();
    state.counters["cb"] = pool.count_chunks();
    state.counters["acb"] = pool.count_available_chunks();
    state.counters["aob"] = pool.count_available_objects();
  }

  std::vector<object_type *> objects(state.range(0));

  for (auto _ : state) {
    for (size_t index = 0; index < objects.size(); ++index) {
      objects[index] = ungrd::acquire_object<object_type>();
      new (objects[index]) object_type;
    }

    benchmark::DoNotOptimize(objects);

    for (size_t index = 0; index < objects.size(); ++index) {
      objects[index]->~object_type();
      ungrd::release_object<object_type>(objects[index]);
    }
  }

  {
    auto &pool = ungrd::get_object_pool<object_type>();
    state.counters["ca"] = pool.count_chunks();
    state.counters["aca"] = pool.count_available_chunks();
    state.counters["aoa"] = pool.count_available_objects();
  }
}
GENERATE_BENCHMARKS(BM_ObjectPool_AcquireManyReleaseMany)
