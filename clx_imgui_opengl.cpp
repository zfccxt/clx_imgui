#include "clx_imgui_opengl.hpp"

#include <calcium.hpp>
#include <opengl/opengl_window.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace clx::imgui::opengl {

GLFWwindow* bound_window_;

void OnBindRenderTarget(const std::shared_ptr<cl::RenderTarget>& render_target) {
  auto window = std::dynamic_pointer_cast<cl::opengl::OpenGLWindow>(render_target);
  bound_window_ = window->GetGlfwWindow();
  ImGui_ImplGlfw_InitForOpenGL(bound_window_, true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

void Cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Begin() {
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
    glfwMakeContextCurrent(bound_window_);
  }
}

}
