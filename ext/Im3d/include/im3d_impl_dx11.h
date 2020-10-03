#pragma once

#include "im3d.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

bool Im3d_ImplDX11_Init(ID3D11Device* device, ID3D11DeviceContext* context);
void Im3d_ImplDX11_Shutdown();

// Currently uses Dear ImGui in the implementation, so while it must be called after Im3d::EndFrame, it must also be called before the ImGui::EndFrame
void Im3d_ImplDX11_RenderText(Im3d::Mat4 const& cameraViewProj, Im3d::Vec2 const& viewportSize);

// Called after Im3d::EndFrame
void Im3d_ImplDX11_RenderDrawList(Im3d::Mat4 const& cameraViewProj, Im3d::Vec2 const& viewportSize);
