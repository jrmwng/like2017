#pragma once

/* Author: jrmwng @ 2017 */

#include <type_traits>

namespace jrmwng
{
	namespace cnn
	{
		template <typename T, size_t... uSIZE>
		class Ctensor;
		template <typename T, typename Tleft, typename Tright>
		class Cconvolution;
		template <typename Ttensor, template <typename T> class Cmax, size_t... uSTRIDE>
		class Cpooling;
		template <typename Ttensor>
		class Celementwise;


		template <typename T, typename Tleft, typename Tright>
		Cconvolution<T, Tleft, Tright>
			convolute(Tleft const & left, Tright const & right);
		template <template <typename T> class Cmax, size_t... uSTRIDES, typename Ttensor>
		Cpooling<Ttensor, Cmax, uSTRIDES...>
			pooling(Ttensor const & tensor);
		template <typename Ttensor>
		Celementwise<Ttensor>
			elementwise(Ttensor & tensor);


		template <typename T, size_t uSIZE0, size_t... uSIZES>
		class Ctensor<T, uSIZE0, uSIZES...>
			: public std::array<Ctensor<T, uSIZES...>, uSIZE0>
		{
		public:
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
			T m_t;
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
			typedef Cconvolution<T, Ctensor<Tleft, uLEFT...>, Ctensor<Tright, uRIGHT...>>
				Tsub;

			Tleft_tensor const & m_left;
			Tright_tensor const & m_right;
		public:
			Cconvolution(Tleft_tensor const & left, Tright_tensor const & right)
				: m_left(left)
				, m_right(right)
			{}

			template <typename... Tindex>
			T operator () (size_t uLeftIndex, Tindex... index) const
			{
				T t = Tsub(m_left[uLeftIndex], m_right[0])(index...);
				{
					for (size_t uRightIndex = 1; uRightIndex < uRIGHT0; uRightIndex++)
					{
						t += Tsub(m_left[uLeftIndex], m_right[uRightIndex])(index...);
					}
				}
				return t;
			}
		};
		template <typename T, typename Tleft, typename Tright>
		Cconvolution<T, Tleft, Tright>
			convolute(Tleft const & left, Tright const & right)
		{
			return Cconvolution<T, Tleft, Tright>(left, right);
		}

		template <typename T>
		struct Cmax
		{
			T const & operator () (T const & left, T const & right) const
			{
				return (left < right) ? right : left;
			}
		};

		template <typename T, template <typename T> class Cmax>
		class Cpooling<Ctensor<T>, Cmax>
		{
			Ctensor<T> const & m_tensor;
		public:
			Cpooling(Ctensor<T> const & tensor)
				: m_tensor(tensor)
			{}

			T operator () () const
			{
				return m_tensor();
			}
		};
		template <typename T, size_t uSIZE0, size_t... uSIZES, template <typename T> class Cmax, size_t uSTRIDE0, size_t... uSTRIDES>
		class Cpooling<Ctensor<T, uSIZE0, uSIZES...>, Cmax, uSTRIDE0, uSTRIDES...>
		{
			typedef Ctensor<T, uSIZE0, uSIZES...>
				Ttensor;
			typedef Cpooling<Ctensor<T, uSIZES...>, Cmax, uSTRIDES...>
				Tsub;

			Ttensor const & m_tensor;
		public:
			Cpooling(Ttensor const & tensor)
				: m_tensor(tensor)
			{}

			template <typename... Tindex>
			T operator () (size_t uIndex, Tindex... index) const
			{
				T t = Tsub(m_tensor[uIndex * uSTRIDE0])(index...);
				{
					Cmax<T> const fnMax;

					for (size_t uOffset = 1; uOffset < uSTRIDE0; uOffset++)
					{
						t = fnMax(t, Tsub(m_tensor[uIndex * uSTRIDE0 + uOffset])(index...));
					}
				}
				return t;
			}
		};
		template <template <typename T> class Cmax, size_t... uSTRIDES, typename Ttensor>
		Cpooling<Ttensor, Cmax, uSTRIDES...>
			pooling(Ttensor const & tensor)
		{
			return Cpooling<Ttensor, Cmax, uSTRIDES...>(tensor);
		}


		template <typename Ttensor>
		class Celementwise
		{
			Ttensor & m_tensor;
		public:
			Celementwise(Ttensor & tensor)
				: m_tensor(tensor)
			{}

			template <typename... Tindex>
			decltype(auto) operator () (Tindex ... index) const
			{
				return m_tensor(index...);
			}
		};
		template <typename T, size_t uSIZE0, size_t... uSIZES>
		class Celementwise<Ctensor<T, uSIZE0, uSIZES...>>
		{
			friend class Celementwise;

			Ctensor<T, uSIZE0, uSIZES...> & m_tensor;

			template <typename Tthat, typename... Tindex>
			void assign(Tthat const & that, Tindex ... index) const
			{
				for (size_t uIndex = 0; uIndex < uSIZE0; uIndex++)
				{
					elementwise(m_tensor[uIndex]).assign(that, index..., uIndex);
				}
			}
		public:
			Celementwise(Ctensor<T, uSIZE0, uSIZES...> & tensor)
				: m_tensor(tensor)
			{}

			template <typename Tthat>
			typename Celementwise const & operator = (Tthat const & that) const
			{
				assign(that);
				return *this;
			}

			template <typename... Tindex>
			T const & operator () (Tindex ... index) const
			{
				return m_tensor(index...);
			}
		};
		template <typename T>
		class Celementwise<Ctensor<T>>
		{
			friend class Celementwise;

			Ctensor<T> & m_tensor;

			template <typename Tthat, typename... Tindex>
			void assign(Celementwise<Tthat> const & that, Tindex ... index) const
			{
				m_tensor = that(index...);
			}
			template <typename Tthat, typename... Tindex>
			void assign(Tthat const & that, Tindex ... index) const
			{
				m_tensor = that;
			}
		public:
			Celementwise(Ctensor<T> & tensor)
				: m_tensor(tensor)
			{}

			T const & operator () () const
			{
				return m_tensor();
			}
		};
		template <typename Ttensor>
		Celementwise<Ttensor> elementwise(Ttensor & tensor)
		{
			return Celementwise<Ttensor>(tensor);
		}

		//Ctensor<short, 24, 24, 10> convolute(Ctensor<unsigned char, 28, 28, 3> const &, Ctensor<char, 5, 5, 3> const &);

		//Ctensor<short, 12, 12, 10> pooling_max(Ctensor<short, 24, 24, 10> const &);

		//Ctensor<int, 10> convolute(Ctensor<short, 12, 12, 10> const &, Ctensor<short, 12, 12, 10, 10> const &);
	}
}
