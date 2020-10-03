#include "im3d_impl_dx11.h"

#include "im3d_impl_dx11_shader.h" // k_ShaderBase
#include "im3d.h"
#include "im3d_math.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#include <imgui.h>

#include <cstdio>

namespace
{
	struct D3DShader
	{
		ID3DBlob* VSBlob;
		ID3D11VertexShader* VS;
		ID3DBlob* GSBlob;
		ID3D11GeometryShader* GS;
		ID3DBlob* PSBlob;
		ID3D11PixelShader* PS;
	};

	[[nodiscard]] ID3DBlob* LoadBlob(char const* shaderType, const char* target, char const* primitiveType)
	{
		const D3D_SHADER_MACRO defines[] =
		{
			shaderType, "",
			primitiveType, "",
			nullptr, nullptr
		};

		ID3DBlob* result;
		ID3DBlob* err;

		D3DCompile(k_ShaderBase, sizeof(k_ShaderBase), nullptr, defines, nullptr, "main", target, D3DCOMPILE_ENABLE_STRICTNESS, 0, &result, &err);
		if (result == nullptr)
		{
			std::printf("Im3d: error compiling shaders, cannot initialize. '%s'", static_cast<char const*>(err->GetBufferPointer()));
			err->Release();
		}

		return result;
	}

	[[nodiscard]] bool AcquireVS(D3DShader& shader, ID3D11Device* device, char const* primitiveType)
	{
		return (shader.VSBlob = LoadBlob("VERTEX_SHADER", "vs_4_0", primitiveType))
			&& device->CreateVertexShader(shader.VSBlob->GetBufferPointer(), shader.VSBlob->GetBufferSize(), nullptr, &shader.VS) == S_OK;
	}

	[[nodiscard]] bool AcquireGS(D3DShader& shader, ID3D11Device* device, char const* primitiveType)
	{
		return (shader.GSBlob = LoadBlob("GEOMETRY_SHADER", "gs_4_0", primitiveType))
			&& device->CreateGeometryShader(shader.GSBlob->GetBufferPointer(), shader.GSBlob->GetBufferSize(), nullptr, &shader.GS) == S_OK;
	}

	[[nodiscard]] bool AcquirePS(D3DShader& shader, ID3D11Device* device, char const* primitiveType)
	{
		return (shader.PSBlob = LoadBlob("PIXEL_SHADER", "ps_4_0", primitiveType))
			&& device->CreatePixelShader(shader.PSBlob->GetBufferPointer(), shader.PSBlob->GetBufferSize(), nullptr, &shader.PS) == S_OK;
	}

	[[nodiscard]] bool AcquirePoints(D3DShader& shader, ID3D11Device* device)
	{
		return AcquireVS(shader, device, "POINTS")
			&& AcquireGS(shader, device, "POINTS")
			&& AcquirePS(shader, device, "POINTS");
	}

	[[nodiscard]] bool AcquireLines(D3DShader& shader, ID3D11Device* device)
	{
		return AcquireVS(shader, device, "LINES")
			&& AcquireGS(shader, device, "LINES")
			&& AcquirePS(shader, device, "LINES");
	}

	[[nodiscard]] bool AcquireTriangles(D3DShader& shader, ID3D11Device* device)
	{
		return AcquireVS(shader, device, "TRIANGLES")
			&& AcquirePS(shader, device, "TRIANGLES");
	}

	void Release(D3DShader const& shader)
	{
		if (shader.VSBlob) shader.VSBlob->Release();
		if (shader.VS) shader.VS->Release();
		if (shader.GSBlob) shader.GSBlob->Release();
		if (shader.GS) shader.GS->Release();
		if (shader.PSBlob) shader.PSBlob->Release();
		if (shader.PS) shader.PS->Release();
	}

	D3DShader g_Im3dShaderPoints;
	D3DShader g_Im3dShaderLines;
	D3DShader g_Im3dShaderTriangles;
	ID3D11InputLayout* g_Im3dInputLayout;
	ID3D11RasterizerState* g_Im3dRasterizerState;
	ID3D11BlendState* g_Im3dBlendState;
	ID3D11DepthStencilState* g_Im3dDepthStencilState;
	ID3D11Buffer* g_Im3dConstantBuffer;
	ID3D11Buffer* g_Im3dVertexBuffer;
	Im3d::U32 g_Im3dVertexBufferCount;
	ID3D11Device* g_Im3dDevice;
	ID3D11DeviceContext* g_Im3dDeviceContext;

	struct Layout
	{
		Im3d::Mat4 ViewProj;
		Im3d::Vec2 Viewport;
		unsigned char pad[8];
	};
	static_assert(sizeof(Layout) % 16 == 0);
}

bool Im3d_ImplDX11_Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
	if (!(
		AcquirePoints(g_Im3dShaderPoints, device)
		&& AcquireLines(g_Im3dShaderLines, device)
		&& AcquireTriangles(g_Im3dShaderTriangles, device)
		))
	{
		return false;
	}

	{	
		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION_SIZE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, (UINT)offsetof(Im3d::VertexData, m_positionSize), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",         0, DXGI_FORMAT_R8G8B8A8_UNORM,       0, (UINT)offsetof(Im3d::VertexData, m_color),        D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HRESULT result = device->CreateInputLayout(desc, 2, g_Im3dShaderPoints.VSBlob->GetBufferPointer(), g_Im3dShaderPoints.VSBlob->GetBufferSize(), &g_Im3dInputLayout);
		if (result != S_OK)
		{
			std::fprintf(stderr, "Im3d: error creating input layout, could not initialize.");
			return false;
		}
	}

	{	
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE; // culling invalid for points/lines (they are view-aligned), valid but optional for triangles
		HRESULT result = device->CreateRasterizerState(&desc, &g_Im3dRasterizerState);
		if (result != S_OK)
		{
			std::fprintf(stderr, "Im3d: error creating rasterizer state, could not initialize.");
			return false;
		}
	}

	{	
		D3D11_DEPTH_STENCIL_DESC desc = {};
		HRESULT result = device->CreateDepthStencilState(&desc, &g_Im3dDepthStencilState);
		if (result != S_OK)
		{
			std::fprintf(stderr, "Im3d: error creating depth stencil state, could not initialize.");
			return false;
		}
	}

	{	
		D3D11_BLEND_DESC desc = {};
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HRESULT result = device->CreateBlendState(&desc, &g_Im3dBlendState);
		if (result != S_OK)
		{
			std::fprintf(stderr, "Im3d: error creating blend state, could not initialize.");
			return false;
		}
	}

	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(Layout);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT result = device->CreateBuffer(&desc, nullptr, &g_Im3dConstantBuffer);
		if (result != S_OK)
		{
			std::fprintf(stderr, "Im3d: error creating constant buffer, could not initialize.");
			return false;
		}
	}

	g_Im3dDevice = device;
	g_Im3dDeviceContext = context;

	return true;
}

void Im3d_ImplDX11_Shutdown()
{
	Release(g_Im3dShaderPoints);
	Release(g_Im3dShaderLines);
	Release(g_Im3dShaderTriangles);

	if (g_Im3dInputLayout)        g_Im3dInputLayout->Release();
	if (g_Im3dRasterizerState)    g_Im3dRasterizerState->Release();
	if (g_Im3dBlendState)         g_Im3dBlendState->Release();
	if (g_Im3dDepthStencilState)  g_Im3dDepthStencilState->Release();
	if (g_Im3dConstantBuffer)     g_Im3dConstantBuffer->Release();
	if (g_Im3dVertexBuffer)       g_Im3dVertexBuffer->Release();
}

namespace
{
	void InitializeContext(ID3D11DeviceContext* context, Im3d::Vec2 const& viewportSize)
	{
		D3D11_VIEWPORT viewport =
		{
			0.0f, 0.0f, // TopLeftX, TopLeftY
			(float)viewportSize.x, (float)viewportSize.y,
			0.0f, 1.0f // MinDepth, MaxDepth
		};
		context->RSSetViewports(1, &viewport);
		context->OMSetBlendState(g_Im3dBlendState, nullptr, 0xffffffff);
		context->OMSetDepthStencilState(g_Im3dDepthStencilState, 0);
		context->RSSetState(g_Im3dRasterizerState);
	}

	void UploadConstants(ID3D11DeviceContext* context, Im3d::Mat4 const& cameraViewProj, Im3d::Vec2 const& viewport)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (context->Map(g_Im3dConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) != S_OK)
		{
			IM3D_ASSERT(false);
		}

		Layout* layout = new(mappedResource.pData) Layout;
		layout->ViewProj = cameraViewProj;
		layout->Viewport = viewport;
		context->Unmap(g_Im3dConstantBuffer, 0);
	}

	void EnsureVertexCapacity(ID3D11Device* device, Im3d::U32 vertexCount)
	{
		if (g_Im3dVertexBufferCount < vertexCount)
		{
			if (g_Im3dVertexBuffer)
				g_Im3dVertexBuffer->Release();

			g_Im3dVertexBufferCount = vertexCount;

			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = g_Im3dVertexBufferCount * sizeof(Im3d::VertexData);
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			if (device->CreateBuffer(&desc, nullptr, &g_Im3dVertexBuffer) != S_OK)
			{
				IM3D_ASSERT(false);
			}
		}
	}

	void SetupPrimitive(ID3D11DeviceContext* context, Im3d::DrawPrimitiveType primType)
	{
		switch (primType)
		{
		case Im3d::DrawPrimitive_Points:
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			context->VSSetShader(g_Im3dShaderPoints.VS, nullptr, 0);
			context->GSSetShader(g_Im3dShaderPoints.GS, nullptr, 0);
			context->GSSetConstantBuffers(0, 1, &g_Im3dConstantBuffer);
			context->PSSetShader(g_Im3dShaderPoints.PS, nullptr, 0);
			break;
		case Im3d::DrawPrimitive_Lines:
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			context->VSSetShader(g_Im3dShaderLines.VS, nullptr, 0);
			context->GSSetShader(g_Im3dShaderLines.GS, nullptr, 0);
			context->GSSetConstantBuffers(0, 1, &g_Im3dConstantBuffer);
			context->PSSetShader(g_Im3dShaderLines.PS, nullptr, 0);
			break;
		case Im3d::DrawPrimitive_Triangles:
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->VSSetShader(g_Im3dShaderTriangles.VS, nullptr, 0);
			context->PSSetShader(g_Im3dShaderTriangles.PS, nullptr, 0);
			break;
		default:
			IM3D_ASSERT(false);
			return;
		};

		context->VSSetConstantBuffers(0, 1, &g_Im3dConstantBuffer);
	}
}

void Im3d_ImplDX11_RenderText(Im3d::Mat4 const& cameraViewProj, Im3d::Vec2 const& viewportSize)
{
	// Invisible ImGui window which covers the screen.
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32_BLACK_TRANS);
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2((float)viewportSize.x, (float)viewportSize.y));
	ImGui::Begin("Im3dText", nullptr, 0
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	ImDrawList* imDrawList = ImGui::GetWindowDrawList();
	Im3d::U32 const count = Im3d::GetTextDrawListCount();
	for (Im3d::U32 i = 0; i < count; ++i)
	{
		const Im3d::TextDrawList& textDrawList = Im3d::GetTextDrawLists()[i];

		for (Im3d::U32 j = 0; j < textDrawList.m_textDataCount; ++j)
		{
			const Im3d::TextData& textData = textDrawList.m_textData[j];
			if (textData.m_positionSize.w == 0.0f || textData.m_color.getA() == 0.0f)
			{
				continue;
			}

			// Project world -> screen space.
			Im3d::Vec4 clip = cameraViewProj * Im3d::Vec4(textData.m_positionSize.x, textData.m_positionSize.y, textData.m_positionSize.z, 1.0f);
			Im3d::Vec2 screen = Im3d::Vec2(clip.x / clip.w, clip.y / clip.w);

			// Cull text which falls offscreen. Note that this doesn't take into account text size but works well enough in practice.
			if (clip.w < 0.0f || screen.x >= 1.0f || screen.y >= 1.0f)
			{
				continue;
			}

			// Pixel coordinates for the ImGuiWindow ImGui.
			screen = screen * Im3d::Vec2(0.5f) + Im3d::Vec2(0.5f);
			screen.y = 1.0f - screen.y; // screen space origin is reversed by the projection.
			screen = screen * (Im3d::Vec2)ImGui::GetWindowSize();

			// All text data is stored in a single buffer; each textData instance has an offset into this buffer.
			const char* text = textDrawList.m_textBuffer + textData.m_textBufferOffset;

			// Calculate the final text size in pixels to apply alignment flags correctly.
			ImGui::SetWindowFontScale(textData.m_positionSize.w); // NB no CalcTextSize API which takes a font/size directly...
			Im3d::Vec2 textSize = ImGui::CalcTextSize(text, text + textData.m_textLength);
			ImGui::SetWindowFontScale(1.0f);

			// Generate a pixel offset based on text flags.
			Im3d::Vec2 textOffset = Im3d::Vec2(-textSize.x * 0.5f, -textSize.y * 0.5f); // default to center
			if ((textData.m_flags & Im3d::TextFlags_AlignLeft) != 0)
			{
				textOffset.x = -textSize.x;
			} 
			else if ((textData.m_flags & Im3d::TextFlags_AlignRight) != 0)
			{
				textOffset.x = 0.0f;
			}

			if ((textData.m_flags & Im3d::TextFlags_AlignTop) != 0)
			{
				textOffset.y = -textSize.y;
			} 
			else if ((textData.m_flags & Im3d::TextFlags_AlignBottom) != 0)
			{
				textOffset.y = 0.0f;
			}

			// Add text to the window draw list.
			screen = screen + textOffset;
			imDrawList->AddText(nullptr, textData.m_positionSize.w * ImGui::GetFontSize(), screen, textData.m_color.getABGR(), text, text + textData.m_textLength);
		}
	}

	ImGui::End();
	ImGui::PopStyleColor(1);
}

void Im3d_ImplDX11_RenderDrawList(Im3d::Mat4 const& cameraViewProj, Im3d::Vec2 const& viewportSize)
{
	auto const device = g_Im3dDevice;
	auto const context = g_Im3dDeviceContext;

	InitializeContext(context, viewportSize);

	UploadConstants(context, cameraViewProj, viewportSize);

	for (Im3d::U32 i = 0, n = Im3d::GetDrawListCount(); i < n; ++i)
	{
		Im3d::DrawList const& drawList = Im3d::GetDrawLists()[i];

		EnsureVertexCapacity(device, drawList.m_vertexCount);

		// upload vertex data
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if (context->Map(g_Im3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) != S_OK)
			{
				IM3D_ASSERT(false);
			}
			memcpy(mappedResource.pData, drawList.m_vertexData, drawList.m_vertexCount * sizeof(Im3d::VertexData));
			context->Unmap(g_Im3dVertexBuffer, 0);
		}

		SetupPrimitive(context, drawList.m_primType);

		UINT stride = sizeof(Im3d::VertexData);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &g_Im3dVertexBuffer, &stride, &offset);
		context->IASetInputLayout(g_Im3dInputLayout);
		context->Draw(drawList.m_vertexCount, 0);

		context->VSSetShader(nullptr, nullptr, 0);
		context->GSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);
	}	
}