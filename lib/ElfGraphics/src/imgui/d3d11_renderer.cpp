#include "imgui/d3d11_renderer.h"

#include "Ogre/Root.h"
#include "Ogre/Camera.h"
#include "Ogre/boost/Matrix4Traits.h"

#include <imgui_impl_dx11.h>
#include <im3d_impl_dx11.h>
#include <im3d_boost_traits.h>
#include <d3d11.h>

#include <boost/qvm/mat_operations4.hpp>

namespace ot::egfx::imgui
{
	namespace
	{
		Im3d::Mat4 get_view_projection_matrix(Ogre::Camera const* camera)
		{
			Ogre::Matrix4 const& camera_view = camera->getViewMatrix();
			const Ogre::Matrix4& projection_matrix = camera->getProjectionMatrix();
			Ogre::Matrix4 const view_projection = projection_matrix * camera_view;

			return boost::qvm::convert_to<Im3d::Mat4>(view_projection);
		}
	}

	bool d3d11_renderer::initialize()
	{
		auto& root = Ogre::Root::getSingleton();
		Ogre::RenderSystem* const renderSystem = root.getRenderSystem();

		ID3D11Device* d3dDevice = nullptr;
		renderSystem->getCustomAttribute("D3DDEVICE", &d3dDevice);
		if (d3dDevice == nullptr)
			return false;

		ID3D11DeviceContext* immediateContext;
		d3dDevice->GetImmediateContext(&immediateContext);

		if (!ImGui_ImplDX11_Init(d3dDevice, immediateContext))
			return false;

		ImGui_ImplDX11_NewFrame(); // creates the font atlas

		if (!Im3d_ImplDX11_Init(d3dDevice, immediateContext))
			return false;

		return true;
	}

	void d3d11_renderer::shutdown()
	{
		Im3d_ImplDX11_Shutdown();
		ImGui_ImplDX11_Shutdown();
	}

	void d3d11_renderer::render(Ogre::Camera const* camera)
	{
		// Not calling renderer::render because of the way Im3d text depends on ImGui
		Im3d::EndFrame();

		if (camera != nullptr)
		{
			Im3d::Mat4 const view_proj_matrix = get_view_projection_matrix(camera);
			Im3d::Vec2 const& viewport_size = Im3d::GetAppData().m_viewportSize;

			// Im3d_ImplDX11_RenderText(view_proj_matrix, viewport_size); // TODO: fix me, issue #5
			Im3d_ImplDX11_RenderDrawList(view_proj_matrix, viewport_size);
		}

		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}
