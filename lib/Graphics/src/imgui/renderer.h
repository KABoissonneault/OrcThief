#pragma once

namespace ot::graphics::imgui
{
	class renderer
	{
	public:
		virtual ~renderer() = 0;

		virtual bool initialize() = 0;
		virtual void shutdown() = 0;
		virtual void new_frame() = 0;
		virtual void render() = 0;
	};
}
