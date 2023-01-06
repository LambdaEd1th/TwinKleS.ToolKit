#pragma once

#include "core/utility/utility.hpp"

namespace TwinStar::Core::Tool::PopCap::PAK {

	M_record_of_map(
		M_wrap(Version),
		M_wrap(
			(Integer) number,
			(Boolean) compress_resource_data,
		),
	);

	using VersionPackage = ValuePackage<
		Version{.number = 0_i, .compress_resource_data = k_false},
		Version{.number = 0_i, .compress_resource_data = k_true}
	>;

	// ----------------

	inline constexpr auto check_version (
		Version const &                               it,
		std::initializer_list<Integer::Value> const & number,
		std::initializer_list<Boolean::Value> const & compress_resource_data
	) -> ZBoolean {
		auto result = true;
		result &= VersionPackage::has(it);
		result &= (number.size() == 0 || Range::has(number, it.number.value));
		result &= (compress_resource_data.size() == 0 || Range::has(compress_resource_data, it.compress_resource_data.value));
		return result;
	}

}
