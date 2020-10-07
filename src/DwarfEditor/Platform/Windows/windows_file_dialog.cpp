#include "platform/file_dialog.h"

#include "platform/Windows/windows_main.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <ShObjIdl.h>

namespace ws = winrt::Windows::Storage;
namespace wf = winrt::Windows::Foundation;

namespace ot::dedit::platform
{
	namespace
	{
		wf::IAsyncAction do_open_file_dialog(std::string_view file_type, path_callback cb)
		{
			ws::Pickers::FileOpenPicker picker;
			picker.as<IInitializeWithWindow>()->Initialize(windows::get_main_window());

			picker.FileTypeFilter().Append(winrt::to_hstring(file_type));

			ws::StorageFile const file = co_await picker.PickSingleFileAsync();
			if (file == nullptr)
			{
				cb(make_error_code(std::errc::operation_canceled), {});
				co_return;
			}

			cb({}, to_string(file.Path()));
		}

		wf::IAsyncAction do_save_file_dialog(file_type const& f, path_callback cb)
		{
			ws::Pickers::FileSavePicker picker;
			picker.as<IInitializeWithWindow>()->Initialize(windows::get_main_window());

			std::vector<winrt::hstring> extensions;
			extensions.reserve(f.extensions.size());
			std::transform(f.extensions.begin(), f.extensions.end(), std::back_inserter(extensions), [](auto s) { return winrt::to_hstring(s); });
			picker.FileTypeChoices().Insert(winrt::to_hstring(f.name), winrt::single_threaded_vector(std::move(extensions)));

			ws::StorageFile const file = co_await picker.PickSaveFileAsync();
			if (file == nullptr)
			{
				cb(make_error_code(std::errc::operation_canceled), {});
				co_return;
			}

			cb({}, to_string(file.Path()));
		}
	}

	void open_file_dialog(std::string_view file_type, path_callback cb)
	{
		do_open_file_dialog(file_type, std::move(cb));
	}

	void save_file_dialog(file_type const& f, path_callback cb)
	{
		do_save_file_dialog(f, std::move(cb));
	}
}
