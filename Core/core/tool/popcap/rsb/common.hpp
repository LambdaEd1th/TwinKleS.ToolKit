#pragma once

// TODO : refactor

#include "core/utility/utility.hpp"
#include "core/tool/popcap/rsb/version.hpp"

namespace TwinStar::Core::Tool::PopCap::RSB::Common {

	#pragma region

	inline constexpr auto k_padding_unit_size = Size{0x1000_sz};

	#pragma endregion

	#pragma region

	M_enumeration(
		M_wrap(ResourceType),
		M_wrap(
			generic,
			texture,
		),
	);

	#pragma endregion

	#pragma region

	M_record_of_list(
		M_wrap(ResourceDataSectionStoreMode),
		M_wrap(
			(Boolean) compress_generic,
			(Boolean) compress_texture,
		),
	);

	inline constexpr auto k_resource_data_section_store_mode_flag_count = Size{2_sz};

	namespace ResourceDataStoreModeFlag {
		inline constexpr auto compress_texture = Size{1_ix};
		inline constexpr auto compress_generic = Size{2_ix};
	}

	inline auto resource_data_section_store_mode_to_data (
		ResourceDataSectionStoreMode const & it
	) -> IntegerU32 {
		auto bitset = BitSet<k_resource_data_section_store_mode_flag_count>{};
		bitset.set(ResourceDataStoreModeFlag::compress_generic, it.compress_generic);
		bitset.set(ResourceDataStoreModeFlag::compress_texture, it.compress_texture);
		return cbw<IntegerU32>(bitset.to_integer());
	}

	inline auto resource_data_section_store_mode_from_data (
		IntegerU32 const & bitset_integer
	) -> ResourceDataSectionStoreMode {
		auto it = ResourceDataSectionStoreMode{};
		auto bitset = BitSet<k_resource_data_section_store_mode_flag_count>{};
		bitset.from_integer(cbw<IntegerU8>(bitset_integer));
		it.compress_generic = bitset.get(ResourceDataStoreModeFlag::compress_generic);
		it.compress_texture = bitset.get(ResourceDataStoreModeFlag::compress_texture);
		return it;
	}

	#pragma endregion

	#pragma region

	M_record_of_list(
		M_wrap(SubgroupCategory),
		M_wrap(
			(Optional<Integer>) resolution,
			(Optional<String>) locale,
		),
	);

	#pragma endregion

	#pragma region

	using StringBlockFixed128 = StaticArray<Character, 128_sz>;

	#pragma endregion

	#pragma region

	namespace StringMapSection {

		inline constexpr auto k_block_size = Size{bs_static_size<IntegerU32>()};

		template <typename TAdditionalBlock> requires IsPureInstance<TAdditionalBlock>
		inline auto adjust_sequence (
			Map<String, TAdditionalBlock> & list
		) -> Void {
			Range::each(
				list,
				[] (auto & element) {
					element.key.as_upper_case();
				}
			);
			Range::sort(
				list,
				[] (auto & thix, auto & that) {
					return thix.key > that.key;
				}
			);
			return;
		}

		// ----------------

		template <typename TAdditionalBlock> requires IsPureInstance<TAdditionalBlock>
		inline auto compute_ripe_size (
			Map<String, TAdditionalBlock> const & list
		) -> Size {
			struct WorkOption {
				Size inherit_length;
			};
			auto work_option = Array<Optional<WorkOption>>{list.size()};
			if (!list.empty()) {
				work_option[1_ix].set(
					WorkOption{
						.inherit_length = k_none_size,
					}
				);
			}
			auto block_count = k_none_size;
			for (auto & index : SizeRange{list.size()}) {
				auto & element = list.at(index);
				{
					auto a_string_has_child = Array<Boolean>{element.key.size() + 1_sz};
					for (auto & index_1 : SizeRange{index + 2_ix, list.size()}) {
						if (!work_option[index_1].has()) {
							auto common_size = Range::common_size(list.at(index).key, list.at(index_1).key);
							if (!a_string_has_child[common_size] && common_size >= work_option[index].get().inherit_length) {
								a_string_has_child[common_size] = k_true;
								work_option[index_1].set(
									WorkOption{
										.inherit_length = common_size,
									}
								);
							}
						}
					}
				}
				auto character_index = k_begin_index;
				if (work_option[index]) {
					character_index += work_option[index].get().inherit_length;
					work_option[index].reset();
				}
				block_count += element.key.size() - character_index + 1_sz;
				auto additional_size = bs_size(element.value);
				assert_test(is_padded_size(additional_size, bs_static_size<IntegerU32>()));
				block_count += additional_size / bs_static_size<IntegerU32>();
			}
			return block_count * k_block_size;
		}

		template <typename TAdditionalBlock> requires IsPureInstance<TAdditionalBlock>
		inline auto encode (
			Map<String, TAdditionalBlock> const & list,
			OByteStreamView &                     data
		) -> Void {
			auto stream = IOByteStreamView{data.reserve_view()};
			struct WorkOption {
				Size inherit_length;
				Size parent_offset;
			};
			auto work_option = Array<Optional<WorkOption>>{list.size()};
			if (!list.empty()) {
				work_option.first().set(
					WorkOption{
						.inherit_length = k_none_size,
						.parent_offset = k_begin_index,
					}
				);
				for (auto & index : SizeRange{list.size()}) {
					auto & element = list.at(index);
					auto   current_string_has_child = Array<Boolean>{element.key.size() + 1_sz};
					for (auto & index_1 : SizeRange{index + k_next_index, list.size()}) {
						if (!work_option[index_1]) {
							auto common_size = Range::common_size(element.key, list.at(index_1).key);
							if (!current_string_has_child[common_size] && common_size >= work_option[index].get().inherit_length) {
								current_string_has_child[common_size] = k_true;
								work_option[index_1].set(
									WorkOption{
										.inherit_length = common_size,
										.parent_offset = stream.position() / k_block_size + (common_size - work_option[index].get().inherit_length),
									}
								);
							}
						}
					}
					auto character_index = k_begin_index;
					if (work_option[index]) {
						character_index += work_option[index].get().inherit_length;
						auto current_position = stream.position();
						stream.set_position(work_option[index].get().parent_offset * k_block_size);
						auto origin_value = stream.read_of<IntegerU32>();
						origin_value |= cbw<IntegerU32>(current_position / k_block_size) << k_type_bit_count<IntegerU8>;
						stream.backward(bs_static_size<IntegerU32>());
						stream.write(origin_value);
						stream.set_position(current_position);
						work_option[index].reset();
					}
					while (character_index < element.key.size()) {
						stream.write(cbw<IntegerU32>(element.key[character_index]));
						++character_index;
					}
					stream.write(cbw<IntegerU32>(CharacterType::k_null));
					stream.write(element.value);
				}
			}
			data.forward(stream.position());
			return;
		}

		template <typename TAdditionalBlock> requires IsPureInstance<TAdditionalBlock>
		inline auto decode (
			Map<String, TAdditionalBlock> & list,
			CByteListView const &           data
		) -> Void {
			auto stream = IByteStreamView{data};
			list.allocate(stream.reserve() / k_block_size);
			auto parent_string_list = Array<Optional<CStringView>>{stream.reserve() / k_block_size};
			while (!stream.full()) {
				list.append();
				auto & element = list.last();
				auto   string_length_in_next_stream = k_none_size;
				{
					auto string_begin_position = stream.position();
					while (cbw<Character>(clip_bit(stream.read_of<IntegerU32>(), 1_ix * k_type_bit_count<IntegerU8>, 1_sz * k_type_bit_count<IntegerU8>)) != CharacterType::k_null) {
						++string_length_in_next_stream;
					}
					stream.set_position(string_begin_position);
				}
				if (auto & parent_string = parent_string_list[stream.position() / k_block_size]) {
					element.key.allocate(parent_string.get().size() + string_length_in_next_stream);
					element.key.append_list(parent_string.get());
					parent_string.reset();
				} else {
					element.key.allocate(string_length_in_next_stream);
				}
				while (k_true) {
					auto current_unit = stream.read_of<IntegerU32>();
					auto child_string_offset = clip_bit(current_unit, 2_ix * k_type_bit_count<IntegerU8>, 3_sz * k_type_bit_count<IntegerU8>);
					auto current_character = cbw<Character>(clip_bit(current_unit, 1_ix * k_type_bit_count<IntegerU8>, 1_sz * k_type_bit_count<IntegerU8>));
					if (child_string_offset != 0x000000_iu32) {
						parent_string_list[cbw<Size>(child_string_offset)].set(element.key);
					}
					if (current_character == CharacterType::k_null) {
						break;
					}
					element.key.append(current_character);
				}
				stream.read(element.value);
			}
			list.shrink_to_fit();
			return;
		}

	}

	#pragma endregion

}

namespace TwinStar::Core::Tool::PopCap::RSGP {
	using namespace RSB::Common;
}

namespace TwinStar::Core::Tool::PopCap::RSB {
	using namespace RSB::Common;
}

namespace TwinStar::Core::Tool::PopCap::RSBPatch {
	using namespace RSB::Common;
}
