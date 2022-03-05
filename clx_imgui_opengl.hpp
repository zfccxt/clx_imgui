#pragma once

#include <calcium.hpp>

namespace clx::imgui::opengl {

void OnBindRenderTarget(const std::shared_ptr<cl::RenderTarget>& render_target);
void Cleanup();
void BeginFrame();
void EndFrame();

}
