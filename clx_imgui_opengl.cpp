#include "clx_imgui_opengl.hpp"

#include <calcium.hpp>
#include <opengl/opengl_window.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace clx::imgui::opengl {

cl::opengl::OpenGLWindow* bound_window_;
bool prev_depth_test_enable_;

void OnBindRenderTarget(const std::shared_ptr<cl::RenderTarget>& render_target) {
  bound_window_ = (cl::opengl::OpenGLWindow*)render_target.get();
  ImGui_ImplGlfw_InitForOpenGL(bound_window_->GetGlfwWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

void Cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Begin() {
  prev_depth_test_enable_ = bound_window_->IsDepthTestEnabled();
  bound_window_->SetDepthTestEnable(false);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void End() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(bound_window_->GetGlfwWindow());
  }

  bound_window_->SetDepthTestEnable(prev_depth_test_enable_);
}

}
