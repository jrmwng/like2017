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
		template <typename Titerator, typename Tmany_iterator, typename Tparams>
		class linq_select_many_iterator
			: public linq_iterator<Titerator>
		{
			Tmany_iterator m_itManyCurrent;
			Tmany_iterator m_itManyEnd;
			Titerator const m_itEnd;
			Tparams const m_params;
		public:
			template <typename Tcontainer>
			linq_select_many_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams && params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itEnd(container.end())
				, m_params(std::forward<Tparams>(params))
			{
				if (m_itCurrent != m_itEnd)
				{
					auto const valueCurrent = std::get<0>(m_params)(*m_itCurrent);
					m_itManyCurrent = valueCurrent.begin();
					m_itManyEnd = valueCurrent.end();
				}
			}
			decltype(auto) operator * () const
			{
				return std::get<1>(m_params)(*m_itCurrent, *m_itManyCurrent);
			}
			linq_select_many_iterator & operator = (linq_select_many_iterator const & that)
			{
				m_itCurrent = that.m_itCurrent;
				m_itManyCurrent = that.m_itManyCurrent;
				m_itManyEnd = that.m_itManyEnd;
				return *this;
			}
			linq_select_many_iterator & operator ++ ()
			{
				if (++m_itManyCurrent == m_itManyEnd)
				{
					if (++m_itCurrent != m_itEnd)
					{
						auto const valueCurrent = std::get<0>(m_params)(*m_itCurrent);
						m_itManyCurrent = valueCurrent.begin();
						m_itManyEnd = valueCurrent.end();
					}
				}
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
			decltype(auto) key() const
			{
				return std::get<0>(m_params)(*m_itCurrent);
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
			Titerator m_itNext;
			Titerator const m_itBegin;
			Titerator const m_itEnd;
			Tparams const m_params;
		public:
			template <typename Tcontainer>
			linq_order_by_iterator(Titerator && itCurrent, Tcontainer const & container, Tparams && params)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itNext(container.end())
				, m_itBegin(container.begin())
				, m_itEnd(container.end())
				, m_params(std::forward<Tparams>(params))
			{
				if (m_itCurrent != m_itEnd)
				{
					auto const & fnGet = std::get<0>(m_params);
					auto const & fnLess = std::get<1>(m_params);

					Titerator itMin = m_itCurrent;
					Titerator itNext = m_itCurrent;
					{
						auto valueMin = *itMin;
						auto valueNext = valueMin; // *itNext;

						for (Titerator it = itMin; it != m_itEnd; ++it)
						{
							auto const value = *it;

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
				m_itCurrent = that.m_itCurrent;
				m_itNext    = that.m_itNext;
				return *this;
			}
			linq_order_by_iterator & operator ++ ()
			{
				auto const & fnGet = std::get<0>(m_params);
				auto const & fnLess = std::get<1>(m_params);
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

		template <typename Titerator, typename Tfunc>
		class linq_skip_while_iterator
			: public linq_iterator<Titerator>
		{
			Titerator const m_itEnd;
			Tfunc const m_func;
		public:
			template <typename Tcontainer>
			linq_skip_while_iterator(Titerator && itCurrent, Tcontainer const & container, Tfunc && func)
				: linq_iterator<Titerator>(std::forward<Titerator>(itCurrent))
				, m_itEnd(container.end())
				, m_func(std::forward<Tfunc>(func))
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
				m_itCurrent = that.m_itCurrent;
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
			decltype(auto) aggregate(Tvalue && value, Taccumulate && fnAccumulate) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Tparams = std::tuple<Tvalue, Taccumulate>;
				using Taccumulate_iterator = linq_group_accumulate_iterator<Titerator, Tparams>;
				using Taccumulate_container = linq_container<linq_group<Tcontainer>, Tparams, Taccumulate_iterator>;
				using Taccumulate_enumerable = linq_enumerable<Taccumulate_container>;
				return Taccumulate_enumerable(linq_group<Tcontainer>(*this), Tparams(std::forward<Tvalue>(value), std::forward<Taccumulate>(fnAccumulate)));
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
				using Treturn = std::decay_t<decltype(*Tcontainer::begin())>;

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
				using Treturn = std::decay_t<decltype(*Tcontainer::begin())>;

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
				using Treturn = std::decay_t<decltype(*Tcontainer::begin())>;

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
			template <typename Tthat, typename Tequal>
			bool sequential_equal(Tthat const & that, Tequal && fnEqual)
			{
				auto const linqThat = from(that);
				auto const itThisBegin = begin();
				auto const itThisEnd = end();
				auto const itThatBegin = linqThat.begin();
				auto const itThatEnd = linqThat.end();
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
			bool sequential_equal(Tthat const & that)
			{
				using Tvalue = std::decay_t<decltype(*Tcontainer::begin())>;
				return sequential_equal(that, std::equal_to<Tvalue>());
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
			decltype(auto) concat(Tcontainer1 && container1) const
			{
				auto linqContainer1 = from(std::forward<Tcontainer>(container1));
				using Tlinq_container1 = std::decay_t<decltype(linqContainer1)>;
				using Titerator = decltype(Tcontainer::begin());
				using Tconcat_iterator = linq_concat_iterator<Titerator>;
				using Tconcat_container = linq_concat_container<linq_enumerable<Tcontainer>, Tlinq_container1, Tconcat_iterator>;
				using Tconcat_enumerable = linq_enumerable<Tconcat_container>;
				return Tconcat_enumerable(linq_enumerable<Tcontainer>(*this), std::forward<Tlinq_container1>(linqContainer1));
			}
			// TODO: distinct
			template <typename Tthat, typename Tequal>
			decltype(auto) except(Tthat && that, Tequal && fnEqual) const
			{
				auto linqThat = from(std::forward<Tthat>(that));
				return where([itBegin = linqThat.begin(), itEnd = linqThat.end(), fnEqual](auto const & valueCurrent)
				{
					return !std::any_of(itBegin, itEnd, std::bind2nd(fnEqual, valueCurrent));
				});
			}
			template <typename Tthat>
			decltype(auto) except(Tthat && that) const
			{
				using Tvalue = std::decay_t<decltype(*Tcontainer::begin())>;
				return except(std::forward<Tthat>(that), std::equal_to<Tvalue>());
			}
			template <typename Tthat_container, typename Tequal>
			decltype(auto) intersection(linq_enumerable<Tthat_container> const & that, Tequal && fnEqual) const
			{
				return where([itBegin = that.begin(), itEnd = that.end(), fnEqual](auto const & valueCurrent)
				{
					return std::any_of(itBegin, itEnd, std::bind2nd(fnEqual, valueCurrent));
				});
			}
			template <typename Tthat_container>
			decltype(auto) intersection(linq_enumerable<Tthat_container> const & that) const
			{
				using Tvalue = std::decay_t<decltype(*that.begin())>;
				return intersection(that, std::equal_to<Tvalue>());
			}
			template <typename Tfunc>
			decltype(auto) skip_while(Tfunc && func) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Tskip_while_iterator = linq_skip_while_iterator<Titerator, Tfunc>;
				using Tskip_while_container = linq_container<linq_enumerable<Tcontainer>, Tfunc, Tskip_while_iterator>;
				using Tskip_while_enumerable = linq_enumerable<Tskip_while_container>;
				return Tskip_while_enumerable(linq_enumerable<Tcontainer>(*this), std::forward<Tfunc>(func));
			}
			template <typename Tfunc>
			decltype(auto) select(Tfunc && func) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Tselect_iterator = linq_select_iterator<Titerator, Tfunc>;
				using Tselect_container = linq_container<linq_enumerable<Tcontainer>, Tfunc, Tselect_iterator>;
				using Tselect_enumerable = linq_enumerable<Tselect_container>;
				return Tselect_enumerable(linq_enumerable<Tcontainer>(*this), std::forward<Tfunc>(func));
			}
			template <typename Tmany, typename Tselect>
			decltype(auto) select_many(Tmany && fnMany, Tselect && fnSelect) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Tmany_iterator = decltype(fnMany(*Tcontainer::begin()).begin());
				using Tparams = std::tuple<Tmany, Tselect>;
				using Tselect_many_iterator = linq_select_many_iterator<Titerator, Tmany_iterator, Tparams>;
				using Tselect_many_container = linq_container<linq_enumerable<Tcontainer>, Tparams, Tselect_many_iterator>;
				using Tselect_many_enumerable = linq_enumerable<Tselect_many_container>;
				return Tselect_many_enumerable(linq_enumerable<Tcontainer>(*this), Tparams(std::forward<Tmany>(fnMany), std::forward<Tselect>(fnSelect)));
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
			decltype(auto) where(Tfunc && func) const
			{
				using Titerator = decltype(Tcontainer::begin());
				using Twhere_iterator = linq_where_iterator<Titerator, Tfunc>;
				using Twhere_container = linq_container<linq_enumerable<Tcontainer>, Tfunc, Twhere_iterator>;
				using Twhere_enumerable = linq_enumerable<Twhere_container>;
				return Twhere_enumerable(linq_enumerable<Tcontainer>(*this), std::forward<Tfunc>(func));
			}
			template <typename Tget, typename Tequal>
			decltype(auto) group_by(Tget && fnGet, Tequal && fnEqual) const
			{
				using Tparams = std::tuple<Tget, Tequal>;
				using Titerator = decltype(Tcontainer::begin());
				using Tgroup_by_iterator = linq_group_by_iterator<Titerator, Tparams>;
				using Tgroup_by_container = linq_container<linq_enumerable<Tcontainer>, Tparams, Tgroup_by_iterator>;
				using Tgroup_by_enumerable = linq_group<Tgroup_by_container>;
				return Tgroup_by_enumerable(linq_enumerable<Tcontainer>(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tequal>(fnEqual)));
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
				using Torder_by_container = linq_container<linq_enumerable<Tcontainer>, Tparams, Torder_by_iterator>;
				using Torder_by_enumerable = linq_enumerable<Torder_by_container>;
				return Torder_by_enumerable(linq_enumerable<Tcontainer>(*this), Tparams(std::forward<Tget>(fnGet), std::forward<Tcompare>(fnCompare)));
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
