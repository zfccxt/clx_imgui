#include "clx_imgui.hpp"

#include <calcium.hpp>
#include <imgui.h>

#include "clx_imgui_opengl.hpp"
#include "clx_imgui_vulkan.hpp"

namespace clx::imgui {

cl::Backend backend_;

void Init(std::shared_ptr<cl::Context> context) {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags  |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags  |= ImGuiConfigFlags_DockingEnable;

  backend_ = context->GetBackend();
}

void BindRenderTarget(const std::shared_ptr<cl::RenderTarget>& render_target) {
  switch (backend_) {
    case cl::Backend::kOpenGL:
      clx::imgui::opengl::OnBindRenderTarget(render_target);
      break;

    case cl::Backend::kVulkan:
      clx::imgui::vulkan::OnBindRenderTarget(render_target); 
      break;
  }
}

void Cleanup() {
  ImGui::DestroyContext();
}

void BeginFrame() {
  switch (backend_) {
    case cl::Backend::kOpenGL:
      clx::imgui::opengl::BeginFrame();
      break;

    case cl::Backend::kVulkan:
      clx::imgui::vulkan::BeginFrame();
      break;
  }
}

void EndFrame() {
  switch (backend_) {
    case cl::Backend::kOpenGL:
      clx::imgui::opengl::EndFrame();
      break;

    case cl::Backend::kVulkan:
      clx::imgui::vulkan::EndFrame();
      break;
  }
}

cl::ExtensionDetails LoadExtension() {
  cl::ExtensionDetails extension_details;
  extension_details.on_create = Init;
  extension_details.on_destroy = Cleanup;
  extension_details.on_bind_render_target = BindRenderTarget;
  return extension_details;
}

}
