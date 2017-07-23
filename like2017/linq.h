#pragma once

/* Author: jrmwng @ 2017 */

#include <algorithm>
#include <iterator>
#include <numeric>
#include <tuple>
#include <type_traits>

namespace jrmwng
{
	namespace linq
	{
		template <typename Tcontainer, typename Tparams, typename Titerator>
		class linq_container
		{
			Tcontainer const m_container;
			Tparams const m_params;
		public:
			linq_container(Tcontainer && container, Tparams && params)
				: m_container(std::forward<Tcontainer>(container))
				, m_params(std::forward<Tparams>(params))
			{}

			Titerator begin() const
			{
				return Titerator(m_container.begin(), m_container, Tparams(m_params));
			}
			Titerator end() const
			{
				return Titerator(m_container.end(), m_container, Tparams(m_params));
			}
		};
		template <typename Tcontainer0, typename Tcontainer1, typename Titerator>
		class linq_concat_container
		{
			Tcontainer0 const m_container0;
			Tcontainer1 const m_container1;
		public:
			linq_concat_container(Tcontainer0 && container0, Tcontainer1 && container1)
				: m_container0(std::forward<Tcontainer0>(container0))
				, m_container1(std::forward<Tcontainer1>(container1))
			{}

			Titerator begin() const
			{
				return Titerator(m_container0.begin(), m_container0, m_container1);
			}
			Titerator end() const
			{
				return Titerator(m_container1.end(), m_container0, m_container1);
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
			: public std::iterator_traits<Titerator>
		{
		protected:
			Titerator m_itCurrent;

		public:
			typedef std::forward_iterator_tag iterator_category;

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

		template <typename Titerator, typename Tfunc>
		class linq_select_iterator
			: public linq_iterator<Titerator>
		{
			Tfunc const m_func;
		public:
			template <typename Tcontainer>
			linq_select_iterator(Titerator && itCurrent, Tcontainer const & container, Tfunc && func)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_func(std::forward<Tfunc>(func))
			{}
			decltype(auto) operator * () const
			{
				return m_func(*m_itCurrent);
			}
			linq_select_iterator & operator = (linq_select_iterator const & that)
			{
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			linq_select_iterator & operator ++ ()
			{
				++m_itCurrent;
				return *this;
			}
		};
		template <typename Titerator, typename Tfunc>
		class linq_where_iterator
			: public linq_iterator<Titerator>
		{
			Titerator const m_itEnd;
			Tfunc const m_func;
		public:
			template <typename Tcontainer>
			linq_where_iterator(Titerator && itCurrent, Tcontainer const & container, Tfunc && func)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itEnd(container.end())
				, m_func(std::forward<Tfunc>(func))
			{
				for (; m_itCurrent != m_itEnd && !m_func(*m_itCurrent); ++m_itCurrent)
				{
					// NOP
				}
			}
			linq_where_iterator & operator = (linq_where_iterator const & that)
			{
				m_itCurrent = that.m_itCurrent;
				return *this;
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
		{
			Titerator m_itNext;
			Titerator const m_itBegin;
			Titerator const m_itEnd;
			Tparams const m_params;
		public:
			template <typename Tcontainer>
			linq_group_by_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams && params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itNext(itCurrent)
				, m_itBegin(container.begin())
				, m_itEnd(container.end())
				, m_params(std::forward<Tparams>(params))
			{
				if (itCurrent != m_itEnd)
				{
					auto const & fnGet = std::get<0>(params);
					auto const & fnEqual = std::get<1>(params);

					auto const valueCurrent = fnGet(*itCurrent);

					Titerator itNext = itCurrent;
					{
						while (++itNext != m_itEnd && fnEqual(valueCurrent, fnGet(*itNext)))
						{
							// NOP
						}
					}
					m_itNext = itNext;
				}
			}
			linq_group_by_iterator & operator = (linq_group_by_iterator const & that)
			{
				m_itCurrent = that.m_itCurrent;
				m_itNext = that.m_itNext;
				return *this;
			}
			linq_group_by_iterator & operator ++ ()
			{
				m_itCurrent = m_itNext;

				if (m_itNext != m_itEnd)
				{
					auto const & fnGet = std::get<0>(m_params);
					auto const & fnEqual = std::get<1>(m_params);
					auto const valueCurrent = fnGet(*m_itNext);

					Titerator itNext = m_itNext;
					{
						while (++itNext != m_itEnd && fnEqual(valueCurrent, fnGet(*itNext)))
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
				using Trange = linq_range<Titerator>;
				using Trange_enumerable = linq_enumerable<Trange>;
				return Trange_enumerable(m_itCurrent, m_itNext);
			}
			Titerator begin() const
			{
				return m_itCurrent;
			}
			Titerator end() const
			{
				return m_itNext;
			}
		};
		template <typename Titerator, typename Tparams>
		class linq_order_by_iterator
			: public linq_iterator<Titerator>
		{
			Titerator const m_itBegin;
			Titerator const m_itEnd;
			Tparams const m_params;
		public:
			template <typename Tcontainer>
			linq_order_by_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams && params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itBegin(container.begin())
				, m_itEnd(container.end())
				, m_params(std::forward<Tparams>(params))
			{
				if (m_itCurrent != m_itEnd)
				{
					m_itCurrent = std::min_element(m_itBegin, m_itEnd, [this](auto const & left, auto const & right)
					{
						return std::get<1>(m_params)(
							std::get<0>(m_params)(left),
							std::get<0>(m_params)(right));
					});
					for (auto it = m_itBegin; it != itCurrent; ++it)
					{
						++*this;
					}
				}
			}
			linq_order_by_iterator & operator = (linq_order_by_iterator const & that)
			{
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			linq_order_by_iterator & operator ++ ()
			{
				auto const & fnGet = std::get<0>(m_params);
				auto const & fnCompare = std::get<1>(m_params);
				auto const valueKey = fnGet(*m_itCurrent);

				for (Titerator itCurrent = m_itCurrent; ++itCurrent != m_itEnd; )
				{
					auto valueCurrent = fnGet(*itCurrent);

					if (fnCompare(valueKey, valueCurrent) == fnCompare(valueCurrent, valueKey))
					{
						m_itCurrent = itCurrent;
						return *this;
					}
				}
				Titerator itCurrent = std::min_element(m_itBegin, m_itEnd, [&, this](auto const & left, auto const & right)
				{
					auto const valueLeft = fnGet(left);
					auto const valueRight = fnGet(right);

					return (fnCompare(valueKey, valueLeft) == fnCompare(valueKey, valueRight)) == fnCompare(valueLeft, valueRight);
				});
				if (fnCompare(valueKey, fnGet(*itCurrent)))
				{
					m_itCurrent = itCurrent;
				}
				else
				{
					m_itCurrent = m_itEnd;
				}
				return *this;
			}
		};

		template <typename Titerator>
		class linq_concat_iterator
			: public linq_iterator<Titerator>
		{
			Titerator const m_itEnd0;
			Titerator const m_itBegin1;
		public:
			template <typename Tcontainer0, typename Tcontainer1>
			linq_concat_iterator(Titerator && itCurrent, Tcontainer0 const & container0, Tcontainer1 const & container1)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itEnd0(container0.end())
				, m_itBegin1(container1.begin())
			{}
			linq_concat_iterator & operator = (linq_concat_iterator const & that)
			{
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
			linq_concat_iterator & operator ++ ()
			{
				if (++m_itCurrent == m_itEnd0)
				{
					m_itCurrent = m_itBegin1;
				}
				return *this;
			}
		};

		template <typename Titerator, typename Tparams>
		class linq_group_accumulate_iterator
			: public linq_iterator<Titerator>
		{
			Tparams const m_params;
		public:
			template <typename Tcontainer>
			linq_group_accumulate_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams && params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_params(std::forward<Tparams>(params))
			{}

			decltype(auto) operator * () const
			{
				auto const itBegin = m_itCurrent.begin();
				auto const itEnd = m_itCurrent.end();
				return std::accumulate(itBegin, itEnd, std::get<0>(m_params)(*itBegin), std::get<1>(m_params));
			}

			linq_group_accumulate_iterator & operator = (linq_group_accumulate_iterator const & that)
			{
				m_itCurrent = that.m_itCurrent;
				return *this;
			}
		};

		template <typename Tcontainer>
		class linq_group
			: public Tcontainer
		{
		public:
			template <typename... Targs>
			linq_group(Targs && ... args)
				: Tcontainer(std::forward<Targs>(args)...)
			{}

			template <typename Tvalue, typename Taccumulate>
			decltype(auto) aggregate(Tvalue value, Taccumulate && fnAccumulate) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Tparams = std::tuple<Tvalue, Taccumulate>;
				using Taccumulate_iterator = linq_group_accumulate_iterator<Titerator, Tparams>;
				using Taccumulate_container = linq_container<Tcontainer, Tparams, Taccumulate_iterator>;
				using Taccumulate_enumerable = linq_enumerable<Taccumulate_container>;
				return Taccumulate_enumerable(Tcontainer(*this), Tparams(value, std::forward<Taccumulate>(fnAccumulate)));
			}
			template <typename Tget>
			decltype(auto) count(Tget && fnGet) const
			{
				return aggregate(
					[](auto const &)
				{
					return 0;
				},
					[fnGet](auto const accumulator, auto const & obj)
				{
					return accumulator + (fnGet(obj) ? 1 : 0);
				});
			}
			template <typename Treturn, typename Tget>
			Treturn sum(Tget && fnGet) const
			{
				return aggregate(
					[](auto const &)
				{
					return Treturn(0);
				},
					[fnGet](Treturn const accumulator, auto const & obj)->Treturn
				{
					return static_cast<Treturn>(accumulator + fnGet(obj));
				});
			}
			template <typename Treturn, typename Tget>
			Treturn average(Tget && fnGet) const
			{
				using Tvalue = std::decay_t<decltype(std::forward<Tget>(fnGet)(*(*Tcontainer::begin()).begin()))>;

				return aggregate(
					[](auto const &)
				{
					return std::make_tuple(Treturn(0), 0);
				},
					[fnGet](auto const accumulator, auto const & obj)
				{
					return std::make_tuple(static_cast<Treturn>(std::get<0>(accumulator) + fnGet(obj)), std::get<1>(accumulator) + 1);
				})
					.select([](auto const & tuple)
				{
					return static_cast<Treturn>(std::get<0>(tuple) / std::get<1>(tuple));
				});
			}

			template <typename Tget, typename Tcompare>
			decltype(auto) min(Tget && fnGet, Tcompare && fnCompare) const
			{
				return aggregate(
					[fnGet](auto const &obj)
				{
					return fnGet(obj);
				},
					[fnGet, fnCompare](auto const accumulator, auto const & obj)
				{
					return std::min(accumulator, fnGet(obj), fnCompare);
				});
			}
			template <typename Tget>
			decltype(auto) min(Tget && fnGet) const
			{
				using Tcompare = std::less<decltype(std::forward<Tget>(fnGet)(*(*Tcontainer::begin()).begin()))>;
				return min(std::forward<Tget>(fnGet), Tcompare());
			}
			template <typename Tget, typename Tcompare>
			decltype(auto) max(Tget && fnGet, Tcompare && fnCompare) const
			{
				return aggregate(
					[fnGet](auto const &obj)
				{
					return fnGet(obj);
				},
					[fnGet, fnCompare](auto const accumulator, auto const & obj)
				{
					return std::max(accumulator, fnGet(obj), fnCompare);
				});
			}
			template <typename Tget>
			decltype(auto) max(Tget && fnGet) const
			{
				using Tcompare = std::less<decltype(std::forward<Tget>(fnGet)(*(*Tcontainer::begin()).begin()))>;
				return max(std::forward<Tget>(fnGet), Tcompare());
			}
		};

		template <typename Tcontainer>
		class linq_enumerable
			: public Tcontainer
		{
		public:
			template <typename... Targs>
			linq_enumerable(Targs && ... args)
				: Tcontainer(std::forward<Targs>(args)...)
			{}

			template <typename Tvalue, typename Taccumulate>
			Tvalue aggregate(Tvalue value, Taccumulate && fnAccumulate) const
			{
				return std::accumulate(Tcontainer::begin(), Tcontainer::end(), value, std::forward<Taccumulate>(fnAccumulate));
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
				using Tvalue = std::decay_t<decltype(*Tcontainer::begin())>;
				return average<Treturn>(std::identity<Tvalue>());
			}
			template <typename Tvalue, typename Tfunc>
			bool contains(Tvalue const & value, Tfunc && fnEqual) const
			{
				return any([value, fnEqual](auto const & obj)
				{
					return fnEqual(obj, value);
				});
			}
			template <typename Tvalue>
			bool contains(Tvalue const & value) const
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
			template <typename Treturn, typename Tget>
			Treturn sum(Tget && fnGet) const
			{
				return aggregate(Treturn(0), [fnGet](Treturn accumulator, auto const & obj)->Treturn
				{
					return static_cast<Treturn>(accumulator + fnGet(obj));
				});
			}
			template <typename Treturn>
			Treturn sum() const
			{
				using Tvalue = std::decay_t<decltype(*Tcontainer::begin())>;
				return sum<Treturn>(std::identity<Tvalue>());
			}

			template <typename Tcast>
			decltype(auto) cast() const
			{
				return select([](auto && obj)->Tcast
				{
					return (Tcast)(std::forward<decltype(obj)>(obj));
				});
			}
			template <typename Tcontainer1>
			decltype(auto) concat(linq_enumerable<Tcontainer1> && container1) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Tconcat_iterator = linq_concat_iterator<Titerator>;
				using Tconcat_container = linq_concat_container<Tcontainer, linq_enumerable<Tcontainer1>, Tconcat_iterator>;
				using Tconcat_enumerable = linq_enumerable<Tconcat_container>;
				return Tconcat_enumerable(Tcontainer(*this), std::forward<linq_enumerable<Tcontainer1>>(container1));
			}
			template <typename Tfunc>
			decltype(auto) select(Tfunc && func) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Tselect_iterator = linq_select_iterator<Titerator, Tfunc>;
				using Tselect_container = linq_container<Tcontainer, Tfunc, Tselect_iterator>;
				using Tselect_enumerable = linq_enumerable<Tselect_container>;
				return Tselect_enumerable(Tcontainer(*this), std::forward<Tfunc>(func));
			}
			template <typename Tfunc>
			decltype(auto) where(Tfunc && func) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Twhere_iterator = linq_where_iterator<Titerator, Tfunc>;
				using Twhere_container = linq_container<Tcontainer, Tfunc, Twhere_iterator>;
				using Twhere_enumerable = linq_enumerable<Twhere_container>;
				return Twhere_enumerable(Tcontainer(*this), std::forward<Tfunc>(func));
			}
			template <typename Tget, typename Tequal>
			decltype(auto) group_by(Tget && fnGet, Tequal && fnEqual) const
			{
				using Tparams = std::tuple<Tget, Tequal>;
				using Titerator = decltype(Tcontainer::begin());
				using Tgroup_by_iterator = linq_group_by_iterator<Titerator, Tparams>;
				using Tgroup_by_container = linq_container<Tcontainer, Tparams, Tgroup_by_iterator>;
				using Tgroup_by_enumerable = linq_group<Tgroup_by_container>;
				return Tgroup_by_enumerable(Tcontainer(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tequal>(fnEqual)));
			}
			template <typename Tget>
			decltype(auto) group_by(Tget && fnGet) const
			{
				using Tequal = std::equal_to<decltype(fnGet(*Tcontainer::begin()))>;
				return group_by(std::forward<Tget>(fnGet), Tequal());
			}
			template <typename Tget, typename Tcompare>
			decltype(auto) order_by(Tget && fnGet, Tcompare && fnCompare) const
			{
				using Tparams = std::tuple<Tget, Tcompare>;
				using Titerator = decltype(Tcontainer::begin());
				using Torder_by_iterator = linq_order_by_iterator<Titerator, Tparams>;
				using Torder_by_container = linq_container<Tcontainer, Tparams, Torder_by_iterator>;
				using Torder_by_enumerable = linq_enumerable<Torder_by_container>;
				return Torder_by_enumerable(Tcontainer(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tcompare>(fnCompare)));
			}
			template <typename Tget>
			decltype(auto) order_by(Tget && fnGet) const
			{
				using Tcompare = std::less<decltype(fnGet(*Tcontainer::begin()))>;
				return order_by(std::forward<Tget>(fnGet), Tcompare());
			}
		};
		template <typename Tcontainer>
		decltype(auto) from(Tcontainer & container)
		{
			using Tfrom = linq_from<Tcontainer &>;

			return linq_enumerable<Tfrom>(container);
		}
	}
}
