/**
 * cztodoby
 * https://github.com/ruifig/cztodoby
 *
 * C++ Compile time bomb.
 * The provided macro(s) allow adding TODOs with an expiration date and time. Once that date and time are hit, compilation fails.
 * 
 * The macros you should use are:
 * - CZ_COMPILE_TIMEBOMB, CZ_COMPILE_TIMEBOMB_USER : Allows specifying a custom message
 * - CZ_TODO_BY, CZ_TODO_BY_USER : Uses a generic message
 *
 * All other macros and code in cz::details are for internal use only.
 */

#pragma once

/**
 * By default, time bombs are enabled for all builds.
 * To disable them for certain builds, define the `CZTODOBY_ENABLED` macro as `0` in the corresponding build configuration(s).
 * No need to change this file.
 */
#ifndef CZTODOBY_ENABLED
	#define CZTODOBY_ENABLED 1
#endif

#if CZTODOBY_ENABLED

#include <chrono>
#include <string_view>

/**
 * This is an helper macro, and there is no need to use this directly.
 * Use the other macros instead.
 * 
 * Given a string literal in the same format as __DATE__ and __TIME__ (e.g "Dec  1 2024") it computes at compile time a number
 * in the form YYYYMMDDhhmmss, so it can be used to compare against other timestamps.
 *
 * To make things easier it allows the day to be prefixed with 0, so "Dec 01 2024" is also valid.
 * 
 * When also specified a time, it needs to be in the following format (e.g): "Dec 01 2024 23:59:59".
 *
 * All 3 components are validated (month, day and year), so any typos simply won't compile, such as "Jam 1 2024" or "Jan 33 2024".
 * It also checks for leap years, so "Feb 29 2023" won't compile, since 2023 is not a leap year.
 * The year needs to be in the [1970,9999] range. 
 *
 * For the time, it expects hours to be in [00,23] and minutes/second to be [00,59].
 */
#define CZ_DATE_TO_NUMBER(date) ::cz::details::ConstEvalDate::toNumber( []() { return date; })

// Used internally
#define CZ_COMPILE_TIMEBOMB_IMPL(enabled, expirationDate, msg)                                               \
		static_assert(((enabled)==false) || (CZ_DATE_TO_NUMBER(__DATE__ " " __TIME__) < CZ_DATE_TO_NUMBER(expirationDate)), msg)

/**
 * Inserts a compile time bomb.
 * Once the translation unit's date and time of compilation hits the specified time, it fails to compile.
 *
 * E.g:
 * CZ_COMPILE_TIMEBOMB("Dec 01 2024 15:00:00"); // Once the current date and time hits 1st of Dec 2024 at 3pm, it fails to compile.
 */
#define CZ_COMPILE_TIMEBOMB(expirationDate, msg) \
	CZ_COMPILE_TIMEBOMB_IMPL(false, expirationDate, msg)

/** Shortcut to insert TODOs in the code */
#define CZ_TODO_BY(expirationDate) \
	CZ_COMPILE_TIMEBOMB_IMPL(false, expirationDate, "TODO hit expiration date/time. Please fix!")


/**
 * Same as CZ_COMPILE_TIMEBOMB, but only triggers if the current user name (at compile time) matches the specified user name
 *
 * The username is case insensitive.
 */
#define CZ_COMPILE_TIMEBOMB_USER(username, expirationDate, msg) \
	CZ_COMPILE_TIMEBOMB_IMPL(::cz::details::ConstEvalDate::equalsCi(std::string_view(username), std::string_view(CZTODOBY_USER)), expirationDate, msg)

/**
 * Same as CZ_TODO_BY_USER, but only triggers if the current user name (at compile time) matches the specified user name
 *
 * The username is case insensitive.
 */
#define CZ_TODO_BY_USER(username, expirationDate) \
	CZ_COMPILE_TIMEBOMB_IMPL(::cz::details::ConstEvalDate::equalsCi(std::string_view(username), std::string_view(CZTODOBY_USER)), expirationDate, "TODO hit expiration date/time. Please fix!")

/**
 * Utility functions to parse things.
 */
namespace cz::details::ConstEvalDate
{
	// Checks if the specified indexes are digits
	template<typename... Indexes>
	consteval bool isDigit(const std::string_view str, Indexes... idx)
	{
		return ((str[idx] >= '0' && str[idx] <= '9') && ...);
	}
	// Checks if the specified indexes are all the specified character
	template<typename... Indexes>
	consteval bool isChar(const std::string_view str, char ch, Indexes... idx)
	{
		return ((str[idx] == ch) && ...);
	}

	// Validates if the string is in the format "Mmm dd yyyy" or "Mmm dd yyyy hh:mm:ss"
	// It doesn't validate if things are actually in range. That's the higher level code's reponsability
	consteval bool validateCharacters(const std::string_view date)
	{
		// Mmm dd yyyy hh:mm:ss
		// 01234567890123456789
		bool validDate = isChar(date, ' ', 3u, 6u) && (date[4] == ' ' || isDigit(date, 4u)) && isDigit(date, 5u, 7u, 8u, 9u, 10u);
		bool validTime = (date.size() == 11) ||
			(isChar(date, ' ', 11u) && isChar(date, ':', 14u, 17u) && isDigit(date, 12u, 13u, 15u, 16u, 18u, 19u));
		return (date.size() == 11u || date.size() == 20u) && validDate && validTime;
	}

	// Gets a number with n digits
	consteval unsigned int getNumber(const std::string_view str, unsigned int startIdx, unsigned int n)
	{
		unsigned int res = 0;
		unsigned int mul = 1;
		for(unsigned int i = (startIdx + n - 1); i >= startIdx; i--)
		{
			res += (str[i] - '0') * mul;
			mul *= 10;
		}
		return res;
	}

	consteval bool monthEquals(const std::string_view date, char const (&str)[4])
	{
		return date[0] == str[0] && date[1] == str[1] && date[2] == str[2];
	}

	// Returns the month [1, 12] or 0 if invalid.
	consteval unsigned int getMonth(const std::string_view date)
	{
		return
			monthEquals(date, "Jan")   ?  1u :
			monthEquals(date, "Feb")   ?  2u : 
			monthEquals(date, "Mar")   ?  3u :
			monthEquals(date, "Apr")   ?  4u :
			monthEquals(date, "May")   ?  5u :
			monthEquals(date, "Jun")   ?  6u :
			monthEquals(date, "Jul")   ?  7u :
			monthEquals(date, "Aug")   ?  8u :
			monthEquals(date, "Sep")   ?  9u :
			monthEquals(date, "Oct")   ? 10u :
			monthEquals(date, "Nov")   ? 11u :
			monthEquals(date, "Dec")   ? 12u :
			0;
	}

	// Returns the day of the month.
	// It doesn't validate that is actually a valid day of the month. That's done in higher code 
	consteval unsigned int getDay(const std::string_view date)
	{
		return (date[4] == ' ' ? 0u : (date[4] - '0')) * 10u + (date[5] - '0');
	}

	// Returns the year, but doesn't validate it
	consteval unsigned int getYear(const std::string_view date)
	{
		return getNumber(date, 7, 4); 
	}

	// Validates the full date (year, month and day)
	consteval bool isValidDate(unsigned int year, unsigned int month, unsigned int day)
	{
		unsigned int maxDays =
			(month == 1u || month == 3u || month == 5u || month ==  7u || month == 8u || month == 10u || month == 12u) ? 31u :
			(month == 4u || month == 6u || month == 9u || month == 11u) ? 30u :
			std::chrono::year(static_cast<int>(year)).is_leap() ? 29u : 28u;

		return year >= 1970 && year <= 9999 && month >= 1 && month <= 12 && day >= 1 && day <= maxDays;
	}

	// Validates if its a valid time
	consteval bool isValidTime(unsigned int hours, unsigned int minutes, unsigned int seconds)
	{
		return hours <= 23 && minutes <= 59 && seconds <= 59;
	}

	// Implementation detail:
	// To allow us to use static_asserts, this function accepts a constexpr lambda (>= C++17). If it accepted a string literal
	// directly, we couldn't validate things with static_assert.
	// A decent explanation available at : https://blog.therocode.net/2018/09/compile-time-string-parsing
	template<typename StringHolder>
	consteval uint64_t toNumber(StringHolder holder)
	{
		constexpr std::string_view date = holder();

		// Reject any strings that are not the exact expected format (Mmm dd yyyy)
		static_assert((date.size() == 11 || date.size() == 20) && validateCharacters(date), "Invalid string format");

		constexpr unsigned int year = getYear(date);
		constexpr unsigned int month = getMonth(date);
		constexpr unsigned int day = getDay(date);
		// Mmm dd yyyy hh:mm:ss
		// 01234567890123456789
		constexpr unsigned int hours = date.size() == 11 ? 0 : getNumber(date, 12, 2);
		constexpr unsigned int minutes = date.size() == 11 ? 0 : getNumber(date, 15, 2);
		constexpr unsigned int seconds = date.size() == 11 ? 0 : getNumber(date, 18, 2);
		// Reject any strings that where the combination of the 3 elements are invalid.
		static_assert(isValidDate(year, month, day), "Invalid date");
		static_assert(isValidTime(hours, minutes, seconds), "Invalid time");

		return
			uint64_t(year) * (uint64_t)10000000000 + month * 100000000 + day * 1000000 +
			hours * 10000 + minutes * 100 + seconds;
	}

	// Converts an ASCII letter to lower case
	consteval char chToLower(char ch)
	{
		return (ch>='A' && ch<='Z') ? ch + ('a' - 'A') : ch;
	}

	// Helper function for equalsCi
	template<typename StringHolder>
	consteval bool equalsCi_helper(StringHolder a, StringHolder b)
	{
		bool res = true;
		for(unsigned int i = 0; i<a.size(); i++)
		{
			res = res && (chToLower(a[i]) == chToLower(b[i]));
		}

		return res;
	}

	// Checks if two strings are equal (ignoring case)
	template<typename StringHolder>
	consteval bool equalsCi(StringHolder a, StringHolder b)
	{
		return a.size() == b.size() && equalsCi_helper(a, b);
	}

} // namespace cz::details::ConstEvalDate

#else

#define CZ_COMPILE_TIMEBOMB(expirationDate, msg)
#define CZ_TODO_BY(expirationDate)
#define CZ_TODO_BY_USER(username, expirationDate)

#endif

