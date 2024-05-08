#ifndef VOIDJS_GC_GC_BASE_H
#define VOIDJS_GC_GC_BASE_H

#include <cstdint>
#include <cstddef>

namespace voidjs {
namespace gc {

template <typename Derived>
class GCBase {
 public:
  std::uintptr_t Allocate(std::size_t size) {
    AsDerived()->Allocate(size);
  }

  void Collect() {
    AsDerived()->Collect();
  }

 private:
  Derived* AsDerived() {
    return static_cast<Derived*>(this);
  }
};

}  // namespace gc
}  // namespace voidjs


#endif  // VOIDJS_GC_GC_BASE_H
