#ifndef UNGRD_OBJECT_POOL_HPP_342D7BEE3B97459E81B29FD160C4E869
#define UNGRD_OBJECT_POOL_HPP_342D7BEE3B97459E81B29FD160C4E869

#include "cxx/static_bitset.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <memory>
#include <vector>

namespace ungrd {

template <typename TObject>
class object_pool {
public:
  using object_type = TObject;

public:
  constexpr size_t count_chunks() const { return chunks_.size(); }

  constexpr size_t count_available_chunks() const {
    return available_chunks_.size();
  }

  constexpr size_t count_available_objects() const {
    size_t count = 0;
    for (auto const &chunk : chunks_)
      count += chunk->size() - chunk->count_used_objects();
    return count;
  }

public:
  TObject *acquire() {
    if (available_chunks_.empty()) {
      auto *chunk = chunks_.emplace_back(std::make_unique<chunk_type>()).get();
      available_chunks_.emplace_back(chunk);
    }

    auto &chunk = *available_chunks_.back();
    auto const [object, has_none_available] = chunk.acquire();
    if (has_none_available)
      available_chunks_.pop_back();

    return object;
  }

  void release(TObject *object) {
    for (auto &chunk : chunks_) {
      if (chunk->contains(object)) {
        if (chunk->release(object))
          available_chunks_.emplace_back(chunk.get());
        return;
      }
    }
    assert(not "object was not contained in any chunk");
  }

public:
  struct acquire_deleter {
    void operator()(TObject *object) const { pool->release(object); }

    object_pool *pool;
  };

public:
  using acquire_unique_ptr = std::unique_ptr<TObject, acquire_deleter>;

  auto acquire_unique() {
    return acquire_unique_ptr{this->acquire(), acquire_deleter{this}};
  }

  using acquire_shared_ptr = std::shared_ptr<TObject>;

  auto acquire_shared() {
    return acquire_shared_ptr{this->acquire(), acquire_deleter{this}};
  }

public:
  void clear() {
    available_chunks_.clear();
    chunks_.clear();
  }

private:
  struct chunk_type {
    static constexpr size_t chunk_size = [] {
      size_t const object_size = sizeof(TObject);
      size_t const chunk_bytes = 4096;
      return std::max<size_t>(chunk_bytes / object_size, 1);
    }();

    std::array<object_type, chunk_size> objects = {};
    static_bitset<chunk_size> used = {};

    constexpr size_t size() const { return objects.size(); }

    constexpr bool contains(TObject *object) const {
      auto const *first = objects.data();
      auto const *last = first + objects.size();
      return first <= object and object < last;
    }

    constexpr auto acquire() {
      assert(not used.all());
      size_t index = used.countr_one();
      used.set(index);
      return std::make_pair(&objects[index], used.all());
    }

    constexpr bool release(TObject *object) {
      assert(contains(object));
      bool const had_none_available = used.all();
      auto const index = std::distance(objects.data(), object);
      used.reset(index);
      return had_none_available;
    }

    constexpr size_t count_used_objects() const {
      return used.count_set_bits();
    }
  };

private:
  std::vector<std::unique_ptr<chunk_type>> chunks_;
  std::vector<chunk_type *> available_chunks_;
};

template <typename TObject>
object_pool<TObject> &get_object_pool() {
  static auto the_pool = std::make_unique<object_pool<TObject>>();
  return *the_pool;
}

template <typename TObject>
TObject *acquire_object() {
  auto *object = get_object_pool<TObject>().acquire();
  return object;
}

template <typename TObject>
void release_object(TObject *object) {
  get_object_pool<TObject>().release(object);
}

template <typename TObject>
struct acquire_deleter {
  void operator()(TObject *object) const { release_object(object); }

  friend void swap(acquire_deleter &, acquire_deleter &) noexcept {}
};

template <typename TObject>
auto acquire_unique() {
  using deleter_type = acquire_deleter<TObject>;
  return std::unique_ptr<TObject, deleter_type>{
      acquire_object<TObject>(), deleter_type{}};
}

template <typename TObject>
using acquire_unique_ptr_t = decltype(acquire_unique<TObject>());

template <typename TObject>
auto acquire_shared() {
  using deleter_type = acquire_deleter<TObject>;
  return std::shared_ptr<TObject>{acquire_object<TObject>(), deleter_type{}};
}

template <typename TObject>
using acquire_shared_ptr_t = decltype(acquire_shared<TObject>());

// ============================================================

template <typename TObject>
class new_delete_object_pool {
public:
  using object_type = TObject;

public:
  constexpr size_t count_chunks() const { return 0; }

  constexpr size_t count_available_chunks() const { return 0; }

  constexpr size_t count_available_objects() const { return 0; }

public:
  TObject *acquire() { return new TObject; }

  void release(TObject *object) { delete object; }

public:
  using acquire_deleter = std::default_delete<TObject>;

public:
  using acquire_unique_ptr = std::unique_ptr<TObject, acquire_deleter>;

  auto acquire_unique() { return acquire_unique_ptr{this->acquire()}; }

  using acquire_shared_ptr = std::shared_ptr<TObject>;

  auto acquire_shared() { return acquire_shared_ptr{this->acquire()}; }

public:
  void clear() {}
};

} // namespace ungrd

#endif // UNGRD_OBJECT_POOL_HPP_342D7BEE3B97459E81B29FD160C4E869
