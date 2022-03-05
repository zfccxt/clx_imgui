#include "clx_imgui_vulkan.hpp"

#include <algorithm>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <vulkan/vulkan_command_buffer_utils.hpp>
#include <vulkan/vulkan_window.hpp>

#ifndef NDEBUG
  #define VK_CHECK(x) if ((x != VK_SUCCESS)) { assert(false); }
#else
  #define VK_CHECK(x) x
#endif

namespace clx::imgui::vulkan {

#pragma warning(push)
#pragma warning(disable : 26812)

VkAllocationCallbacks* allocator_;
VkDescriptorPool descriptor_pool_;
VkDevice device_;
VkQueue queue_;
// TODO: change to a weak ptr and do checks to see if the window was destroyed?
std::shared_ptr<cl::vulkan::VulkanWindow> vulkan_window_;

void OnBindRenderTarget(const std::shared_ptr<cl::RenderTarget>& render_target) {
  auto vulkan_window = std::dynamic_pointer_cast<cl::vulkan::VulkanWindow>(render_target);
  vulkan_window_ = vulkan_window;
  allocator_ = vulkan_window->GetAllocator();
  device_ = vulkan_window->GetDevice();
  queue_ = vulkan_window->GetGraphicsQueue();

  vkDeviceWaitIdle(device_);

  // I have absolutely no idea why imgui doesn't just allocate its own descriptor pool under the hood
  VkDescriptorPoolSize pool_sizes[] = {
    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
  };
  VkDescriptorPoolCreateInfo pool_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
  pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  VK_CHECK(vkCreateDescriptorPool(device_, &pool_info, allocator_, &descriptor_pool_));

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();

  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplGlfw_InitForVulkan(vulkan_window->GetGlfwWindow(), true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vulkan_window->GetInstance();
  init_info.PhysicalDevice = vulkan_window->GetPhysicalDevice();
  init_info.Device = device_;
  init_info.QueueFamily = vulkan_window->GetGraphicsQueueFamily();
  init_info.Queue = queue_;
  // TODO: Why is this an option? I think this should just be VK_NULL_HANDLE forever
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = descriptor_pool_;
  init_info.Subpass = 0;
  init_info.MinImageCount = std::max(vulkan_window->GetMinImageCount(), 2U);
  init_info.ImageCount = vulkan_window->GetImageCount();
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = allocator_;
  init_info.CheckVkResultFn = [](VkResult err){ VK_CHECK(err); };
  ImGui_ImplVulkan_Init(&init_info, vulkan_window->GetRenderPass());

  VkCommandBuffer command_buffer = cl::vulkan::BeginSingleUseCommandBuffer(vulkan_window->GetContextData());
  ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
  cl::vulkan::EndAndSubmitSingleUseCommandBuffer(vulkan_window->GetContextData(), command_buffer);

  VK_CHECK(vkDeviceWaitIdle(device_));
  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Cleanup() {
  VK_CHECK(vkDeviceWaitIdle(device_));

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  vkDestroyDescriptorPool(device_, descriptor_pool_, allocator_);
}

void BeginFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void EndFrame() {
  ImGui::Render();
  ImDrawData* main_draw_data = ImGui::GetDrawData();
  const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
  if (!main_is_minimized) {
    ImGui_ImplVulkan_RenderDrawData(main_draw_data, vulkan_window_->GetCurrentRenderCommandBuffer());
  }
  
  ImGuiIO& io = ImGui::GetIO(); 
  // Update and Render additional Platform Windows
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }
}

#pragma warning(pop)

}
