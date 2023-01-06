#pragma once

#include "core/utility/container/list/list_view.hpp"

namespace TwinStar::Core {

	#pragma region type

	template <typename TElement, auto t_size> requires
		CategoryConstraint<IsPureInstance<TElement>>
		&& (IsSameV<t_size, Size>)
	class StaticArray {

	public:

		using Element = TElement;

		using Iterator = Pointer<Element>;

		using View = ListView<Element, false>;

		using VElement = Element;

		using VIterator = Pointer<Element>;

		using VView = ListView<Element, false>;

		using CElement = Element const;

		using CIterator = Pointer<Element const>;

		using CView = ListView<Element, true>;

		using RawArray = ZArray<Element, t_size.value>;

		using ValidRawArray = ZArray<Element, t_size == 0_sz ? (1_szz) : (t_size.value)>;

	protected:

		ValidRawArray m_data{Element{}};

	public:

		#pragma region structor

		constexpr ~StaticArray (
		) = default;

		// ----------------

		constexpr StaticArray (
		) = default;

		constexpr StaticArray (
			StaticArray const & that
		) :
			StaticArray{} {
			thiz = that;
		}

		constexpr StaticArray (
			StaticArray && that
		) :
			StaticArray{} {
			thiz = as_moveable(that);
		}

		// ----------------

		explicit constexpr StaticArray (
			CView const & view
		) :
			StaticArray{} {
			Range::assign_from(make_range_n(thiz.begin(), min(view.size(), thiz.size())), view);
		}

		explicit constexpr StaticArray (
			CIterator const & begin,
			Size const &      size
		) :
			StaticArray{CView{begin, size}} {
		}

		explicit constexpr StaticArray (
			RawArray const & raw_array
		) :
			StaticArray{CView{make_pointer(static_cast<decltype(&*raw_array)>(raw_array)), t_size}} {
		}

		#pragma endregion

		#pragma region operator

		constexpr auto operator = (
			StaticArray const & that
		) -> StaticArray& {
			Range::assign_from(thiz, that);
			return thiz;
		}

		constexpr auto operator = (
			StaticArray && that
		) -> StaticArray& {
			Range::assign_from(thiz, make_moveable_range_of(that));
			return thiz;
		}

		// ----------------

		constexpr auto operator [] (
			Size const & index
		) -> VElement& {
			return thiz.at(index);
		}

		constexpr auto operator [] (
			Size const & index
		) const -> CElement& {
			return thiz.at(index);
		}

		#pragma endregion

		#pragma region size & index

		constexpr auto size (
		) const -> Size {
			return t_size;
		}

		// ----------------

		constexpr auto begin_index (
		) const -> Size {
			return k_begin_index;
		}

		constexpr auto end_index (
		) const -> Size {
			return thiz.size();
		}

		// ----------------

		constexpr auto first_index (
		) const -> Size {
			assert_condition(!thiz.empty());
			return thiz.begin_index();
		}

		constexpr auto last_index (
		) const -> Size {
			assert_condition(!thiz.empty());
			return thiz.end_index() - k_next_index;
		}

		#pragma endregion

		#pragma region iterator

		constexpr auto iterator (
			Size const & index
		) -> VIterator {
			assert_condition(index <= thiz.end_index());
			return Iterator{thiz.m_data + index.value};
		}

		constexpr auto begin (
		) -> VIterator {
			return Iterator{thiz.m_data + thiz.begin_index().value};
		}

		constexpr auto end (
		) -> VIterator {
			return Iterator{thiz.m_data + thiz.end_index().value};
		}

		// ----------------

		constexpr auto iterator (
			Size const & index
		) const -> CIterator {
			assert_condition(index <= thiz.end_index());
			return CIterator{thiz.m_data + index.value};
		}

		constexpr auto begin (
		) const -> CIterator {
			return CIterator{thiz.m_data + thiz.begin_index().value};
		}

		constexpr auto end (
		) const -> CIterator {
			return CIterator{thiz.m_data + thiz.end_index().value};
		}

		#pragma endregion

		#pragma region element

		constexpr auto at (
			Size const & index
		) -> VElement& {
			assert_condition(index < thiz.end_index());
			return thiz.m_data[index.value];
		}

		constexpr auto first (
		) -> VElement& {
			return thiz.at(thiz.first_index());
		}

		constexpr auto last (
		) -> VElement& {
			return thiz.at(thiz.last_index());
		}

		// ----------------

		constexpr auto at (
			Size const & index
		) const -> CElement& {
			assert_condition(index < thiz.end_index());
			return thiz.m_data[index.value];
		}

		constexpr auto first (
		) const -> CElement& {
			return thiz.at(thiz.first_index());
		}

		constexpr auto last (
		) const -> CElement& {
			return thiz.at(thiz.last_index());
		}

		#pragma endregion

		#pragma region view

		constexpr auto view (
		) -> VView {
			return VView{thiz.begin(), thiz.size()};
		}

		constexpr auto view (
		) const -> CView {
			return CView{thiz.begin(), thiz.size()};
		}

		#pragma endregion

	public:

		#pragma region operator

		friend constexpr auto operator == (
			StaticArray const & thix,
			StaticArray const & that
		) -> bool {
			return Range::equal(thix, that);
		}

		#pragma endregion

	};

	#pragma endregion

	#pragma region utility

	template <typename Element, typename ... Argument> requires
		CategoryConstraint<IsPureInstance<Element> && IsValid<Argument ...>>
		&& (IsConstructible<Element, Argument &&> && ...)
	inline constexpr auto make_static_array (
		Argument && ... argument
	) -> StaticArray<Element, mbw<Size>(sizeof...(Argument))> {
		auto result = StaticArray<Element, mbw<Size>(sizeof...(Argument))>{};
		Generalization::each_with<>(
			[&] <auto index, typename CurrentArgument> (ValuePackage<index>, CurrentArgument && current_argument) {
				restruct(result.at(mbw<Size>(index)), as_forward<CurrentArgument>(current_argument));
			},
			as_forward<Argument>(argument) ...
		);
		return result;
	}

	#pragma endregion

}
