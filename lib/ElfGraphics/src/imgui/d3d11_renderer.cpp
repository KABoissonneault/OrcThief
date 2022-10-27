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

		d3d_device = nullptr;
		renderSystem->getCustomAttribute("D3DDEVICE", &d3d_device);
		if (d3d_device == nullptr)
			return false;

		ID3D11DeviceContext* immediate_context;
		d3d_device->GetImmediateContext(&immediate_context);

		if (!ImGui_ImplDX11_Init(d3d_device, immediate_context))
			return false;

		ImGui_ImplDX11_NewFrame(); // creates the font atlas

		if (!Im3d_ImplDX11_Init(d3d_device, immediate_context))
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

			Im3d_ImplDX11_RenderText(view_proj_matrix, viewport_size);
			Im3d_ImplDX11_RenderDrawList(view_proj_matrix, viewport_size);
		}

		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	bool d3d11_renderer::load_texture(std::span<unsigned char> image_rgba_data, int pitch, texture& t)
	{
		t.width = pitch / 4;
		t.height = (int)image_rgba_data.size_bytes() / pitch;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = t.width;
		desc.Height = t.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;


		D3D11_SUBRESOURCE_DATA sub_resource;
		sub_resource.pSysMem = image_rgba_data.data();
		sub_resource.SysMemPitch = pitch;
		sub_resource.SysMemSlicePitch = 0;

		ID3D11Texture2D* texture_ptr;
		HRESULT result = d3d_device->CreateTexture2D(&desc, &sub_resource, &texture_ptr);
		if (result != S_OK)
		{
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		ZeroMemory(&srv_desc, sizeof(srv_desc));
		srv_desc.Format = desc.Format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = desc.MipLevels;
		srv_desc.Texture2D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* srv_ptr;
		result = d3d_device->CreateShaderResourceView(texture_ptr, &srv_desc, &srv_ptr);
		texture_ptr->Release();
		if (result != S_OK)
		{
			return false;
		}

		t.texture_id = srv_ptr;
		t.renderer_system = this;
		return true;
	}

	void d3d11_renderer::free_texture(texture& t)
	{
		if (t.texture_id == nullptr)
			return;

		static_cast<ID3D11ShaderResourceView*>(t.texture_id)->Release();
	}
}
