#include "imgui/renderer.h"

#include "Ogre/Camera.h"
#include "Ogre/boost/Matrix4Traits.h"

#include <imgui.h>
#include <im3d.h>
#include <im3d_boost_traits.h>

#include <cmath>

#include <boost/qvm/mat_operations4.hpp>

namespace ot::egfx::imgui
{
	renderer::~renderer() = default;

	// Assumes the ImGui Platform's new frame has already handled io.DisplaySize
	void renderer::pre_update(Ogre::Camera const* camera)
	{
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();

		Im3d::AppData& ad = Im3d::GetAppData();
		ad.m_deltaTime = io.DeltaTime;
		ad.m_viewportSize = io.DisplaySize;
		ad.m_viewOrigin = camera->getRealPosition();
		ad.m_viewDirection = camera->getDirection();
		ad.m_worldUp = Im3d::Vec3(0.f, 1.f, 0.f);
		ad.m_projOrtho = false;

		ad.m_projScaleY = std::tan(camera->getFOVy().valueRadians() * 0.5f) * 2.0f;

		ad.setCullFrustum(boost::qvm::convert_to<Im3d::Mat4>(camera->getProjectionMatrix()), true);

		Im3d::NewFrame();
	}

	void renderer::render(Ogre::Camera const* camera)
	{
		(void)camera;
		Im3d::EndFrame();
		ImGui::Render();
	}
}
