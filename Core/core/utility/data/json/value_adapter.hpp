#pragma once

#include "core/utility/base_wrapper/wrapper.hpp"

namespace TwinStar::Core::JSON {

	#pragma region type

	// static auto from ( This       & thix, That && that, Option && ...option ) -> Void;
	// static auto to   ( This const & thix, That && that, Option && ...option ) -> Void;
	template <typename TThat>
	struct ValueAdapter;

	#pragma endregion

}
