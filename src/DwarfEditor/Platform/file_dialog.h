#pragma once

#include <system_error>
#include <functional>
#include <span>

namespace ot::dedit::platform
{
	using path_callback = std::function<void(std::error_code ec, std::string file_path)>;

	// A file type that would appear in a file dialog
	struct file_type
	{
		std::string_view name; // ex: Plain Text
		std::span<std::string_view const> extensions; // ex: {".txt", ".log", ".text"}
	};

	// Async operation to open the system's "open file" dialog
	// 'file_type' is a string of the format ".xxx", where xxx is the preferred file extension
	// Once the operation is over, 'cb' will be invoked with the result code and the selected file path
	// Error conditions:
	//   - std::errc::operation_canceled: User closed the dialog without selecting a file
	void open_file_dialog(std::string_view file_type, path_callback cb);

	// Async operation to open the system's "save file" dialog, and returns the chosen path
	// 'f' is a file type containing a suggestion of the extension the file could take
	// Once the operation is over, 'cb' will be invoked with the result code and the selected file path
	// Error conditions:
	//   - std::errc::operation_canceled: User closed the dialog without selecting a file
	void save_file_dialog(file_type const& f, path_callback cb);

	// Opens the resource referred by the input URI with a native dialog
	void open_resource(std::string_view uri);
}
