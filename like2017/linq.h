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

			linq_iterator(Titerator itCurrent)
				: m_itCurrent(itCurrent)
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
		class linq_where_iterator
			: public linq_iterator<Titerator>
		{
			Titerator const m_itEnd;
			Tfunc const m_func;
		public:
			template <typename Tcontainer>
			linq_where_iterator(Titerator itCurrent, Tcontainer const & container, Tfunc && func)
				: linq_iterator<Titerator>(itCurrent)
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
		template <typename Titerator, typename Tparams, template <typename Tcontainer> class Clinq>
		class linq_group_by_iterator
			: public linq_iterator<Titerator>
		{
			Titerator m_itNext;
			Titerator const m_itBegin;
			Titerator const m_itEnd;
			Tparams const m_params;
		public:
			template <typename Tcontainer>
			linq_group_by_iterator(Titerator itCurrent, Tcontainer const & container, Tparams && params)
				: linq_iterator<Titerator>(itCurrent)
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
				using Tlinq = Clinq<Trange>;
				return Tlinq(m_itCurrent, m_itNext);
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
			linq_order_by_iterator(Titerator itCurrent, Tcontainer const & container, Tparams && params)
				: linq_iterator<Titerator>(itCurrent)
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

		template <typename Titerator, typename Tparams>
		class linq_group_accumulate_iterator
			: public linq_iterator<Titerator>
		{
			Tparams const m_params;
		public:
			template <typename Tcontainer>
			linq_group_accumulate_iterator(Titerator itCurrent, Tcontainer const & container, Tparams && params)
				: linq_iterator<Titerator>(itCurrent)
				, m_params(std::forward<Tparams>(params))
			{}

			decltype(auto) operator * () const
			{
				return std::accumulate(m_itCurrent.begin(), m_itCurrent.end(), std::get<0>(m_params), std::get<1>(m_params));
			}
		};

		template <typename Tcontainer, template <typename Tcontainer> class Clinq>
		class linq_group
			: public Tcontainer
		{
		public:
			template <typename... Targs>
			linq_group(Targs && ... args)
				: Tcontainer(std::forward<Targs>(args)...)
			{}

			template <typename Tvalue, typename Taccumulate>
			decltype(auto) accumulate(Tvalue value, Taccumulate && fnAccumulate) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Tparams = std::tuple<Tvalue, Taccumulate>;
				using Taccumulate_iterator = linq_group_accumulate_iterator<Titerator, Tparams>;
				using Taccumulate_container = linq_container<Tcontainer, Tparams, Taccumulate_iterator>;
				using Tlinq = Clinq<Taccumulate_container>;
				return Tlinq(Tcontainer(*this), Tparams(value, std::forward<Taccumulate>(fnAccumulate)));
			}
			template <typename Tget>
			decltype(auto) sum(Tget && fnGet) const
			{
				using Tvalue = std::decay_t<decltype(std::forward<Tget>(fnGet)(*(*Tcontainer::begin()).begin()))>;

				return accumulate(Tvalue(0), [fnGet](auto const accumulator, auto const & obj)
				{
					return accumulator + fnGet(obj);
				});
			}
			template <typename Tget, typename Tcompare>
			decltype(auto) min(Tget && fnGet, Tcompare && fnCompare) const
			{
				return accumulate(std::forward<Tget>(fnGet)(*(*Tcontainer::begin()).begin()), [fnGet, fnCompare](auto const accumulator, auto const & obj)
				{
					return std::min(accumulator, fnGet(obj), fnCompare);
				});
			}
			template <typename Tget, typename Tcompare>
			decltype(auto) max(Tget && fnGet, Tcompare && fnCompare) const
			{
				return accumulate(std::forward<Tget>(fnGet)(*(*Tcontainer::begin()).begin()), [fnGet, fnCompare](auto const accumulator, auto const & obj)
				{
					return std::max(accumulator, fnGet(obj), fnCompare);
				});
			}
			decltype(auto) count() const
			{
				return accumulate(0, [](auto const accumulator, auto const & obj)
				{
					return 1 + accumulator;
				});
			}
		};

		template <typename Tcontainer>
		class linq
			: public Tcontainer
		{
		public:
			template <typename... Targs>
			linq(Targs && ... args)
				: Tcontainer(std::forward<Targs>(args)...)
			{}

			template <typename Tvalue, typename Taccumulate>
			Tvalue accumulate(Tvalue value, Taccumulate && fnAccumulate)
			{
				return std::accumulate(Tcontainer::begin(), Tcontainer::end(), value, std::forward<Taccumulate>(fnAccumulate));
			}
			template <typename Tget>
			decltype(auto) sum(Tget && fnGet)
			{
				using Tvalue = std::decay_t<decltype(std::forward<Tget>(fnGet)(*Tcontainer::begin()))>;
				return accumulate(Tvalue(0), [fnGet](Tvalue accumulator, auto const & obj)
				{
					return accumulator + fnGet(obj);
				});
			}

			template <typename Tfunc>
			decltype(auto) where(Tfunc && func) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Twhere_iterator = linq_where_iterator<Titerator, Tfunc>;
				using Twhere_container = linq_container<Tcontainer, Tfunc, Twhere_iterator>;
				using Tlinq = linq<Twhere_container>;
				return Tlinq(Tcontainer(*this), std::forward<Tfunc>(func));
			}
			template <typename Tget, typename Tequal>
			decltype(auto) group_by(Tget && fnGet, Tequal && fnEqual) const
			{
				using Tparams = std::tuple<Tget, Tequal>;
				using Titerator = decltype(Tcontainer::begin());
				using Tgroup_by_iterator = linq_group_by_iterator<Titerator, Tparams, jrmwng::linq::linq>;
				using Tgroup_by_container = linq_container<Tcontainer, Tparams, Tgroup_by_iterator>;
				using Tlinq = linq_group<Tgroup_by_container, jrmwng::linq::linq>;
				return Tlinq(Tcontainer(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tequal>(fnEqual)));
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
				using Tlinq = linq<Torder_by_container>;
				return Tlinq(Tcontainer(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tcompare>(fnCompare)));
			}
			template <typename Tget>
			decltype(auto) order_by(Tget && fnGet) const
			{
				using Tcompare = std::less<decltype(fnGet(*Tcontainer::begin()))>;
				return order_by(std::forward<Tget>(fnGet), Tcompare());
			}
		};
		template <typename Tcontainer>
		auto from(Tcontainer & container)
		{
			using Tfrom = linq_from<Tcontainer &>;

			return linq<Tfrom>(container);
		}
	}
}
