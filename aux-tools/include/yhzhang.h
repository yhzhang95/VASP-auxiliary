/* Version 0.1 edited by YehuiZhang
* date: 2018-09-01
* content: Split function(as fast as we can, but need boost/spirit/qi.hpp)
*          Range and LogRange function
*          Vector dimension 1-4D
*          Tiny FFT(5x slower than FFTW)
*/
#ifndef YHZHANG_H
#define YHZHANG_H
/* __VERSION__  0.1 */

#ifndef True
#define True    true
#define False   false
#endif // True

/**** Custom section ****/
#define SPLIT    True
#define RANGE    True
#define MATRIX   False
#define TINYFFT  False
#define FMATH    False


#include <vector>
#include <iostream>
#include <complex>
#include <utility>
#include <type_traits>
#include <functional>
#include <random>

/* predefine */
#if SPLIT
#include <boost/spirit/include/qi.hpp>
#endif

#if TINYFFT
#undef  MATRIX
#define MATRIX True
#endif

#if MATRIX
#undef  RANGE
#define RANGE  True
#endif

#ifndef PI
#define PI                                3.14159265358979323846
#define TPI                               6.28318530717958647692
#define PI_F                              3.1415926535897932f
#define INV_PI_F                          0.31830988618f
#define HALF_PI_F                         1.57079632679f
#endif


namespace zyh
{
#if SPLIT
	/*
	* String to value & split function
	*
	* void stoval(string str, double back)
	* void stoval(string str, int back)
	* double stoval(const_iterator begin, const_iterator end, double back)
	* int stoval(const_iterator begin, const_iterator end, int back)
	*
	* void split(string str, vector<double> result, double tmp)
	* void split(string str, vector<int> result, int tmp)
	* vector<string> split(string str)
	*/
	inline void stoval(const std::string& str, double& back) { boost::spirit::qi::parse(str.begin(), str.end(), boost::spirit::qi::double_, back); }

	inline void stoval(const std::string& str, int& back) { boost::spirit::qi::parse(str.begin(), str.end(), boost::spirit::qi::int_, back); }

	inline double stoval(const std::string::const_iterator& begin, const std::string::const_iterator& end, double& back)
	{
		boost::spirit::qi::parse(begin, end, boost::spirit::qi::double_, back);
		return back;
	}

	inline int stoval(const std::string::const_iterator& begin, const std::string::const_iterator& end, int& back)
	{
		boost::spirit::qi::parse(begin, end, boost::spirit::qi::int_, back);
		return back;
	}

	inline void split(const std::string& str, std::vector<double>& result, double& tmp)
	{
		result.clear();
		enum State { Space, Token };
		auto state = Space;

		std::string::const_iterator TokenBegin;
		for (auto it = str.begin(); it != str.end(); ++it)
		{
			auto const newState = *it == ' ' ? Space : Token;
			if (newState != state)
			{
				switch (newState)
				{
				case Space:result.emplace_back(stoval(TokenBegin, it, tmp)); break;
				case Token:TokenBegin = it; break;
				}
			}
			state = newState;
		}
		if (state == Token) { result.emplace_back(stoval(TokenBegin, str.end(), tmp)); }
	}

	inline void split(const std::string& str, std::vector<int>& result, int& tmp)
	{
		result.clear();
		enum State { Space, Token };
		auto state = Space;

		std::string::const_iterator TokenBegin;
		for (auto it = str.begin(); it != str.end(); ++it)
		{
			auto const newState = *it == ' ' ? Space : Token;
			if (newState != state)
			{
				switch (newState)
				{
				case Space:result.emplace_back(stoval(TokenBegin, it, tmp)); break;
				case Token:TokenBegin = it; break;
				}
			}
			state = newState;
		}
		if (state == Token) { result.emplace_back(stoval(TokenBegin, str.end(), tmp)); }
	}

	inline std::vector<std::string> split(const std::string& str)
	{
		std::vector<std::string> result;
		enum State { Space, Token };
		auto state = Space;

		auto pTokenBegin = 0;
		for (auto it = str.begin(); it != str.end(); ++it)
		{
			auto const newState = *it == ' ' ? Space : Token;
			if (newState != state)
			{
				switch (newState)
				{
				case Space:result.push_back(str.substr(pTokenBegin, (it - str.begin()) - pTokenBegin)); break;
				case Token:pTokenBegin = int(it - str.begin()); break;
				}
			}
			state = newState;
		}
		if (state == Token) { result.push_back(str.substr(pTokenBegin, str.size() - pTokenBegin)); }
		return result;
	}
#endif

#if RANGE
	/*
	* Range [Start, End, Step]
	* Usage: Range(End) ---> (0, End, 1)
	*        Range(Start, End) ---> (Start, End, 2)
	*        Range(Start, End, Step) ---> (Start, End, Step)
	*/
	template <typename T>
	class Range
	{
		T _start, _end, _step;
	public:
		class iterator
		{
			T _value, _step;
		public:
			iterator(const T& value, const T& step) : _value{ value }, _step{ step } {}

			iterator operator++() { this->_value += this->_step; return *this; }
			T& operator*() { return this->_value; }
			const T* operator->() { return &this->_value; }

			bool operator==(const iterator& tmp) { return _value == tmp._value && _step == tmp._step; }
			bool operator!=(const iterator& tmp) { return (_value < tmp._value) == (_step > 0) && _value != tmp._value; }
		};

		Range(const T& start, const T& end, const T& step) :_start(start), _end(end), _step(step)
		{
			if (_step == 0) { std::cout << "Range step argument must not be zero." << std::endl; exit(511); }
			if ((start > end && step > 0) || (start < end && step < 0)) { std::cout << "Range arguments must result in termination." << std::endl; exit(513); }
		}
		Range(const T& start, const T& end) :Range(start, end, 1) { }
		explicit Range(const T& end) :Range(0, end, 1) {}
		Range() :Range(0, 10, 1) {}

		void xrange(const T& start, const T& end, const T& step)
		{
			this->_start = start; this->_end = end; this->_step = step;
			if (_step == 0) { std::cout << "Range step argument must not be zero." << std::endl; exit(511); }
			if ((start > end && step > 0) || (start < end && step < 0)) { std::cout << "Range arguments must result in termination." << std::endl; exit(513); }
		}
		void xrange(const T& start, const T& end) { xrange(start, end, 1); }
		void xrange(const T& end) { xrange(0, end, 1); }
		void xrange() { xrange(0, 10, 1); }

		iterator begin() const { return iterator(_start, _step); }
		iterator end() const { return iterator(_end, _step); }
	};

	typedef Range<int> range;

	/*
	* LogRange [Start, Length, Step]
	* Usage: LogRange(Length) ---> (1, Length, 2)
	*        LogRange(Start, Length) ---> (Start, Length, 2)
	*        LogRange(Start, Length, Step) ---> (Start, Length, Step)
	*/
	template <typename T>
	class LogRange
	{
		T _start, _step;
		int _length;
	public:
		class iterator
		{
			T _value, _step;
			int _index;
		public:
			iterator(const T& value, const T& step, const int& index) : _value{ value }, _step{ step }, _index{ index } {}

			iterator operator++() { this->_value *= this->_step; ++this->_index; return *this; }
			T& operator*() { return this->_value; }
			const T* operator->() { return &this->_value; }

			bool operator==(const iterator& tmp) { return _value == tmp._value && _step == tmp._step && _index == tmp._index; }
			bool operator!=(const iterator& tmp) { return _index != tmp._index; }
		};

		LogRange() : _start(T(1)), _step(T(2)), _length(10) {}
		explicit LogRange(const int& length) :_start(T(1)), _step(T(2)), _length(length) {}
		LogRange(const T& start, const int& length) :_start(start), _step(T(2)), _length(length) {}
		LogRange(const T& start, const int& length, const T& step) :_start(start), _step(step), _length(length) {}

		void xrange() { this->_start = T(1);  this->_step = T(2); this->_length = 10; }
		void xrange(const int& length) { this->_start = T(1); this->_step = T(2); this->_length = length; }
		void xrange(const T& start, const int& length) { this->_start = start; this->_step = T(2); this->_length = length; }
		void xrange(const T& start, const int& length, const T& step) { this->_start = start;  this->_step = step; this->_length = length; }

		iterator begin() const { return iterator(_start, _step, 0); }
		iterator end() const { return iterator(_start, _step, _length); }
	};

	typedef LogRange<int> lrange;

#endif

#if MATRIX
	/* predefine class Vector */
	template <int DIM, typename T, class Enable = void> class Vector {};

	/* sdt::complex<T> */
	typedef std::complex<double>    complexd;
	typedef std::complex<float>     complexf;

	/* zyh::Vector<1,T> => 1D */
	typedef Vector<1, complexd>     vectorcd;
	typedef Vector<1, complexf>     vectorcf;
	typedef Vector<1, double>       vectord;
	typedef Vector<1, float>        vectorf;
	typedef Vector<1, int>          vectori;

	/* zyh::Vector<2,T> => 2D */
	typedef Vector<2, complexd>     vector2cd;
	typedef Vector<2, complexf>     vector2cf;
	typedef Vector<2, double>       vector2d;
	typedef Vector<2, float>        vector2f;
	typedef Vector<2, int>          vector2i;

	/* zyh::Vector<3,T> => 3D */
	typedef Vector<3, complexd>     vector3cd;
	typedef Vector<3, complexf>     vector3cf;
	typedef Vector<3, double>       vector3d;
	typedef Vector<3, float>        vector3f;
	typedef Vector<3, int>          vector3i;

	/* zyh::Vector<4,T> => 4D */
	typedef Vector<4, complexd>     vector4cd;
	typedef Vector<4, complexf>     vector4cf;
	typedef Vector<4, double>       vector4d;
	typedef Vector<4, float>        vector4f;
	typedef Vector<4, int>          vector4i;

	/* zyh::Vector<5,T> => 4D */
	typedef Vector<5, complexd>     vector5cd;
	typedef Vector<5, complexf>     vector5cf;
	typedef Vector<5, double>       vector5d;
	typedef Vector<5, float>        vector5f;
	typedef Vector<5, int>          vector5i;



#define remove_cvr(T)        typename remove_cvr<T>::type
#define not_class(U)         typename std::enable_if<!std::is_class<U>::value, int>::type = 0
#define is_numeric(U)        typename std::enable_if<is_numeric<U>::value, int>::type = 0
#define not_vector(U)        typename std::enable_if<!is_vector<U>::value, int>::type = 0
#define is_vector(U)         typename std::enable_if<is_vector<U>::value, int>::type = 0
#define is_vector2(T,U)      typename std::enable_if<is_vector2<T,U>::value, int>::type = 0
#define is_num_vector(U)     typename std::enable_if<is_num_vector<U>::value, int>::type = 0
#define is_num_vector2(T,U)  typename std::enable_if<is_num_vector2<T,U>::value, int>::type = 0

	/* extract first type from class */
	template<typename T>
	struct extract { typedef std::nullptr_t type; };
	template<template<typename> class X, typename T>
	struct extract<X<T>> { typedef T type; };

	/* extract first and second type from class(work for Vector<int, T, void>) */
	template<typename T>
	struct extract2 { typedef std::nullptr_t type1; typedef std::nullptr_t type2; typedef std::nullptr_t type3; static constexpr int value = 0; };
	template<template<typename> class X, typename T>
	struct extract2<X<T>> { typedef std::nullptr_t type1; typedef std::nullptr_t type2; typedef std::nullptr_t type3; static constexpr int value = 0; };
	template<template<int DIM, typename, typename> class X, int DIM, typename T, typename Enable>
	struct extract2<X<DIM, T, Enable>> { typedef int type1; typedef T type2; typedef Enable type3; static constexpr int value = DIM; };

	/* remove const volatile and reference */
	template<class T>
	struct remove_cvr : std::remove_cv<   typename std::remove_reference<T>::type	> { };

	/* real check */
	template<class T>
	struct is_real : std::integral_constant < bool, std::is_integral<remove_cvr(T)>::value || std::is_floating_point<remove_cvr(T)>::value > { };

	/* complex check */
	template <class T>
	struct is_complex : std::integral_constant < bool,
		is_real< typename extract<remove_cvr(T)>::type >::value && // first type should be real type(int of floating point)
		std::is_same< remove_cvr(T), std::complex< typename extract<remove_cvr(T)>::type >>::value > {}; // check class be complex, may have more useful way???
																										 /* numeric(real or complex) check */
	template<class T>
	struct is_numeric : std::integral_constant < bool, is_real<T>::value || is_complex<T>::value > { };

	/* Vector check */
	template<class T>
	struct is_vector :std::integral_constant < bool,
		std::is_same< int, typename extract2< remove_cvr(T) >::type1 >::value && // first type should be int type
		std::is_same< remove_cvr(T), Vector< extract2<remove_cvr(T)>::value, typename extract2<remove_cvr(T)>::type2>>::value > { }; // check class be Vector, may have more useful way???

	template<class T, class U>
	struct is_vector2 :std::integral_constant < bool,
		std::is_same< int, typename extract2<remove_cvr(T)>::type1 >::value &&
		std::is_same< int, typename extract2<remove_cvr(U)>::type1 >::value &&
		extract2<remove_cvr(T)>::value == extract2<remove_cvr(U)>::value &&
		std::is_same< remove_cvr(T), Vector< extract2<remove_cvr(T)>::value, typename extract2<remove_cvr(T)>::type2> >::value &&
		std::is_same< remove_cvr(U), Vector< extract2<remove_cvr(U)>::value, typename extract2<remove_cvr(U)>::type2> >::value > { };

	template<class T>
	struct is_num_vector :std::integral_constant < bool,
		std::is_same< int, typename extract2<remove_cvr(T)>::type1 >::value &&
		is_numeric<        typename extract2<remove_cvr(T)>::type2 >::value &&
		std::is_same< remove_cvr(T), Vector< extract2<remove_cvr(T)>::value, typename extract2<remove_cvr(T)>::type2>  >::value > { };

	template<class T, class U>
	struct is_num_vector2 :std::integral_constant < bool,
		std::is_same< int, typename extract2<remove_cvr(T)>::type1 >::value &&
		std::is_same< int, typename extract2<remove_cvr(U)>::type1 >::value &&
		extract2<remove_cvr(T)>::value == extract2<remove_cvr(U)>::value &&
		is_numeric<        typename extract2<remove_cvr(T)>::type2 >::value &&
		is_numeric<        typename extract2<remove_cvr(U)>::type2 >::value &&
		std::is_same< remove_cvr(T), Vector< extract2<remove_cvr(T)>::value, typename extract2<remove_cvr(T)>::type2> >::value &&
		std::is_same< remove_cvr(U), Vector< extract2<remove_cvr(U)>::value, typename extract2<remove_cvr(U)>::type2> >::value > { };

	/*
	* Detail for VectorBase(1-5 dimension)
	*    - lhs and rhs stand for Left Hand Side and Right Hand Side in this context.
	*/
	template <int DIM, typename T> class VectorBase
	{
	private:
		/* Setting */
		enum FLAG4D {
			ZERO,
			ALL, // 1d
			_X, _Y, _ALL, _TRANS, // 2d
			__X, __Y, __Z, __XY, __XZ, __YZ, __ALL, // 3d
			___X, ___Y, ___Z, ___T, ___XY, ___XZ, ___XT, ___YZ, ___YT, ___ZT, ___XYZ, ___XYT, ___XZT, ___YZT, ___ALL, // 4d
			____X, ____Y, ____Z, ____T, ____H, ____XY, ____XZ, ____XT, ____XH, ____YZ, ____YT, ____YH, ____ZT, ____ZH, ____TH,
			____XYZ, ____XYT, ____XYH, ____XZT, ____XZH, ____XTH, ____YZT, ____YZH, ____YTH, ____ZTH,
			____XYZT, ____XYZH, ____XYTH, ____XZTH, ____YZTH, ____ALL // 5d
		};
		template <typename U> void size_chk(VectorBase<DIM, U>&& rhs)
		{
			if (DIMFLAG == 1)
				if (x_size != rhs.x_size)
					throw std::invalid_argument("A.size is different from B.size !");
			if (DIMFLAG == 2)
				if (x_size != rhs.x_size || y_size != rhs.y_size)
					throw std::invalid_argument("A.size is different from B.size !");
			if (DIMFLAG == 3)
				if (x_size != rhs.x_size || y_size != rhs.y_size || z_size != rhs.z_size)
					throw std::invalid_argument("A.size is different from B.size !");
			if (DIMFLAG == 4)
				if (x_size != rhs.x_size || y_size != rhs.y_size || z_size != rhs.z_size || t_size != rhs.t_size)
					throw std::invalid_argument("A.size is different from B.size !");
			if (DIMFLAG == 5)
				if (x_size != rhs.x_size || y_size != rhs.y_size || z_size != rhs.z_size || t_size != rhs.t_size || h_size != rhs.h_size)
					throw std::invalid_argument("A.size is different from B.size !");
		}

	public:
		/* Para */
		int x_size, y_size, z_size, t_size, h_size;
		/* iterator and const_iterator */
		class iterator
		{
			/* Para */
			VectorBase* out;
			int x_index, y_index, z_index, t_index, h_index;
			FLAG4D flag;
		public:
			iterator() : out(nullptr), x_index(0), y_index(0), z_index(0), t_index(0), h_index(0), flag(ZERO) {}
			iterator(VectorBase* _out, const int& _x_index, const int& _y_index, const int& _z_index, const int& _t_index, const int& _h_index, const FLAG4D& _flag) :
				out(_out), x_index(_x_index), y_index(_y_index), z_index(_z_index), t_index(_t_index), h_index(_h_index), flag(_flag) {}

			iterator operator++()
			{
				// 1d
				if (flag == ALL)
				{
					++this->x_index;
					return *this;
				}
				// 2d
				if (flag == _X) { ++this->y_index; return *this; }
				if (flag == _Y) { ++this->x_index; return *this; }
				if (flag == _ALL)
				{
					++this->y_index;
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == _TRANS)
				{
					++this->x_index;
					if (this->x_index == out->x_size) { this->x_index = 0; ++this->y_index; }
					return *this;
				}
				// 3d
				if (flag == __X)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == __Y)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == __Z)
				{
					++this->y_index;
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == __XY) { ++this->z_index; return *this; }
				if (flag == __XZ) { ++this->y_index; return *this; }
				if (flag == __YZ) { ++this->x_index; return *this; }
				if (flag == __ALL)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				//4d
				if (flag == ___X)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ___Y)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___Z)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___T)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___XY)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					return *this;
				}
				if (flag == ___XZ)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ___XT)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ___YZ)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___YT)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___ZT)
				{
					++this->y_index;
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___XYZ) { ++this->t_index; return *this; }
				if (flag == ___XYT) { ++this->z_index; return *this; }
				if (flag == ___XZT) { ++this->y_index; return *this; }
				if (flag == ___YZT) { ++this->x_index; return *this; }
				if (flag == ___ALL)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				// 5d
				if (flag == ____X)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____Y)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____Z)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____T)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____H)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____XY)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					return *this;
				}
				if (flag == ____XZ)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____XT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____XH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____YZ)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____YT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____YH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____ZT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____ZH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____TH)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____XYZ)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					return *this;
				}
				if (flag == ____XYT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->z_index; }
					return *this;
				}
				if (flag == ____XYH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					return *this;
				}
				if (flag == ____XZT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____XZH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____XTH)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____YZT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____YZH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____YTH)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____ZTH)
				{
					++this->y_index;
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____XYZT) { ++this->h_index; return *this; }
				if (flag == ____XYZH) { ++this->t_index; return *this; }
				if (flag == ____XYTH) { ++this->z_index; return *this; }
				if (flag == ____XZTH) { ++this->y_index; return *this; }
				if (flag == ____YZTH) { ++this->x_index; return *this; }
				if (flag == ____ALL)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				throw std::invalid_argument("WTF???");
			}
			template <typename U, is_numeric(U)> iterator operator +(U i)
			{
				iterator res{ out, x_index, y_index, z_index, t_index, h_index, flag };
				for (auto t = 0; t < i; ++t) { ++res; }
				return res;
			}
			T& operator*() const
			{
				if (DIM == 1) { return out->data_1d[x_index]; }
				if (DIM == 2) { return out->data_2d[x_index][y_index]; }
				if (DIM == 3) { return out->data_3d[x_index][y_index][z_index]; }
				if (DIM == 4) { return out->data_4d[x_index][y_index][z_index][t_index]; }
				if (DIM == 5) { return out->data_5d[x_index][y_index][z_index][t_index][h_index]; }
				throw std::invalid_argument("Not possible!!");
			}
			const T* operator->() const
			{
				if (DIM == 1) { return &(*out)(x_index); }
				if (DIM == 2) { return &(*out)(x_index, y_index); }
				if (DIM == 3) { return &(*out)(x_index, y_index, z_index); }
				if (DIM == 4) { return &(*out)(x_index, y_index, z_index, t_index); }
				if (DIM == 5) { return &(*out)(x_index, y_index, z_index, t_index, h_index); }
				throw std::invalid_argument("Not possible!!");
			}

			bool operator==(const iterator& tmp) const
			{
				return out == tmp.out && x_index == tmp.x_index && y_index == tmp.y_index &&
					z_index == tmp.z_index && t_index == tmp.t_index && h_index == tmp.h_index && flag == tmp.flag;
			}
			bool operator!=(const iterator& tmp) const
			{
				return out != tmp.out || x_index != tmp.x_index || y_index != tmp.y_index ||
					z_index != tmp.z_index || t_index != tmp.t_index || h_index != tmp.h_index || flag != tmp.flag;
			}
			template <typename U, not_class(U)> iterator operator ,(U rhs)
			{
				if (*this == this->out->end_all()) { throw std::invalid_argument("Out of range!"); }
				this->operator*() = rhs;
				return this->operator++();
			}
		};
		class const_iterator
		{
			/* Para */
			const VectorBase* out;
			int x_index, y_index, z_index, t_index, h_index;
			FLAG4D flag;
		public:
			const_iterator() : out(nullptr), x_index(0), y_index(0), z_index(0), t_index(0), h_index(0), flag(ZERO) {}
			const_iterator(const VectorBase* _out, const int& _x_index, const int& _y_index, const int& _z_index, const int& _t_index, const FLAG4D& _flag) :
				out(_out), x_index(_x_index), y_index(_y_index), z_index(_z_index), t_index(_t_index), h_index(0), flag(_flag) {}

			const_iterator operator++()
			{
				// 1d
				if (flag == ALL)
				{
					++this->x_index;
					return *this;
				}
				// 2d
				if (flag == _X) { ++this->y_index; return *this; }
				if (flag == _Y) { ++this->x_index; return *this; }
				if (flag == _ALL)
				{
					++this->y_index;
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == _TRANS)
				{
					++this->x_index;
					if (this->x_index == out->x_size) { this->x_index = 0; ++this->y_index; }
					return *this;
				}
				// 3d
				if (flag == __X)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == __Y)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == __Z)
				{
					++this->y_index;
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == __XY) { ++this->z_index; return *this; }
				if (flag == __XZ) { ++this->y_index; return *this; }
				if (flag == __YZ) { ++this->x_index; return *this; }
				if (flag == __ALL)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				//4d
				if (flag == ___X)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ___Y)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___Z)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___T)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___XY)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					return *this;
				}
				if (flag == ___XZ)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ___XT)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ___YZ)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___YT)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___ZT)
				{
					++this->y_index;
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ___XYZ) { ++this->t_index; return *this; }
				if (flag == ___XYT) { ++this->z_index; return *this; }
				if (flag == ___XZT) { ++this->y_index; return *this; }
				if (flag == ___YZT) { ++this->x_index; return *this; }
				if (flag == ___ALL)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				// 5d
				if (flag == ____X)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____Y)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____Z)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____T)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____H)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____XY)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					return *this;
				}
				if (flag == ____XZ)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____XT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____XH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____YZ)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____YT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____YH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____ZT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____ZH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____TH)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____XYZ)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					return *this;
				}
				if (flag == ____XYT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->z_index; }
					return *this;
				}
				if (flag == ____XYH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					return *this;
				}
				if (flag == ____XZT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____XZH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____XTH)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					return *this;
				}
				if (flag == ____YZT)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____YZH)
				{
					++this->t_index;
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____YTH)
				{
					++this->z_index;
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____ZTH)
				{
					++this->y_index;
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				if (flag == ____XYZT) { ++this->h_index; return *this; }
				if (flag == ____XYZH) { ++this->t_index; return *this; }
				if (flag == ____XYTH) { ++this->z_index; return *this; }
				if (flag == ____XZTH) { ++this->y_index; return *this; }
				if (flag == ____YZTH) { ++this->x_index; return *this; }
				if (flag == ____ALL)
				{
					++this->h_index;
					if (this->h_index == out->h_size) { this->h_index = 0; ++this->t_index; }
					if (this->t_index == out->t_size) { this->t_index = 0; ++this->z_index; }
					if (this->z_index == out->z_size) { this->z_index = 0; ++this->y_index; }
					if (this->y_index == out->y_size) { this->y_index = 0; ++this->x_index; }
					return *this;
				}
				throw std::invalid_argument("WTF???");
			}
			template <typename U, is_numeric(U)> const_iterator operator +(U i)
			{
				iterator res{ out, x_index, y_index, z_index, t_index, h_index, flag };
				for (auto t = 0; t < i; ++t) { ++res; }
				return res;
			}
			const T& operator*() const
			{
				if (DIM == 1) { return out->data_1d[x_index]; }
				if (DIM == 2) { return out->data_2d[x_index][y_index]; }
				if (DIM == 3) { return out->data_3d[x_index][y_index][z_index]; }
				if (DIM == 4) { return out->data_4d[x_index][y_index][z_index][t_index]; }
				if (DIM == 5) { return out->data_5d[x_index][y_index][z_index][t_index][h_index]; }
				throw std::invalid_argument("Not possible!!");
			}
			const T* operator->() const
			{
				if (DIM == 1) { return &(*out)(x_index); }
				if (DIM == 2) { return &(*out)(x_index, y_index); }
				if (DIM == 3) { return &(*out)(x_index, y_index, z_index); }
				if (DIM == 4) { return &(*out)(x_index, y_index, z_index, t_index); }
				if (DIM == 5) { return &(*out)(x_index, y_index, z_index, t_index, h_index); }
				throw std::invalid_argument("Not possible!!");
			}

			bool operator==(const const_iterator& tmp) const
			{
				return out == tmp.out && x_index == tmp.x_index && y_index == tmp.y_index && z_index == tmp.z_index &&
					t_index == tmp.t_index && h_index == tmp.h_index && flag == tmp.flag;
			}
			bool operator!=(const const_iterator& tmp) const
			{
				return out != tmp.out || x_index != tmp.x_index || y_index != tmp.y_index || z_index != tmp.z_index ||
					t_index != tmp.t_index || h_index != tmp.h_index || flag != tmp.flag;
			}
		};
	protected:
		/* Para */
		std::vector<T>                                                       data_1d;
		std::vector<std::vector<T>>                                          data_2d;
		std::vector<std::vector<std::vector<T>>>                             data_3d;
		std::vector<std::vector<std::vector<std::vector<T>>>>                data_4d;
		std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>   data_5d;
		const int                                                            DIMFLAG;

		/* constructor */
		VectorBase() : x_size(0), y_size(0), z_size(0), t_size(0), h_size(0), DIMFLAG(DIM) {}
		explicit VectorBase(const int& x_axis) : x_size(x_axis), y_size(0), z_size(0), t_size(0), h_size(0), DIMFLAG(DIM)
		{
			data_1d.resize(x_axis);
		}
		template <typename U> VectorBase(const bool& check, const int& x_axis, U content) :
			x_size(x_axis), y_size(0), z_size(0), t_size(0), h_size(0), DIMFLAG(DIM)
		{
			data_1d.resize(x_axis, content);
		}
		VectorBase(const int& x_axis, const int& y_axis) :
			x_size(x_axis), y_size(y_axis), z_size(0), t_size(0), h_size(0), DIMFLAG(DIM)
		{
			data_2d.resize(x_axis,
				std::vector<T>(y_axis));
		}
		template <typename U> VectorBase(const bool& check, const int& x_axis, const int& y_axis, U content) :
			x_size(x_axis), y_size(y_axis), z_size(0), t_size(0), h_size(0), DIMFLAG(DIM)
		{
			data_2d.resize(x_axis,
				std::vector<T>(y_axis, content));
		}
		VectorBase(const int& x_axis, const int& y_axis, const int& z_axis) :
			x_size(x_axis), y_size(y_axis), z_size(z_axis), t_size(0), h_size(0), DIMFLAG(DIM)
		{
			data_3d.resize(x_axis,
				std::vector<std::vector<T>>(y_axis,
					std::vector<T>(z_axis)));
		}
		template <typename U> VectorBase(const bool& check, const int& x_axis, const int& y_axis, const int& z_axis, U content) :
			x_size(x_axis), y_size(y_axis), z_size(z_axis), t_size(0), h_size(0), DIMFLAG(DIM)
		{
			data_3d.resize(x_axis,
				std::vector<std::vector<T>>(y_axis,
					std::vector<T>(z_axis, content)));
		}
		VectorBase(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis) :
			x_size(x_axis), y_size(y_axis), z_size(z_axis), t_size(t_axis), h_size(0), DIMFLAG(DIM)
		{
			data_4d.resize(x_axis,
				std::vector<std::vector<std::vector<T>>>(y_axis,
					std::vector<std::vector<T>>(z_axis,
						std::vector<T>(t_axis))));
		}
		template <typename U> VectorBase(const bool& check, const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, U content) :
			x_size(x_axis), y_size(y_axis), z_size(z_axis), t_size(t_axis), h_size(0), DIMFLAG(DIM)
		{
			data_4d.resize(x_axis,
				std::vector<std::vector<std::vector<T>>>(y_axis,
					std::vector<std::vector<T>>(z_axis,
						std::vector<T>(t_axis, content))));
		}
		VectorBase(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis) :
			x_size(x_axis), y_size(y_axis), z_size(z_axis), t_size(t_axis), h_size(h_axis), DIMFLAG(DIM)
		{
			data_5d.resize(x_axis,
				std::vector<std::vector<std::vector<std::vector<T>>>>(y_axis,
					std::vector<std::vector<std::vector<T>>>(z_axis,
						std::vector<std::vector<T>>(t_axis,
							std::vector<T>(h_axis)))));
		}
		template <typename U> VectorBase(const bool& check, const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis, U content) :
			x_size(x_axis), y_size(y_axis), z_size(z_axis), t_size(t_axis), h_size(h_axis), DIMFLAG(DIM)
		{
			data_5d.resize(x_axis,
				std::vector<std::vector<std::vector<std::vector<T>>>>(y_axis,
					std::vector<std::vector<std::vector<T>>>(z_axis,
						std::vector<std::vector<T>>(t_axis,
							std::vector<T>(h_axis, content)))));
		}

		/* destructor */
		~VectorBase()
		{
			if (DIMFLAG == 1) { data_1d.clear(); }
			else if (DIMFLAG == 2) { data_2d.clear(); }
			else if (DIMFLAG == 3) { data_3d.clear(); }
			else if (DIMFLAG == 4) { data_4d.clear(); }
			else if (DIMFLAG == 5) { data_5d.clear(); }
		}
		/* copy constructor */
		VectorBase(const VectorBase& A) : x_size(A.x_size), y_size(A.y_size), z_size(A.z_size), t_size(A.t_size), h_size(A.h_size),
			data_1d(A.data_1d), data_2d(A.data_2d), data_3d(A.data_3d), data_4d(A.data_4d), data_5d(A.data_5d), DIMFLAG(A.DIMFLAG) {}
		VectorBase& operator =(VectorBase const& rhs)
		{
			data_1d = rhs.data_1d; x_size = rhs.x_size;
			data_2d = rhs.data_2d; y_size = rhs.y_size;
			data_3d = rhs.data_3d; z_size = rhs.z_size;
			data_4d = rhs.data_4d; t_size = rhs.t_size;
			data_5d = rhs.data_5d; h_size = rhs.h_size;
			return *this;
		}
		/* move constructor */
		VectorBase(VectorBase&& A) noexcept : x_size(std::move(A.x_size)), y_size(std::move(A.y_size)),
			z_size(std::move(A.z_size)), t_size(std::move(A.t_size)), h_size(std::move(A.h_size)),
			data_1d(std::move(A.data_1d)), data_2d(std::move(A.data_2d)), data_3d(std::move(A.data_3d)),
			data_4d(std::move(A.data_4d)), data_5d(std::move(A.data_5d)), DIMFLAG(A.DIMFLAG) {}
		/* move assignment operator */
		VectorBase& operator=(VectorBase&& A) noexcept
		{
			data_1d = std::move(A.data_1d);
			data_2d = std::move(A.data_2d);
			data_3d = std::move(A.data_3d);
			data_4d = std::move(A.data_4d);
			data_5d = std::move(A.data_5d);
			x_size = std::move(A.x_size);
			y_size = std::move(A.y_size);
			z_size = std::move(A.z_size);
			t_size = std::move(A.t_size);
			h_size = std::move(A.h_size);
			return *this;
		}

		/* resize */
		void resize(const int& x_axis)
		{
			x_size = x_axis;
			data_1d.resize(x_axis);
		}
		template <typename U> void resize(const bool& check, const int& x_axis, U content)
		{
			x_size = x_axis;
			data_1d.resize(x_axis, content);
		}
		void resize(const int& x_axis, const int& y_axis)
		{
			x_size = x_axis; y_size = y_axis;
			data_2d.resize(x_axis);
			for (auto& line : data_2d) { line.resize(y_axis); }
		}
		template <typename U> void resize(const bool& check, const int& x_axis, const int& y_axis, U content)
		{
			x_size = x_axis; y_size = y_axis;
			data_2d.resize(x_axis);
			for (auto& line : data_2d) { line.resize(y_axis, content); }
		}
		void resize(const int& x_axis, const int& y_axis, const int& z_axis)
		{
			x_size = x_axis; y_size = y_axis; z_size = z_axis;
			data_3d.resize(x_axis);
			for (auto& block : data_3d) { block.resize(y_axis); }
			for (auto& block : data_3d) for (auto& line : block) { line.resize(z_axis); }
		}
		template <typename U> void resize(const bool& check, const int& x_axis, const int& y_axis, const int& z_axis, U content)
		{
			x_size = x_axis; y_size = y_axis; z_size = z_axis;
			data_3d.resize(x_axis);
			for (auto& block : data_3d) { block.resize(y_axis); }
			for (auto& block : data_3d) for (auto& line : block) { line.resize(z_axis, content); }
		}
		void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis)
		{
			x_size = x_axis; y_size = y_axis; z_size = z_axis; t_size = t_axis;
			data_4d.resize(x_axis);
			for (auto& space : data_4d) { space.resize(y_axis); }
			for (auto& space : data_4d) for (auto& block : space) { block.resize(z_axis); }
			for (auto& space : data_4d) for (auto& block : space) for (auto& line : block) { line.resize(t_axis); }
		}
		template <typename U> void resize(const bool& check, const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, U content)
		{
			x_size = x_axis; y_size = y_axis; z_size = z_axis; t_size = t_axis;
			data_4d.resize(x_axis);
			for (auto& space : data_4d) { space.resize(y_axis); }
			for (auto& space : data_4d) for (auto& block : space) { block.resize(z_axis); }
			for (auto& space : data_4d) for (auto& block : space) for (auto& line : block) { line.resize(t_axis, content); }
		}
		void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis)
		{
			x_size = x_axis; y_size = y_axis; z_size = z_axis; t_size = t_axis; h_size = h_axis;
			data_5d.resize(x_axis);
			for (auto& time : data_5d) { time.resize(y_axis); }
			for (auto& time : data_5d) for (auto& space : time) { space.resize(z_axis); }
			for (auto& time : data_5d) for (auto& space : time) for (auto& block : space) { block.resize(t_axis); }
			for (auto& time : data_5d) for (auto& space : time) for (auto& block : space) for (auto& line : block) { line.resize(t_axis); }
		}
		template <typename U> void resize(const bool& check, const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis, U content)
		{
			x_size = x_axis; y_size = y_axis; z_size = z_axis; t_size = t_axis; h_size = h_axis;
			data_5d.resize(x_axis);
			for (auto& time : data_5d) { time.resize(y_axis); }
			for (auto& time : data_5d) for (auto& space : time) { space.resize(z_axis); }
			for (auto& time : data_5d) for (auto& space : time) for (auto& block : space) { block.resize(t_axis); }
			for (auto& time : data_5d) for (auto& space : time) for (auto& block : space) for (auto& line : block) { line.resize(t_axis, content); }
		}


		/* iterator */
		iterator       begin_all() noexcept
		{
			if (DIM == 1) { return { this,      0,      0,      0,      0,      0, ALL }; }
			if (DIM == 2) { return { this,      0,      0,      0,      0,      0, _ALL }; }
			if (DIM == 3) { return { this,      0,      0,      0,      0,      0, __ALL }; }
			if (DIM == 4) { return { this,      0,      0,      0,      0,      0, ___ALL }; }
			if (DIM == 5) { return { this,      0,      0,      0,      0,      0, ____ALL }; }
			return {};
		}
		iterator         end_all() noexcept
		{
			if (DIM == 1) { return { this, x_size,      0,      0,      0,      0, ALL }; }
			if (DIM == 2) { return { this, x_size,      0,      0,      0,      0, _ALL }; }
			if (DIM == 3) { return { this, x_size,      0,      0,      0,      0, __ALL }; }
			if (DIM == 4) { return { this, x_size,      0,      0,      0,      0, ___ALL }; }
			if (DIM == 5) { return { this, x_size,      0,      0,      0,      0, ____ALL }; }
			return {};
		}
		iterator       begin_trans() noexcept
		{
			return { this,      0,      0,      0,      0,      0, _TRANS };
		}
		iterator         end_trans() noexcept
		{
			return { this,      0, y_size,      0,      0,      0, _TRANS };
		}
		iterator       begin_x(const int& x) noexcept
		{
			if (DIM == 2) { return { this,      x,      0,      0,      0,      0, _X }; }
			if (DIM == 3) { return { this,      x,      0,      0,      0,      0, __X }; }
			if (DIM == 4) { return { this,      x,      0,      0,      0,      0, ___X }; }
			if (DIM == 5) { return { this,      x,      0,      0,      0,      0, ____X }; }
			return {};
		}
		iterator         end_x(const int& x) noexcept
		{
			if (DIM == 2) { return { this,      x, y_size,      0,      0,      0, _X }; }
			if (DIM == 3) { return { this,      x, y_size,      0,      0,      0, __X }; }
			if (DIM == 4) { return { this,      x, y_size,      0,      0,      0, ___X }; }
			if (DIM == 5) { return { this,      x, y_size,      0,      0,      0, ____X }; }
			return {};
		}
		iterator       begin_y(const int& y) noexcept
		{
			if (DIM == 2) { return { this,      0,      y,      0,      0,      0, _Y }; }
			if (DIM == 3) { return { this,      0,      y,      0,      0,      0, __Y }; }
			if (DIM == 4) { return { this,      0,      y,      0,      0,      0, ___Y }; }
			if (DIM == 5) { return { this,      0,      y,      0,      0,      0, ____Y }; }
			return {};
		}
		iterator         end_y(const int& y) noexcept
		{
			if (DIM == 2) { return { this, x_size,      y,      0,      0,      0, _Y }; }
			if (DIM == 3) { return { this, x_size,      y,      0,      0,      0, __Y }; }
			if (DIM == 4) { return { this, x_size,      y,      0,      0,      0, ___Y }; }
			if (DIM == 4) { return { this, x_size,      y,      0,      0,      0, ____Y }; }
			return {};
		}
		iterator       begin_z(const int& z) noexcept
		{
			if (DIM == 3) { return { this,      0,      0,      z,      0,      0, __Z }; }
			if (DIM == 4) { return { this,      0,      0,      z,      0,      0, ___Z }; }
			if (DIM == 5) { return { this,      0,      0,      z,      0,      0, ____Z }; }
			return {};
		}
		iterator         end_z(const int& z) noexcept
		{
			if (DIM == 3) { return { this, x_size,      0,      z,      0,      0, __Z }; }
			if (DIM == 4) { return { this, x_size,      0,      z,      0,      0, ___Z }; }
			if (DIM == 5) { return { this, x_size,      0,      z,      0,      0, ____Z }; }
			return {};
		}
		iterator       begin_t(const int& t) noexcept
		{
			if (DIM == 4) { return { this,      0,      0,      0,      t,      0, ___T }; }
			if (DIM == 5) { return { this,      0,      0,      0,      t,      0, ____T }; }
			return {};
		}
		iterator         end_t(const int& t) noexcept
		{
			if (DIM == 4) { return { this, x_size,      0,      0,      t,      0, ___T }; }
			if (DIM == 5) { return { this, x_size,      0,      0,      t,      0, ____T }; }
			return {};
		}
		iterator       begin_h(const int& h) noexcept
		{
			if (DIM == 5) { return { this,      0,      0,      0,      0,      h, ____H }; }
			return {};
		}
		iterator         end_h(const int& h) noexcept
		{
			if (DIM == 5) { return { this, x_size,      0,      0,      0,      h, ____H }; }
			return {};
		}
		iterator       begin_xy(const int& x, const int& y) noexcept
		{
			if (DIM == 3) { return { this,      x,      y,      0,      0,      0, __XY }; }
			if (DIM == 4) { return { this,      x,      y,      0,      0,      0, ___XY }; }
			if (DIM == 5) { return { this,      x,      y,      0,      0,      0, ____XY }; }
			return {};
		}
		iterator         end_xy(const int& x, const int& y) noexcept
		{
			if (DIM == 3) { return { this,      x,      y, z_size,      0,      0, __XY }; }
			if (DIM == 4) { return { this,      x,      y, z_size,      0,      0, ___XY }; }
			if (DIM == 5) { return { this,      x,      y, z_size,      0,      0, ____XY }; }
			return {};
		}
		iterator       begin_xz(const int& x, const int& z) noexcept
		{
			if (DIM == 3) { return { this,      x,      0,      0,      z,      0, __XZ }; }
			if (DIM == 4) { return { this,      x,      0,      0,      z,      0, ___XZ }; }
			if (DIM == 5) { return { this,      x,      0,      0,      z,      0, ____XZ }; }
			return {};
		}
		iterator         end_xz(const int& x, const int& z) noexcept
		{
			if (DIM == 3) { return { this,      x, y_size,      0,      z,      0, __XZ }; }
			if (DIM == 4) { return { this,      x, y_size,      0,      z,      0, ___XZ }; }
			if (DIM == 5) { return { this,      x, y_size,      0,      z,      0, ____XZ }; }
			return {};
		}
		iterator       begin_yz(const int& y, const int& z) noexcept
		{
			if (DIM == 3) { return { this,      0,      y,      z,      0,      0, __YZ }; }
			if (DIM == 4) { return { this,      0,      y,      z,      0,      0, ___YZ }; }
			if (DIM == 5) { return { this,      0,      y,      z,      0,      0, ____YZ }; }
			return {};
		}
		iterator         end_yz(const int& y, const int& z) noexcept
		{
			if (DIM == 3) { return { this, x_size,      y,      z,      0,      0, __YZ }; }
			if (DIM == 4) { return { this, x_size,      y,      z,      0,      0, ___YZ }; }
			if (DIM == 5) { return { this, x_size,      y,      z,      0,      0, ____YZ }; }
			return {};
		}
		iterator       begin_xt(const int& x, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      x,      0,      0,      t,      0, ___XT }; }
			if (DIM == 5) { return { this,      x,      0,      0,      t,      0, ____XT }; }
			return {};
		}
		iterator         end_xt(const int& x, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      x, y_size,      0,      t,      0, ___XT }; }
			if (DIM == 5) { return { this,      x, y_size,      0,      t,      0, ____XT }; }
			return {};
		}
		iterator       begin_yt(const int& y, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      0,      y,      0,      t,      0, ___YT }; }
			if (DIM == 5) { return { this,      0,      y,      0,      t,      0, ____YT }; }
			return {};
		}
		iterator         end_yt(const int& y, const int& t) noexcept
		{
			if (DIM == 4) { return { this, x_size,      y,      0,      t,      0, ___YT }; }
			if (DIM == 5) { return { this, x_size,      y,      0,      t,      0, ____YT }; }
			return {};
		}
		iterator       begin_zt(const int& z, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      0,      0,      z,      t,      0, ___ZT }; }
			if (DIM == 5) { return { this,      0,      0,      z,      t,      0, ____ZT }; }
			return {};
		}
		iterator         end_zt(const int& z, const int& t) noexcept
		{
			if (DIM == 4) { return { this, x_size,      0,      z,      t,      0, ___ZT }; }
			if (DIM == 5) { return { this, x_size,      0,      z,      t,      0, ____ZT }; }
			return {};
		}
		iterator       begin_xh(const int& x, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      0,      0,      0,      h, ____XH }; }
			return {};
		}
		iterator         end_xh(const int& x, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x, y_size,      0,      0,      h, ____XH }; }
			return {};
		}
		iterator       begin_yh(const int& y, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      0,      y,      0,      0,      h, ____YH }; }
			return {};
		}
		iterator         end_yh(const int& y, const int& h) noexcept
		{
			if (DIM == 5) { return { this, x_size,      y,      0,      0,      h, ____YH }; }
			return {};
		}
		iterator       begin_zh(const int& z, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      0,      0,      z,      0,      h, ____ZH }; }
			return {};
		}
		iterator         end_zh(const int& z, const int& h) noexcept
		{
			if (DIM == 5) { return { this, x_size,      0,      z,      0,      h, ____ZH }; }
			return {};
		}
		iterator       begin_th(const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      0,      0,      0,      t,      h, ____TH }; }
			return {};
		}
		iterator         end_th(const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this, x_size,      0,      0,      t,      h, ____TH }; }
			return {};
		}
		iterator       begin_xyz(const int& x, const int& y, const int& z) noexcept
		{
			if (DIM == 4) { return { this,      x,      y,      z,      0,      0, ___XYZ }; }
			if (DIM == 5) { return { this,      x,      y,      z,      0,      0, ____XYZ }; }
			return {};
		}
		iterator         end_xyz(const int& x, const int& y, const int& z) noexcept
		{
			if (DIM == 4) { return { this,      x,      y,      z, t_size,      0, ___XYZ }; }
			if (DIM == 5) { return { this,      x,      y,      z, t_size,      0, ____XYZ }; }
			return {};
		}
		iterator       begin_xyt(const int& x, const int& y, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      x,      y,      0,      t,      0, ___XYT }; }
			if (DIM == 5) { return { this,      x,      y,      0,      t,      0, ____XYT }; }
			return {};
		}
		iterator         end_xyt(const int& x, const int& y, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      x,      y, z_size,      t,      0, ___XYT }; }
			if (DIM == 5) { return { this,      x,      y, z_size,      t,      0, ____XYT }; }
			return {};
		}
		iterator       begin_xzt(const int& x, const int& z, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      x,      0,      z,      t,      0, ___XZT }; }
			if (DIM == 5) { return { this,      x,      0,      z,      t,      0, ____XZT }; }
			return {};
		}
		iterator         end_xzt(const int& x, const int& z, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      x, y_size,      z,      t,      0, ___XZT }; }
			if (DIM == 5) { return { this,      x, y_size,      z,      t,      0, ____XZT }; }
			return {};
		}
		iterator       begin_yzt(const int& y, const int& z, const int& t) noexcept
		{
			if (DIM == 4) { return { this,      0,      y,      z,      t,      0, ___YZT }; }
			if (DIM == 5) { return { this,      0,      y,      z,      t,      0, ____YZT }; }
			return {};
		}
		iterator         end_yzt(const int& y, const int& z, const int& t) noexcept
		{
			if (DIM == 4) { return { this, x_size,      y,      z,      t,      0, ___YZT }; }
			if (DIM == 5) { return { this, x_size,      y,      z,      t,      0, ____YZT }; }
			return {};
		}
		iterator       begin_xyh(const int& x, const int& y, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      0,      0,      h, ____XYH }; }
			return {};
		}
		iterator         end_xyh(const int& x, const int& y, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      y, z_size,      0,      h, ____XYH }; }
			return {};
		}
		iterator       begin_xzh(const int& x, const int& z, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      0,      z,      0,      h, ____XZH }; }
			return {};
		}
		iterator         end_xzh(const int& x, const int& z, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x, y_size,      z,      0,      h, ____XZH }; }
			return {};
		}
		iterator       begin_xth(const int& x, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      0,      0,      t,      h, ____XTH }; }
			return {};
		}
		iterator         end_xth(const int& x, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x, y_size,      0,      t,      h, ____XTH }; }
			return {};
		}
		iterator       begin_yzh(const int& y, const int& z, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      0,      y,      z,      0,      h, ____YZH }; }
			return {};
		}
		iterator         end_yzh(const int& y, const int& z, const int& h) noexcept
		{
			if (DIM == 5) { return { this, x_size,      y,      z,      0,      h, ____YZH }; }
			return {};
		}
		iterator       begin_yth(const int& y, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      0,      y,      0,      t,      h, ____YTH }; }
			return {};
		}
		iterator         end_yth(const int& y, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this, x_size,      y,      0,      t,      h, ____YTH }; }
			return {};
		}
		iterator       begin_zth(const int& z, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      0,      0,      z,      t,      h, ____ZTH }; }
			return {};
		}
		iterator         end_zth(const int& z, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this, x_size,      0,      z,      t,      h, ____ZTH }; }
			return {};
		}
		iterator       begin_xyzt(const int& x, const int& y, const int& z, const int& t) noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      z,      t,      0, ____XYZT }; }
			return {};
		}
		iterator         end_xyzt(const int& x, const int& y, const int& z, const int& t) noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      z,      t, h_size, ____XYZT }; }
			return {};
		}
		iterator       begin_xyzh(const int& x, const int& y, const int& z, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      z,      0,      h, ____XYZH }; }
			return {};
		}
		iterator         end_xyzh(const int& x, const int& y, const int& z, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      z, t_size,      h, ____XYZH }; }
			return {};
		}
		iterator       begin_xyth(const int& x, const int& y, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      0,      t,      h, ____XYTH }; }
			return {};
		}
		iterator         end_xyth(const int& x, const int& y, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      y,  z_size,      t,      h, ____XYTH }; }
			return {};
		}
		iterator       begin_xzth(const int& x, const int& z, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x,      0,      z,      t,      h, ____XZTH }; }
			return {};
		}
		iterator         end_xzth(const int& x, const int& z, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      x, y_size,      z,      t,      h, ____XZTH }; }
			return {};
		}
		iterator       begin_yzth(const int& y, const int& z, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this,      0,      y,      z,      t,      h, ____YZTH }; }
			return {};
		}
		iterator         end_yzth(const int& y, const int& z, const int& t, const int& h) noexcept
		{
			if (DIM == 5) { return { this, x_size,      y,      z,      t,      h, ____YZTH }; }
			return {};
		}
		/* const_iterator */
		const_iterator begin_all() const noexcept
		{
			if (DIM == 1) { return { this,      0,      0,      0,      0,      0, ALL }; }
			if (DIM == 2) { return { this,      0,      0,      0,      0,      0, _ALL }; }
			if (DIM == 3) { return { this,      0,      0,      0,      0,      0, __ALL }; }
			if (DIM == 4) { return { this,      0,      0,      0,      0,      0, ___ALL }; }
			if (DIM == 5) { return { this,      0,      0,      0,      0,      0, ____ALL }; }
			return {};
		}
		const_iterator   end_all() const noexcept
		{
			if (DIM == 1) { return { this, x_size,      0,      0,      0,      0, ALL }; }
			if (DIM == 2) { return { this, x_size,      0,      0,      0,      0, _ALL }; }
			if (DIM == 3) { return { this, x_size,      0,      0,      0,      0, __ALL }; }
			if (DIM == 4) { return { this, x_size,      0,      0,      0,      0, ___ALL }; }
			if (DIM == 5) { return { this, x_size,      0,      0,      0,      0, ____ALL }; }
			return {};

		}
		const_iterator begin_trans() const noexcept
		{
			return { this,      0,      0,      0,      0,      0, _TRANS };
		}
		const_iterator   end_trans() const noexcept
		{
			return { this,      0, y_size,      0,      0,      0, _TRANS };
		}
		const_iterator begin_x(const int& x) const noexcept
		{
			if (DIM == 2) { return { this,      x,      0,      0,      0,      0, _X }; }
			if (DIM == 3) { return { this,      x,      0,      0,      0,      0, __X }; }
			if (DIM == 4) { return { this,      x,      0,      0,      0,      0, ___X }; }
			if (DIM == 5) { return { this,      x,      0,      0,      0,      0, ____X }; }
			return {};
		}
		const_iterator   end_x(const int& x) const noexcept
		{
			if (DIM == 2) { return { this,      x, y_size,      0,      0,      0, _X }; }
			if (DIM == 3) { return { this,      x, y_size,      0,      0,      0, __X }; }
			if (DIM == 4) { return { this,      x, y_size,      0,      0,      0, ___X }; }
			if (DIM == 5) { return { this,      x, y_size,      0,      0,      0, ____X }; }
			return {};
		}
		const_iterator begin_y(const int& y) const noexcept
		{
			if (DIM == 2) { return { this,      0,      y,      0,      0,      0, _Y }; }
			if (DIM == 3) { return { this,      0,      y,      0,      0,      0, __Y }; }
			if (DIM == 4) { return { this,      0,      y,      0,      0,      0, ___Y }; }
			if (DIM == 5) { return { this,      0,      y,      0,      0,      0, ____Y }; }
			return {};
		}
		const_iterator   end_y(const int& y) const noexcept
		{
			if (DIM == 2) { return { this, x_size,      y,      0,      0,      0, _Y }; }
			if (DIM == 3) { return { this, x_size,      y,      0,      0,      0, __Y }; }
			if (DIM == 4) { return { this, x_size,      y,      0,      0,      0, ___Y }; }
			if (DIM == 4) { return { this, x_size,      y,      0,      0,      0, ____Y }; }
			return {};
		}
		const_iterator begin_z(const int& z) const noexcept
		{
			if (DIM == 3) { return { this,      0,      0,      z,      0,      0, __Z }; }
			if (DIM == 4) { return { this,      0,      0,      z,      0,      0, ___Z }; }
			if (DIM == 5) { return { this,      0,      0,      z,      0,      0, ____Z }; }
			return {};
		}
		const_iterator   end_z(const int& z) const noexcept
		{
			if (DIM == 3) { return { this, x_size,      0,      z,      0,      0, __Z }; }
			if (DIM == 4) { return { this, x_size,      0,      z,      0,      0, ___Z }; }
			if (DIM == 5) { return { this, x_size,      0,      z,      0,      0, ____Z }; }
			return {};
		}
		const_iterator begin_t(const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      0,      0,      0,      t,      0, ___T }; }
			if (DIM == 5) { return { this,      0,      0,      0,      t,      0, ____T }; }
			return {};
		}
		const_iterator   end_t(const int& t) const noexcept
		{
			if (DIM == 4) { return { this, x_size,      0,      0,      t,      0, ___T }; }
			if (DIM == 5) { return { this, x_size,      0,      0,      t,      0, ____T }; }
			return {};
		}
		const_iterator begin_h(const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      0,      0,      0,      0,      h, ____H }; }
			return {};
		}
		const_iterator   end_h(const int& h) const noexcept
		{
			if (DIM == 5) { return { this, x_size,      0,      0,      0,      h, ____H }; }
			return {};
		}
		const_iterator begin_xy(const int& x, const int& y) const noexcept
		{
			if (DIM == 3) { return { this,      x,      y,      0,      0,      0, __XY }; }
			if (DIM == 4) { return { this,      x,      y,      0,      0,      0, ___XY }; }
			if (DIM == 5) { return { this,      x,      y,      0,      0,      0, ____XY }; }
			return {};
		}
		const_iterator   end_xy(const int& x, const int& y) const noexcept
		{
			if (DIM == 3) { return { this,      x,      y, z_size,      0,      0, __XY }; }
			if (DIM == 4) { return { this,      x,      y, z_size,      0,      0, ___XY }; }
			if (DIM == 5) { return { this,      x,      y, z_size,      0,      0, ____XY }; }
			return {};
		}
		const_iterator begin_xz(const int& x, const int& z) const noexcept
		{
			if (DIM == 3) { return { this,      x,      0,      0,      z,      0, __XZ }; }
			if (DIM == 4) { return { this,      x,      0,      0,      z,      0, ___XZ }; }
			if (DIM == 5) { return { this,      x,      0,      0,      z,      0, ____XZ }; }
			return {};
		}
		const_iterator   end_xz(const int& x, const int& z) const noexcept
		{
			if (DIM == 3) { return { this,      x, y_size,      0,      z,      0, __XZ }; }
			if (DIM == 4) { return { this,      x, y_size,      0,      z,      0, ___XZ }; }
			if (DIM == 5) { return { this,      x, y_size,      0,      z,      0, ____XZ }; }
			return {};
		}
		const_iterator begin_yz(const int& y, const int& z) const noexcept
		{
			if (DIM == 3) { return { this,      0,      y,      z,      0,      0, __YZ }; }
			if (DIM == 4) { return { this,      0,      y,      z,      0,      0, ___YZ }; }
			if (DIM == 5) { return { this,      0,      y,      z,      0,      0, ____YZ }; }
			return {};
		}
		const_iterator   end_yz(const int& y, const int& z) const noexcept
		{
			if (DIM == 3) { return { this, x_size,      y,      z,      0,      0, __YZ }; }
			if (DIM == 4) { return { this, x_size,      y,      z,      0,      0, ___YZ }; }
			if (DIM == 5) { return { this, x_size,      y,      z,      0,      0, ____YZ }; }
			return {};
		}
		const_iterator begin_xt(const int& x, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      x,      0,      0,      t,      0, ___XT }; }
			if (DIM == 5) { return { this,      x,      0,      0,      t,      0, ____XT }; }
			return {};
		}
		const_iterator   end_xt(const int& x, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      x, y_size,      0,      t,      0, ___XT }; }
			if (DIM == 5) { return { this,      x, y_size,      0,      t,      0, ____XT }; }
			return {};
		}
		const_iterator begin_yt(const int& y, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      0,      y,      0,      t,      0, ___YT }; }
			if (DIM == 5) { return { this,      0,      y,      0,      t,      0, ____YT }; }
			return {};
		}
		const_iterator   end_yt(const int& y, const int& t) const noexcept
		{
			if (DIM == 4) { return { this, x_size,      y,      0,      t,      0, ___YT }; }
			if (DIM == 5) { return { this, x_size,      y,      0,      t,      0, ____YT }; }
			return {};
		}
		const_iterator begin_zt(const int& z, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      0,      0,      z,      t,      0, ___ZT }; }
			if (DIM == 5) { return { this,      0,      0,      z,      t,      0, ____ZT }; }
			return {};
		}
		const_iterator   end_zt(const int& z, const int& t) const noexcept
		{
			if (DIM == 4) { return { this, x_size,      0,      z,      t,      0, ___ZT }; }
			if (DIM == 5) { return { this, x_size,      0,      z,      t,      0, ____ZT }; }
			return {};
		}
		const_iterator begin_xh(const int& x, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      0,      0,      0,      h, ____XH }; }
			return {};
		}
		const_iterator   end_xh(const int& x, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x, y_size,      0,      0,      h, ____XH }; }
			return {};
		}
		const_iterator begin_yh(const int& y, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      0,      y,      0,      0,      h, ____YH }; }
			return {};
		}
		const_iterator   end_yh(const int& y, const int& h) const noexcept
		{
			if (DIM == 5) { return { this, x_size,      y,      0,      0,      h, ____YH }; }
			return {};
		}
		const_iterator begin_zh(const int& z, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      0,      0,      z,      0,      h, ____ZH }; }
			return {};
		}
		const_iterator   end_zh(const int& z, const int& h) const noexcept
		{
			if (DIM == 5) { return { this, x_size,      0,      z,      0,      h, ____ZH }; }
			return {};
		}
		const_iterator begin_th(const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      0,      0,      0,      t,      h, ____TH }; }
			return {};
		}
		const_iterator   end_th(const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this, x_size,      0,      0,      t,      h, ____TH }; }
			return {};
		}
		const_iterator begin_xyz(const int& x, const int& y, const int& z) const noexcept
		{
			if (DIM == 4) { return { this,      x,      y,      z,      0,      0, ___XYZ }; }
			if (DIM == 5) { return { this,      x,      y,      z,      0,      0, ____XYZ }; }
			return {};
		}
		const_iterator   end_xyz(const int& x, const int& y, const int& z) const noexcept
		{
			if (DIM == 4) { return { this,      x,      y,      z, t_size,      0, ___XYZ }; }
			if (DIM == 5) { return { this,      x,      y,      z, t_size,      0, ____XYZ }; }
			return {};
		}
		const_iterator begin_xyt(const int& x, const int& y, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      x,      y,      0,      t,      0, ___XYT }; }
			if (DIM == 5) { return { this,      x,      y,      0,      t,      0, ____XYT }; }
			return {};
		}
		const_iterator   end_xyt(const int& x, const int& y, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      x,      y, z_size,      t,      0, ___XYT }; }
			if (DIM == 5) { return { this,      x,      y, z_size,      t,      0, ____XYT }; }
			return {};
		}
		const_iterator begin_xzt(const int& x, const int& z, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      x,      0,      z,      t,      0, ___XZT }; }
			if (DIM == 5) { return { this,      x,      0,      z,      t,      0, ____XZT }; }
			return {};
		}
		const_iterator   end_xzt(const int& x, const int& z, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      x, y_size,      z,      t,      0, ___XZT }; }
			if (DIM == 5) { return { this,      x, y_size,      z,      t,      0, ____XZT }; }
			return {};
		}
		const_iterator begin_yzt(const int& y, const int& z, const int& t) const noexcept
		{
			if (DIM == 4) { return { this,      0,      y,      z,      t,      0, ___YZT }; }
			if (DIM == 5) { return { this,      0,      y,      z,      t,      0, ____YZT }; }
			return {};
		}
		const_iterator   end_yzt(const int& y, const int& z, const int& t) const noexcept
		{
			if (DIM == 4) { return { this, x_size,      y,      z,      t,      0, ___YZT }; }
			if (DIM == 5) { return { this, x_size,      y,      z,      t,      0, ____YZT }; }
			return {};
		}
		const_iterator begin_xyh(const int& x, const int& y, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      0,      0,      h, ____XYH }; }
			return {};
		}
		const_iterator   end_xyh(const int& x, const int& y, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      y, z_size,      0,      h, ____XYH }; }
			return {};
		}
		const_iterator begin_xzh(const int& x, const int& z, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      0,      z,      0,      h, ____XZH }; }
			return {};
		}
		const_iterator   end_xzh(const int& x, const int& z, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x, y_size,      z,      0,      h, ____XZH }; }
			return {};
		}
		const_iterator begin_xth(const int& x, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      0,      0,      t,      h, ____XTH }; }
			return {};
		}
		const_iterator   end_xth(const int& x, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x, y_size,      0,      t,      h, ____XTH }; }
			return {};
		}
		const_iterator begin_yzh(const int& y, const int& z, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      0,      y,      z,      0,      h, ____YZH }; }
			return {};
		}
		const_iterator   end_yzh(const int& y, const int& z, const int& h) const noexcept
		{
			if (DIM == 5) { return { this, x_size,      y,      z,      0,      h, ____YZH }; }
			return {};
		}
		const_iterator begin_yth(const int& y, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      0,      y,      0,      t,      h, ____YTH }; }
			return {};
		}
		const_iterator   end_yth(const int& y, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this, x_size,      y,      0,      t,      h, ____YTH }; }
			return {};
		}
		const_iterator begin_zth(const int& z, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      0,      0,      z,      t,      h, ____ZTH }; }
			return {};
		}
		const_iterator   end_zth(const int& z, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this, x_size,      0,      z,      t,      h, ____ZTH }; }
			return {};
		}
		const_iterator begin_xyzt(const int& x, const int& y, const int& z, const int& t) const noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      z,      t,      0, ____XYZT }; }
			return {};
		}
		const_iterator   end_xyzt(const int& x, const int& y, const int& z, const int& t) const noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      z,      t, h_size, ____XYZT }; }
			return {};
		}
		const_iterator begin_xyzh(const int& x, const int& y, const int& z, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      z,      0,      h, ____XYZH }; }
			return {};
		}
		const_iterator   end_xyzh(const int& x, const int& y, const int& z, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      z, t_size,      h, ____XYZH }; }
			return {};
		}
		const_iterator begin_xyth(const int& x, const int& y, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      y,      0,      t,      h, ____XYTH }; }
			return {};
		}
		const_iterator   end_xyth(const int& x, const int& y, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      y,  z_size,      t,      h, ____XYTH }; }
			return {};
		}
		const_iterator begin_xzth(const int& x, const int& z, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x,      0,      z,      t,      h, ____XZTH }; }
			return {};
		}
		const_iterator   end_xzth(const int& x, const int& z, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      x, y_size,      z,      t,      h, ____XZTH }; }
			return {};
		}
		const_iterator begin_yzth(const int& y, const int& z, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this,      0,      y,      z,      t,      h, ____YZTH }; }
			return {};
		}
		const_iterator   end_yzth(const int& y, const int& z, const int& t, const int& h) const noexcept
		{
			if (DIM == 5) { return { this, x_size,      y,      z,      t,      h, ____YZTH }; }
			return {};
		}

		/* vector function */
		int Dimension() const { return DIMFLAG; }
		void clear()
		{
			x_size = y_size = z_size = t_size = h_size = 0;
			data_1d.clear();
			data_2d.clear();
			data_3d.clear();
			data_4d.clear();
			data_5d.clear();
		}
		void swap(VectorBase& B) noexcept
		{
			data_1d.swap(B.data_1d);
			data_2d.swap(B.data_2d);
			data_3d.swap(B.data_3d);
			data_4d.swap(B.data_4d);
			data_5d.swap(B.data_5d);
			std::swap(DIMFLAG, B.DIMFLAG);
			std::swap(x_size, B.x_size);
			std::swap(y_size, B.y_size);
			std::swap(z_size, B.z_size);
			std::swap(t_size, B.t_size);
			std::swap(h_size, B.h_size);
		}

		/* basic function */
		void rand(const double& Min, const double& Max)
		{
			std::default_random_engine rng;
			rng.seed(std::random_device()());
#ifdef __linux__ 
			std::uniform_real_distribution<double> dis(Min, Max);
#elif _WIN32
			const std::uniform_real_distribution<double> dis(Min, Max);
#endif
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = dis(rng); }
		}
		void randi(const int& Min, const int& Max)
		{
			std::default_random_engine rng;
			rng.seed(std::random_device()());
#ifdef __linux__ 
			std::uniform_int_distribution<int> dis(Min, Max);
#elif _WIN32
			const std::uniform_int_distribution<int> dis(Min, Max);
#endif
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = dis(rng); }
		}
		void randc(const double& MinR, const double& MaxR, const double& MinI, const double& MaxI)
		{
			std::default_random_engine rng;
			rng.seed(std::random_device()());
#ifdef __linux__ 
			std::uniform_real_distribution<double> disr(MinR, MaxR), disi(MinI, MaxI);
#elif _WIN32
			const std::uniform_real_distribution<double> disr(MinR, MaxR), disi(MinI, MaxI);
#endif		
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = complexd(disr(rng), disi(rng)); }
		}
		void zeros() { for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = 0; } }
		void ones() { for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = 1; } }
		void exp() { for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = std::exp(*iter); } }
		void exp2() { for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = std::exp2(*iter); } }
		void expm1() { for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = std::expm1(*iter); } }
		void log() { for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = std::log(*iter); } }
		void log2() { for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = std::log2(*iter); } }
		void log10() { for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = std::log10(*iter); } }
		// dot [in-class] 1d
		template <typename U> remove_cvr(T) dot1(U B)
		{
			if (x_size != B.x_size) { throw std::invalid_argument("len(A), len(B)... is different!"); }
			remove_cvr(T) res = 0;
			for (auto i : range(x_size)) { res += (*this)(i) * B(i); }
			return res;
		}
		template <typename U> VectorBase<1, T> cross(U B)
		{
			if (x_size != B.x_size || x_size != 3) { throw std::invalid_argument("len(A), len(B)... is different or length != 3 !"); }
			VectorBase<1, T> res(3, 0);
			for (auto i : range(3)) { auto m = (i + 1) % 3, n = (i + 2) % 3;  res(i) = (*this)(m)*B(n) - (*this)(n)*B(m); }
			return res;
		}
		// dot [in-class] 2d matrix dot
		template <typename U> VectorBase<2, T> dot2(U B)
		{
			if (y_size != B.x_size) { throw std::invalid_argument("A(i,j), B(k,l)... j is different from k!"); }
			VectorBase<2, T> res(x_size, B.y_size, 0);
			for (auto i : range(x_size)) { for (auto j : range(B.y_size)) { for (auto k : range(y_size)) { res(i, j) += (*this)(i, k) * B(k, j); } } }
			return res;
		}
		void transpose()
		{
			VectorBase<2, T> tmp(*this);
			std::swap(x_size, y_size);
			this->resize(x_size, y_size);
			for (auto i : range(x_size)) { for (auto j : range(y_size)) { (*this)(i, j) = tmp(j, i); } }
		}

		/* operator overload */
		// operator =
		template <typename U, is_vector(U)> VectorBase<DIM, T>& operator = (U rhs)
		{
			if (DIMFLAG != rhs.DIMFLAG) { throw std::invalid_argument("Dimension should be the same!"); }
			data_1d = rhs.data_1d;
			data_2d = rhs.data_2d;
			data_3d = rhs.data_3d;
			data_4d = rhs.data_4d;
			x_size = rhs.x_size;
			y_size = rhs.y_size;
			z_size = rhs.z_size;
			t_size = rhs.t_size;
			return *this;
		}
		template <typename U, not_vector(U)> VectorBase<DIM, T>& operator = (U content)
		{
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = content; } return *this;
		}
		// operator +=
		template <typename U, is_vector(U)> VectorBase<DIM, T>& operator +=(U rhs)
		{
			if (DIMFLAG != rhs.DIMFLAG) { throw std::invalid_argument("Dimension should be the same!"); }
			size_chk(rhs);
			for (auto iterA = this->begin_all(), iterB = rhs.begin_all(); iterA != this->end_all(); ++iterA, ++iterB) { *iterA += *iterB; }
			return *this;
		}
		template <typename U, not_vector(U)> VectorBase<DIM, T>& operator +=(U content)
		{
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter += content; } return *this;
		}
		// operator -=
		template <typename U, is_num_vector(U)> VectorBase<DIM, T>& operator -=(U rhs)
		{
			if (DIMFLAG != rhs.DIMFLAG) { throw std::invalid_argument("Dimension should be the same!"); }
			size_chk(rhs);
			for (auto iterA = this->begin_all(), iterB = rhs.begin_all(); iterA != this->end_all(); ++iterA, ++iterB) { *iterA -= *iterB; }
			return *this;
		}
		template <typename U, is_numeric(U)> VectorBase<DIM, T>& operator -=(U content)
		{
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter -= content; } return *this;
		}
		// operator *=
		template <typename U, is_num_vector(U)> VectorBase<DIM, T>& operator *=(U rhs)
		{
			if (DIMFLAG != rhs.DIMFLAG) { throw std::invalid_argument("Dimension should be the same!"); }
			size_chk(rhs);
			for (auto iterA = this->begin_all(), iterB = rhs.begin_all(); iterA != this->end_all(); ++iterA, ++iterB) { *iterA *= *iterB; }
			return *this;
		}
		template <typename U, is_numeric(U)> VectorBase<DIM, T>& operator *=(U content)
		{
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter *= content; } return *this;
		}
		// operator /=
		template <typename U, is_num_vector(U)> VectorBase<DIM, T>& operator /=(U rhs)
		{
			if (DIMFLAG != rhs.DIMFLAG) { throw std::invalid_argument("Dimension should be the same!"); }
			size_chk(rhs);
			for (auto iterA = this->begin_all(), iterB = rhs.begin_all(); iterA != this->end_all(); ++iterA, ++iterB) { *iterA /= *iterB; }
			return *this;
		}
		template <typename U, is_numeric(U)> VectorBase<DIM, T>& operator /=(U content)
		{
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter /= content; } return *this;
		}
		// operator ^=
		template <typename U, is_num_vector(U)> VectorBase<DIM, T>& operator ^=(U rhs)
		{
			if (DIMFLAG != rhs.DIMFLAG) { throw std::invalid_argument("Dimension should be the same!"); }
			size_chk(rhs);
			for (auto iterA = this->begin_all(), iterB = rhs.begin_all(); iterA != this->end_all(); ++iterA, ++iterB) { *iterA = std::pow(*iterA, *iterB); }
			return *this;
		}
		template <typename U, is_numeric(U)> VectorBase<DIM, T>& operator ^=(U content)
		{
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter = std::pow(*iter, content); } return *this;
		}
		// operator %=
		template <typename U, is_num_vector(U)> VectorBase<DIM, T>& operator %=(U rhs)  // carefully
		{
			if (DIMFLAG != rhs.DIMFLAG) { throw std::invalid_argument("Dimension should be the same!"); }
			size_chk(rhs);
			for (auto iterA = this->begin_all(), iterB = rhs.begin_all(); iterA != this->end_all(); ++iterA, ++iterB) { *iterA %= *iterB; }
			return *this;
		}
		template <typename U, is_numeric(U)> VectorBase<DIM, T>& operator %=(U content)
		{
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { *iter %= content; } return *this;
		} // carefully
	};

	template <typename T>
	class Vector<1, T, typename std::enable_if<is_numeric<T>::value>::type> : public VectorBase<1, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<1, T>{} {}
		explicit Vector(const int& x_axis) : VectorBase<1, T>{ x_axis } {}
		template <typename U> Vector(const int& x_axis, U content) : VectorBase<1, T>{ true,x_axis,content } {}
		/* initializer_list */
		Vector(std::initializer_list<T> v) : VectorBase<1, T>{} { VectorBase<1, T>::data_1d = { v }; }

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<1, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<1, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<1, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<1, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis)
		{
			VectorBase<1, T>::resize(x_axis);
		}
		template <typename U> void resize(const int& x_axis, U content) { VectorBase<1, T>::resize(true, x_axis, content); }

		/* iterator */
		typename std::vector<T>::iterator begin() noexcept { return VectorBase<1, T>::data_1d.begin(); }
		typename std::vector<T>::iterator   end() noexcept { return VectorBase<1, T>::data_1d.end(); }
		typename std::vector<T>::const_iterator begin() const noexcept { return VectorBase<1, T>::data_1d.begin(); }
		typename std::vector<T>::const_iterator   end() const noexcept { return VectorBase<1, T>::data_1d.end(); }
		typename VectorBase<1, T>::iterator begin_all() noexcept { return VectorBase<1, T>::begin_all(); }
		typename VectorBase<1, T>::iterator   end_all() noexcept { return VectorBase<1, T>::end_all(); }
		typename VectorBase<1, T>::const_iterator begin_all() const noexcept { return VectorBase<1, T>::begin_all(); }
		typename VectorBase<1, T>::const_iterator   end_all() const noexcept { return VectorBase<1, T>::end_all(); }

		/* vector function */
		int size() const { return VectorBase<1, T>::x_size; }
		std::vector<int> size_array() { return std::vector<int>{ VectorBase<1, T>::x_size }; }
		bool empty() const { return VectorBase<1, T>::x_size == 0 ? True : False; }
		std::vector<T>& data() { return VectorBase<1, T>::data_1d; }
		T& at(const int& idx) { return VectorBase<1, T>::data_1d[idx]; }
		void clear() { VectorBase<1, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<1, T>::swap(B); }
		template <typename U> void push_back(U content) { VectorBase<1, T>::data_1d.push_back(content); ++VectorBase<1, T>::x_size; }
		T sum()
		{
			T sum = 0;
			for (auto iter = this->begin_all(); iter != this->end_all(); ++iter) { sum += *iter; }
			return sum;
		}

		/* basic function */
		T& operator()(const int& x_index)
		{
			return VectorBase<1, T>::data_1d[x_index];
		}
		T& operator[](const int& x_index)
		{
			return VectorBase<1, T>::data_1d[x_index];
		}
		void rand(const double& Min, const double& Max) { VectorBase<1, T>::rand(Min, Max); }
		void rand(const double& Max) { rand(0, Max); }
		void rand() { rand(0, 1); }
		void randi(const int& Min, const int& Max) { VectorBase<1, T>::randi(Min, Max); }
		void randi(const int& Max) { randi(0, Max); }
		void randi() { randi(0, 1); }
		void randc(const double& MinR, const double& MaxR, const double& MinI, const double& MaxI) { VectorBase<1, T>::randc(MinR, MaxR, MinI, MaxI); }
		void randc(const double& MaxR, const double& MaxI) { randc(0, MaxR, 0, MaxI); }
		void randc() { randc(0, 1, 0, 1); }
		void zeros() { VectorBase<1, T>::zeros(); }
		void ones() { VectorBase<1, T>::ones(); }
		void exp() { VectorBase<1, T>::exp(); }
		void exp2() { VectorBase<1, T>::exp2(); }
		void expm1() { VectorBase<1, T>::expm1(); }
		void log() { VectorBase<1, T>::log(); }
		void log2() { VectorBase<1, T>::log2(); }
		void log10() { VectorBase<1, T>::log10(); }
		// dot [in-class] 1d
		template <typename U, is_num_vector(U)> T dot(U B)
		{
			if (B.DIMFLAG != 1) { throw std::invalid_argument("1D array x 1D array dot function!"); }
			remove_cvr(T) res = VectorBase<1, T>::dot1(B);
			return res;
		}
		template <typename U, is_num_vector(U)> Vector cross(U B)
		{
			if (B.DIMFLAG != 1) { throw std::invalid_argument("1D array x 1D array cross function!"); }
			Vector<1, T> res = VectorBase<1, T>::cross(B);
			return res;
		}

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<1, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<1, T>::operator+=(content);
			return *this;
		}
		template <typename U> Vector& operator -=(U content)
		{
			VectorBase<1, T>::operator-=(content);
			return *this;
		}
		template <typename U> Vector& operator *=(U content)
		{
			VectorBase<1, T>::operator*=(content);
			return *this;
		}
		template <typename U> Vector& operator /=(U content)
		{
			VectorBase<1, T>::operator/=(content);
			return *this;
		}
		template <typename U> Vector& operator ^=(U content)
		{
			VectorBase<1, T>::operator^=(content);
			return *this;
		}
		template <typename U> Vector& operator %=(U content)
		{
			VectorBase<1, T>::operator%=(content);
			return *this;
		}

	};

	template <typename T>
	class Vector<1, T, typename std::enable_if<!is_numeric<T>::value>::type> : public VectorBase<1, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<1, T>{} {}
		explicit Vector(const int& x_axis) : VectorBase<1, T>{ x_axis } {}
		template <typename U> Vector(const int& x_axis, U content) : VectorBase<1, T>{ true,x_axis,content } {}
		/* initializer_list */
		Vector(std::initializer_list<T> v) : VectorBase<1, T>{} { VectorBase<1, T>::data_1d = { v }; }

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<1, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<1, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<1, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<1, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis)
		{
			VectorBase<1, T>::resize(x_axis);
		}
		template <typename U> void resize(const int& x_axis, U content) { VectorBase<1, T>::resize(true, x_axis, content); }

		/* iterator */
		typename std::vector<T>::iterator begin() noexcept { return VectorBase<1, T>::data_1d.begin(); }
		typename std::vector<T>::iterator   end() noexcept { return VectorBase<1, T>::data_1d.end(); }
		typename std::vector<T>::const_iterator begin() const noexcept { return VectorBase<1, T>::data_1d.begin(); }
		typename std::vector<T>::const_iterator   end() const noexcept { return VectorBase<1, T>::data_1d.end(); }
		typename VectorBase<1, T>::iterator begin_all() noexcept { return VectorBase<1, T>::begin_all(); }
		typename VectorBase<1, T>::iterator   end_all() noexcept { return VectorBase<1, T>::end_all(); }
		typename VectorBase<1, T>::const_iterator begin_all() const noexcept { return VectorBase<1, T>::begin_all(); }
		typename VectorBase<1, T>::const_iterator   end_all() const noexcept { return VectorBase<1, T>::end_all(); }

		/* vector function */
		int size() const { return VectorBase<1, T>::x_size; }
		std::vector<int> size_array() { return std::vector<int>{ VectorBase<1, T>::x_size }; }
		bool empty() const { return VectorBase<1, T>::x_size == 0 ? True : False; }
		std::vector<T>& data() { return VectorBase<1, T>::data_1d; }
		T& at(const int& idx) { return VectorBase<1, T>::data_1d[idx]; }
		void clear() { VectorBase<1, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<1, T>::swap(B); }
		template <typename U> void push_back(U content) { VectorBase<1, T>::data_1d.push_back(content); ++VectorBase<1, T>::x_size; }

		/* basic function */
		T& operator()(const int& x_index)
		{
			return VectorBase<1, T>::data_1d[x_index];
		}
		T& operator[](const int& x_index)
		{
			return VectorBase<1, T>::data_1d[x_index];
		}

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<1, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<1, T>::operator+=(content);
			return *this;
		}

	};

	template <typename T>
	class Vector<2, T, typename std::enable_if<is_numeric<T>::value>::type> : public VectorBase<2, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<2, T>{} {}
		Vector(const int& x_axis, const int& y_axis) : VectorBase<2, T>{ x_axis,y_axis } {}
		template <typename U> Vector(const int& x_axis, const int& y_axis, U content) : VectorBase<2, T>{ true,x_axis,y_axis,content } {}

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<2, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<2, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<2, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<2, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis, const int& y_axis)
		{
			VectorBase<2, T>::resize(x_axis, y_axis);
		}
		template <typename U> void resize(const int& x_axis, const int& y_axis, U content)
		{
			VectorBase<2, T>::resize(true, x_axis, y_axis, content);
		}

		/* iterator */
		typename std::vector<std::vector<T>>::iterator       begin() noexcept { return VectorBase<2, T>::data_2d.begin(); }
		typename std::vector<std::vector<T>>::iterator         end() noexcept { return VectorBase<2, T>::data_2d.end(); }
		typename std::vector<std::vector<T>>::const_iterator begin() const noexcept { return VectorBase<2, T>::data_2d.begin(); }
		typename std::vector<std::vector<T>>::const_iterator   end() const noexcept { return VectorBase<2, T>::data_2d.end(); }
		typename VectorBase<2, T>::iterator       begin_all() noexcept { return VectorBase<2, T>::begin_all(); }
		typename VectorBase<2, T>::iterator         end_all() noexcept { return VectorBase<2, T>::end_all(); }
		typename VectorBase<2, T>::iterator       begin_trans() noexcept { return VectorBase<2, T>::begin_trans(); }
		typename VectorBase<2, T>::iterator         end_trans() noexcept { return VectorBase<2, T>::end_trans(); }
		typename VectorBase<2, T>::iterator       begin_x(const int& x) noexcept { return VectorBase<2, T>::begin_x(x); }
		typename VectorBase<2, T>::iterator         end_x(const int& x) noexcept { return VectorBase<2, T>::end_x(x); }
		typename VectorBase<2, T>::iterator       begin_y(const int& y) noexcept { return VectorBase<2, T>::begin_y(y); }
		typename VectorBase<2, T>::iterator         end_y(const int& y) noexcept { return VectorBase<2, T>::end_y(y); }
		typename VectorBase<2, T>::const_iterator begin_all() const noexcept { return VectorBase<2, T>::begin_all(); }
		typename VectorBase<2, T>::const_iterator   end_all() const noexcept { return VectorBase<2, T>::end_all(); }
		typename VectorBase<2, T>::const_iterator begin_trans() const noexcept { return VectorBase<2, T>::begin_trans(); }
		typename VectorBase<2, T>::const_iterator   end_trans() const noexcept { return VectorBase<2, T>::end_trans(); }
		typename VectorBase<2, T>::const_iterator begin_x(const int& x) const noexcept { return VectorBase<2, T>::begin_x(x); }
		typename VectorBase<2, T>::const_iterator   end_x(const int& x) const noexcept { return VectorBase<2, T>::end_x(x); }
		typename VectorBase<2, T>::const_iterator begin_y(const int& y) const noexcept { return VectorBase<2, T>::begin_y(y); }
		typename VectorBase<2, T>::const_iterator   end_y(const int& y) const noexcept { return VectorBase<2, T>::end_y(y); }

		/* vector function */
		int size() const { return VectorBase<2, T>::x_size * VectorBase<2, T>::y_size; }
		std::vector<int> size_array() { return std::vector<int>{ VectorBase<2, T>::x_size, VectorBase<2, T>::y_size }; }
		bool empty() const { return VectorBase<2, T>::x_size * VectorBase<2, T>::y_size == 0 ? True : False; }
		std::vector<std::vector<T>>& data() { return VectorBase<2, T>::data_2d; }
		std::vector<T>& at(const int& idx) { return VectorBase<2, T>::data_2d[idx]; }
		void clear() { VectorBase<2, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<2, T>::swap(B); }

		/* basic function */
		T& operator()(const int& x_index, const int& y_index) { return VectorBase<2, T>::data_2d[x_index][y_index]; }
		std::vector<T>& operator()(const int& x_index) { return VectorBase<2, T>::data_2d[x_index]; }
		std::vector<T>& operator[](const int& x_index) { return VectorBase<2, T>::data_2d[x_index]; }
		void rand(const double& Min, const double& Max) { VectorBase<2, T>::rand(Min, Max); }
		void rand(const double& Max) { rand(0, Max); }
		void rand() { rand(0, 1); }
		void randi(const int& Min, const int& Max) { VectorBase<2, T>::randi(Min, Max); }
		void randi(const int& Max) { randi(0, Max); }
		void randi() { randi(0, 1); }
		void randc(const double& MinR, const double& MaxR, const double& MinI, const double& MaxI)
		{
			VectorBase<2, T>::randc(MinR, MaxR, MinI, MaxI);
		}
		void randc(const double& MaxR, const double& MaxI) { randc(0, MaxR, 0, MaxI); }
		void randc() { randc(0, 1, 0, 1); }
		void zeros() { VectorBase<2, T>::zeros(); }
		void ones() { VectorBase<2, T>::ones(); }
		void exp() { VectorBase<2, T>::exp(); }
		void exp2() { VectorBase<2, T>::exp2(); }
		void expm1() { VectorBase<2, T>::expm1(); }
		void log() { VectorBase<2, T>::log(); }
		void log2() { VectorBase<2, T>::log2(); }
		void log10() { VectorBase<2, T>::log10(); }
		// dot [in-class] 2d matrix dot
		template <typename U, is_num_vector(U)> VectorBase<2, T> dot(U B)
		{
			if (B.DIMFLAG != 2) { throw std::invalid_argument("2D array x 2D array dot function!"); }
			VectorBase<2, T> res = VectorBase<2, T>::dot2(B);
			return res;
		}
		void transpose() { VectorBase<2, T>::transpose(); }

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<2, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<2, T>::operator+=(content);
			return *this;
		}
		template <typename U> Vector& operator -=(U content)
		{
			VectorBase<2, T>::operator-=(content);
			return *this;
		}
		template <typename U> Vector& operator *=(U content)
		{
			VectorBase<2, T>::operator*=(content);
			return *this;
		}
		template <typename U> Vector& operator /=(U content)
		{
			VectorBase<2, T>::operator/=(content);
			return *this;
		}
		template <typename U> Vector& operator ^=(U content)
		{
			VectorBase<2, T>::operator^=(content);
			return *this;
		}
		template <typename U> Vector& operator %=(U content)
		{
			VectorBase<2, T>::operator%=(content);
			return *this;
		}
	};

	template <typename T>
	class Vector<2, T, typename std::enable_if<!is_numeric<T>::value>::type> : public VectorBase<2, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<2, T>{} {}
		Vector(const int& x_axis, const int& y_axis) : VectorBase<2, T>{ x_axis,y_axis } {}
		template <typename U> Vector(const int& x_axis, const int& y_axis, U content) : VectorBase<2, T>{ true,x_axis,y_axis,content } {}

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<2, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<2, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<2, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<2, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis, const int& y_axis)
		{
			VectorBase<2, T>::resize(x_axis, y_axis);
		}
		template <typename U> void resize(const int& x_axis, const int& y_axis, U content)
		{
			VectorBase<2, T>::resize(true, x_axis, y_axis, content);
		}

		/* iterator */
		typename std::vector<std::vector<T>>::iterator       begin() noexcept { return VectorBase<2, T>::data_2d.begin(); }
		typename std::vector<std::vector<T>>::iterator         end() noexcept { return VectorBase<2, T>::data_2d.end(); }
		typename std::vector<std::vector<T>>::const_iterator begin() const noexcept { return VectorBase<2, T>::data_2d.begin(); }
		typename std::vector<std::vector<T>>::const_iterator   end() const noexcept { return VectorBase<2, T>::data_2d.end(); }
		typename VectorBase<2, T>::iterator       begin_all() noexcept { return VectorBase<2, T>::begin_all(); }
		typename VectorBase<2, T>::iterator         end_all() noexcept { return VectorBase<2, T>::end_all(); }
		typename VectorBase<2, T>::iterator       begin_trans() noexcept { return VectorBase<2, T>::begin_trans(); }
		typename VectorBase<2, T>::iterator         end_trans() noexcept { return VectorBase<2, T>::end_trans(); }
		typename VectorBase<2, T>::iterator       begin_x(const int& x) noexcept { return VectorBase<2, T>::begin_x(x); }
		typename VectorBase<2, T>::iterator         end_x(const int& x) noexcept { return VectorBase<2, T>::end_x(x); }
		typename VectorBase<2, T>::iterator       begin_y(const int& y) noexcept { return VectorBase<2, T>::begin_y(y); }
		typename VectorBase<2, T>::iterator         end_y(const int& y) noexcept { return VectorBase<2, T>::end_y(y); }
		typename VectorBase<2, T>::const_iterator begin_all() const noexcept { return VectorBase<2, T>::begin_all(); }
		typename VectorBase<2, T>::const_iterator   end_all() const noexcept { return VectorBase<2, T>::end_all(); }
		typename VectorBase<2, T>::const_iterator begin_trans() const noexcept { return VectorBase<2, T>::begin_trans(); }
		typename VectorBase<2, T>::const_iterator   end_trans() const noexcept { return VectorBase<2, T>::end_trans(); }
		typename VectorBase<2, T>::const_iterator begin_x(const int& x) const noexcept { return VectorBase<2, T>::begin_x(x); }
		typename VectorBase<2, T>::const_iterator   end_x(const int& x) const noexcept { return VectorBase<2, T>::end_x(x); }
		typename VectorBase<2, T>::const_iterator begin_y(const int& y) const noexcept { return VectorBase<2, T>::begin_y(y); }
		typename VectorBase<2, T>::const_iterator   end_y(const int& y) const noexcept { return VectorBase<2, T>::end_y(y); }

		/* vector function */
		int size() const { return VectorBase<2, T>::x_size * VectorBase<2, T>::y_size; }
		std::vector<int> size_array() { return std::vector<int>{ VectorBase<2, T>::x_size, VectorBase<2, T>::y_size }; }
		bool empty() const { return VectorBase<2, T>::x_size * VectorBase<2, T>::y_size == 0 ? True : False; }
		std::vector<std::vector<T>>& data() { return VectorBase<2, T>::data_2d; }
		std::vector<T>& at(const int& idx) { return VectorBase<2, T>::data_2d[idx]; }
		void clear() { VectorBase<2, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<2, T>::swap(B); }

		/* basic function */
		T& operator()(const int& x_index, const int& y_index) { return VectorBase<2, T>::data_2d[x_index][y_index]; }
		std::vector<T>& operator()(const int& x_index) { return VectorBase<2, T>::data_2d[x_index]; }
		std::vector<T>& operator[](const int& x_index) { return VectorBase<2, T>::data_2d[x_index]; }
		void transpose() { VectorBase<2, T>::transpose(); }

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<2, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<2, T>::operator+=(content);
			return *this;
		}
	};

	template <typename T>
	class Vector<3, T, typename std::enable_if<is_numeric<T>::value>::type> : public VectorBase<3, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<3, T>{} {}
		Vector(const int& x_axis, const int& y_axis, const int& z_axis) : VectorBase<3, T>{ x_axis,y_axis,z_axis } {}
		template <typename U> Vector(const int& x_axis, const int& y_axis, const int& z_axis, U content) :
			VectorBase<3, T>{ true,x_axis,y_axis,z_axis,content } {}

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<3, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<3, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<3, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<3, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis, const int& y_axis, const int& z_axis)
		{
			VectorBase<3, T>::resize(x_axis, y_axis, z_axis);
		}
		template <typename U> void resize(const int& x_axis, const int& y_axis, const int& z_axis, U content)
		{
			VectorBase<3, T>::resize(true, x_axis, y_axis, z_axis, content);
		}

		/* iterator */
		typename std::vector<std::vector<std::vector<T>>>::iterator       begin() noexcept { return VectorBase<3, T>::data_3d.begin(); }
		typename std::vector<std::vector<std::vector<T>>>::iterator         end() noexcept { return VectorBase<3, T>::data_3d.end(); }
		typename std::vector<std::vector<std::vector<T>>>::const_iterator begin() const noexcept { return VectorBase<3, T>::data_3d.begin(); }
		typename std::vector<std::vector<std::vector<T>>>::const_iterator   end() const noexcept { return VectorBase<3, T>::data_3d.end(); }
		typename VectorBase<3, T>::iterator begin_all() noexcept { return VectorBase<3, T>::begin_all(); }
		typename VectorBase<3, T>::iterator   end_all() noexcept { return VectorBase<3, T>::end_all(); }
		typename VectorBase<3, T>::iterator begin_x(const int& x) noexcept { return VectorBase<3, T>::begin_x(x); }
		typename VectorBase<3, T>::iterator   end_x(const int& x) noexcept { return VectorBase<3, T>::end_x(x); }
		typename VectorBase<3, T>::iterator begin_y(const int& y) noexcept { return VectorBase<3, T>::begin_y(y); }
		typename VectorBase<3, T>::iterator   end_y(const int& y) noexcept { return VectorBase<3, T>::end_y(y); }
		typename VectorBase<3, T>::iterator begin_z(const int& z) noexcept { return VectorBase<3, T>::begin_z(z); }
		typename VectorBase<3, T>::iterator   end_z(const int& z) noexcept { return VectorBase<3, T>::end_z(z); }
		typename VectorBase<3, T>::iterator begin_xy(const int& x, const int& y) noexcept { return VectorBase<3, T>::begin_xy(x, y); }
		typename VectorBase<3, T>::iterator   end_xy(const int& x, const int& y) noexcept { return VectorBase<3, T>::end_xy(x, y); }
		typename VectorBase<3, T>::iterator begin_xz(const int& x, const int& z) noexcept { return VectorBase<3, T>::begin_xz(x, z); }
		typename VectorBase<3, T>::iterator   end_xz(const int& x, const int& z) noexcept { return VectorBase<3, T>::end_xz(x, z); }
		typename VectorBase<3, T>::iterator begin_yz(const int& y, const int& z) noexcept { return VectorBase<3, T>::begin_yz(y, z); }
		typename VectorBase<3, T>::iterator   end_yz(const int& y, const int& z) noexcept { return VectorBase<3, T>::end_yz(y, z); }
		typename VectorBase<3, T>::const_iterator begin_all() const noexcept { return VectorBase<3, T>::begin_all(); }
		typename VectorBase<3, T>::const_iterator   end_all() const noexcept { return VectorBase<3, T>::end_all(); }
		typename VectorBase<3, T>::const_iterator begin_x(const int& x) const noexcept { return VectorBase<3, T>::begin_x(x); }
		typename VectorBase<3, T>::const_iterator   end_x(const int& x) const noexcept { return VectorBase<3, T>::end_x(x); }
		typename VectorBase<3, T>::const_iterator begin_y(const int& y) const noexcept { return VectorBase<3, T>::begin_y(y); }
		typename VectorBase<3, T>::const_iterator   end_y(const int& y) const noexcept { return VectorBase<3, T>::end_y(y); }
		typename VectorBase<3, T>::const_iterator begin_z(const int& z) const noexcept { return VectorBase<3, T>::begin_z(z); }
		typename VectorBase<3, T>::const_iterator   end_z(const int& z) const noexcept { return VectorBase<3, T>::end_z(z); }
		typename VectorBase<3, T>::const_iterator begin_xy(const int& x, const int& y) const noexcept { return VectorBase<3, T>::begin_xy(x, y); }
		typename VectorBase<3, T>::const_iterator   end_xy(const int& x, const int& y) const noexcept { return VectorBase<3, T>::end_xy(x, y); }
		typename VectorBase<3, T>::const_iterator begin_xz(const int& x, const int& z) const noexcept { return VectorBase<3, T>::begin_xz(x, z); }
		typename VectorBase<3, T>::const_iterator   end_xz(const int& x, const int& z) const noexcept { return VectorBase<3, T>::end_xz(x, z); }
		typename VectorBase<3, T>::const_iterator begin_yz(const int& y, const int& z) const noexcept { return VectorBase<3, T>::begin_yz(y, z); }
		typename VectorBase<3, T>::const_iterator   end_yz(const int& y, const int& z) const noexcept { return VectorBase<3, T>::end_yz(y, z); }

		/* vector function */
		int size() const { return VectorBase<3, T>::x_size * VectorBase<3, T>::y_size * VectorBase<3, T>::z_size; }
		std::vector<int> size_array() { return std::vector<int>{ VectorBase<3, T>::x_size, VectorBase<3, T>::y_size, VectorBase<3, T>::z_size }; }
		bool empty() const { return VectorBase<3, T>::x_size * VectorBase<3, T>::y_size * VectorBase<3, T>::z_size == 0 ? True : False; }
		std::vector<std::vector<std::vector<T>>>& data() { return VectorBase<3, T>::data_3d; }
		std::vector<std::vector<T>>& at(const int& idx) { return VectorBase<3, T>::data_3d[idx]; }
		void clear() { VectorBase<3, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<3, T>::swap(B); }

		/* basic function */
		T& operator()(const int& x_index, const int& y_index, const int& z_index) { return VectorBase<3, T>::data_3d[x_index][y_index][z_index]; }
		std::vector<T>& operator()(const int& x_index, const int& y_index) { return VectorBase<3, T>::data_3d[x_index][y_index]; }
		std::vector<std::vector<T>>& operator()(const int& x_index) { return VectorBase<3, T>::data_3d[x_index]; }
		std::vector<std::vector<T>>& operator[](const int& x_index) { return VectorBase<3, T>::data_3d[x_index]; }
		void rand(const double& Min, const double& Max) { VectorBase<3, T>::rand(Min, Max); }
		void rand(const double& Max) { rand(0, Max); }
		void rand() { rand(0, 1); }
		void randi(const int& Min, const int& Max) { VectorBase<3, T>::randi(Min, Max); }
		void randi(const int& Max) { randi(0, Max); }
		void randi() { randi(0, 1); }
		void randc(const double& MinR, const double& MaxR, const double& MinI, const double& MaxI)
		{
			VectorBase<3, T>::randc(MinR, MaxR, MinI, MaxI);
		}
		void randc(const double& MaxR, const double& MaxI) { randc(0, MaxR, 0, MaxI); }
		void randc() { randc(0, 1, 0, 1); }
		void zeros() { VectorBase<3, T>::zeros(); }
		void ones() { VectorBase<3, T>::ones(); }
		void exp() { VectorBase<3, T>::exp(); }
		void exp2() { VectorBase<3, T>::exp2(); }
		void expm1() { VectorBase<3, T>::expm1(); }
		void log() { VectorBase<3, T>::log(); }
		void log2() { VectorBase<3, T>::log2(); }
		void log10() { VectorBase<3, T>::log10(); }

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<3, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<3, T>::operator+=(content);
			return *this;
		}
		template <typename U> Vector& operator -=(U content)
		{
			VectorBase<3, T>::operator-=(content);
			return *this;
		}
		template <typename U> Vector& operator *=(U content)
		{
			VectorBase<3, T>::operator*=(content);
			return *this;
		}
		template <typename U> Vector& operator /=(U content)
		{
			VectorBase<3, T>::operator/=(content);
			return *this;
		}
		template <typename U> Vector& operator ^=(U content)
		{
			VectorBase<3, T>::operator^=(content);
			return *this;
		}
		template <typename U> Vector& operator %=(U content)
		{
			VectorBase<3, T>::operator%=(content);
			return *this;
		}
	};

	template <typename T>
	class Vector<3, T, typename std::enable_if<!is_numeric<T>::value>::type> : public VectorBase<3, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<3, T>{} {}
		Vector(const int& x_axis, const int& y_axis, const int& z_axis) : VectorBase<3, T>{ x_axis,y_axis,z_axis } {}
		template <typename U> Vector(const int& x_axis, const int& y_axis, const int& z_axis, U content) :
			VectorBase<3, T>{ true,x_axis,y_axis,z_axis,content } {}

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<3, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<3, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<3, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<3, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis, const int& y_axis, const int& z_axis)
		{
			VectorBase<3, T>::resize(x_axis, y_axis, z_axis);
		}
		template <typename U> void resize(const int& x_axis, const int& y_axis, const int& z_axis, U content)
		{
			VectorBase<3, T>::resize(true, x_axis, y_axis, z_axis, content);
		}

		/* iterator */
		typename std::vector<std::vector<std::vector<T>>>::iterator       begin() noexcept { return VectorBase<3, T>::data_3d.begin(); }
		typename std::vector<std::vector<std::vector<T>>>::iterator         end() noexcept { return VectorBase<3, T>::data_3d.end(); }
		typename std::vector<std::vector<std::vector<T>>>::const_iterator begin() const noexcept { return VectorBase<3, T>::data_3d.begin(); }
		typename std::vector<std::vector<std::vector<T>>>::const_iterator   end() const noexcept { return VectorBase<3, T>::data_3d.end(); }
		typename VectorBase<3, T>::iterator begin_all() noexcept { return VectorBase<3, T>::begin_all(); }
		typename VectorBase<3, T>::iterator   end_all() noexcept { return VectorBase<3, T>::end_all(); }
		typename VectorBase<3, T>::iterator begin_x(const int& x) noexcept { return VectorBase<3, T>::begin_x(x); }
		typename VectorBase<3, T>::iterator   end_x(const int& x) noexcept { return VectorBase<3, T>::end_x(x); }
		typename VectorBase<3, T>::iterator begin_y(const int& y) noexcept { return VectorBase<3, T>::begin_y(y); }
		typename VectorBase<3, T>::iterator   end_y(const int& y) noexcept { return VectorBase<3, T>::end_y(y); }
		typename VectorBase<3, T>::iterator begin_z(const int& z) noexcept { return VectorBase<3, T>::begin_z(z); }
		typename VectorBase<3, T>::iterator   end_z(const int& z) noexcept { return VectorBase<3, T>::end_z(z); }
		typename VectorBase<3, T>::iterator begin_xy(const int& x, const int& y) noexcept { return VectorBase<3, T>::begin_xy(x, y); }
		typename VectorBase<3, T>::iterator   end_xy(const int& x, const int& y) noexcept { return VectorBase<3, T>::end_xy(x, y); }
		typename VectorBase<3, T>::iterator begin_xz(const int& x, const int& z) noexcept { return VectorBase<3, T>::begin_xz(x, z); }
		typename VectorBase<3, T>::iterator   end_xz(const int& x, const int& z) noexcept { return VectorBase<3, T>::end_xz(x, z); }
		typename VectorBase<3, T>::iterator begin_yz(const int& y, const int& z) noexcept { return VectorBase<3, T>::begin_yz(y, z); }
		typename VectorBase<3, T>::iterator   end_yz(const int& y, const int& z) noexcept { return VectorBase<3, T>::end_yz(y, z); }
		typename VectorBase<3, T>::const_iterator begin_all() const noexcept { return VectorBase<3, T>::begin_all(); }
		typename VectorBase<3, T>::const_iterator   end_all() const noexcept { return VectorBase<3, T>::end_all(); }
		typename VectorBase<3, T>::const_iterator begin_x(const int& x) const noexcept { return VectorBase<3, T>::begin_x(x); }
		typename VectorBase<3, T>::const_iterator   end_x(const int& x) const noexcept { return VectorBase<3, T>::end_x(x); }
		typename VectorBase<3, T>::const_iterator begin_y(const int& y) const noexcept { return VectorBase<3, T>::begin_y(y); }
		typename VectorBase<3, T>::const_iterator   end_y(const int& y) const noexcept { return VectorBase<3, T>::end_y(y); }
		typename VectorBase<3, T>::const_iterator begin_z(const int& z) const noexcept { return VectorBase<3, T>::begin_z(z); }
		typename VectorBase<3, T>::const_iterator   end_z(const int& z) const noexcept { return VectorBase<3, T>::end_z(z); }
		typename VectorBase<3, T>::const_iterator begin_xy(const int& x, const int& y) const noexcept { return VectorBase<3, T>::begin_xy(x, y); }
		typename VectorBase<3, T>::const_iterator   end_xy(const int& x, const int& y) const noexcept { return VectorBase<3, T>::end_xy(x, y); }
		typename VectorBase<3, T>::const_iterator begin_xz(const int& x, const int& z) const noexcept { return VectorBase<3, T>::begin_xz(x, z); }
		typename VectorBase<3, T>::const_iterator   end_xz(const int& x, const int& z) const noexcept { return VectorBase<3, T>::end_xz(x, z); }
		typename VectorBase<3, T>::const_iterator begin_yz(const int& y, const int& z) const noexcept { return VectorBase<3, T>::begin_yz(y, z); }
		typename VectorBase<3, T>::const_iterator   end_yz(const int& y, const int& z) const noexcept { return VectorBase<3, T>::end_yz(y, z); }

		/* vector function */
		int size() const { return VectorBase<3, T>::x_size * VectorBase<3, T>::y_size * VectorBase<3, T>::z_size; }
		std::vector<int> size_array() { return std::vector<int>{ VectorBase<3, T>::x_size, VectorBase<3, T>::y_size, VectorBase<3, T>::z_size }; }
		bool empty() const { return VectorBase<3, T>::x_size * VectorBase<3, T>::y_size * VectorBase<3, T>::z_size == 0 ? True : False; }
		std::vector<std::vector<std::vector<T>>>& data() { return VectorBase<3, T>::data_3d; }
		std::vector<std::vector<T>>& at(const int& idx) { return VectorBase<3, T>::data_3d[idx]; }
		void clear() { VectorBase<3, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<3, T>::swap(B); }

		/* basic function */
		T& operator()(const int& x_index, const int& y_index, const int& z_index) { return VectorBase<3, T>::data_3d[x_index][y_index][z_index]; }
		std::vector<T>& operator()(const int& x_index, const int& y_index) { return VectorBase<3, T>::data_3d[x_index][y_index]; }
		std::vector<std::vector<T>>& operator()(const int& x_index) { return VectorBase<3, T>::data_3d[x_index]; }
		std::vector<std::vector<T>>& operator[](const int& x_index) { return VectorBase<3, T>::data_3d[x_index]; }

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<3, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<3, T>::operator+=(content);
			return *this;
		}
	};

	template <typename T>
	class Vector<4, T, typename std::enable_if<is_numeric<T>::value>::type> : public VectorBase<4, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<4, T>{} {}
		Vector(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis) :
			VectorBase<4, T>{ x_axis,y_axis,z_axis,t_axis } {}
		template <typename U> Vector(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, U content) :
			VectorBase<4, T>{ true,x_axis,y_axis,z_axis,t_axis,content } {}

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<4, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<4, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<4, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<4, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis)
		{
			VectorBase<4, T>::resize(x_axis, y_axis, z_axis, t_axis);
		}
		template <typename U> void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, U content)
		{
			VectorBase<4, T>::resize(true, x_axis, y_axis, z_axis, t_axis, content);
		}

		/* iterator */
		typename std::vector<std::vector<std::vector<std::vector<T>>>>::iterator       begin() noexcept { return VectorBase<4, T>::data_4d.begin(); }
		typename std::vector<std::vector<std::vector<std::vector<T>>>>::iterator         end() noexcept { return VectorBase<4, T>::data_4d.end(); }
		typename std::vector<std::vector<std::vector<std::vector<T>>>>::const_iterator begin() const noexcept { return VectorBase<4, T>::data_4d.begin(); }
		typename std::vector<std::vector<std::vector<std::vector<T>>>>::const_iterator   end() const noexcept { return VectorBase<4, T>::data_4d.end(); }
		typename VectorBase<4, T>::iterator begin_all() noexcept { return VectorBase<4, T>::begin_all(); }
		typename VectorBase<4, T>::iterator   end_all() noexcept { return VectorBase<4, T>::end_all(); }
		typename VectorBase<4, T>::iterator begin_x(const int& x) noexcept { return VectorBase<4, T>::begin_x(x); }
		typename VectorBase<4, T>::iterator   end_x(const int& x) noexcept { return VectorBase<4, T>::end_x(x); }
		typename VectorBase<4, T>::iterator begin_y(const int& y) noexcept { return VectorBase<4, T>::begin_y(y); }
		typename VectorBase<4, T>::iterator   end_y(const int& y) noexcept { return VectorBase<4, T>::end_y(y); }
		typename VectorBase<4, T>::iterator begin_z(const int& z) noexcept { return VectorBase<4, T>::begin_z(z); }
		typename VectorBase<4, T>::iterator   end_z(const int& z) noexcept { return VectorBase<4, T>::end_z(z); }
		typename VectorBase<4, T>::iterator begin_t(const int& t) noexcept { return VectorBase<4, T>::begin_t(t); }
		typename VectorBase<4, T>::iterator   end_t(const int& t) noexcept { return VectorBase<4, T>::end_t(t); }
		typename VectorBase<4, T>::iterator begin_xy(const int& x, const int& y) noexcept { return VectorBase<4, T>::begin_xy(x, y); }
		typename VectorBase<4, T>::iterator   end_xy(const int& x, const int& y) noexcept { return VectorBase<4, T>::end_xy(x, y); }
		typename VectorBase<4, T>::iterator begin_xz(const int& x, const int& z) noexcept { return VectorBase<4, T>::begin_xy(x, z); }
		typename VectorBase<4, T>::iterator   end_xz(const int& x, const int& z) noexcept { return VectorBase<4, T>::end_xy(x, z); }
		typename VectorBase<4, T>::iterator begin_yz(const int& y, const int& z) noexcept { return VectorBase<4, T>::begin_yz(y, z); }
		typename VectorBase<4, T>::iterator   end_yz(const int& y, const int& z) noexcept { return VectorBase<4, T>::end_yz(y, z); }
		typename VectorBase<4, T>::iterator begin_xt(const int& x, const int& t) noexcept { return VectorBase<4, T>::begin_xt(x, t); }
		typename VectorBase<4, T>::iterator   end_xt(const int& x, const int& t) noexcept { return VectorBase<4, T>::end_xt(x, t); }
		typename VectorBase<4, T>::iterator begin_yt(const int& y, const int& t) noexcept { return VectorBase<4, T>::begin_yt(y, t); }
		typename VectorBase<4, T>::iterator   end_yt(const int& y, const int& t) noexcept { return VectorBase<4, T>::end_yt(y, t); }
		typename VectorBase<4, T>::iterator begin_zt(const int& z, const int& t) noexcept { return VectorBase<4, T>::begin_zt(z, t); }
		typename VectorBase<4, T>::iterator   end_zt(const int& z, const int& t) noexcept { return VectorBase<4, T>::end_zt(z, t); }
		typename VectorBase<4, T>::iterator begin_xyz(const int& x, const int& y, const int& z) noexcept { return VectorBase<4, T>::begin_xyz(x, y, z); }
		typename VectorBase<4, T>::iterator   end_xyz(const int& x, const int& y, const int& z) noexcept { return VectorBase<4, T>::end_xyz(x, y, z); }
		typename VectorBase<4, T>::iterator begin_xyt(const int& x, const int& y, const int& t) noexcept { return VectorBase<4, T>::begin_xyt(x, y, t); }
		typename VectorBase<4, T>::iterator   end_xyt(const int& x, const int& y, const int& t) noexcept { return VectorBase<4, T>::end_xyt(x, y, t); }
		typename VectorBase<4, T>::iterator begin_xzt(const int& x, const int& z, const int& t) noexcept { return VectorBase<4, T>::begin_xzt(x, z, t); }
		typename VectorBase<4, T>::iterator   end_xzt(const int& x, const int& z, const int& t) noexcept { return VectorBase<4, T>::end_xzt(x, z, t); }
		typename VectorBase<4, T>::iterator begin_yzt(const int& y, const int& z, const int& t) noexcept { return VectorBase<4, T>::begin_yzt(y, z, t); }
		typename VectorBase<4, T>::iterator   end_yzt(const int& y, const int& z, const int& t) noexcept { return VectorBase<4, T>::end_yzt(y, z, t); }
		typename VectorBase<4, T>::const_iterator begin_all() const noexcept { return VectorBase<4, T>::begin_all(); }
		typename VectorBase<4, T>::const_iterator   end_all() const noexcept { return VectorBase<4, T>::end_all(); }
		typename VectorBase<4, T>::const_iterator begin_x(const int& x) const noexcept { return VectorBase<4, T>::begin_x(x); }
		typename VectorBase<4, T>::const_iterator   end_x(const int& x) const noexcept { return VectorBase<4, T>::end_x(x); }
		typename VectorBase<4, T>::const_iterator begin_y(const int& y) const noexcept { return VectorBase<4, T>::begin_y(y); }
		typename VectorBase<4, T>::const_iterator   end_y(const int& y) const noexcept { return VectorBase<4, T>::end_y(y); }
		typename VectorBase<4, T>::const_iterator begin_z(const int& z) const noexcept { return VectorBase<4, T>::begin_z(z); }
		typename VectorBase<4, T>::const_iterator   end_z(const int& z) const noexcept { return VectorBase<4, T>::end_z(z); }
		typename VectorBase<4, T>::const_iterator begin_t(const int& t) const noexcept { return VectorBase<4, T>::begin_t(t); }
		typename VectorBase<4, T>::const_iterator   end_t(const int& t) const noexcept { return VectorBase<4, T>::end_t(t); }
		typename VectorBase<4, T>::const_iterator begin_xy(const int& x, const int& y) const noexcept { return VectorBase<4, T>::begin_xy(x, y); }
		typename VectorBase<4, T>::const_iterator   end_xy(const int& x, const int& y) const noexcept { return VectorBase<4, T>::end_xy(x, y); }
		typename VectorBase<4, T>::const_iterator begin_xz(const int& x, const int& z) const noexcept { return VectorBase<4, T>::begin_xz(x, z); }
		typename VectorBase<4, T>::const_iterator   end_xz(const int& x, const int& z) const noexcept { return VectorBase<4, T>::end_xz(x, z); }
		typename VectorBase<4, T>::const_iterator begin_yz(const int& y, const int& z) const noexcept { return VectorBase<4, T>::begin_yz(y, z); }
		typename VectorBase<4, T>::const_iterator   end_yz(const int& y, const int& z) const noexcept { return VectorBase<4, T>::end_yz(y, z); }
		typename VectorBase<4, T>::const_iterator begin_xt(const int& x, const int& t) const noexcept { return VectorBase<4, T>::begin_xt(x, t); }
		typename VectorBase<4, T>::const_iterator   end_xt(const int& x, const int& t) const noexcept { return VectorBase<4, T>::end_xt(x, t); }
		typename VectorBase<4, T>::const_iterator begin_yt(const int& y, const int& t) const noexcept { return VectorBase<4, T>::begin_yt(y, t); }
		typename VectorBase<4, T>::const_iterator   end_yt(const int& y, const int& t) const noexcept { return VectorBase<4, T>::end_yt(y, t); }
		typename VectorBase<4, T>::const_iterator begin_zt(const int& z, const int& t) const noexcept { return VectorBase<4, T>::begin_zt(z, t); }
		typename VectorBase<4, T>::const_iterator   end_zt(const int& z, const int& t) const noexcept { return VectorBase<4, T>::end_zt(z, t); }
		typename VectorBase<4, T>::const_iterator begin_xyz(const int& x, const int& y, const int& z) const noexcept { return VectorBase<4, T>::begin_xyz(x, y, z); }
		typename VectorBase<4, T>::const_iterator   end_xyz(const int& x, const int& y, const int& z) const noexcept { return VectorBase<4, T>::end_xyz(x, y, z); }
		typename VectorBase<4, T>::const_iterator begin_xyt(const int& x, const int& y, const int& t) const noexcept { return VectorBase<4, T>::begin_xyt(x, y, t); }
		typename VectorBase<4, T>::const_iterator   end_xyt(const int& x, const int& y, const int& t) const noexcept { return VectorBase<4, T>::end_xyt(x, y, t); }
		typename VectorBase<4, T>::const_iterator begin_xzt(const int& x, const int& z, const int& t) const noexcept { return VectorBase<4, T>::begin_xzt(x, z, t); }
		typename VectorBase<4, T>::const_iterator   end_xzt(const int& x, const int& z, const int& t) const noexcept { return VectorBase<4, T>::end_xzt(x, z, t); }
		typename VectorBase<4, T>::const_iterator begin_yzt(const int& y, const int& z, const int& t) const noexcept { return VectorBase<4, T>::begin_yzt(y, z, t); }
		typename VectorBase<4, T>::const_iterator   end_yzt(const int& y, const int& z, const int& t) const noexcept { return VectorBase<4, T>::end_yzt(y, z, t); }

		/* vector function */
		int size() const { return VectorBase<4, T>::x_size * VectorBase<4, T>::y_size * VectorBase<4, T>::z_size * VectorBase<4, T>::t_size; }
		std::vector<int> size_array() { return std::vector<int>{ VectorBase<4, T>::x_size, VectorBase<4, T>::y_size, VectorBase<4, T>::z_size, VectorBase<4, T>::t_size }; }
		bool empty() const { return VectorBase<4, T>::x_size * VectorBase<4, T>::y_size * VectorBase<4, T>::z_size * VectorBase<4, T>::t_size == 0 ? True : False; }
		std::vector<std::vector<std::vector<std::vector<T>>>>& data() { return VectorBase<4, T>::data_4d; }
		std::vector<std::vector<std::vector<T>>>& at(const int& idx) { return VectorBase<4, T>::data_4d[idx]; }
		void clear() { VectorBase<4, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<4, T>::swap(B); }

		/* basic function */
		T& operator()(const int& x_index, const int& y_index, const int& z_index, const int& t_index)
		{
			return VectorBase<4, T>::data_4d[x_index][y_index][z_index][t_index];
		}
		std::vector<T>& operator()(const int& x_index, const int& y_index, const int& z_index) { return VectorBase<4, T>::data_4d[x_index][y_index][z_index]; }
		std::vector<std::vector<T>>& operator()(const int& x_index, const int& y_index) { return VectorBase<4, T>::data_4d[x_index][y_index]; }
		std::vector<std::vector<std::vector<T>>>& operator()(const int& x_index) { return VectorBase<4, T>::data_4d[x_index]; }
		std::vector<std::vector<std::vector<T>>>& operator[](const int& x_index) { return VectorBase<4, T>::data_4d[x_index]; }
		void rand(const double& Min, const double& Max) { VectorBase<4, T>::rand(Min, Max); }
		void rand(const double& Max) { rand(0, Max); }
		void rand() { rand(0, 1); }
		void randi(const int& Min, const int& Max) { VectorBase<4, T>::randi(Min, Max); }
		void randi(const int& Max) { randi(0, Max); }
		void randi() { randi(0, 1); }
		void randc(const double& MinR, const double& MaxR, const double& MinI, const double& MaxI)
		{
			VectorBase<4, T>::randc(MinR, MaxR, MinI, MaxI);
		}
		void randc(const double& MaxR, const double& MaxI) { randc(0, MaxR, 0, MaxI); }
		void randc() { randc(0, 1, 0, 1); }
		void zeros() { VectorBase<4, T>::zeros(); }
		void ones() { VectorBase<4, T>::ones(); }
		void exp() { VectorBase<4, T>::exp(); }
		void exp2() { VectorBase<4, T>::exp2(); }
		void expm1() { VectorBase<4, T>::expm1(); }
		void log() { VectorBase<4, T>::log(); }
		void log2() { VectorBase<4, T>::log2(); }
		void log10() { VectorBase<4, T>::log10(); }

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<4, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<4, T>::operator+=(content);
			return *this;
		}
		template <typename U> Vector& operator -=(U content)
		{
			VectorBase<4, T>::operator-=(content);
			return *this;
		}
		template <typename U> Vector& operator *=(U content)
		{
			VectorBase<4, T>::operator*=(content);
			return *this;
		}
		template <typename U> Vector& operator /=(U content)
		{
			VectorBase<4, T>::operator/=(content);
			return *this;
		}
		template <typename U> Vector& operator ^=(U content)
		{
			VectorBase<4, T>::operator^=(content);
			return *this;
		}
		template <typename U> Vector& operator %=(U content)
		{
			VectorBase<4, T>::operator%=(content);
			return *this;
		}
	};

	template <typename T>
	class Vector<4, T, typename std::enable_if<!is_numeric<T>::value>::type> : public VectorBase<4, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<4, T>{} {}
		Vector(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis) :
			VectorBase<4, T>{ x_axis,y_axis,z_axis,t_axis } {}
		template <typename U> Vector(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, U content) :
			VectorBase<4, T>{ true,x_axis,y_axis,z_axis,t_axis,content } {}

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<4, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<4, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<4, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<4, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis)
		{
			VectorBase<4, T>::resize(x_axis, y_axis, z_axis, t_axis);
		}
		template <typename U> void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, U content)
		{
			VectorBase<4, T>::resize(true, x_axis, y_axis, z_axis, t_axis, content);
		}

		/* iterator */
		typename std::vector<std::vector<std::vector<std::vector<T>>>>::iterator       begin() noexcept { return VectorBase<4, T>::data_4d.begin(); }
		typename std::vector<std::vector<std::vector<std::vector<T>>>>::iterator         end() noexcept { return VectorBase<4, T>::data_4d.end(); }
		typename std::vector<std::vector<std::vector<std::vector<T>>>>::const_iterator begin() const noexcept { return VectorBase<4, T>::data_4d.begin(); }
		typename std::vector<std::vector<std::vector<std::vector<T>>>>::const_iterator   end() const noexcept { return VectorBase<4, T>::data_4d.end(); }
		typename VectorBase<4, T>::iterator begin_all() noexcept { return VectorBase<4, T>::begin_all(); }
		typename VectorBase<4, T>::iterator   end_all() noexcept { return VectorBase<4, T>::end_all(); }
		typename VectorBase<4, T>::iterator begin_x(const int& x) noexcept { return VectorBase<4, T>::begin_x(x); }
		typename VectorBase<4, T>::iterator   end_x(const int& x) noexcept { return VectorBase<4, T>::end_x(x); }
		typename VectorBase<4, T>::iterator begin_y(const int& y) noexcept { return VectorBase<4, T>::begin_y(y); }
		typename VectorBase<4, T>::iterator   end_y(const int& y) noexcept { return VectorBase<4, T>::end_y(y); }
		typename VectorBase<4, T>::iterator begin_z(const int& z) noexcept { return VectorBase<4, T>::begin_z(z); }
		typename VectorBase<4, T>::iterator   end_z(const int& z) noexcept { return VectorBase<4, T>::end_z(z); }
		typename VectorBase<4, T>::iterator begin_t(const int& t) noexcept { return VectorBase<4, T>::begin_t(t); }
		typename VectorBase<4, T>::iterator   end_t(const int& t) noexcept { return VectorBase<4, T>::end_t(t); }
		typename VectorBase<4, T>::iterator begin_xy(const int& x, const int& y) noexcept { return VectorBase<4, T>::begin_xy(x, y); }
		typename VectorBase<4, T>::iterator   end_xy(const int& x, const int& y) noexcept { return VectorBase<4, T>::end_xy(x, y); }
		typename VectorBase<4, T>::iterator begin_xz(const int& x, const int& z) noexcept { return VectorBase<4, T>::begin_xy(x, z); }
		typename VectorBase<4, T>::iterator   end_xz(const int& x, const int& z) noexcept { return VectorBase<4, T>::end_xy(x, z); }
		typename VectorBase<4, T>::iterator begin_yz(const int& y, const int& z) noexcept { return VectorBase<4, T>::begin_yz(y, z); }
		typename VectorBase<4, T>::iterator   end_yz(const int& y, const int& z) noexcept { return VectorBase<4, T>::end_yz(y, z); }
		typename VectorBase<4, T>::iterator begin_xt(const int& x, const int& t) noexcept { return VectorBase<4, T>::begin_xt(x, t); }
		typename VectorBase<4, T>::iterator   end_xt(const int& x, const int& t) noexcept { return VectorBase<4, T>::end_xt(x, t); }
		typename VectorBase<4, T>::iterator begin_yt(const int& y, const int& t) noexcept { return VectorBase<4, T>::begin_yt(y, t); }
		typename VectorBase<4, T>::iterator   end_yt(const int& y, const int& t) noexcept { return VectorBase<4, T>::end_yt(y, t); }
		typename VectorBase<4, T>::iterator begin_zt(const int& z, const int& t) noexcept { return VectorBase<4, T>::begin_zt(z, t); }
		typename VectorBase<4, T>::iterator   end_zt(const int& z, const int& t) noexcept { return VectorBase<4, T>::end_zt(z, t); }
		typename VectorBase<4, T>::iterator begin_xyz(const int& x, const int& y, const int& z) noexcept { return VectorBase<4, T>::begin_xyz(x, y, z); }
		typename VectorBase<4, T>::iterator   end_xyz(const int& x, const int& y, const int& z) noexcept { return VectorBase<4, T>::end_xyz(x, y, z); }
		typename VectorBase<4, T>::iterator begin_xyt(const int& x, const int& y, const int& t) noexcept { return VectorBase<4, T>::begin_xyt(x, y, t); }
		typename VectorBase<4, T>::iterator   end_xyt(const int& x, const int& y, const int& t) noexcept { return VectorBase<4, T>::end_xyt(x, y, t); }
		typename VectorBase<4, T>::iterator begin_xzt(const int& x, const int& z, const int& t) noexcept { return VectorBase<4, T>::begin_xzt(x, z, t); }
		typename VectorBase<4, T>::iterator   end_xzt(const int& x, const int& z, const int& t) noexcept { return VectorBase<4, T>::end_xzt(x, z, t); }
		typename VectorBase<4, T>::iterator begin_yzt(const int& y, const int& z, const int& t) noexcept { return VectorBase<4, T>::begin_yzt(y, z, t); }
		typename VectorBase<4, T>::iterator   end_yzt(const int& y, const int& z, const int& t) noexcept { return VectorBase<4, T>::end_yzt(y, z, t); }
		typename VectorBase<4, T>::const_iterator begin_all() const noexcept { return VectorBase<4, T>::begin_all(); }
		typename VectorBase<4, T>::const_iterator   end_all() const noexcept { return VectorBase<4, T>::end_all(); }
		typename VectorBase<4, T>::const_iterator begin_x(const int& x) const noexcept { return VectorBase<4, T>::begin_x(x); }
		typename VectorBase<4, T>::const_iterator   end_x(const int& x) const noexcept { return VectorBase<4, T>::end_x(x); }
		typename VectorBase<4, T>::const_iterator begin_y(const int& y) const noexcept { return VectorBase<4, T>::begin_y(y); }
		typename VectorBase<4, T>::const_iterator   end_y(const int& y) const noexcept { return VectorBase<4, T>::end_y(y); }
		typename VectorBase<4, T>::const_iterator begin_z(const int& z) const noexcept { return VectorBase<4, T>::begin_z(z); }
		typename VectorBase<4, T>::const_iterator   end_z(const int& z) const noexcept { return VectorBase<4, T>::end_z(z); }
		typename VectorBase<4, T>::const_iterator begin_t(const int& t) const noexcept { return VectorBase<4, T>::begin_t(t); }
		typename VectorBase<4, T>::const_iterator   end_t(const int& t) const noexcept { return VectorBase<4, T>::end_t(t); }
		typename VectorBase<4, T>::const_iterator begin_xy(const int& x, const int& y) const noexcept { return VectorBase<4, T>::begin_xy(x, y); }
		typename VectorBase<4, T>::const_iterator   end_xy(const int& x, const int& y) const noexcept { return VectorBase<4, T>::end_xy(x, y); }
		typename VectorBase<4, T>::const_iterator begin_xz(const int& x, const int& z) const noexcept { return VectorBase<4, T>::begin_xz(x, z); }
		typename VectorBase<4, T>::const_iterator   end_xz(const int& x, const int& z) const noexcept { return VectorBase<4, T>::end_xz(x, z); }
		typename VectorBase<4, T>::const_iterator begin_yz(const int& y, const int& z) const noexcept { return VectorBase<4, T>::begin_yz(y, z); }
		typename VectorBase<4, T>::const_iterator   end_yz(const int& y, const int& z) const noexcept { return VectorBase<4, T>::end_yz(y, z); }
		typename VectorBase<4, T>::const_iterator begin_xt(const int& x, const int& t) const noexcept { return VectorBase<4, T>::begin_xt(x, t); }
		typename VectorBase<4, T>::const_iterator   end_xt(const int& x, const int& t) const noexcept { return VectorBase<4, T>::end_xt(x, t); }
		typename VectorBase<4, T>::const_iterator begin_yt(const int& y, const int& t) const noexcept { return VectorBase<4, T>::begin_yt(y, t); }
		typename VectorBase<4, T>::const_iterator   end_yt(const int& y, const int& t) const noexcept { return VectorBase<4, T>::end_yt(y, t); }
		typename VectorBase<4, T>::const_iterator begin_zt(const int& z, const int& t) const noexcept { return VectorBase<4, T>::begin_zt(z, t); }
		typename VectorBase<4, T>::const_iterator   end_zt(const int& z, const int& t) const noexcept { return VectorBase<4, T>::end_zt(z, t); }
		typename VectorBase<4, T>::const_iterator begin_xyz(const int& x, const int& y, const int& z) const noexcept { return VectorBase<4, T>::begin_xyz(x, y, z); }
		typename VectorBase<4, T>::const_iterator   end_xyz(const int& x, const int& y, const int& z) const noexcept { return VectorBase<4, T>::end_xyz(x, y, z); }
		typename VectorBase<4, T>::const_iterator begin_xyt(const int& x, const int& y, const int& t) const noexcept { return VectorBase<4, T>::begin_xyt(x, y, t); }
		typename VectorBase<4, T>::const_iterator   end_xyt(const int& x, const int& y, const int& t) const noexcept { return VectorBase<4, T>::end_xyt(x, y, t); }
		typename VectorBase<4, T>::const_iterator begin_xzt(const int& x, const int& z, const int& t) const noexcept { return VectorBase<4, T>::begin_xzt(x, z, t); }
		typename VectorBase<4, T>::const_iterator   end_xzt(const int& x, const int& z, const int& t) const noexcept { return VectorBase<4, T>::end_xzt(x, z, t); }
		typename VectorBase<4, T>::const_iterator begin_yzt(const int& y, const int& z, const int& t) const noexcept { return VectorBase<4, T>::begin_yzt(y, z, t); }
		typename VectorBase<4, T>::const_iterator   end_yzt(const int& y, const int& z, const int& t) const noexcept { return VectorBase<4, T>::end_yzt(y, z, t); }

		/* vector function */
		int size() const { return VectorBase<4, T>::x_size * VectorBase<4, T>::y_size * VectorBase<4, T>::z_size * VectorBase<4, T>::t_size; }
		std::vector<int> size_array() { return std::vector<int>{ VectorBase<4, T>::x_size, VectorBase<4, T>::y_size, VectorBase<4, T>::z_size, VectorBase<4, T>::t_size }; }
		bool empty() const { return VectorBase<4, T>::x_size * VectorBase<4, T>::y_size * VectorBase<4, T>::z_size * VectorBase<4, T>::t_size == 0 ? True : False; }
		std::vector<std::vector<std::vector<std::vector<T>>>>& data() { return VectorBase<4, T>::data_4d; }
		std::vector<std::vector<std::vector<T>>>& at(const int& idx) { return VectorBase<4, T>::data_4d[idx]; }
		void clear() { VectorBase<4, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<4, T>::swap(B); }

		/* basic function */
		T& operator()(const int& x_index, const int& y_index, const int& z_index, const int& t_index)
		{
			return VectorBase<4, T>::data_4d[x_index][y_index][z_index][t_index];
		}
		std::vector<T>& operator()(const int& x_index, const int& y_index, const int& z_index) { return VectorBase<4, T>::data_4d[x_index][y_index][z_index]; }
		std::vector<std::vector<T>>& operator()(const int& x_index, const int& y_index) { return VectorBase<4, T>::data_4d[x_index][y_index]; }
		std::vector<std::vector<std::vector<T>>>& operator()(const int& x_index) { return VectorBase<4, T>::data_4d[x_index]; }
		std::vector<std::vector<std::vector<T>>>& operator[](const int& x_index) { return VectorBase<4, T>::data_4d[x_index]; }

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<4, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<4, T>::operator+=(content);
			return *this;
		}
	};

	template <typename T>
	class Vector<5, T, typename std::enable_if<is_numeric<T>::value>::type> : public VectorBase<5, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<5, T>{} {}
		Vector(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis) :
			VectorBase<5, T>{ x_axis,y_axis,z_axis,t_axis,h_axis } {}
		template <typename U> Vector(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis, U content) :
			VectorBase<5, T>{ true,x_axis,y_axis,z_axis,t_axis,h_axis,content } {}

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<5, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<5, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<5, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<5, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis)
		{
			VectorBase<5, T>::resize(x_axis, y_axis, z_axis, t_axis, h_axis);
		}
		template <typename U> void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis, U content)
		{
			VectorBase<5, T>::resize(true, x_axis, y_axis, z_axis, t_axis, h_axis, content);
		}

		/* iterator */
		typename std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>::iterator       begin() noexcept { return VectorBase<5, T>::data_5d.begin(); }
		typename std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>::iterator         end() noexcept { return VectorBase<5, T>::data_5d.end(); }
		typename std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>::const_iterator begin() const noexcept { return VectorBase<5, T>::data_5d.begin(); }
		typename std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>::const_iterator   end() const noexcept { return VectorBase<5, T>::data_5d.end(); }
		typename VectorBase<5, T>::iterator begin_all() noexcept { return VectorBase<5, T>::begin_all(); }
		typename VectorBase<5, T>::iterator   end_all() noexcept { return VectorBase<5, T>::end_all(); }
		typename VectorBase<5, T>::iterator begin_x(const int& x) noexcept { return VectorBase<5, T>::begin_x(x); }
		typename VectorBase<5, T>::iterator   end_x(const int& x) noexcept { return VectorBase<5, T>::end_x(x); }
		typename VectorBase<5, T>::iterator begin_y(const int& y) noexcept { return VectorBase<5, T>::begin_y(y); }
		typename VectorBase<5, T>::iterator   end_y(const int& y) noexcept { return VectorBase<5, T>::end_y(y); }
		typename VectorBase<5, T>::iterator begin_z(const int& z) noexcept { return VectorBase<5, T>::begin_z(z); }
		typename VectorBase<5, T>::iterator   end_z(const int& z) noexcept { return VectorBase<5, T>::end_z(z); }
		typename VectorBase<5, T>::iterator begin_t(const int& t) noexcept { return VectorBase<5, T>::begin_t(t); }
		typename VectorBase<5, T>::iterator   end_t(const int& t) noexcept { return VectorBase<5, T>::end_t(t); }
		typename VectorBase<5, T>::iterator begin_h(const int& h) noexcept { return VectorBase<5, T>::begin_h(h); }
		typename VectorBase<5, T>::iterator   end_h(const int& h) noexcept { return VectorBase<5, T>::end_h(h); }
		typename VectorBase<5, T>::iterator begin_xy(const int& x, const int& y) noexcept { return VectorBase<5, T>::begin_xy(x, y); }
		typename VectorBase<5, T>::iterator   end_xy(const int& x, const int& y) noexcept { return VectorBase<5, T>::end_xy(x, y); }
		typename VectorBase<5, T>::iterator begin_xz(const int& x, const int& z) noexcept { return VectorBase<5, T>::begin_xy(x, z); }
		typename VectorBase<5, T>::iterator   end_xz(const int& x, const int& z) noexcept { return VectorBase<5, T>::end_xy(x, z); }
		typename VectorBase<5, T>::iterator begin_yz(const int& y, const int& z) noexcept { return VectorBase<5, T>::begin_yz(y, z); }
		typename VectorBase<5, T>::iterator   end_yz(const int& y, const int& z) noexcept { return VectorBase<5, T>::end_yz(y, z); }
		typename VectorBase<5, T>::iterator begin_xt(const int& x, const int& t) noexcept { return VectorBase<5, T>::begin_xt(x, t); }
		typename VectorBase<5, T>::iterator   end_xt(const int& x, const int& t) noexcept { return VectorBase<5, T>::end_xt(x, t); }
		typename VectorBase<5, T>::iterator begin_yt(const int& y, const int& t) noexcept { return VectorBase<5, T>::begin_yt(y, t); }
		typename VectorBase<5, T>::iterator   end_yt(const int& y, const int& t) noexcept { return VectorBase<5, T>::end_yt(y, t); }
		typename VectorBase<5, T>::iterator begin_zt(const int& z, const int& t) noexcept { return VectorBase<5, T>::begin_zt(z, t); }
		typename VectorBase<5, T>::iterator   end_zt(const int& z, const int& t) noexcept { return VectorBase<5, T>::end_zt(z, t); }
		typename VectorBase<5, T>::iterator begin_xh(const int& x, const int& h) noexcept { return VectorBase<5, T>::begin_xh(x, h); }
		typename VectorBase<5, T>::iterator   end_xh(const int& x, const int& h) noexcept { return VectorBase<5, T>::end_xh(x, h); }
		typename VectorBase<5, T>::iterator begin_yh(const int& y, const int& h) noexcept { return VectorBase<5, T>::begin_yh(y, h); }
		typename VectorBase<5, T>::iterator   end_yh(const int& y, const int& h) noexcept { return VectorBase<5, T>::end_yh(y, h); }
		typename VectorBase<5, T>::iterator begin_zh(const int& z, const int& h) noexcept { return VectorBase<5, T>::begin_zh(z, h); }
		typename VectorBase<5, T>::iterator   end_zh(const int& z, const int& h) noexcept { return VectorBase<5, T>::end_zh(z, h); }
		typename VectorBase<5, T>::iterator begin_th(const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_th(t, h); }
		typename VectorBase<5, T>::iterator   end_th(const int& t, const int& h) noexcept { return VectorBase<5, T>::end_th(t, h); }
		typename VectorBase<5, T>::iterator begin_xyz(const int& x, const int& y, const int& z) noexcept { return VectorBase<5, T>::begin_xyz(x, y, z); }
		typename VectorBase<5, T>::iterator   end_xyz(const int& x, const int& y, const int& z) noexcept { return VectorBase<5, T>::end_xyz(x, y, z); }
		typename VectorBase<5, T>::iterator begin_xyt(const int& x, const int& y, const int& t) noexcept { return VectorBase<5, T>::begin_xyt(x, y, t); }
		typename VectorBase<5, T>::iterator   end_xyt(const int& x, const int& y, const int& t) noexcept { return VectorBase<5, T>::end_xyt(x, y, t); }
		typename VectorBase<5, T>::iterator begin_xyh(const int& x, const int& y, const int& h) noexcept { return VectorBase<5, T>::begin_xyh(x, y, h); }
		typename VectorBase<5, T>::iterator   end_xyh(const int& x, const int& y, const int& h) noexcept { return VectorBase<5, T>::end_xyh(x, y, h); }
		typename VectorBase<5, T>::iterator begin_xzt(const int& x, const int& z, const int& t) noexcept { return VectorBase<5, T>::begin_xzt(x, z, t); }
		typename VectorBase<5, T>::iterator   end_xzt(const int& x, const int& z, const int& t) noexcept { return VectorBase<5, T>::end_xzt(x, z, t); }
		typename VectorBase<5, T>::iterator begin_xzh(const int& x, const int& z, const int& h) noexcept { return VectorBase<5, T>::begin_xzh(x, z, h); }
		typename VectorBase<5, T>::iterator   end_xzh(const int& x, const int& z, const int& h) noexcept { return VectorBase<5, T>::end_xzh(x, z, h); }
		typename VectorBase<5, T>::iterator begin_xth(const int& x, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_xth(x, t, h); }
		typename VectorBase<5, T>::iterator   end_xth(const int& x, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_xth(x, t, h); }
		typename VectorBase<5, T>::iterator begin_yzt(const int& y, const int& z, const int& t) noexcept { return VectorBase<5, T>::begin_yzt(y, z, t); }
		typename VectorBase<5, T>::iterator   end_yzt(const int& y, const int& z, const int& t) noexcept { return VectorBase<5, T>::end_yzt(y, z, t); }
		typename VectorBase<5, T>::iterator begin_yzh(const int& y, const int& z, const int& h) noexcept { return VectorBase<5, T>::begin_yzh(y, z, h); }
		typename VectorBase<5, T>::iterator   end_yzh(const int& y, const int& z, const int& h) noexcept { return VectorBase<5, T>::end_yzh(y, z, h); }
		typename VectorBase<5, T>::iterator begin_yth(const int& y, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_yth(y, t, h); }
		typename VectorBase<5, T>::iterator   end_yth(const int& y, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_yth(y, t, h); }
		typename VectorBase<5, T>::iterator begin_zth(const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_zth(z, t, h); }
		typename VectorBase<5, T>::iterator   end_zth(const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_zth(z, t, h); }
		typename VectorBase<5, T>::iterator begin_xyzt(const int& x, const int& y, const int& z, const int& t) noexcept { return VectorBase<5, T>::begin_xyzt(x, y, z, t); }
		typename VectorBase<5, T>::iterator   end_xyzt(const int& x, const int& y, const int& z, const int& t) noexcept { return VectorBase<5, T>::end_xyzt(x, y, z, t); }
		typename VectorBase<5, T>::iterator begin_xyzh(const int& x, const int& y, const int& z, const int& h) noexcept { return VectorBase<5, T>::begin_xyzh(x, y, z, h); }
		typename VectorBase<5, T>::iterator   end_xyzh(const int& x, const int& y, const int& z, const int& h) noexcept { return VectorBase<5, T>::end_xyzh(x, y, z, h); }
		typename VectorBase<5, T>::iterator begin_xyth(const int& x, const int& y, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_xyth(x, y, t, h); }
		typename VectorBase<5, T>::iterator   end_xyth(const int& x, const int& y, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_xyth(x, y, t, h); }
		typename VectorBase<5, T>::iterator begin_xzth(const int& x, const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_xzth(x, z, t, h); }
		typename VectorBase<5, T>::iterator   end_xzth(const int& x, const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_xzth(x, z, t, h); }
		typename VectorBase<5, T>::iterator begin_yzth(const int& y, const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_yzth(y, z, t, h); }
		typename VectorBase<5, T>::iterator   end_yzth(const int& y, const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_yzth(y, z, t, h); }
		typename VectorBase<5, T>::const_iterator begin_all() const noexcept { return VectorBase<5, T>::begin_all(); }
		typename VectorBase<5, T>::const_iterator   end_all() const noexcept { return VectorBase<5, T>::end_all(); }
		typename VectorBase<5, T>::const_iterator begin_x(const int& x) const noexcept { return VectorBase<5, T>::begin_x(x); }
		typename VectorBase<5, T>::const_iterator   end_x(const int& x) const noexcept { return VectorBase<5, T>::end_x(x); }
		typename VectorBase<5, T>::const_iterator begin_y(const int& y) const noexcept { return VectorBase<5, T>::begin_y(y); }
		typename VectorBase<5, T>::const_iterator   end_y(const int& y) const noexcept { return VectorBase<5, T>::end_y(y); }
		typename VectorBase<5, T>::const_iterator begin_z(const int& z) const noexcept { return VectorBase<5, T>::begin_z(z); }
		typename VectorBase<5, T>::const_iterator   end_z(const int& z) const noexcept { return VectorBase<5, T>::end_z(z); }
		typename VectorBase<5, T>::const_iterator begin_t(const int& t) const noexcept { return VectorBase<5, T>::begin_t(t); }
		typename VectorBase<5, T>::const_iterator   end_t(const int& t) const noexcept { return VectorBase<5, T>::end_t(t); }
		typename VectorBase<5, T>::const_iterator begin_h(const int& h) const noexcept { return VectorBase<5, T>::begin_h(h); }
		typename VectorBase<5, T>::const_iterator   end_h(const int& h) const noexcept { return VectorBase<5, T>::end_h(h); }
		typename VectorBase<5, T>::const_iterator begin_xy(const int& x, const int& y) const noexcept { return VectorBase<5, T>::begin_xy(x, y); }
		typename VectorBase<5, T>::const_iterator   end_xy(const int& x, const int& y) const noexcept { return VectorBase<5, T>::end_xy(x, y); }
		typename VectorBase<5, T>::const_iterator begin_xz(const int& x, const int& z) const noexcept { return VectorBase<5, T>::begin_xy(x, z); }
		typename VectorBase<5, T>::const_iterator   end_xz(const int& x, const int& z) const noexcept { return VectorBase<5, T>::end_xy(x, z); }
		typename VectorBase<5, T>::const_iterator begin_yz(const int& y, const int& z) const noexcept { return VectorBase<5, T>::begin_yz(y, z); }
		typename VectorBase<5, T>::const_iterator   end_yz(const int& y, const int& z) const noexcept { return VectorBase<5, T>::end_yz(y, z); }
		typename VectorBase<5, T>::const_iterator begin_xt(const int& x, const int& t) const noexcept { return VectorBase<5, T>::begin_xt(x, t); }
		typename VectorBase<5, T>::const_iterator   end_xt(const int& x, const int& t) const noexcept { return VectorBase<5, T>::end_xt(x, t); }
		typename VectorBase<5, T>::const_iterator begin_yt(const int& y, const int& t) const noexcept { return VectorBase<5, T>::begin_yt(y, t); }
		typename VectorBase<5, T>::const_iterator   end_yt(const int& y, const int& t) const noexcept { return VectorBase<5, T>::end_yt(y, t); }
		typename VectorBase<5, T>::const_iterator begin_zt(const int& z, const int& t) const noexcept { return VectorBase<5, T>::begin_zt(z, t); }
		typename VectorBase<5, T>::const_iterator   end_zt(const int& z, const int& t) const noexcept { return VectorBase<5, T>::end_zt(z, t); }
		typename VectorBase<5, T>::const_iterator begin_xh(const int& x, const int& h) const noexcept { return VectorBase<5, T>::begin_xh(x, h); }
		typename VectorBase<5, T>::const_iterator   end_xh(const int& x, const int& h) const noexcept { return VectorBase<5, T>::end_xh(x, h); }
		typename VectorBase<5, T>::const_iterator begin_yh(const int& y, const int& h) const noexcept { return VectorBase<5, T>::begin_yh(y, h); }
		typename VectorBase<5, T>::const_iterator   end_yh(const int& y, const int& h) const noexcept { return VectorBase<5, T>::end_yh(y, h); }
		typename VectorBase<5, T>::const_iterator begin_zh(const int& z, const int& h) const noexcept { return VectorBase<5, T>::begin_zh(z, h); }
		typename VectorBase<5, T>::const_iterator   end_zh(const int& z, const int& h) const noexcept { return VectorBase<5, T>::end_zh(z, h); }
		typename VectorBase<5, T>::const_iterator begin_th(const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_th(t, h); }
		typename VectorBase<5, T>::const_iterator   end_th(const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_th(t, h); }
		typename VectorBase<5, T>::const_iterator begin_xyz(const int& x, const int& y, const int& z) const noexcept { return VectorBase<5, T>::begin_xyz(x, y, z); }
		typename VectorBase<5, T>::const_iterator   end_xyz(const int& x, const int& y, const int& z) const noexcept { return VectorBase<5, T>::end_xyz(x, y, z); }
		typename VectorBase<5, T>::const_iterator begin_xyt(const int& x, const int& y, const int& t) const noexcept { return VectorBase<5, T>::begin_xyt(x, y, t); }
		typename VectorBase<5, T>::const_iterator   end_xyt(const int& x, const int& y, const int& t) const noexcept { return VectorBase<5, T>::end_xyt(x, y, t); }
		typename VectorBase<5, T>::const_iterator begin_xyh(const int& x, const int& y, const int& h) const noexcept { return VectorBase<5, T>::begin_xyh(x, y, h); }
		typename VectorBase<5, T>::const_iterator   end_xyh(const int& x, const int& y, const int& h) const noexcept { return VectorBase<5, T>::end_xyh(x, y, h); }
		typename VectorBase<5, T>::const_iterator begin_xzt(const int& x, const int& z, const int& t) const noexcept { return VectorBase<5, T>::begin_xzt(x, z, t); }
		typename VectorBase<5, T>::const_iterator   end_xzt(const int& x, const int& z, const int& t) const noexcept { return VectorBase<5, T>::end_xzt(x, z, t); }
		typename VectorBase<5, T>::const_iterator begin_xzh(const int& x, const int& z, const int& h) const noexcept { return VectorBase<5, T>::begin_xzh(x, z, h); }
		typename VectorBase<5, T>::const_iterator   end_xzh(const int& x, const int& z, const int& h) const noexcept { return VectorBase<5, T>::end_xzh(x, z, h); }
		typename VectorBase<5, T>::const_iterator begin_xth(const int& x, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_xth(x, t, h); }
		typename VectorBase<5, T>::const_iterator   end_xth(const int& x, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_xth(x, t, h); }
		typename VectorBase<5, T>::const_iterator begin_yzt(const int& y, const int& z, const int& t) const noexcept { return VectorBase<5, T>::begin_yzt(y, z, t); }
		typename VectorBase<5, T>::const_iterator   end_yzt(const int& y, const int& z, const int& t) const noexcept { return VectorBase<5, T>::end_yzt(y, z, t); }
		typename VectorBase<5, T>::const_iterator begin_yzh(const int& y, const int& z, const int& h) const noexcept { return VectorBase<5, T>::begin_yzh(y, z, h); }
		typename VectorBase<5, T>::const_iterator   end_yzh(const int& y, const int& z, const int& h) const noexcept { return VectorBase<5, T>::end_yzh(y, z, h); }
		typename VectorBase<5, T>::const_iterator begin_yth(const int& y, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_yth(y, t, h); }
		typename VectorBase<5, T>::const_iterator   end_yth(const int& y, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_yth(y, t, h); }
		typename VectorBase<5, T>::const_iterator begin_zth(const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_zth(z, t, h); }
		typename VectorBase<5, T>::const_iterator   end_zth(const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_zth(z, t, h); }
		typename VectorBase<5, T>::const_iterator begin_xyzt(const int& x, const int& y, const int& z, const int& t) const noexcept { return VectorBase<5, T>::begin_xyzt(x, y, z, t); }
		typename VectorBase<5, T>::const_iterator   end_xyzt(const int& x, const int& y, const int& z, const int& t) const noexcept { return VectorBase<5, T>::end_xyzt(x, y, z, t); }
		typename VectorBase<5, T>::const_iterator begin_xyzh(const int& x, const int& y, const int& z, const int& h) const noexcept { return VectorBase<5, T>::begin_xyzh(x, y, z, h); }
		typename VectorBase<5, T>::const_iterator   end_xyzh(const int& x, const int& y, const int& z, const int& h) const noexcept { return VectorBase<5, T>::end_xyzh(x, y, z, h); }
		typename VectorBase<5, T>::const_iterator begin_xyth(const int& x, const int& y, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_xyth(x, y, t, h); }
		typename VectorBase<5, T>::const_iterator   end_xyth(const int& x, const int& y, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_xyth(x, y, t, h); }
		typename VectorBase<5, T>::const_iterator begin_xzth(const int& x, const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_xzth(x, z, t, h); }
		typename VectorBase<5, T>::const_iterator   end_xzth(const int& x, const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_xzth(x, z, t, h); }
		typename VectorBase<5, T>::const_iterator begin_yzth(const int& y, const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_yzth(y, z, t, h); }
		typename VectorBase<5, T>::const_iterator   end_yzth(const int& y, const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_yzth(y, z, t, h); }

		/* vector function */
		int size() const
		{
			return VectorBase<5, T>::x_size *
				VectorBase<5, T>::y_size *
				VectorBase<5, T>::z_size *
				VectorBase<5, T>::t_size *
				VectorBase<5, T>::h_size;
		}
		std::vector<int> size_array()
		{
			return std::vector<int>{ VectorBase<5, T>::x_size,
				VectorBase<5, T>::y_size,
				VectorBase<5, T>::z_size,
				VectorBase<5, T>::t_size,
				VectorBase<5, T>::h_size };
		}
		bool empty() const
		{
			return VectorBase<5, T>::x_size *
				VectorBase<5, T>::y_size *
				VectorBase<5, T>::z_size *
				VectorBase<5, T>::t_size *
				VectorBase<5, T>::h_size == 0 ? True : False;
		}
		std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>& data() { return VectorBase<5, T>::data_5d; }
		std::vector<std::vector<std::vector<std::vector<T>>>>& at(const int& idx) { return VectorBase<5, T>::data_5d[idx]; }
		void clear() { VectorBase<5, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<5, T>::swap(B); }

		/* basic function */
		T& operator()(const int& x_index, const int& y_index, const int& z_index, const int& t_index, const int& h_index)
		{
			return VectorBase<5, T>::data_5d[x_index][y_index][z_index][t_index][h_index];
		}
		std::vector<T>& operator()(const int& x_index, const int& y_index, const int& z_index, const int& t_index)
		{
			return VectorBase<5, T>::data_5d[x_index][y_index][z_index][t_index];
		}
		std::vector<std::vector<T>>& operator()(const int& x_index, const int& y_index, const int& z_index)
		{
			return VectorBase<5, T>::data_5d[x_index][y_index][z_index];
		}
		std::vector<std::vector<std::vector<T>>>& operator()(const int& x_index, const int& y_index)
		{
			return VectorBase<5, T>::data_5d[x_index][y_index];
		}
		std::vector<std::vector<std::vector<std::vector<T>>>>& operator()(const int& x_index)
		{
			return VectorBase<5, T>::data_5d[x_index];
		}
		std::vector<std::vector<std::vector<std::vector<T>>>>& operator[](const int& x_index)
		{
			return VectorBase<5, T>::data_5d[x_index];
		}
		void rand(const double& Min, const double& Max) { VectorBase<5, T>::rand(Min, Max); }
		void rand(const double& Max) { rand(0, Max); }
		void rand() { rand(0, 1); }
		void randi(const int& Min, const int& Max) { VectorBase<5, T>::randi(Min, Max); }
		void randi(const int& Max) { randi(0, Max); }
		void randi() { randi(0, 1); }
		void randc(const double& MinR, const double& MaxR, const double& MinI, const double& MaxI)
		{
			VectorBase<5, T>::randc(MinR, MaxR, MinI, MaxI);
		}
		void randc(const double& MaxR, const double& MaxI) { randc(0, MaxR, 0, MaxI); }
		void randc() { randc(0, 1, 0, 1); }
		void zeros() { VectorBase<5, T>::zeros(); }
		void ones() { VectorBase<5, T>::ones(); }
		void exp() { VectorBase<5, T>::exp(); }
		void exp2() { VectorBase<5, T>::exp2(); }
		void expm1() { VectorBase<5, T>::expm1(); }
		void log() { VectorBase<5, T>::log(); }
		void log2() { VectorBase<5, T>::log2(); }
		void log10() { VectorBase<5, T>::log10(); }

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<5, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<5, T>::operator+=(content);
			return *this;
		}
		template <typename U> Vector& operator -=(U content)
		{
			VectorBase<5, T>::operator-=(content);
			return *this;
		}
		template <typename U> Vector& operator *=(U content)
		{
			VectorBase<5, T>::operator*=(content);
			return *this;
		}
		template <typename U> Vector& operator /=(U content)
		{
			VectorBase<5, T>::operator/=(content);
			return *this;
		}
		template <typename U> Vector& operator ^=(U content)
		{
			VectorBase<5, T>::operator^=(content);
			return *this;
		}
		template <typename U> Vector& operator %=(U content)
		{
			VectorBase<5, T>::operator%=(content);
			return *this;
		}
	};

	template <typename T>
	class Vector<5, T, typename std::enable_if<!is_numeric<T>::value>::type> : public VectorBase<5, T>
	{
	public:
		/* constructor */
		Vector() : VectorBase<5, T>{} {}
		Vector(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis) :
			VectorBase<5, T>{ x_axis,y_axis,z_axis,t_axis,h_axis } {}
		template <typename U> Vector(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis, U content) :
			VectorBase<5, T>{ true,x_axis,y_axis,z_axis,t_axis,h_axis,content } {}

		/* destructor */
		~Vector() = default;
		/* copy constructor */
		Vector(const Vector& A) : VectorBase<5, T>(A) {}
		Vector& operator =(Vector const& rhs)
		{
			VectorBase<5, T>::operator=(rhs);
			return *this;
		}
		/* move constructor */
		Vector(Vector&& A) noexcept : VectorBase<5, T>(std::move(A)) {}
		/* move assignment operator */
		Vector& operator=(Vector&& A) noexcept
		{
			VectorBase<5, T>(std::move(A));
			return *this;
		}

		/* resize */
		void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis)
		{
			VectorBase<5, T>::resize(x_axis, y_axis, z_axis, t_axis, h_axis);
		}
		template <typename U> void resize(const int& x_axis, const int& y_axis, const int& z_axis, const int& t_axis, const int& h_axis, U content)
		{
			VectorBase<5, T>::resize(true, x_axis, y_axis, z_axis, t_axis, h_axis, content);
		}

		/* iterator */
		typename std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>::iterator       begin() noexcept { return VectorBase<5, T>::data_5d.begin(); }
		typename std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>::iterator         end() noexcept { return VectorBase<5, T>::data_5d.end(); }
		typename std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>::const_iterator begin() const noexcept { return VectorBase<5, T>::data_5d.begin(); }
		typename std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>::const_iterator   end() const noexcept { return VectorBase<5, T>::data_5d.end(); }
		typename VectorBase<5, T>::iterator begin_all() noexcept { return VectorBase<5, T>::begin_all(); }
		typename VectorBase<5, T>::iterator   end_all() noexcept { return VectorBase<5, T>::end_all(); }
		typename VectorBase<5, T>::iterator begin_x(const int& x) noexcept { return VectorBase<5, T>::begin_x(x); }
		typename VectorBase<5, T>::iterator   end_x(const int& x) noexcept { return VectorBase<5, T>::end_x(x); }
		typename VectorBase<5, T>::iterator begin_y(const int& y) noexcept { return VectorBase<5, T>::begin_y(y); }
		typename VectorBase<5, T>::iterator   end_y(const int& y) noexcept { return VectorBase<5, T>::end_y(y); }
		typename VectorBase<5, T>::iterator begin_z(const int& z) noexcept { return VectorBase<5, T>::begin_z(z); }
		typename VectorBase<5, T>::iterator   end_z(const int& z) noexcept { return VectorBase<5, T>::end_z(z); }
		typename VectorBase<5, T>::iterator begin_t(const int& t) noexcept { return VectorBase<5, T>::begin_t(t); }
		typename VectorBase<5, T>::iterator   end_t(const int& t) noexcept { return VectorBase<5, T>::end_t(t); }
		typename VectorBase<5, T>::iterator begin_h(const int& h) noexcept { return VectorBase<5, T>::begin_h(h); }
		typename VectorBase<5, T>::iterator   end_h(const int& h) noexcept { return VectorBase<5, T>::end_h(h); }
		typename VectorBase<5, T>::iterator begin_xy(const int& x, const int& y) noexcept { return VectorBase<5, T>::begin_xy(x, y); }
		typename VectorBase<5, T>::iterator   end_xy(const int& x, const int& y) noexcept { return VectorBase<5, T>::end_xy(x, y); }
		typename VectorBase<5, T>::iterator begin_xz(const int& x, const int& z) noexcept { return VectorBase<5, T>::begin_xy(x, z); }
		typename VectorBase<5, T>::iterator   end_xz(const int& x, const int& z) noexcept { return VectorBase<5, T>::end_xy(x, z); }
		typename VectorBase<5, T>::iterator begin_yz(const int& y, const int& z) noexcept { return VectorBase<5, T>::begin_yz(y, z); }
		typename VectorBase<5, T>::iterator   end_yz(const int& y, const int& z) noexcept { return VectorBase<5, T>::end_yz(y, z); }
		typename VectorBase<5, T>::iterator begin_xt(const int& x, const int& t) noexcept { return VectorBase<5, T>::begin_xt(x, t); }
		typename VectorBase<5, T>::iterator   end_xt(const int& x, const int& t) noexcept { return VectorBase<5, T>::end_xt(x, t); }
		typename VectorBase<5, T>::iterator begin_yt(const int& y, const int& t) noexcept { return VectorBase<5, T>::begin_yt(y, t); }
		typename VectorBase<5, T>::iterator   end_yt(const int& y, const int& t) noexcept { return VectorBase<5, T>::end_yt(y, t); }
		typename VectorBase<5, T>::iterator begin_zt(const int& z, const int& t) noexcept { return VectorBase<5, T>::begin_zt(z, t); }
		typename VectorBase<5, T>::iterator   end_zt(const int& z, const int& t) noexcept { return VectorBase<5, T>::end_zt(z, t); }
		typename VectorBase<5, T>::iterator begin_xh(const int& x, const int& h) noexcept { return VectorBase<5, T>::begin_xh(x, h); }
		typename VectorBase<5, T>::iterator   end_xh(const int& x, const int& h) noexcept { return VectorBase<5, T>::end_xh(x, h); }
		typename VectorBase<5, T>::iterator begin_yh(const int& y, const int& h) noexcept { return VectorBase<5, T>::begin_yh(y, h); }
		typename VectorBase<5, T>::iterator   end_yh(const int& y, const int& h) noexcept { return VectorBase<5, T>::end_yh(y, h); }
		typename VectorBase<5, T>::iterator begin_zh(const int& z, const int& h) noexcept { return VectorBase<5, T>::begin_zh(z, h); }
		typename VectorBase<5, T>::iterator   end_zh(const int& z, const int& h) noexcept { return VectorBase<5, T>::end_zh(z, h); }
		typename VectorBase<5, T>::iterator begin_th(const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_th(t, h); }
		typename VectorBase<5, T>::iterator   end_th(const int& t, const int& h) noexcept { return VectorBase<5, T>::end_th(t, h); }
		typename VectorBase<5, T>::iterator begin_xyz(const int& x, const int& y, const int& z) noexcept { return VectorBase<5, T>::begin_xyz(x, y, z); }
		typename VectorBase<5, T>::iterator   end_xyz(const int& x, const int& y, const int& z) noexcept { return VectorBase<5, T>::end_xyz(x, y, z); }
		typename VectorBase<5, T>::iterator begin_xyt(const int& x, const int& y, const int& t) noexcept { return VectorBase<5, T>::begin_xyt(x, y, t); }
		typename VectorBase<5, T>::iterator   end_xyt(const int& x, const int& y, const int& t) noexcept { return VectorBase<5, T>::end_xyt(x, y, t); }
		typename VectorBase<5, T>::iterator begin_xyh(const int& x, const int& y, const int& h) noexcept { return VectorBase<5, T>::begin_xyh(x, y, h); }
		typename VectorBase<5, T>::iterator   end_xyh(const int& x, const int& y, const int& h) noexcept { return VectorBase<5, T>::end_xyh(x, y, h); }
		typename VectorBase<5, T>::iterator begin_xzt(const int& x, const int& z, const int& t) noexcept { return VectorBase<5, T>::begin_xzt(x, z, t); }
		typename VectorBase<5, T>::iterator   end_xzt(const int& x, const int& z, const int& t) noexcept { return VectorBase<5, T>::end_xzt(x, z, t); }
		typename VectorBase<5, T>::iterator begin_xzh(const int& x, const int& z, const int& h) noexcept { return VectorBase<5, T>::begin_xzh(x, z, h); }
		typename VectorBase<5, T>::iterator   end_xzh(const int& x, const int& z, const int& h) noexcept { return VectorBase<5, T>::end_xzh(x, z, h); }
		typename VectorBase<5, T>::iterator begin_xth(const int& x, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_xth(x, t, h); }
		typename VectorBase<5, T>::iterator   end_xth(const int& x, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_xth(x, t, h); }
		typename VectorBase<5, T>::iterator begin_yzt(const int& y, const int& z, const int& t) noexcept { return VectorBase<5, T>::begin_yzt(y, z, t); }
		typename VectorBase<5, T>::iterator   end_yzt(const int& y, const int& z, const int& t) noexcept { return VectorBase<5, T>::end_yzt(y, z, t); }
		typename VectorBase<5, T>::iterator begin_yzh(const int& y, const int& z, const int& h) noexcept { return VectorBase<5, T>::begin_yzh(y, z, h); }
		typename VectorBase<5, T>::iterator   end_yzh(const int& y, const int& z, const int& h) noexcept { return VectorBase<5, T>::end_yzh(y, z, h); }
		typename VectorBase<5, T>::iterator begin_yth(const int& y, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_yth(y, t, h); }
		typename VectorBase<5, T>::iterator   end_yth(const int& y, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_yth(y, t, h); }
		typename VectorBase<5, T>::iterator begin_zth(const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_zth(z, t, h); }
		typename VectorBase<5, T>::iterator   end_zth(const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_zth(z, t, h); }
		typename VectorBase<5, T>::iterator begin_xyzt(const int& x, const int& y, const int& z, const int& t) noexcept { return VectorBase<5, T>::begin_xyzt(x, y, z, t); }
		typename VectorBase<5, T>::iterator   end_xyzt(const int& x, const int& y, const int& z, const int& t) noexcept { return VectorBase<5, T>::end_xyzt(x, y, z, t); }
		typename VectorBase<5, T>::iterator begin_xyzh(const int& x, const int& y, const int& z, const int& h) noexcept { return VectorBase<5, T>::begin_xyzh(x, y, z, h); }
		typename VectorBase<5, T>::iterator   end_xyzh(const int& x, const int& y, const int& z, const int& h) noexcept { return VectorBase<5, T>::end_xyzh(x, y, z, h); }
		typename VectorBase<5, T>::iterator begin_xyth(const int& x, const int& y, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_xyth(x, y, t, h); }
		typename VectorBase<5, T>::iterator   end_xyth(const int& x, const int& y, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_xyth(x, y, t, h); }
		typename VectorBase<5, T>::iterator begin_xzth(const int& x, const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_xzth(x, z, t, h); }
		typename VectorBase<5, T>::iterator   end_xzth(const int& x, const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_xzth(x, z, t, h); }
		typename VectorBase<5, T>::iterator begin_yzth(const int& y, const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::begin_yzth(y, z, t, h); }
		typename VectorBase<5, T>::iterator   end_yzth(const int& y, const int& z, const int& t, const int& h) noexcept { return VectorBase<5, T>::end_yzth(y, z, t, h); }
		typename VectorBase<5, T>::const_iterator begin_all() const noexcept { return VectorBase<5, T>::begin_all(); }
		typename VectorBase<5, T>::const_iterator   end_all() const noexcept { return VectorBase<5, T>::end_all(); }
		typename VectorBase<5, T>::const_iterator begin_x(const int& x) const noexcept { return VectorBase<5, T>::begin_x(x); }
		typename VectorBase<5, T>::const_iterator   end_x(const int& x) const noexcept { return VectorBase<5, T>::end_x(x); }
		typename VectorBase<5, T>::const_iterator begin_y(const int& y) const noexcept { return VectorBase<5, T>::begin_y(y); }
		typename VectorBase<5, T>::const_iterator   end_y(const int& y) const noexcept { return VectorBase<5, T>::end_y(y); }
		typename VectorBase<5, T>::const_iterator begin_z(const int& z) const noexcept { return VectorBase<5, T>::begin_z(z); }
		typename VectorBase<5, T>::const_iterator   end_z(const int& z) const noexcept { return VectorBase<5, T>::end_z(z); }
		typename VectorBase<5, T>::const_iterator begin_t(const int& t) const noexcept { return VectorBase<5, T>::begin_t(t); }
		typename VectorBase<5, T>::const_iterator   end_t(const int& t) const noexcept { return VectorBase<5, T>::end_t(t); }
		typename VectorBase<5, T>::const_iterator begin_h(const int& h) const noexcept { return VectorBase<5, T>::begin_h(h); }
		typename VectorBase<5, T>::const_iterator   end_h(const int& h) const noexcept { return VectorBase<5, T>::end_h(h); }
		typename VectorBase<5, T>::const_iterator begin_xy(const int& x, const int& y) const noexcept { return VectorBase<5, T>::begin_xy(x, y); }
		typename VectorBase<5, T>::const_iterator   end_xy(const int& x, const int& y) const noexcept { return VectorBase<5, T>::end_xy(x, y); }
		typename VectorBase<5, T>::const_iterator begin_xz(const int& x, const int& z) const noexcept { return VectorBase<5, T>::begin_xy(x, z); }
		typename VectorBase<5, T>::const_iterator   end_xz(const int& x, const int& z) const noexcept { return VectorBase<5, T>::end_xy(x, z); }
		typename VectorBase<5, T>::const_iterator begin_yz(const int& y, const int& z) const noexcept { return VectorBase<5, T>::begin_yz(y, z); }
		typename VectorBase<5, T>::const_iterator   end_yz(const int& y, const int& z) const noexcept { return VectorBase<5, T>::end_yz(y, z); }
		typename VectorBase<5, T>::const_iterator begin_xt(const int& x, const int& t) const noexcept { return VectorBase<5, T>::begin_xt(x, t); }
		typename VectorBase<5, T>::const_iterator   end_xt(const int& x, const int& t) const noexcept { return VectorBase<5, T>::end_xt(x, t); }
		typename VectorBase<5, T>::const_iterator begin_yt(const int& y, const int& t) const noexcept { return VectorBase<5, T>::begin_yt(y, t); }
		typename VectorBase<5, T>::const_iterator   end_yt(const int& y, const int& t) const noexcept { return VectorBase<5, T>::end_yt(y, t); }
		typename VectorBase<5, T>::const_iterator begin_zt(const int& z, const int& t) const noexcept { return VectorBase<5, T>::begin_zt(z, t); }
		typename VectorBase<5, T>::const_iterator   end_zt(const int& z, const int& t) const noexcept { return VectorBase<5, T>::end_zt(z, t); }
		typename VectorBase<5, T>::const_iterator begin_xh(const int& x, const int& h) const noexcept { return VectorBase<5, T>::begin_xh(x, h); }
		typename VectorBase<5, T>::const_iterator   end_xh(const int& x, const int& h) const noexcept { return VectorBase<5, T>::end_xh(x, h); }
		typename VectorBase<5, T>::const_iterator begin_yh(const int& y, const int& h) const noexcept { return VectorBase<5, T>::begin_yh(y, h); }
		typename VectorBase<5, T>::const_iterator   end_yh(const int& y, const int& h) const noexcept { return VectorBase<5, T>::end_yh(y, h); }
		typename VectorBase<5, T>::const_iterator begin_zh(const int& z, const int& h) const noexcept { return VectorBase<5, T>::begin_zh(z, h); }
		typename VectorBase<5, T>::const_iterator   end_zh(const int& z, const int& h) const noexcept { return VectorBase<5, T>::end_zh(z, h); }
		typename VectorBase<5, T>::const_iterator begin_th(const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_th(t, h); }
		typename VectorBase<5, T>::const_iterator   end_th(const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_th(t, h); }
		typename VectorBase<5, T>::const_iterator begin_xyz(const int& x, const int& y, const int& z) const noexcept { return VectorBase<5, T>::begin_xyz(x, y, z); }
		typename VectorBase<5, T>::const_iterator   end_xyz(const int& x, const int& y, const int& z) const noexcept { return VectorBase<5, T>::end_xyz(x, y, z); }
		typename VectorBase<5, T>::const_iterator begin_xyt(const int& x, const int& y, const int& t) const noexcept { return VectorBase<5, T>::begin_xyt(x, y, t); }
		typename VectorBase<5, T>::const_iterator   end_xyt(const int& x, const int& y, const int& t) const noexcept { return VectorBase<5, T>::end_xyt(x, y, t); }
		typename VectorBase<5, T>::const_iterator begin_xyh(const int& x, const int& y, const int& h) const noexcept { return VectorBase<5, T>::begin_xyh(x, y, h); }
		typename VectorBase<5, T>::const_iterator   end_xyh(const int& x, const int& y, const int& h) const noexcept { return VectorBase<5, T>::end_xyh(x, y, h); }
		typename VectorBase<5, T>::const_iterator begin_xzt(const int& x, const int& z, const int& t) const noexcept { return VectorBase<5, T>::begin_xzt(x, z, t); }
		typename VectorBase<5, T>::const_iterator   end_xzt(const int& x, const int& z, const int& t) const noexcept { return VectorBase<5, T>::end_xzt(x, z, t); }
		typename VectorBase<5, T>::const_iterator begin_xzh(const int& x, const int& z, const int& h) const noexcept { return VectorBase<5, T>::begin_xzh(x, z, h); }
		typename VectorBase<5, T>::const_iterator   end_xzh(const int& x, const int& z, const int& h) const noexcept { return VectorBase<5, T>::end_xzh(x, z, h); }
		typename VectorBase<5, T>::const_iterator begin_xth(const int& x, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_xth(x, t, h); }
		typename VectorBase<5, T>::const_iterator   end_xth(const int& x, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_xth(x, t, h); }
		typename VectorBase<5, T>::const_iterator begin_yzt(const int& y, const int& z, const int& t) const noexcept { return VectorBase<5, T>::begin_yzt(y, z, t); }
		typename VectorBase<5, T>::const_iterator   end_yzt(const int& y, const int& z, const int& t) const noexcept { return VectorBase<5, T>::end_yzt(y, z, t); }
		typename VectorBase<5, T>::const_iterator begin_yzh(const int& y, const int& z, const int& h) const noexcept { return VectorBase<5, T>::begin_yzh(y, z, h); }
		typename VectorBase<5, T>::const_iterator   end_yzh(const int& y, const int& z, const int& h) const noexcept { return VectorBase<5, T>::end_yzh(y, z, h); }
		typename VectorBase<5, T>::const_iterator begin_yth(const int& y, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_yth(y, t, h); }
		typename VectorBase<5, T>::const_iterator   end_yth(const int& y, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_yth(y, t, h); }
		typename VectorBase<5, T>::const_iterator begin_zth(const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_zth(z, t, h); }
		typename VectorBase<5, T>::const_iterator   end_zth(const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_zth(z, t, h); }
		typename VectorBase<5, T>::const_iterator begin_xyzt(const int& x, const int& y, const int& z, const int& t) const noexcept { return VectorBase<5, T>::begin_xyzt(x, y, z, t); }
		typename VectorBase<5, T>::const_iterator   end_xyzt(const int& x, const int& y, const int& z, const int& t) const noexcept { return VectorBase<5, T>::end_xyzt(x, y, z, t); }
		typename VectorBase<5, T>::const_iterator begin_xyzh(const int& x, const int& y, const int& z, const int& h) const noexcept { return VectorBase<5, T>::begin_xyzh(x, y, z, h); }
		typename VectorBase<5, T>::const_iterator   end_xyzh(const int& x, const int& y, const int& z, const int& h) const noexcept { return VectorBase<5, T>::end_xyzh(x, y, z, h); }
		typename VectorBase<5, T>::const_iterator begin_xyth(const int& x, const int& y, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_xyth(x, y, t, h); }
		typename VectorBase<5, T>::const_iterator   end_xyth(const int& x, const int& y, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_xyth(x, y, t, h); }
		typename VectorBase<5, T>::const_iterator begin_xzth(const int& x, const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_xzth(x, z, t, h); }
		typename VectorBase<5, T>::const_iterator   end_xzth(const int& x, const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_xzth(x, z, t, h); }
		typename VectorBase<5, T>::const_iterator begin_yzth(const int& y, const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::begin_yzth(y, z, t, h); }
		typename VectorBase<5, T>::const_iterator   end_yzth(const int& y, const int& z, const int& t, const int& h) const noexcept { return VectorBase<5, T>::end_yzth(y, z, t, h); }

		/* vector function */
		int size() const
		{
			return VectorBase<5, T>::x_size *
				VectorBase<5, T>::y_size *
				VectorBase<5, T>::z_size *
				VectorBase<5, T>::t_size *
				VectorBase<5, T>::h_size;
		}
		std::vector<int> size_array()
		{
			return std::vector<int>{ VectorBase<5, T>::x_size,
				VectorBase<5, T>::y_size,
				VectorBase<5, T>::z_size,
				VectorBase<5, T>::t_size,
				VectorBase<5, T>::h_size };
		}
		bool empty() const
		{
			return VectorBase<5, T>::x_size *
				VectorBase<5, T>::y_size *
				VectorBase<5, T>::z_size *
				VectorBase<5, T>::t_size *
				VectorBase<5, T>::h_size == 0 ? True : False;
		}
		std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>& data() { return VectorBase<5, T>::data_5d; }
		std::vector<std::vector<std::vector<std::vector<T>>>>& at(const int& idx) { return VectorBase<5, T>::data_5d[idx]; }
		void clear() { VectorBase<5, T>::clear(); }
		void swap(Vector& B) noexcept { VectorBase<5, T>::swap(B); }

		/* basic function */
		T& operator()(const int& x_index, const int& y_index, const int& z_index, const int& t_index, const int& h_index)
		{
			return VectorBase<5, T>::data_5d[x_index][y_index][z_index][t_index][h_index];
		}
		std::vector<T>& operator()(const int& x_index, const int& y_index, const int& z_index, const int& t_index)
		{
			return VectorBase<5, T>::data_5d[x_index][y_index][z_index][t_index];
		}
		std::vector<std::vector<T>>& operator()(const int& x_index, const int& y_index, const int& z_index)
		{
			return VectorBase<5, T>::data_5d[x_index][y_index][z_index];
		}
		std::vector<std::vector<std::vector<T>>>& operator()(const int& x_index, const int& y_index)
		{
			return VectorBase<5, T>::data_5d[x_index][y_index];
		}
		std::vector<std::vector<std::vector<std::vector<T>>>>& operator()(const int& x_index)
		{
			return VectorBase<5, T>::data_5d[x_index];
		}
		std::vector<std::vector<std::vector<std::vector<T>>>>& operator[](const int& x_index)
		{
			return VectorBase<5, T>::data_5d[x_index];
		}

		/* operator overload */
		template <typename U> Vector& operator = (U content)
		{
			VectorBase<5, T>::operator=(content);
			return *this;
		}
		template <typename U> Vector& operator +=(U content)
		{
			VectorBase<5, T>::operator+=(content);
			return *this;
		}
	};

	template <typename T, is_vector(remove_cvr(T))>	typename extract2<remove_cvr(T)>::type2 sum(T B)
	{
		typename extract2<remove_cvr(T)>::type2 sum = 0;
		for (auto iter = B.begin_all(); iter != B.end_all(); ++iter) { sum += *iter; }
		return sum;
	}
	template <typename T, typename U, is_vector2(T, U)> void size_chk(T lhs, U rhs)
	{
		if (lhs.size_array() != rhs.size_array())
			throw std::invalid_argument("Left.size is different from Right.size !");
	}
	/* operator overload */
	// dot [out-class] 1d
	template <typename T, typename U, is_num_vector2(T, U)> typename extract2<remove_cvr(T)>::type2 dot(T lhs, U rhs)
	{
		if (lhs.DIMFLAG != 1 || rhs.DIMFLAG != 1) { throw std::invalid_argument("Dimension should be the same!"); }
		if (lhs.x_size != rhs.x_size) { throw std::invalid_argument("Left_length, Right_length... is different!"); }
		typename extract2<remove_cvr(T)>::type2 res = 0;
		for (auto i : range(lhs.x_size)) { res += lhs(i) * rhs(i); }
		return res;
	}
	template <typename T, typename U, is_num_vector2(T, U)> remove_cvr(T) cross(T lhs, U rhs)
	{
		if (lhs.DIMFLAG != 1 || rhs.DIMFLAG != 1) { throw std::invalid_argument("Dimension should be the same!"); }
		if (lhs.x_size != rhs.x_size || lhs.x_size != 3) { throw std::invalid_argument("len(Left), len(Right)... is different or length != 3 !"); }
		remove_cvr(T) res(3, 0);
		for (auto i : range(3)) { auto m = (i + 1) % 3, n = (i + 2) % 3;  res(i) = lhs(m) * rhs(n) - lhs(n) * rhs(m); }
		return res;
	}
	// dot [out-class] 2d matrix dot
	template <typename T, typename U, is_num_vector2(T, U)> remove_cvr(T) dot(T lhs, U rhs)
	{
		if (lhs.DIMFLAG != 2 || rhs.DIMFLAG != 2) { throw std::invalid_argument("Dimension should be the same!"); }
		if (lhs.y_size != rhs.x_size) { throw std::invalid_argument("Left(i,j), Right(k,l)... j is different from k!"); }
		remove_cvr(T) res(lhs.x_size, rhs.y_size, 0);
		for (auto i : range(lhs.x_size)) { for (auto j : range(rhs.y_size)) { for (auto k : range(lhs.y_size)) { res(i, j) += lhs(i, k) * rhs(k, j); } } }
		return res;
	}
	// operator +
	template <typename T, typename U, is_num_vector2(T, U)>            remove_cvr(T) operator + (T lhs, U rhs)
	{
		size_chk(lhs, rhs); remove_cvr(T) res(lhs);
		for (auto iterA = res.begin_all(), iterB = rhs.begin_all(); iterA != res.end_all(); ++iterA, ++iterB) { *iterA += *iterB; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator + (T     lhs, U content)
	{
		remove_cvr(T) res(lhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter += content; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator + (U content, T     rhs) { return rhs + content; }
	// operator -
	template <typename T, typename U, is_num_vector2(T, U)>            remove_cvr(T) operator - (T lhs, U rhs)
	{
		size_chk(lhs, rhs); remove_cvr(T) res(lhs);
		for (auto iterA = res.begin_all(), iterB = rhs.begin_all(); iterA != res.end_all(); ++iterA, ++iterB) { *iterA -= *iterB; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator - (T     lhs, U content)
	{
		remove_cvr(T) res(lhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter -= content; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator - (U content, T     rhs)
	{
		remove_cvr(T) res(rhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter = T(content) - *iter; }
		return res;
	}
	// operator *
	template <typename T, typename U, is_num_vector2(T, U)>            remove_cvr(T) operator * (T lhs, U rhs)
	{
		size_chk(lhs, rhs); remove_cvr(T) res(lhs);
		for (auto iterA = res.begin_all(), iterB = rhs.begin_all(); iterA != res.end_all(); ++iterA, ++iterB) { *iterA *= *iterB; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator * (T     lhs, U content)
	{
		remove_cvr(T) res(lhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter *= content; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator * (U content, T     rhs) { return rhs * content; }
	// operator /
	template <typename T, typename U, is_num_vector2(T, U)>            remove_cvr(T) operator / (T lhs, U rhs)
	{
		size_chk(lhs, rhs); remove_cvr(T) res(lhs);
		for (auto iterA = res.begin_all(), iterB = rhs.begin_all(); iterA != res.end_all(); ++iterA, ++iterB) { *iterA /= *iterB; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator / (T     lhs, U content)
	{
		remove_cvr(T) res(lhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter /= content; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator / (U content, T     rhs)
	{
		remove_cvr(T) res(rhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter = T(content) / *iter; }
		return res;
	}
	// operator ^
	template <typename T, typename U, is_num_vector2(T, U)>            remove_cvr(T) operator ^ (T lhs, U rhs)
	{
		size_chk(lhs, rhs);
		remove_cvr(T) res(lhs);
		for (auto iterA = res.begin_all(), iterB = rhs.begin_all(); iterA != res.end_all(); ++iterA, ++iterB) { *iterA = std::pow(*iterA, *iterB); }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator ^ (T     lhs, U content)
	{
		remove_cvr(T) res(lhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter = std::pow(*iter, content); }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator ^ (U content, T     rhs)
	{
		remove_cvr(T) res(rhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter = std::pow(content, *iter); }
		return res;
	}
	// operator %
	template <typename T, typename U, is_num_vector2(T, U)>            remove_cvr(T) operator % (T lhs, U rhs)
	{
		size_chk(lhs, rhs);
		remove_cvr(T) res(lhs);
		for (auto iterA = res.begin_all(), iterB = rhs.begin_all(); iterA != res.end_all(); ++iterA, ++iterB) { *iterA %= *iterB; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator % (T     lhs, U content)
	{
		remove_cvr(T) res(lhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter %= content; }
		return res;
	}
	template <typename T, typename U, is_num_vector(T), is_numeric(U)> remove_cvr(T) operator % (U content, T     rhs)
	{
		remove_cvr(T) res(rhs);
		for (auto iter = res.begin_all(); iter != res.end_all(); ++iter) { *iter = T(content) % *iter; }
		return res;
	}
	// operator ==
	template <typename T, typename U, is_vector2(T, U)>                         bool operator ==(T lhs, U rhs)
	{
		if (lhs.DIMFLAG == 1) if (lhs.x_size != rhs.x_size) { return False; }
		if (lhs.DIMFLAG == 2) if (lhs.x_size != rhs.x_size || lhs.y_size != rhs.y_size) { return False; }
		if (lhs.DIMFLAG == 3) if (lhs.x_size != rhs.x_size || lhs.y_size != rhs.y_size || lhs.z_size != rhs.z_size) { return False; }
		if (lhs.DIMFLAG == 4) if (lhs.x_size != rhs.x_size || lhs.y_size != rhs.y_size || lhs.z_size != rhs.z_size || lhs.t_size != rhs.t_size) { return False; }

		for (auto iterA = lhs.begin_all(), iterB = rhs.begin_all(); iterA != lhs.end_all(); ++iterA, ++iterB) { if (*iterA != *iterB) { return False; } }
		return True;
	}
	template <typename T, typename U, is_vector(T), is_numeric(U)>              bool operator ==(T     lhs, U content)
	{
		for (auto iter = lhs.begin_all(); iter != lhs.end_all(); ++iter) { if (*iter != content) { return False; } }
		return True;
	}
	template <typename T, typename U, is_vector(T), is_numeric(U)>              bool operator ==(U content, T     rhs) { return rhs == content; }
	// operator !=
	template <typename T, typename U, is_vector2(T, U)>                         bool operator !=(T lhs, U rhs)
	{
		if (lhs.DIMFLAG == 1) if (lhs.x_size != rhs.x_size) { return True; }
		if (lhs.DIMFLAG == 2) if (lhs.x_size != rhs.x_size || lhs.y_size != rhs.y_size) { return True; }
		if (lhs.DIMFLAG == 3) if (lhs.x_size != rhs.x_size || lhs.y_size != rhs.y_size || lhs.z_size != rhs.z_size) { return True; }
		if (lhs.DIMFLAG == 4) if (lhs.x_size != rhs.x_size || lhs.y_size != rhs.y_size || lhs.z_size != rhs.z_size || lhs.t_size != rhs.t_size) { return True; }

		for (auto iterA = lhs.begin_all(), iterB = rhs.begin_all(); iterA != lhs.end_all(); ++iterA, ++iterB) { if (*iterA != *iterB) { return True; } }
		return False;
	}
	template <typename T, typename U, is_vector(T), is_numeric(U)>              bool operator !=(T     lhs, U content)
	{
		for (auto iter = lhs.begin_all(); iter != lhs.end_all(); ++iter) { if (*iter != content) { return True; } }
		return False;
	}
	template <typename T, typename U, is_vector(T), is_numeric(U)>              bool operator !=(U content, T     rhs) { return rhs != content; }

	template <int DIM, typename T, typename U> typename VectorBase<DIM, T>::iterator operator <<(Vector<DIM, T>& lhs, U rhs)
	{
		auto iter = lhs.begin_all();
		*iter = rhs;
		return ++iter;
	}

#endif

#if FMATH
	/*
	* Fast SinCos from Unreal Engine 4
	*/
	inline void SinCos(float* ScalarSin, float* ScalarCos, const float& Value)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		auto quotient = INV_PI_F * 0.5f*Value;
		if (Value >= 0.0f) { quotient = float(int(quotient + 0.5f)); }
		else { quotient = float(int(quotient - 0.5f)); }
		auto y = Value - 2.0f*PI_F*quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > HALF_PI_F) { y = PI_F - y; sign = -1.0f; }
		else if (y < -HALF_PI_F) { y = -PI_F - y; sign = -1.0f; }
		else { sign = +1.0f; }

		const auto y2 = y * y;

		// 11-degree minimax approximation
		*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		const auto p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*ScalarCos = sign * p;
	}

#endif

#if TINYFFT
	typedef vectori::iterator   iteri_type;
	typedef vectorcd::iterator  itercd_type;
	typedef vector2cd::iterator iter2cd_type;
	typedef vector3cd::iterator iter3cd_type;


	inline complexd FFT_Euler(const double& Value)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		auto quotient = INV_PI_F * 0.5f*Value;
		if (Value >= 0.0f) { quotient = float(int(quotient + 0.5f)); }
		else { quotient = float(int(quotient - 0.5f)); }
		auto y = Value - 2.0f*PI_F*quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > HALF_PI_F) { y = PI_F - y; sign = -1.0f; }
		else if (y < -HALF_PI_F) { y = -PI_F - y; sign = -1.0f; }
		else { sign = +1.0f; }

		const auto y2 = y * y;

		// 11-degree minimax approximation
		const auto Sin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		const auto p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		const auto Cos = sign * p;

		return complexd{ Cos,Sin };
	}

	inline void General_DFT(const iter2cd_type& begin, const int& N)
	{
		vectorcd save(N, complexd{ 0,0 });
		for (auto k = 0; k < N; ++k)
		{
			auto iter = begin;
			for (auto n = 0; n < N; ++n, ++iter) { save(k) += *iter * exp(complexd{ 0,-TPI * n*k / N }); }
		}
		auto iter = begin;
		for (auto k = 0; k < N; ++k, ++iter) { *iter = save(k); }
	}

	/*
	*  Implements the Cooley-Tukey FFT algorithm. [ N1*N2 = N ]
	*/
	template <typename ITER> void FFT_Mixed_radix(const ITER& begin, const int N, const int N1, const int N2, iteri_type Fac, const iteri_type FacEnd)
	{
		auto iter1 = begin, iter2 = begin;
		const auto FLAG = Fac != FacEnd;
		vector2cd TEMP(N2, N1, complexd{ 0,0 });

		for (auto iter = TEMP.begin_all(); iter != TEMP.end_all(); ++iter1, ++iter) { *iter = *iter1; }

		for (auto k1 = 0; k1 < N1; ++k1)
		{
			if (FLAG) { FFT_Mixed_radix(TEMP.begin_y(k1), N2, (*Fac), N2 / *Fac, Fac + 1, FacEnd); }
			else { General_DFT(TEMP.begin_y(k1), N2); }
		}

		for (auto k1 : range(N1))
			for (auto k2 : range(N2))
				//TEMP(k2, k1) *= exp(complexd{ 0,-TPI * k1*k2 / N });
				TEMP(k2, k1) *= FFT_Euler(-TPI * k1*k2 / N);

		for (auto k2 = 0; k2 < N2; ++k2)
			General_DFT(TEMP.begin_x(k2), N1);

		for (auto iter = TEMP.begin_trans(); iter != TEMP.end_trans(); ++iter2, ++iter) { *iter2 = *iter; }
	}

	inline vectori Factors(const int& In)
	{
		vectori save;
		auto N = In, Fac = 3;
		for (; N % 2 == 0; N /= 2) { save.push_back(2); }
		for (; Fac < int(ceil(sqrt(In))); Fac += 2)
		{
			for (; N % Fac == 0; N /= Fac) { save.push_back(Fac); }
		}
		if (N != 1) { save.push_back(N); }
		return save;
	}

	template <typename ITER> void FFT(const ITER begin, const int& size)
	{
		auto Fac = Factors(size);
		FFT_Mixed_radix(begin, size, Fac(0), size / Fac(0), Fac.begin_all() + 1, Fac.end_all());
	}

	inline void FFT(vectorcd& In)
	{
		FFT(In.begin_all(), In.x_size);
	}

	inline void FFT2D(vector2cd& In)
	{
		/*
		* x = 0     | (     0,     0 ) ... (     0, ysize ) | <= FFT
		* x = 1     | (     1,     0 ) ... (     1, ysize ) | <= FFT
		* ...       |       ...        ...        ...       | ...
		* x = xsize | ( xsize,     0 ) ... ( xsize, ysize ) | <= FFT
		*/
		for (auto x : range(In.x_size))
			FFT(In.begin_x(x), In.y_size);

		/*
		* y = 0     | (     0,     0 ) ... (     0, zsize ) | <= FFT
		* y = 1     | (     1,     0 ) ... (     1, zsize ) | <= FFT
		* ...       |       ...        ...        ...       | ...
		* y = ysize | ( ysize,     0 ) ... ( ysize, zsize ) | <= FFT
		*/
		for (auto y : range(In.y_size))
			FFT(In.begin_y(y), In.x_size);
	}

	inline void FFT3D(vector3cd& In)
	{
		for (auto x : range(In.x_size))
			for (auto y : range(In.y_size))
				FFT(In.begin_xy(x, y), In.z_size);

		for (auto x : range(In.x_size))
			for (auto z : range(In.z_size))
				FFT(In.begin_xz(x, z), In.y_size);

		for (auto y : range(In.y_size))
			for (auto z : range(In.z_size))
				FFT(In.begin_yz(y, z), In.x_size);
	}

	inline void SEP2(itercd_type& _iter, const size_t& N)
	{
		vectorcd TMP(N / 2); int i; itercd_type iter, iter2;
		for (i = 0, iter = _iter + 1; i < N / 2; ++i, ++iter, ++iter) { TMP(i) = *iter; }
		for (i = 0, iter2 = iter = _iter; i < N / 2; ++i, ++iter, ++iter2, ++iter2) { *iter = *iter2; }
		for (i = 0, iter = _iter + N / 2; i < N / 2; ++i, ++iter) { *iter = TMP(i); }
	}

	inline void FFT_Radix2(itercd_type& iter, const size_t& N)
	{
		if (N < 2) { return; }

		/* | ------------ | ------------ | */
		auto Start = iter;
		auto Middle = iter + N / 2;
		SEP2(Start, N);
		FFT_Radix2(Start, N / 2);
		FFT_Radix2(Middle, N / 2);

		/* restore */
		for (auto k = 0; k < N / 2; ++k)
		{
			const auto z0 = *(Start + k);
			const auto z1 = *(Middle + k);
			const auto fac = exp(complexd{ 0, -TPI * k / N }) * z1;
			*(Start + k) = z0 + fac;
			*(Middle + k) = z0 - fac;
		}
	}
#endif

}

#endif // YHZHANG_H
