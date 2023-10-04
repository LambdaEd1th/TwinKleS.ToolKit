#pragma once

#include "shell/common.hpp"
#include "shell/bridge/library.hpp"
#include "shell/bridge/invoker.hpp"
#include "shell/bridge/host.hpp"

namespace TwinStar::Shell::Bridge {

	#pragma region type

	struct Launcher {

		#pragma region function

		static auto launch (
			Host &                           host,
			Library &                        library,
			std::string const &              script,
			std::vector<std::string> const & argument
		) -> std::string {
			host.start();
			auto result = std::string{};
			auto exception = std::optional<std::exception_ptr>{};
			try {
				auto callback = std::bind(&Host::execute, &host, std::placeholders::_1);
				Invoker::version(library);
				Invoker::prepare(library);
				result = Invoker::execute(library, callback, script, argument);
			} catch (...) {
				exception.emplace(std::current_exception());
			}
			host.finish();
			if (exception.has_value()) {
				std::rethrow_exception(exception.value());
			}
			return result;
		}

		#pragma endregion

	};

	#pragma endregion

}
