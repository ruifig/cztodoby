/**
 * cztodoby
 * https://github.com/ruifig/cztodoby
 *
 * C++ Compile time bomb.
 * The provided macro(s) allow adding TODOs with an expiration date and time. Once that date and time are hit, compilation fails.
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

/**
 * Inserts a compile time bomb.
 * Once the translation unit's date and time of compilation hits the specified time, it fails to compile.
 *
 * E.g:
 * CZ_COMPILE_TIMEBOMB("Dec 01 2024 15:00:00"); // Once the current date and time hits 1st of Dec 2024 at 3pm, it fails to compile.
 */
#define CZ_COMPILE_TIMEBOMB(expirationDate, msg)                     \
	static_assert(CZ_DATE_TO_NUMBER(__DATE__ " " __TIME__) < CZ_DATE_TO_NUMBER(expirationDate), msg)

/** Shortcut to insert TODOs in the code */
#define CZ_TODO_BY(expirationDate) CZ_COMPILE_TIMEBOMB(expirationDate, "TODO hit expiration date/time. Please fix!")

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
		bool validDate = isChar(date, ' ', 3, 6) && (date[4] == ' ' || isDigit(date, 4)) && isDigit(date, 5, 7, 8, 9, 10);
		bool validTime = (date.size() == 11) ||
			(isChar(date, ' ', 11) && isChar(date, ':', 14, 17) && isDigit(date, 12, 13, 15, 16, 18, 19));
		return (date.size() == 11 || date.size() == 20) && validDate && validTime;
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
			monthEquals(date, "Jan")   ?  1 :
			monthEquals(date, "Feb")   ?  2 : 
			monthEquals(date, "Mar")   ?  3 :
			monthEquals(date, "Apr")   ?  4 :
			monthEquals(date, "May")   ?  5 :
			monthEquals(date, "Jun")   ?  6 :
			monthEquals(date, "Jul")   ?  7 :
			monthEquals(date, "Aug")   ?  8 :
			monthEquals(date, "Sep")   ?  9 :
			monthEquals(date, "Oct")   ? 10 :
			monthEquals(date, "Nov")   ? 11 :
			monthEquals(date, "Dec")   ? 12 :
			0;
	}

	// Returns the day of the month.
	// It doesn't validate that is actually a valid day of the month. That's done in higher code 
	consteval unsigned int getDay(const std::string_view date)
	{
		return (date[4] == ' ' ? 0 : (date[4] - '0')) * 10 + (date[5] - '0');
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
			(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) ? 31 :
			(month == 4 || month == 6 || month == 9 || month == 11) ? 30 :
			std::chrono::year(year).is_leap() ? 29 : 28;

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

} // namespace cz::details::ConstEvalDate

#else

#define CZ_COMPILE_TIMEBOMB(expirationDate, msg)
#define CZ_TODO_BY(expirationDate)

#endif



