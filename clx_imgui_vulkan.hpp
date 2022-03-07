#pragma once

#include <calcium.hpp>

namespace clx::imgui::vulkan {

void OnBindRenderTarget(const std::shared_ptr<cl::RenderTarget>& render_target);
void Cleanup();
void Begin();
void End();

}
