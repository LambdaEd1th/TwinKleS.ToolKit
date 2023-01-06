#pragma once

#include "core/utility/utility.hpp"

namespace TwinStar::Core::Tool::PopCap::Reanim {

	M_enumeration(
		M_wrap(VersionPlatform),
		M_wrap(
			desktop,
			phone,
		),
	);

	// ----------------

	M_record_of_map(
		M_wrap(Version),
		M_wrap(
			(VersionPlatform) platform,
			(Boolean) variant_64,
		),
	);

	using VersionPackage = ValuePackage<
		Version{.platform = VersionPlatform::Constant::desktop(), .variant_64 = k_false},
		Version{.platform = VersionPlatform::Constant::phone(), .variant_64 = k_false},
		Version{.platform = VersionPlatform::Constant::phone(), .variant_64 = k_true}
	>;

	// ----------------

	inline constexpr auto check_version (
		Version const &                                it,
		std::initializer_list<VersionPlatform> const & platform,
		std::initializer_list<Boolean::Value> const &  variant_64
	) -> ZBoolean {
		auto result = true;
		result &= VersionPackage::has(it);
		result &= (platform.size() == 0 || Range::has(platform, it.platform));
		result &= (variant_64.size() == 0 || Range::has(variant_64, it.variant_64.value));
		return result;
	}

}
