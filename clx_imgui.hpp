#pragma once

// TODO: Allow using this extension with multiple contexts at once

#include <calcium.hpp>
#include <imgui.h>

namespace clx::imgui {

cl::ExtensionDetails LoadExtension();

void BeginFrame();
void EndFrame();

}
