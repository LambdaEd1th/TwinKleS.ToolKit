#pragma once

#include "shell/base.hpp"
#include "shell/library/library.hpp"

#if defined M_system_windows
#include "Windows.h"
#endif
#if defined M_system_linux || defined M_system_macos || defined M_system_android || defined M_system_ios
#include "dlfcn.h"
#endif

namespace TwinStar::Shell {

	#pragma region detail

	namespace Detail {

		using LibraryHandle = void *;

		// ----------------

		inline auto open_library (
			std::string_view const & path
		) -> LibraryHandle {
			#if defined M_system_windows
			auto path_absolute = std::string{path} + "."s;
			auto path_w = utf8_to_utf16(reinterpret_cast<std::u8string const &>(path_absolute));
			auto result = LoadLibraryW(reinterpret_cast<LPCWSTR>(path_w.c_str()));
			if (!result) {
				throw std::runtime_error{"can not open library : "s + std::string{path}};
			}
			#endif
			#if defined M_system_linux || defined M_system_macos || defined M_system_android || defined M_system_ios
			auto result = dlopen(path.data(), RTLD_LAZY | RTLD_LOCAL);
			if (!result) {
				throw std::runtime_error{"can not open library : "s + std::string{path}};
			}
			#endif
			return result;
		}

		inline auto close_library (
			LibraryHandle const & handle
		) -> void {
			#if defined M_system_windows
			auto result = FreeLibrary(static_cast<HMODULE>(handle));
			if (result != TRUE) {
				throw std::runtime_error{"can not close library"s};
			}
			#endif
			#if defined M_system_linux || defined M_system_macos || defined M_system_android || defined M_system_ios
			auto result = dlclose(handle);
			if (result != 0) {
				throw std::runtime_error{"can not close library"s};
			}
			#endif
			return;
		}

		template <typename Symbol>
		inline auto get_symbol (
			LibraryHandle const &    handle,
			std::string_view const & symbol
		) -> Symbol {
			#if defined M_system_windows
			auto result = GetProcAddress(static_cast<HMODULE>(handle), symbol.data());
			if (!result) {
				throw std::runtime_error{"can not get symbol : "s + std::string{symbol}};
			}
			#endif
			#if defined M_system_linux || defined M_system_macos || defined M_system_android || defined M_system_ios
			auto result = dlsym(handle, symbol.data());
			if (!result) {
				throw std::runtime_error{"can not get symbol : "s + std::string{symbol}};
			}
			#endif
			return reinterpret_cast<Symbol>(result);
		}

	}

	#pragma endregion

	#pragma region type

	class DynamicLibrary :
		public Library {

	protected:

		SymbolTable           m_symbol;
		Detail::LibraryHandle m_handle;

	public:

		#pragma region structor

		virtual ~DynamicLibrary (
		) override {
			Detail::close_library(thiz.m_handle);
		}

		// ----------------

		DynamicLibrary (
		) = delete;

		DynamicLibrary (
			DynamicLibrary const & that
		) = delete;

		DynamicLibrary (
			DynamicLibrary && that
		) = delete;

		// ----------------

		explicit DynamicLibrary (
			std::nullptr_t           _,
			std::string_view const & path
		) :
			m_symbol{},
			m_handle{} {
			thiz.m_handle = Detail::open_library(path);
			thiz.m_symbol = {
				.version = Detail::get_symbol<decltype(Core::version) *>(thiz.m_handle, k_symbol_id.version),
				.execute = Detail::get_symbol<decltype(Core::execute) *>(thiz.m_handle, k_symbol_id.execute),
			};
		}

		#pragma endregion

		#pragma region operator

		auto operator = (
			DynamicLibrary const & that
		) -> DynamicLibrary& = delete;

		auto operator = (
			DynamicLibrary && that
		) -> DynamicLibrary& = delete;

		#pragma endregion

		#pragma region interface

		virtual auto version (
		) -> Core::Size const* override {
			return thiz.m_symbol.version();
		}

		virtual auto execute (
			Core::String const &        script,
			Core::Boolean const &       script_is_path,
			Core::StringList const &    argument,
			Core::ShellCallback const & shell_callback
		) -> Core::String const* override {
			return thiz.m_symbol.execute(script, script_is_path, argument, shell_callback);
		}

		#pragma endregion

	};

	#pragma endregion

}
