#pragma once

/* Author: jrmwng @ 2017 */

#include <type_traits>

namespace jrmwng
{
	namespace cnn
	{
		template <typename T, size_t... uSIZES>
		class Carray;
		template <typename T, size_t... uSIZE>
		class Ctensor;
		template <typename T, typename Tleft, typename Tright>
		class Cconvolution;
		template <template <typename T> class Cmax, typename Ttensor, size_t... uSTRIDE>
		class Cpooling;

		template <typename T, size_t uSIZE0, size_t uSIZE1, size_t... uSIZES>
		class Carray<T, uSIZE0, uSIZE1, uSIZES...>
			: public Carray<std::array<T, uSIZE0>, uSIZE1, uSIZES...>
		{
		public:
			template <typename... Tindex>
			T const & operator () (size_t uIndexX, Tindex... index) const
			{
				Carray<std::array<T, uSIZE0>, uSIZE1, uSIZES...> const & arrayThis = *this;
				return arrayThis(index...)[uIndexX];
			}
			template <typename... Tindex>
			T & operator () (size_t uIndexX, Tindex... index)
			{
				Carray<std::array<T, uSIZE0>, uSIZE1, uSIZES...> & arrayThis = *this;
				return arrayThis(index...)[uIndexX];
			}
		};
		template <typename T, size_t uSIZE0>
		class Carray<T, uSIZE0>
			: public std::array<T, uSIZE0>
		{
		public:
			T const & operator () (size_t uIndexY) const
			{
				return (*this)[uIndexY];
			}
			T & operator () (size_t uIndexY)
			{
				return (*this)[uIndexY];
			}
		};


		template <typename T, size_t uSIZE0, size_t... uSIZES>
		class Ctensor<T, uSIZE0, uSIZES...>
			: public std::array<Ctensor<T, uSIZES...>, uSIZE0>
		{
			friend class Ctensor;

			template <typename Tfunc, typename... Tindex>
			void for_each(Tfunc const & func, Tindex... index)
			{
				for (size_t uIndex = 0; uIndex < uSIZE0; uIndex++)
				{
					(*this)[uIndex].for_each(func, index..., uIndex);
				}
			}

			template <typename Tvalue>
			struct Cassignment
			{
				typename Ctensor & m_tensor;
				Tvalue const & m_value;

				Cassignment(typename Ctensor & tensor, Tvalue const & value)
					: m_tensor(tensor)
					, m_value(value)
				{}

				template <typename... Tindex>
				void operator () (Tindex ... index) const
				{
					m_tensor(index...) = m_value;
				}
			};
			template <typename Tvalue>
			decltype(auto) assignment(Tvalue const & value)
			{
				return Cassignment<Tvalue>(*this, value);
			}
			template <typename Tthat>
			struct Celementwise_assignment
			{
				typename Ctensor & m_tensor;
				Tthat const & m_that;

				Celementwise_assignment(typename Ctensor & tensor, Tthat const & that)
					: m_tensor(tensor)
					, m_that(that)
				{}

				template <typename... Tindex>
				void operator () (Tindex ... index) const
				{
					m_tensor(index...) = m_that(index...);
				}
			};
			template <typename Tthat>
			decltype(auto) elementwise_assignment(Tthat const & that)
			{
				return Celementwise_assignment<Tthat>(*this, that);
			}

		public:
			typename Ctensor & operator = (T const & t)
			{
				for_each(assignment(t));
				return *this;
			}
			template <typename Tleft, size_t... uLEFT, typename Tright, size_t... uRIGHT>
			typename Ctensor & operator = (Cconvolution<T, Ctensor<Tleft, uLEFT...>, Ctensor<Tright, uRIGHT...>> const & that)
			{
				for_each(elementwise_assignment(that));
				return *this;
			}
			template <template <typename T> class Cmax, typename Ttensor, size_t... uSTRIDES>
			typename Ctensor & operator = (Cpooling<Cmax, Ttensor, uSTRIDES...> const & that)
			{
				for_each(elementwise_assignment(that));
				return *this;
			}

			template <typename... Tindex>
			T const & operator () (size_t uSubIndex, Tindex ... index) const
			{
				return (*this)[uSubIndex](index...);
			}
			template <typename... Tindex>
			T & operator () (size_t uSubIndex, Tindex ... index)
			{
				return (*this)[uSubIndex](index...);
			}
		};
		template <typename T>
		class Ctensor<T>
		{
			friend class Ctensor;

			T m_t;

			template <typename Tfunc, typename... Tindex>
			void for_each(Tfunc const & func, Tindex... index)
			{
				func(index...);
			}
		public:

			typename Ctensor & operator = (T const & t)
			{
				m_t = t;
				return *this;
			}
			operator T const & () const
			{
				return m_t;
			}
			T const & operator () () const
			{
				return m_t;
			}
			T & operator () ()
			{
				return m_t;
			}
		};

		template <typename T, typename Tleft, typename Tright>
		class Cconvolution<T, Ctensor<Tleft>, Ctensor<Tright>>
		{
			typedef Ctensor<Tleft>
				Tleft_tensor;
			typedef Ctensor<Tright>
				Tright_tensor;

			Tleft_tensor const & m_left;
			Tright_tensor const & m_right;
		public:
			Cconvolution(Tleft_tensor const & left, Tright_tensor const & right)
				: m_left(left)
				, m_right(right)
			{}
			T operator () () const
			{
				return m_left() * m_right();
			}
		};
		template <typename T, typename Tleft, size_t uLEFT0, size_t... uLEFT, typename Tright, size_t uRIGHT0, size_t... uRIGHT>
		class Cconvolution<T, Ctensor<Tleft, uLEFT0, uLEFT...>, Ctensor<Tright, uRIGHT0, uRIGHT...>>
		{
			typedef Ctensor<Tleft, uLEFT0, uLEFT...>
				Tleft_tensor;
			typedef Ctensor<Tright, uRIGHT0, uRIGHT...>
				Tright_tensor;

			Tleft_tensor const & m_left;
			Tright_tensor const & m_right;
		public:
			typedef Cconvolution<T, Ctensor<Tleft, uLEFT...>, Ctensor<Tright, uRIGHT...>>
				sub_type;

			Cconvolution(Tleft_tensor const & left, Tright_tensor const & right)
				: m_left(left)
				, m_right(right)
			{}

			template <typename... Tindex>
			T operator () (size_t uLeftIndex, Tindex... index) const
			{
				T t = sub_type(m_left[uLeftIndex], m_right[0])(index...);
				{
					for (size_t uRightIndex = 1; uRightIndex < uRIGHT0; uRightIndex++)
					{
						t += sub_type(m_left[uLeftIndex], m_right[uRightIndex])(index...);
					}
				}
				return t;
			}
		};

		template <typename T, typename Tleft, size_t... uLEFT, typename Tright, size_t... uRIGHT>
		decltype(auto) convolute(Ctensor<Tleft, uLEFT...> const & left, Ctensor<Tright, uRIGHT...> const & right)
		{
			return Cconvolution<T, Ctensor<Tleft, uLEFT...>, Ctensor<Tright, uRIGHT...>>(left, right);
		}

		template <typename T>
		struct Cmax
		{
			T const & operator () (T const & left, T const & right) const
			{
				return (left < right) ? right : left;
			}
		};
		template <template <typename T> class Cmax, typename Ttensor>
		class Cpooling<Cmax, Ttensor>
		{
			Ttensor const & m_tensor;
		public:
			Cpooling(Ttensor const & tensor)
				: m_tensor(tensor)
			{}

			decltype(auto) operator () () const
			{
				return m_tensor();
			}
		};
		template <template <typename T> class Cmax, typename T, size_t uSIZE0, size_t... uSIZES, size_t uSTRIDE0, size_t... uSTRIDES>
		class Cpooling<Cmax, Ctensor<T, uSIZE0, uSIZES...>, uSTRIDE0, uSTRIDES...>
		{
			typedef Ctensor<T, uSIZE0, uSIZES...>
				Ttensor;

			Ttensor const & m_tensor;
		public:
			typedef Cpooling<Cmax, Ctensor<T, uSIZES...>, uSTRIDES...>
				sub_type;

			Cpooling(Ttensor const & tensor)
				: m_tensor(tensor)
			{}

			template <typename... Tindex>
			decltype(auto) operator () (size_t uIndex, Tindex... index) const
			{
				auto t = sub_type(m_tensor[uIndex * uSTRIDE0])(index...);
				{
					for (size_t uOffset = 1; uOffset < uSTRIDE0; uOffset++)
					{
						t = Cmax<decltype(t)>()(t, sub_type(m_tensor[uIndex * uSTRIDE0 + uOffset])(index...));
					}
				}
				return t;
			}
		};
		template <template <typename T> class Cmax, size_t... uSTRIDES, typename Ttensor>
		decltype(auto) pooling(Ttensor const & tensor)
		{
			return Cpooling<Cmax, Ttensor, uSTRIDES...>(tensor);
		}

		//Ctensor<short, 24, 24, 10> convolute(Ctensor<unsigned char, 28, 28, 3> const &, Ctensor<char, 5, 5, 3> const &);

		//Ctensor<short, 12, 12, 10> pooling_max(Ctensor<short, 24, 24, 10> const &);

		//Ctensor<int, 10> convolute(Ctensor<short, 12, 12, 10> const &, Ctensor<short, 12, 12, 10, 10> const &);
	}
}
