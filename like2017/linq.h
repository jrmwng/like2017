#pragma once

/* Author: jrmwng @ 2017 */

#include <functional>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <array>
#include <tuple>
#include <type_traits>

#pragma push_macro("DEBUG_CHECK")
#ifdef DEBUG_CHECK
#undef DEBUG_CHECK
#endif
#ifdef _DEBUG
#define DEBUG_CHECK(that) debug_check(that)
#else
#define DEBUG_CHECK(that) 
#endif

namespace jrmwng
{
	namespace linq
	{
		template <typename Tcontainer>
		class linq_enumerable;

		template <typename Tcontainer, typename Titerator>
		class linq_container
		{
			typedef typename Titerator::params_type
				Tparams;

			Tcontainer const m_container;
			Tparams const m_params;
		public:
			linq_container(Tcontainer && container, Tparams && params)
				: m_container(std::forward<Tcontainer>(container))
				, m_params(std::forward<Tparams>(params))
			{}

			Titerator begin() const
			{
				return Titerator(m_container.begin(), m_container, m_params);
			}
			Titerator end() const
			{
				return Titerator(m_container.end(), m_container, m_params);
			}
		};
		template <typename Tcontainer0, typename Tcontainer1, typename Titerator>
		class linq_pair_container
		{
			typedef typename Titerator::params_type
				Tparams;

			Tcontainer0 const m_container0;
			Tcontainer1 const m_container1;
			Tparams const m_params;
		public:
			linq_pair_container(Tcontainer0 && container0, Tcontainer1 && container1, Tparams && params)
				: m_container0(std::forward<Tcontainer0>(container0))
				, m_container1(std::forward<Tcontainer1>(container1))
				, m_params(std::forward<Tparams>(params))
			{}

			Titerator begin() const
			{
				return Titerator(m_container0.begin(), m_container1.begin(), m_params);
			}
			Titerator end() const
			{
				return Titerator(m_container0.end(), m_container1.end(), m_params);
			}
		};

		template <typename Titerator>
		class linq_range
		{
			Titerator const m_itBegin;
			Titerator const m_itEnd;
		public:
			linq_range(Titerator itBegin, Titerator itEnd)
				: m_itBegin(itBegin)
				, m_itEnd(itEnd)
			{}
			Titerator begin() const
			{
				return m_itBegin;
			}
			Titerator end() const
			{
				return m_itEnd;
			}
		};

		template <typename Tcontainer>
		class linq_from
		{
			Tcontainer const m_container;

		public:
			linq_from(Tcontainer && container)
				: m_container(std::forward<Tcontainer>(container))
			{}

			decltype(auto) begin() const
			{
				return m_container.begin();
			}
			decltype(auto) end() const
			{
				return m_container.end();
			}
		};

		template <typename Titerator>
		class linq_iterator
		{
		protected:
			Titerator m_itCurrent;

		public:
			linq_iterator(Titerator && itCurrent)
				: m_itCurrent(std::forward<Titerator>(itCurrent))
			{}
			decltype(auto) operator * () const
			{
				return *m_itCurrent;
			}
			bool operator != (linq_iterator const & that) const
			{
				return m_itCurrent != that.m_itCurrent;
			}
			bool operator == (linq_iterator const & that) const
			{
				return m_itCurrent == that.m_itCurrent;
			}
			linq_iterator & operator ++ ()
			{
				++m_itCurrent;
				return *this;
			}
		};

		template <typename Titerator, typename Tfunc, typename Treturn>
		class linq_select_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, typename std::remove_reference<Treturn>::type>
		{
			Tfunc const m_func;

			void debug_check(linq_select_iterator const & that) const
			{
#ifdef _DEBUG
				if (typeid(m_func) != typeid(that.m_func))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tfunc
				params_type;

			template <typename Tcontainer>
			linq_select_iterator(Titerator && itCurrent, Tcontainer const & container, Tfunc const & func)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_func(func)
			{}
			Treturn operator * () const
			{
				static_assert(std::is_same<Treturn, decltype(m_func(*m_itCurrent))>::value, "Mismatch of return type 'Treturn'");
				return m_func(*m_itCurrent);
			}
			bool operator == (linq_select_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent == that.m_itCurrent;
			}
			bool operator != (linq_select_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent != that.m_itCurrent;
			}
			linq_select_iterator & operator = (linq_select_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			linq_select_iterator & operator ++ ()
			{
				++m_itCurrent;
				return *this;
			}
		};
		template <typename Titerator, typename Tmany_iterator, typename Tparams, typename Treturn>
		class linq_select_many_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, typename std::remove_reference<Treturn>::type>
		{
			typedef typename std::tuple_element<0, Tparams>::type
				Tmany;
			typedef typename std::tuple_element<1, Tparams>::type
				Tselect;

			std::array<char, sizeof(Tmany_iterator)> m_itManyCurrent;
			std::array<char, sizeof(Tmany_iterator)> m_itManyEnd;
			Titerator const m_itEnd;
			Tmany const m_fnMany;
			Tselect const m_fnSelect;
			template <typename T>
			static void construct(std::array<char, sizeof(T)> & array, T const & t)
			{
				new (array.data()) T(t);
			}
			template <typename T>
			static void construct(std::array<char, sizeof(T)> & array, T && t)
			{
				new (array.data()) T(std::forward<T>(t));
			}
			template <typename T>
			static void destruct(std::array<char, sizeof(T)> & array)
			{
				reinterpret_cast<T*>(array.data())->~T();
			}
			template <typename T>
			static T const & read(std::array<char, sizeof(T)> const & array)
			{
				return *reinterpret_cast<T const*>(array.data());
			}
			template <typename T>
			static T & modify(std::array<char, sizeof(T)> & array)
			{
				return *reinterpret_cast<T*>(array.data());
			}
			void debug_check(linq_select_many_iterator const & that) const
			{
#ifdef _DEBUG
				if (m_itEnd != that.m_itEnd ||
					typeid(m_fnMany) != typeid(that.m_fnMany) ||
					typeid(m_fnSelect) != typeid(that.m_fnSelect))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tparams
				params_type;

			template <typename Tcontainer>
			linq_select_many_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams const & params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itEnd(container.end())
				, m_fnMany(std::get<0>(params))
				, m_fnSelect(std::get<1>(params))
			{
				if (m_itCurrent != m_itEnd)
				{
					auto const valueCurrent = m_fnMany(*m_itCurrent);
					construct<Tmany_iterator>(m_itManyCurrent, valueCurrent.begin());
					construct<Tmany_iterator>(m_itManyEnd, valueCurrent.end());
				}
			}
			linq_select_many_iterator(linq_select_many_iterator const & that)
				: linq_iterator<Titerator>(std::move(that))
				, m_itEnd(that.m_itEnd)
				, m_fnMany(that.m_fnMany)
				, m_fnSelect(that.m_fnSelect)
			{
				if (m_itCurrent != m_itEnd)
				{
					construct<Tmany_iterator>(m_itManyCurrent, read<Tmany_iterator>(that.m_itManyCurrent));
					construct<Tmany_iterator>(m_itManyEnd, read<Tmany_iterator>(that.m_itManyEnd));
				}
			}
			~linq_select_many_iterator()
			{
				if (m_itCurrent != m_itEnd)
				{
					destruct<Tmany_iterator>(m_itManyCurrent);
					destruct<Tmany_iterator>(m_itManyEnd);
				}
			}
			linq_select_many_iterator & operator = (linq_select_many_iterator const & that)
			{
				DEBUG_CHECK(that);
				if (m_itCurrent != m_itEnd)
				{
					destruct<Tmany_iterator>(m_itManyCurrent);
					destruct<Tmany_iterator>(m_itManyEnd);
				}
				m_itCurrent = that.m_itCurrent;
				if (m_itCurrent != m_itEnd)
				{
					construct<Tmany_iterator>(m_itManyCurrent, read<Tmany_iterator>(that.m_itManyCurrent));
					construct<Tmany_iterator>(m_itManyEnd, read<Tmany_iterator>(that.m_itManyEnd));
				}
				return *this;
			}
			Treturn operator * () const
			{
				static_assert(std::is_same<Treturn, decltype(m_fnSelect(*m_itCurrent, *read<Tmany_iterator>(m_itManyCurrent)))>::value, "Mismatch of return type 'Treturn'");
				return m_fnSelect(*m_itCurrent, *read<Tmany_iterator>(m_itManyCurrent));
			}
			linq_select_many_iterator & operator ++ ()
			{
				if (++modify<Tmany_iterator>(m_itManyCurrent) == read<Tmany_iterator>(m_itManyEnd))
				{
					destruct<Tmany_iterator>(m_itManyCurrent);
					destruct<Tmany_iterator>(m_itManyEnd);

					if (++m_itCurrent != m_itEnd)
					{
						auto const valueCurrent = m_fnMany(*m_itCurrent);
						construct<Tmany_iterator>(m_itManyCurrent, valueCurrent.begin());
						construct<Tmany_iterator>(m_itManyEnd, valueCurrent.end());
					}
				}
				return *this;
			}
			bool operator == (linq_select_many_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return
					m_itCurrent == that.m_itCurrent &&
					(
						m_itCurrent == m_itEnd ||
						(
							read<Tmany_iterator>(m_itManyCurrent) == read<Tmany_iterator>(that.m_itManyCurrent)
						)
					);
			}
			bool operator != (linq_select_many_iterator const & that) const
			{
				return !(*this == that);
			}
		};
		template <typename Titerator, typename Tparams, typename Treturn>
		class linq_group_join_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, typename std::remove_reference<Treturn>::type>
		{
			typedef typename std::tuple_element<0, Tparams>::type
				Tinner;
			typedef typename std::tuple_element<1, Tparams>::type
				Touter_key_selector;
			typedef typename std::tuple_element<2, Tparams>::type
				Tinner_key_selector;
			typedef typename std::tuple_element<3, Tparams>::type
				Tresult_selector;

			Tinner const m_Inner;
			Touter_key_selector const m_fnOuterKeySelector;
			Tinner_key_selector const m_fnInnerKeySelector;
			Tresult_selector const m_fnResultSelector;

			void debug_check(linq_group_join_iterator const & that) const
			{
#ifdef _DEBUG
				if (typeid(m_Inner) != typeid(that.m_Inner) ||
					typeid(m_fnOuterKeySelector) != typeid(that.m_fnOuterKeySelector) ||
					typeid(m_fnInnerKeySelector) != typeid(that.m_fnInnerKeySelector) ||
					typeid(m_fnResultSelector) != typeid(that.m_fnResultSelector))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tparams
				params_type;

			template <typename Tcontainer>
			linq_group_join_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams const & params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_Inner(std::get<0>(params))
				, m_fnOuterKeySelector(std::get<1>(params))
				, m_fnInnerKeySelector(std::get<2>(params))
				, m_fnResultSelector(std::get<3>(params))
			{}
			bool operator == (linq_group_join_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent == that.m_itCurrent;
			}
			bool operator != (linq_group_join_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent != that.m_itCurrent;
			}
			linq_group_join_iterator & operator = (linq_group_join_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			decltype(auto) operator * () const
			{
				return m_fnResultSelector(
					*m_itCurrent,
					m_Inner.where([keyOuter = m_fnOuterKeySelector(*m_itCurrent), this](auto const & objInner)
				{
					return keyOuter == m_fnInnerKeySelector(objInner);
				}));
			}
		};
		template <typename Titerator, typename Tfunc>
		class linq_where_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, typename Titerator::value_type>
		{
			Titerator const m_itEnd;
			Tfunc const m_func;

			void debug_check(linq_where_iterator const & that) const
			{
#ifdef _DEBUG
				if (m_itEnd != that.m_itEnd ||
					typeid(m_func) != typeid(that.m_func))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tfunc
				params_type;

			template <typename Tcontainer>
			linq_where_iterator(Titerator && itCurrent, Tcontainer const & container, Tfunc const & func)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itEnd(container.end())
				, m_func(func)
			{
				for (; m_itCurrent != m_itEnd && !m_func(*m_itCurrent); ++m_itCurrent)
				{
					// NOP
				}
			}
			linq_where_iterator & operator = (linq_where_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			bool operator == (linq_where_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent == that.m_itCurrent;
			}
			bool operator != (linq_where_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent != that.m_itCurrent;
			}
			linq_where_iterator & operator ++ ()
			{
				Titerator itCurrent = m_itCurrent;
				{
					while (++itCurrent != m_itEnd && !m_func(*itCurrent))
					{
						// NOP
					}
				}
				m_itCurrent = itCurrent;
				return *this;
			}
		};
		template <typename Titerator, typename Tparams>
		class linq_group_by_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, linq_enumerable<linq_range<Titerator>>>
		{
			typedef typename std::tuple_element<0, Tparams>::type
				Tget;
			typedef typename std::tuple_element<1, Tparams>::type
				Tequal;

			Titerator const m_itBegin;
			Titerator const m_itEnd;
			Tget const m_fnGet;
			Tequal const m_fnEqual;

			void debug_check(linq_group_by_iterator const & that) const
			{
#ifdef _DEBUG
				if (m_itBegin != that.m_itBegin ||
					m_itEnd != that.m_itEnd ||
					typeid(m_fnGet) != typeid(that.m_fnGet) ||
					typeid(m_fnEqual) != typeid(that.m_fnEqual))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tparams
				params_type;

			template <typename Tcontainer>
			linq_group_by_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams const & params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itBegin(container.begin())
				, m_itEnd(container.end())
				, m_fnGet(std::get<0>(params))
				, m_fnEqual(std::get<1>(params))
			{}
			typename linq_group_by_iterator & operator = (linq_group_by_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			bool operator == (linq_group_by_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent == that.m_itCurrent;
			}
			bool operator != (linq_group_by_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent != that.m_itCurrent;
			}
			typename linq_group_by_iterator & operator ++ ()
			{
				for (
					auto const itMiddle = ++m_itCurrent;

					m_itCurrent != m_itEnd &&
					std::any_of(m_itBegin, itMiddle,
						[this](auto const & obj)
				{
					return m_fnEqual(m_fnGet(obj), m_fnGet(*m_itCurrent));
				});

					++m_itCurrent
					)
				{ }
				return *this;
			}
			decltype(auto) operator * () const
			{
				typedef linq_range<Titerator>
					Trange;
				typedef linq_enumerable<Trange>
					Trange_enumerable;
				//static_assert(std::is_same<Treturn, decltype(linq_enumerable<linq_range<Titerator>>(m_itCurrent, m_itEnd).where(std::bind(m_fnEqual, std::placeholders::_1, m_fnGet(*m_itCurrent))))>::value, "Mismatch of return type 'Treturn'");
				return Trange_enumerable(m_itCurrent, m_itEnd)
					.where([this](auto const & obj)
				{
					return m_fnEqual(m_fnGet(obj), m_fnGet(*m_itCurrent));
				});
			}
			decltype(auto) key() const
			{
				return m_fnGet(*m_itCurrent);
			}
		};
		template <typename Titerator, typename Tparams>
		class linq_uniq_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, linq_enumerable<linq_range<Titerator>>>
		{
			typedef typename std::tuple_element<0, Tparams>::type
				Tget;
			typedef typename std::tuple_element<1, Tparams>::type
				Tequal;

			Titerator m_itNext;
			Titerator const m_itBegin;
			Titerator const m_itEnd;
			Tget const m_fnGet;
			Tequal const m_fnEqual;

			void debug_check(linq_uniq_iterator const & that) const
			{
#ifdef _DEBUG
				if (m_itBegin != that.m_itBegin ||
					m_itEnd != that.m_itEnd ||
					typeid(m_fnGet) != typeid(that.m_fnGet) ||
					typeid(m_fnEqual) != typeid(that.m_fnEqual))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tparams
				params_type;

			template <typename Tcontainer>
			linq_uniq_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams const & params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itNext(itCurrent)
				, m_itBegin(container.begin())
				, m_itEnd(container.end())
				, m_fnGet(std::get<0>(params))
				, m_fnEqual(std::get<1>(params))
			{
				if (itCurrent != m_itEnd)
				{
					auto const valueCurrent = m_fnGet(*itCurrent);

					Titerator itNext = itCurrent;
					{
						while (++itNext != m_itEnd && m_fnEqual(valueCurrent, m_fnGet(*itNext)))
						{
							// NOP
						}
					}
					m_itNext = itNext;
				}
			}
			linq_uniq_iterator & operator = (linq_uniq_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_itCurrent = that.m_itCurrent;
				m_itNext = that.m_itNext;
				return *this;
			}
			bool operator == (linq_uniq_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent == that.m_itCurrent && m_itNext == that.m_itNext;
			}
			bool operator != (linq_uniq_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent != that.m_itCurrent || m_itNext != that.m_itNext;
			}
			linq_uniq_iterator & operator ++ ()
			{
				m_itCurrent = m_itNext;

				if (m_itNext != m_itEnd)
				{
					auto const valueCurrent = m_fnGet(*m_itNext);

					Titerator itNext = m_itNext;
					{
						while (++itNext != m_itEnd && m_fnEqual(valueCurrent, m_fnGet(*itNext)))
						{
							// NOP
						}
					}
					m_itNext = itNext;
				}
				return *this;
			}
			decltype(auto) operator * () const
			{
				typedef linq_range<Titerator>
					Trange;
				typedef linq_enumerable<Trange>
					Trange_enumerable;
				static_assert(std::is_same<Trange_enumerable, value_type>::value, "Mismatch of return type 'Treturn'");
				return Trange_enumerable(m_itCurrent, m_itNext);
			}
			decltype(auto) key() const
			{
				return m_fnGet(*m_itCurrent);
			}
		};
		template <typename Titerator, typename Tparams>
		class linq_order_by_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, typename Titerator::value_type>
		{
			typedef typename std::tuple_element<0, Tparams>::type
				Tget;
			typedef typename std::tuple_element<1, Tparams>::type
				Tless;

			Titerator m_itNext;
			Titerator const m_itBegin;
			Titerator const m_itEnd;
			Tget const m_fnGet;
			Tless const m_fnLess;

			void debug_check(linq_order_by_iterator const & that) const
			{
#ifdef _DEBUG
				if (m_itBegin != that.m_itBegin ||
					m_itEnd != that.m_itEnd ||
					typeid(m_fnGet) != typeid(that.m_fnGet) ||
					typeid(m_fnLess) != typeid(that.m_fnLess))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tparams
				params_type;

			template <typename Tcontainer>
			linq_order_by_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams const & params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itNext(container.end())
				, m_itBegin(container.begin())
				, m_itEnd(container.end())
				, m_fnGet(std::get<0>(params))
				, m_fnLess(std::get<1>(params))
			{
				if (m_itCurrent != m_itEnd)
				{
					auto const & fnGet = m_fnGet;
					auto const & fnLess = m_fnLess;

					Titerator itMin = m_itCurrent;
					Titerator itNext = m_itCurrent;
					{
						auto valueMin = fnGet(*itMin);
						auto valueNext = valueMin; // *itNext;

						for (Titerator it = itMin; it != m_itEnd; ++it)
						{
							auto const value = fnGet(*it);

							if (fnLess(value, valueMin))
							{
								valueNext = std::exchange(valueMin, value);
								itNext = std::exchange(itMin, it);
							}
							else if (fnLess(valueMin, value) == fnLess(value, valueNext))
							{
								valueNext = value;
								itNext = it;
							}
						}
					}
					m_itCurrent = itMin;
					m_itNext    = itNext;
				}
			}
			linq_order_by_iterator & operator = (linq_order_by_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_itCurrent = that.m_itCurrent;
				m_itNext    = that.m_itNext;
				return *this;
			}
			bool operator == (linq_order_by_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent == that.m_itCurrent;
			}
			bool operator != (linq_order_by_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent != that.m_itCurrent;
			}
			linq_order_by_iterator & operator ++ ()
			{
				auto const & fnGet = m_fnGet;
				auto const & fnLess = m_fnLess;
				auto const valueKey = fnGet(*m_itCurrent);
				auto valueNext = fnGet(*m_itNext);

				while (++m_itCurrent != m_itEnd)
				{
					auto const valueCurrent = fnGet(*m_itCurrent);

					if (fnLess(valueKey, valueCurrent) == fnLess(valueCurrent, valueKey))
					{
						return *this;
					}
					if ((fnLess(valueKey, valueCurrent) == fnLess(valueKey, valueNext)) == fnLess(valueCurrent, valueNext))
					{
						valueNext = valueCurrent;
						m_itNext = m_itCurrent;
					}
				}

				if (fnLess(valueKey, valueNext))
				{
					Titerator itNextNext = m_itNext;
					{
						auto valueNextNext = valueNext;

						for (Titerator itCurrent = m_itBegin; itCurrent != m_itNext; ++itCurrent)
						{
							auto const valueCurrent = fnGet(*itCurrent);

							if ((fnLess(valueNext, valueCurrent) == fnLess(valueNext, valueNextNext)) == fnLess(valueCurrent, valueNextNext))
							{
								itNextNext = itCurrent;
								valueNextNext = valueCurrent;
							}
						}
					}
					m_itCurrent = std::exchange(m_itNext, itNextNext);
				}
				return *this;
			}
		};

		template <typename Titerator0, typename Titerator1>
		class linq_concat_iterator
			: public std::iterator<std::forward_iterator_tag, typename Titerator0::value_type>
		{
			static_assert(std::is_same<typename Titerator0::value_type, typename Titerator1::value_type>::value, "Mismatch of iterator value type");
			Titerator0 m_it0;
			Titerator1 m_it1;
			Titerator0 const m_itEnd0;

			void debug_check(linq_concat_iterator const & that) const
			{
#ifdef _DEBUG
				if (m_itEnd0 != that.m_itEnd0)
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Titerator0
				params_type;

			linq_concat_iterator(Titerator0 && it0, Titerator1 && it1, Titerator0 const & itEnd0)
				: m_it0(std::forward<Titerator0>(it0))
				, m_it1(std::forward<Titerator1>(it1))
				, m_itEnd0(itEnd0)
			{}
			linq_concat_iterator & operator = (linq_concat_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_it0 = that.m_it0;
				m_it1 = that.m_it1;
				return *this;
			}
			linq_concat_iterator & operator ++ ()
			{
				if (m_it0 != m_itEnd0)
				{
					++m_it0;
				}
				else
				{
					++m_it1;
				}
				return *this;
			}
			decltype(auto) operator * () const
			{
				if (m_it0 != m_itEnd0)
				{
					return *m_it0;
				}
				else
				{
					return *m_it1;
				}
			}
			bool operator == (linq_concat_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_it0 == that.m_it0 && m_it1 == that.m_it1;
			}
			bool operator != (linq_concat_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_it0 != that.m_it0 || m_it1 != that.m_it1;
			}
		};

		template <typename Titerator, typename Tfunc>
		class linq_skip_while_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, typename Titerator::value_type>
		{
			Titerator const m_itEnd;
			Tfunc const m_func;

			void debug_check(linq_skip_while_iterator const & that) const
			{
#ifdef _DEBUG
				if (m_itEnd != that.m_itEnd ||
					typeid(m_func) != typeid(that.m_func))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tfunc
				params_type;

			template <typename Tcontainer>
			linq_skip_while_iterator(Titerator && itCurrent, Tcontainer const & container, Tfunc const & func)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itEnd(container.end())
				, m_func(func)
			{
				for (; m_itCurrent != m_itEnd; ++m_itCurrent)
				{
					if (!m_func(*m_itCurrent))
					{
						break;
					}
				}
			}
			linq_skip_while_iterator & operator = (linq_skip_while_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			bool operator == (linq_skip_while_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent == that.m_itCurrent;
			}
			bool operator != (linq_skip_while_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent != that.m_itCurrent;
			}
		};
		template <typename Titerator, typename Tfunc>
		class linq_take_while_iterator
			: public linq_iterator<Titerator>
			, public std::iterator<std::forward_iterator_tag, typename Titerator::value_type>
		{
			Titerator const m_itEnd;
			Tfunc const m_func;

			void debug_check(linq_take_while_iterator const & that) const
			{
#ifdef _DEBUG
				if (m_itEnd != that.m_itEnd ||
					typeid(m_func) != typeid(that.m_func))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tfunc
				params_type;

			template <typename Tcontainer>
			linq_take_while_iterator(Titerator && itCurrent, Tcontainer const & container, Tfunc const & func)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itEnd(container.end())
				, m_func(func)
			{
				if (m_itCurrent != m_itEnd && !m_func(*m_itCurrent))
				{
					m_itCurrent = m_itEnd;
				}
			}
			linq_take_while_iterator & operator = (linq_take_while_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			bool operator == (linq_take_while_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent == that.m_itCurrent;
			}
			bool operator != (linq_take_while_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_itCurrent != that.m_itCurrent;
			}
			linq_take_while_iterator & operator ++ ()
			{
				if (++m_itCurrent != m_itEnd && !m_func(*m_itCurrent))
				{
					m_itCurrent = m_itEnd;
				}
				return *this;
			}
		};
		template <typename Titerator0, typename Titerator1, typename Tfunc, typename Treturn>
		class linq_zip_iterator
			: public std::iterator<std::forward_iterator_tag, typename std::remove_reference<Treturn>::type>
		{
			Titerator0 m_it0;
			Titerator1 m_it1;
			Tfunc const m_func;

			void debug_check(linq_zip_iterator const & that) const
			{
#ifdef _DEBUG
				if (typeid(m_func) != typeid(that.m_func))
				{
					__debugbreak();
				}
#endif
			}
		public:
			typedef Tfunc
				params_type;

			linq_zip_iterator(Titerator0 && it0, Titerator1 && it1, Tfunc const & func)
				: m_it0(std::forward<Titerator0>(it0))
				, m_it1(std::forward<Titerator1>(it1))
				, m_func(func)
			{}

			bool operator == (linq_zip_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_it0 == that.m_it0 && m_it1 == that.m_it1;
			}
			bool operator != (linq_zip_iterator const & that) const
			{
				DEBUG_CHECK(that);
				return m_it0 != that.m_it0 || m_it1 != that.m_it1;
			}

			Treturn operator * () const
			{
				static_assert(std::is_same<Treturn, decltype(m_func(*m_it0, *m_it1))>::value, "Mismatch of return type 'Treturn'");
				return m_func(*m_it0, *m_it1);
			}

			linq_zip_iterator & operator = (linq_zip_iterator const & that)
			{
				DEBUG_CHECK(that);
				m_it0 = that.m_it0;
				m_it1 = that.m_it1;
				return *this;
			}

			linq_zip_iterator & operator ++ ()
			{
				++m_it0;
				++m_it1;
				return *this;
			}
		};

		template <typename Tcontainer>
		class linq_enumerable
			: public Tcontainer
		{
		public:
			typedef Tcontainer
				container_type;

			template <typename... Targs>
			linq_enumerable(Targs && ... args)
				: Tcontainer(std::forward<Targs>(args)...)
			{}

			template <typename Tvalue, typename Taccumulate>
			Tvalue aggregate(Tvalue value, Taccumulate && fnAccumulate) const
			{
				return std::accumulate(Tcontainer::begin(), Tcontainer::end(), value, std::forward<Taccumulate>(fnAccumulate));
			}
			template <template <typename T> class Taccumulate, typename Tvalue>
			Tvalue aggregate(Tvalue value) const
			{
				return aggregate(value, Taccumulate<Tvalue>());
			}
			template <typename Tfunc>
			bool all(Tfunc && func) const
			{
				return std::all_of(Tcontainer::begin(), Tcontainer::end(), std::forward<Tfunc>(func));
			}
			bool any() const
			{
				return Tcontainer::begin() != Tcontainer::end();
			}
			template <typename Tfunc>
			bool any(Tfunc && func) const
			{
				return std::any_of(Tcontainer::begin(), Tcontainer::end(), std::forward<Tfunc>(func));
			}
			template <typename Treturn, typename Tget>
			Treturn average(Tget && fnGet) const
			{
				auto const tuple = aggregate(std::make_tuple(Treturn(0), 0), [fnGet](auto const & accumulator, auto const & obj)
				{
					return std::make_tuple(static_cast<Treturn>(std::get<0>(accumulator) + fnGet(obj)), std::get<1>(accumulator) + 1);
				});
				return static_cast<Treturn>(std::get<0>(tuple)) / static_cast<Treturn>(std::get<1>(tuple));
			}
			template <typename Treturn>
			Treturn average() const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Tvalue;
				return average<Treturn>(std::identity<Tvalue>());
			}
			template <typename Tvalue, typename Tfunc>
			bool contains(Tvalue value, Tfunc && fnEqual) const
			{
				return any([value, fnEqual](auto const & obj)
				{
					return fnEqual(obj, value);
				});
			}
			template <typename Tvalue>
			bool contains(Tvalue value) const
			{
				return contains(value, std::equal_to<Tvalue>());
			}
			template <typename Tfunc>
			decltype(auto) count(Tfunc && func) const
			{
				return std::count_if(Tcontainer::begin(), Tcontainer::end(), std::forward<Tfunc>(func));
			}
			decltype(auto) count() const
			{
				return count([](auto const &)
				{
					return true;
				});
			}
			decltype(auto) element_at(size_t uIndex) const
			{
				for (auto it = Tcontainer::begin(), itEnd = Tcontainer::end(); it != itEnd; ++it)
				{
					if (uIndex-- == 0)
					{
						return *it;
					}
				}
				throw std::exception();
			}
			decltype(auto) element_at_or_default(size_t uIndex) const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Treturn;

				for (auto it = Tcontainer::begin(), itEnd = Tcontainer::end(); it != itEnd; ++it)
				{
					if (uIndex-- == 0)
					{
						return Treturn(*it);
					}
				}
				return Treturn();
			}
			template <typename Tfunc>
			decltype(auto) first(Tfunc && func) const
			{
				auto const itBegin = Tcontainer::begin();
				auto const itEnd = Tcontainer::end();

				auto const it = std::find_if(itBegin, itEnd, std::forward<Tfunc>(func));

				if (it != itEnd)
				{
					return *it;
				}
				else
				{
					throw std::exception();
				}
			}
			decltype(auto) first() const
			{
				return first([](auto const &)
				{
					return true;
				});
			}
			template <typename Tfunc>
			decltype(auto) first_or_default(Tfunc && func) const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Treturn;

				auto const itBegin = Tcontainer::begin();
				auto const itEnd = Tcontainer::end();

				auto const it = std::find_if(itBegin, itEnd, std::forward<Tfunc>(func));

				if (it != itEnd)
				{
					return Treturn(*it);
				}
				else
				{
					return Treturn();
				}
			}
			decltype(auto) first_or_default() const
			{
				return first_or_default([](auto const &)
				{
					return true;
				});
			}
			template <typename Tfunc>
			decltype(auto) last(Tfunc && func) const
			{
				auto const itBegin = Tcontainer::begin();
				auto const itEnd = Tcontainer::end();

				auto itReturn = std::find_if(itBegin, itEnd, func);
				{
					if (itReturn == itEnd)
					{
						throw std::exception();
					}
					for (auto it = itReturn; ++it != itEnd; )
					{
						if (func(*it))
						{
							itReturn = it;
						}
					}
				}
				return *itReturn;
			}
			decltype(auto) last() const
			{
				return last([](auto const &)
				{
					return true;
				});
			}
			template <typename Tfunc>
			decltype(auto) last_or_default(Tfunc && func) const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Treturn;

				auto const itBegin = Tcontainer::begin();
				auto const itEnd = Tcontainer::end();

				auto itReturn = std::find_if(itBegin, itEnd, func);
				{
					if (itReturn == itEnd)
					{
						return Treturn();
					}
					for (auto it = itReturn; ++it != itEnd; )
					{
						if (func(*it))
						{
							itReturn = it;
						}
					}
				}
				return Treturn(*itReturn);
			}
			decltype(auto) last_or_default() const
			{
				return last_or_default([](auto const &)
				{
					return true;
				});
			}
			template <typename Tless>
			decltype(auto) max(Tless && fnLess) const
			{
				return *std::max_element(Tcontainer::begin(), Tcontainer::end(), std::forward<Tless>(fnLess));
			}
			decltype(auto) max() const
			{
				return *std::max_element(Tcontainer::begin(), Tcontainer::end());
			}
			template <typename Tless>
			decltype(auto) min(Tless && fnLess) const
			{
				return *std::min_element(Tcontainer::begin(), Tcontainer::end(), std::forward<Tless>(fnLess));
			}
			decltype(auto) min() const
			{
				return *std::min_element(Tcontainer::begin(), Tcontainer::end());
			}
			template <typename Treturn, typename Tget>
			Treturn product(Tget && fnGet) const
			{
				return select(std::forward<Tget>(fnGet)).product<Treturn>();
			}
			template <typename Treturn>
			Treturn product() const
			{
				return aggregate<std::multiplies>(Treturn(1));
			}
			template <typename Tthat, typename Tequal>
			bool sequential_equal(Tthat const & that, Tequal && fnEqual) const
			{
				auto const itThisBegin = begin();
				auto const itThisEnd = end();
				auto const itThatBegin = that.begin();
				auto const itThatEnd = that.end();
				auto itThis = itThisBegin;
				auto itThat = itThatBegin;
				{
					for (; itThis != itThisEnd && itThat != itThatEnd; ++itThis, ++itThat)
					{
						if (fnEqual(*itThis, *itThat) == false)
						{
							return false;
						}
					}
				}
				return itThis == itThisEnd && itThat == itThatEnd;
			}
			template <typename Tthat>
			bool sequential_equal(Tthat const & that) const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Tvalue;
				return sequential_equal(that, std::equal_to<Tvalue>());
			}
			template <typename Treturn, typename Tget>
			Treturn sum(Tget && fnGet) const
			{
				return select(std::forward<Tget>(fnGet)).sum<Treturn>();
			}
			template <typename Treturn>
			Treturn sum() const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Tvalue;
				return aggregate<std::plus>(Treturn(0));
			}

			template <typename Tcast>
			decltype(auto) cast() const
			{
				return select([](auto && obj)->Tcast
				{
					return (Tcast)(std::forward<decltype(obj)>(obj));
				});
			}
			template <typename Tthat>
			decltype(auto) concat(Tthat && that) const
			{
				typedef decltype(Tcontainer::begin())
					Titerator0;
				typedef decltype(that.begin())
					Titerator1;
				typedef linq_concat_iterator<Titerator0, Titerator1>
					Tconcat_iterator;
				typedef linq_pair_container<Tcontainer, Tthat, Tconcat_iterator>
					Tconcat_container;
				typedef linq_enumerable<Tconcat_container>
					Tconcat_enumerable;
				return Tconcat_enumerable(Tcontainer(*this), std::forward<Tthat>(that), Tcontainer::end());
			}
			// TODO: distinct
			template <typename Tthat, typename Tequal>
			decltype(auto) except(Tthat const & that, Tequal && fnEqual) const
			{
				return where([itBegin = that.begin(), itEnd = that.end(), fnEqual](auto const & valueCurrent)
				{
					return !std::any_of(itBegin, itEnd, std::bind2nd(fnEqual, valueCurrent));
				});
			}
			template <typename Tthat>
			decltype(auto) except(Tthat const & that) const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Tvalue;
				return except(that, std::equal_to<Tvalue>());
			}
			template <typename Tthat, typename Tequal>
			decltype(auto) intersection(Tthat const & that, Tequal && fnEqual) const
			{
				return where([itBegin = that.begin(), itEnd = that.end(), fnEqual](auto const & valueCurrent)
				{
					return std::any_of(itBegin, itEnd, std::bind2nd(fnEqual, valueCurrent));
				});
			}
			template <typename Tthat>
			decltype(auto) intersection(Tthat const & that) const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Tvalue;
				return intersection(that, std::equal_to<Tvalue>());
			}
			template <typename Tfunc>
			decltype(auto) select(Tfunc && func) const
			{
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef decltype(func(*Tcontainer::begin()))
					Treturn;
				typedef linq_select_iterator<Titerator, Tfunc, Treturn>
					Tselect_iterator;
				typedef linq_container<Tcontainer, Tselect_iterator>
					Tselect_container;
				typedef linq_enumerable<Tselect_container>
					Tselect_enumerable;
				return Tselect_enumerable(Tcontainer(*this), std::forward<Tfunc>(func));
			}
			template <typename Tmany, typename Tselect>
			decltype(auto) select_many(Tmany && fnMany, Tselect && fnSelect) const
			{
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef decltype(fnMany(*Tcontainer::begin()).begin())
					Tmany_iterator;
				typedef std::tuple<Tmany, Tselect>
					Tparams;
				typedef decltype(fnSelect(*Tcontainer::begin(), *fnMany(*Tcontainer::begin()).begin()))
					Treturn;
				typedef linq_select_many_iterator<Titerator, Tmany_iterator, Tparams, Treturn>
					Tselect_many_iterator;
				typedef linq_container<Tcontainer, Tselect_many_iterator>
					Tselect_many_container;
				typedef linq_enumerable<Tselect_many_container>
					Tselect_many_enumerable;
				return Tselect_many_enumerable(Tcontainer(*this), Tparams(std::forward<Tmany>(fnMany), std::forward<Tselect>(fnSelect)));
			}
			template <typename Tmany>
			decltype(auto) select_many(Tmany && fnMany) const
			{
				return select_many(std::forward<Tmany>(fnMany), [](auto, auto objMany)
				{
					return objMany;
				});
			}
			template <typename Tfunc>
			decltype(auto) skip_while(Tfunc && func) const
			{
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef linq_skip_while_iterator<Titerator, Tfunc>
					Tskip_while_iterator;
				typedef linq_container<Tcontainer, Tskip_while_iterator>
					Tskip_while_container;
				typedef linq_enumerable<Tskip_while_container>
					Tskip_while_enumerable;
				return Tskip_while_enumerable(Tcontainer(*this), std::forward<Tfunc>(func));
			}
			template <typename Tfunc>
			decltype(auto) take_while(Tfunc && func) const
			{
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef linq_take_while_iterator<Titerator, Tfunc>
					Ttake_while_iterator;
				typedef linq_container<Tcontainer, Ttake_while_iterator>
					Ttake_while_container;
				typedef linq_enumerable<Ttake_while_container>
					Ttake_while_enumerable;
				return Ttake_while_enumerable(Tcontainer(*this), std::forward<Tfunc>(func));
			}
			template <typename Tthat, typename Touter_key_selector, typename Tinner_key_selector, typename Tresult_selector>
			decltype(auto) group_join(Tthat && that, Touter_key_selector && fnOuterKeySelector, Tinner_key_selector && fnInnerKeySelector, Tresult_selector && fnResultSelector)
			{
				auto linqInner = from(std::forward<Tthat>(that));
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef decltype(linqInner)
					Tinner;
				typedef std::tuple<Tinner, Touter_key_selector, Tinner_key_selector, Tresult_selector>
					Tparams;
				typedef decltype(fnResultSelector(*Tcontainer::begin(), linqInner))
					Treturn;
				typedef linq_group_join_iterator<Titerator, Tparams, Treturn>
					Tgroup_join_iterator;
				typedef linq_container<Tcontainer, Tgroup_join_iterator>
					Tgroup_join_container;
				typedef linq_enumerable<Tgroup_join_container>
					Tgroup_join_enumerable;
				return Tgroup_join_enumerable(Tcontainer(*this), Tparams(std::move(linqInner), std::forward<Touter_key_selector>(fnOuterKeySelector), std::forward<Tinner_key_selector>(fnInnerKeySelector), std::forward<Tresult_selector>(fnResultSelector)));
			}
			template <typename Tfunc>
			decltype(auto) where(Tfunc && func) const
			{
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef linq_where_iterator<Titerator, Tfunc>
					Twhere_iterator;
				typedef linq_container<Tcontainer, Twhere_iterator>
					Twhere_container;
				typedef linq_enumerable<Twhere_container>
					Twhere_enumerable;
				return Twhere_enumerable(Tcontainer(*this), std::forward<Tfunc>(func));
			}
			template <typename Tget, typename Tequal>
			decltype(auto) group_by(Tget && fnGet, Tequal && fnEqual) const
			{
				typedef std::tuple<Tget, Tequal>
					Tparams;
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef linq_group_by_iterator<Titerator, Tparams>
					Tgroup_by_iterator;
				typedef linq_container<Tcontainer, Tgroup_by_iterator>
					Tgroup_by_container;
				typedef linq_enumerable<Tgroup_by_container>
					Tgroup_by_enumerable;
				return Tgroup_by_enumerable(Tcontainer(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tequal>(fnEqual)));
			}
			template <typename Tget>
			decltype(auto) group_by(Tget && fnGet) const
			{
				typedef decltype(fnGet(*Tcontainer::begin()))
					Tvalue;
				return group_by(std::forward<Tget>(fnGet), std::equal_to<Tvalue>());
			}
			template <typename Tget, typename Tequal>
			decltype(auto) uniq(Tget && fnGet, Tequal && fnEqual) const
			{
				typedef std::tuple<Tget, Tequal>
					Tparams;
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef linq_uniq_iterator<Titerator, Tparams>
					Tuniq_iterator;
				typedef linq_container<Tcontainer, Tuniq_iterator>
					Tuniq_container;
				typedef linq_enumerable<Tuniq_container>
					Tuniq_enumerable;
				return Tuniq_enumerable(Tcontainer(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tequal>(fnEqual)));
			}
			template <typename Tget>
			decltype(auto) uniq(Tget && fnGet) const
			{
				typedef decltype(fnGet(*Tcontainer::begin()))
					Tvalue;
				return uniq(std::forward<Tget>(fnGet), std::equal_to<Tvalue>());
			}
			template <typename Tget, typename Tcompare>
			decltype(auto) order_by(Tget && fnGet, Tcompare && fnCompare) const
			{
				typedef std::tuple<Tget, Tcompare>
					Tparams;
				typedef decltype(Tcontainer::begin())
					Titerator;
				typedef linq_order_by_iterator<Titerator, Tparams>
					Torder_by_iterator;
				typedef linq_container<Tcontainer, Torder_by_iterator>
					Torder_by_container;
				typedef linq_enumerable<Torder_by_container>
					Torder_by_enumerable;
				return Torder_by_enumerable(Tcontainer(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tcompare>(fnCompare)));
			}
			template <template <typename T> class Tcompare, typename Tget>
			decltype(auto) order_by(Tget && fnGet) const
			{
				typedef typename std::decay<decltype(fnGet(*Tcontainer::begin()))>::type
					Tvalue;
				return order_by(std::forward<Tget>(fnGet), Tcompare<Tvalue>());
			}
			template <template <typename T> class Tcompare>
			decltype(auto) order_by() const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Tvalue;
				return order_by(std::identity<Tvalue>(), Tcompare<Tvalue>());
			}
			template <typename Tthat, typename Tfunc>
			decltype(auto) zip(Tthat && that, Tfunc && func)
			{
				typedef decltype(Tcontainer::begin())
					Titerator0;
				typedef decltype(that.begin())
					Titerator1;
				typedef decltype(func(*Tcontainer::begin(), *that.begin()))
					Treturn;
				typedef linq_zip_iterator < Titerator0, Titerator1, Tfunc, Treturn>
					Tzip_iterator;
				typedef linq_pair_container<Tcontainer, Tthat, Tzip_iterator>
					Tzip_container;
				typedef linq_enumerable<Tzip_container>
					Tzip_enumerable;
				return Tzip_enumerable(Tcontainer(*this), std::forward<Tthat>(that), std::forward<Tfunc>(func));
			}

			//

			template <template <typename T, typename... Trest> class Ccontainer, typename... Trest, typename... Targs>
			decltype(auto) to(Targs && ... args) const
			{
				typedef typename std::decay<decltype(*Tcontainer::begin())>::type
					Tvalue;
				return Ccontainer<Tvalue, Trest...>(Tcontainer::begin(), Tcontainer::end(), std::forward<Targs>(args)...);
			}
			template <class Ccontainer, typename... Targs>
			decltype(auto) to(Targs && ... args) const
			{
				return Ccontainer(Tcontainer::begin(), Tcontainer::end(), std::forward<Targs>(args)...);
			}
		};
		template <typename Tcontainer>
		decltype(auto) from(Tcontainer & container)
		{
			typedef linq_from<Tcontainer &>
				Tfrom;

			return linq_enumerable<Tfrom>(container);
		}
		template <typename Tcontainer>
		linq_enumerable<Tcontainer> const & from(linq_enumerable<Tcontainer> const & that)
		{
			return that;
		}
		template <typename Tcontainer>
		linq_enumerable<Tcontainer> && from(linq_enumerable<Tcontainer> && that)
		{
			return std::forward<linq_enumerable<Tcontainer>>(that);
		}
	}
}

#pragma pop_macro("DEBUG_CHECK")
