#ifndef YHZHANG_H
#define YHZHANG_H
#include <vector>
#include <boost/spirit/include/qi.hpp>

using namespace std;

inline void stoval(const string& str, double& back)
{
	boost::spirit::qi::parse(str.begin(), str.end(), boost::spirit::qi::double_, back);
}

inline void stoval(const string& str, int& back)
{
	boost::spirit::qi::parse(str.begin(), str.end(), boost::spirit::qi::int_, back);
}

inline double stoval(const string::const_iterator& begin, const string::const_iterator& end, double& back)
{
	boost::spirit::qi::parse(begin, end, boost::spirit::qi::double_, back);
	return back;
}

inline int stoval(const string::const_iterator& begin, const string::const_iterator& end, int&back)
{
	boost::spirit::qi::parse(begin, end, boost::spirit::qi::int_, back);
	return back;
}

inline void split(const string& str, vector<double>& result, double& tmp)
{
	result.clear();
	enum State { Space, Token };
	auto state = Space;

	string::const_iterator TokenBegin;
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		auto const newState = (*it == ' ' ? Space : Token);
		if (newState != state)
		{
			switch (newState)
			{
			case Space:
				result.emplace_back(stoval(TokenBegin, it, tmp));
				break;
			case Token:
				TokenBegin = it;
				break;
			}
		}
		state = newState;
	}
	if (state == Token)
		result.emplace_back(stoval(TokenBegin, str.end(), tmp));
}


inline void split(const string& str, vector<int>& result, int& tmp)
{
	result.clear();
	enum State { Space, Token };
	auto state = Space;

	string::const_iterator TokenBegin;
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		auto const newState = (*it == ' ' ? Space : Token);
		if (newState != state)
		{
			switch (newState)
			{
			case Space:
				result.emplace_back(stoval(TokenBegin, it, tmp));
				break;
			case Token:
				TokenBegin = it;
				break;
			}
		}
		state = newState;
	}
	if (state == Token)
		result.emplace_back(stoval(TokenBegin, str.end(), tmp));
}


inline vector<string> split(const string& str)
{
	vector<string> result;
	enum State { Space, Token };
	auto state = Space;

	auto pTokenBegin = 0;
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		auto const newState = (*it == ' ' ? Space : Token);
		if (newState != state)
		{
			switch (newState)
			{
			case Space:
				result.push_back(str.substr(pTokenBegin, (it - str.begin()) - pTokenBegin));
				break;
			case Token:
				pTokenBegin = it - str.begin();
				break;
			}
		}
		state = newState;
	}
	if (state == Token)
		result.push_back(str.substr(pTokenBegin, str.size() - pTokenBegin - 1));
	return result;
}
#endif // YHZHANG_H
