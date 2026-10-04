// SPDX-License-Identifier: BSD-2-Clause

#include <adrenotools/driver.h>
#include <adrenotools/vulkan.h>
#include <dlfcn.h>
#include <string>

namespace {
using PFN_vkGetInstanceProcAddr = adrenotools_vk_void_fn (*)(void *,
                                                             const char *);

void *g_libvulkan{};
PFN_vkGetInstanceProcAddr g_realGetInstanceProcAddr{};

// Returns the directory containing this shared object, which matches the app's
// nativeLibraryDir where the hook libraries are packaged. Empty on failure.
std::string self_lib_dir() {
  Dl_info info{};
  if (!dladdr(reinterpret_cast<const void *>(&self_lib_dir), &info) ||
      !info.dli_fname)
    return {};

  std::string path{info.dli_fname};
  auto slash{path.find_last_of('/')};
  return slash == std::string::npos ? std::string{} : path.substr(0, slash);
}

void ensure_libvulkan() {
  if (g_libvulkan)
    return;

  static const std::string hookLibDir{self_lib_dir()};
  if (hookLibDir.empty())
    return;

  g_libvulkan =
      adrenotools_open_libvulkan(RTLD_NOW, 0, nullptr, hookLibDir.c_str(),
                                 nullptr, nullptr, nullptr, nullptr);
  if (!g_libvulkan)
    return;

  adrenotools_set_turbo(true);

  g_realGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
      dlsym(g_libvulkan, "vkGetInstanceProcAddr"));
}
} // namespace

adrenotools_vk_void_fn vkGetInstanceProcAddr(void *instance,
                                             const char *pName) {
  ensure_libvulkan();
  if (!g_realGetInstanceProcAddr)
    return nullptr;

  return g_realGetInstanceProcAddr(instance, pName);
}
