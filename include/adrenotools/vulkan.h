// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*adrenotools_vk_void_fn)(void);

/**
 * @brief Vulkan-ABI vkGetInstanceProcAddr trampoline
 * @remark If libvulkan has not already been opened by this trampoline, it opens
 * it with ADRENOTOOLS_DRIVER_CUSTOM. Lazy
 * open uses RTLD_NOW. Returns nullptr if libvulkan could not be loaded.
 */
adrenotools_vk_void_fn vkGetInstanceProcAddr(void *instance, const char *pName);

#ifdef __cplusplus
}
#endif
