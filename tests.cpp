#define CZTODOBY_ENABLED 1
#include "crazygaze/TodoBy.h"

//
// The following lines will fail to compile after the specified time stamp.
//
// CZ_COMPILE_TIMEBOMB("Dec 08 2024 00:47:00", "This should have been fixed already.");
// CZ_TODO_BY("Dec 08 2024 00:47:00");
// CZ_TODO_BY("Dec 08 2024"); // If time is missing it assumes 00:00:00

#include <stdio.h>

int main()
{
#if CZTODOBY_ENABLED
	using namespace cz;

	//
	static_assert(details::ConstEvalDate::isDigit("2-1", 0u,2u) == true);
	static_assert(details::ConstEvalDate::isDigit("21-", 0u,2u) == false);
	static_assert(details::ConstEvalDate::isDigit("--1", 0u,2u) == false);
	//
	static_assert(details::ConstEvalDate::isChar("--a-a", 'a', 2u, 4u) == true);
	static_assert(details::ConstEvalDate::isChar("--a-b", 'a', 2u, 4u) == false);
	//
	static_assert(details::ConstEvalDate::getNumber("--12345--", 2u, 5u) == 12345u);
	static_assert(details::ConstEvalDate::getNumber("--1--", 2u, 1u) == 1u);
	//
	static_assert(details::ConstEvalDate::validateCharacters("Dec  2 2024") == true);
	static_assert(details::ConstEvalDate::validateCharacters("Dec 02 2024") == true);
	static_assert(details::ConstEvalDate::validateCharacters("Dec-02 2024") == false);
	static_assert(details::ConstEvalDate::validateCharacters("Dec 02 2024 12:34:56") == true);
	static_assert(details::ConstEvalDate::validateCharacters("Dec 02 2024 12:34 56") == false);

	static_assert(details::ConstEvalDate::getDay("---  2 ----") == 2);
	static_assert(details::ConstEvalDate::getDay("--- 02 ----") == 2);
	static_assert(details::ConstEvalDate::getDay("--- 40 ----") == 40); // This function returns the number, but doesn't validate it

	static_assert(details::ConstEvalDate::getMonth("Jan -- ----") == 1);
	static_assert(details::ConstEvalDate::getMonth("Feb -- ----") == 2);
	static_assert(details::ConstEvalDate::getMonth("Mar -- ----") == 3);
	static_assert(details::ConstEvalDate::getMonth("Apr -- ----") == 4);
	static_assert(details::ConstEvalDate::getMonth("May -- ----") == 5);
	static_assert(details::ConstEvalDate::getMonth("Jun -- ----") == 6);
	static_assert(details::ConstEvalDate::getMonth("Jul -- ----") == 7);
	static_assert(details::ConstEvalDate::getMonth("Aug -- ----") == 8);
	static_assert(details::ConstEvalDate::getMonth("Sep -- ----") == 9);
	static_assert(details::ConstEvalDate::getMonth("Oct -- ----") == 10);
	static_assert(details::ConstEvalDate::getMonth("Nov -- ----") == 11);
	static_assert(details::ConstEvalDate::getMonth("Dec -- ----") == 12);
	static_assert(details::ConstEvalDate::getMonth("Dee -- ----") == 0);

	static_assert(details::ConstEvalDate::getYear("--- -- 0000") == 0);
	static_assert(details::ConstEvalDate::getYear("--- -- 1234") == 1234);

	// Test some of the helper code (2024 is leap year)
	static_assert(details::ConstEvalDate::isValidDate(2024, 1, 31)  == true);   // January
	static_assert(details::ConstEvalDate::isValidDate(2023, 2, 29)  == false);  // February (non-leap year)
	static_assert(details::ConstEvalDate::isValidDate(2024, 2, 29)  == true);   // February (leap year)
	static_assert(details::ConstEvalDate::isValidDate(2024, 3, 31)  == true);   // March
	static_assert(details::ConstEvalDate::isValidDate(2024, 4, 31)  == false);  // April
	static_assert(details::ConstEvalDate::isValidDate(2024, 5, 31)  == true);   // May
	static_assert(details::ConstEvalDate::isValidDate(2024, 6, 31)  == false);  // June
	static_assert(details::ConstEvalDate::isValidDate(2024, 7, 31)  == true);   // July
	static_assert(details::ConstEvalDate::isValidDate(2024, 8, 31)  == true);   // August
	static_assert(details::ConstEvalDate::isValidDate(2024, 9, 31)  == false);  // September
	static_assert(details::ConstEvalDate::isValidDate(2024, 10, 31) == true);   // October
	static_assert(details::ConstEvalDate::isValidDate(2024, 11, 31) == false);  // November
	static_assert(details::ConstEvalDate::isValidDate(2024, 12, 31) == true);   // December
	//
	static_assert(details::ConstEvalDate::isValidDate(1969, 1, 1) == false);
	static_assert(details::ConstEvalDate::isValidDate(1970, 1, 1) == true);
	static_assert(details::ConstEvalDate::isValidDate(9999, 1, 1) == true);
	static_assert(details::ConstEvalDate::isValidDate(10000, 1, 1) == false);
	static_assert(details::ConstEvalDate::isValidDate(2024, 12, 40) == false);

	static_assert(details::ConstEvalDate::equalsCi(std::string_view("Rui"), std::string_view("Rui")) == true);
	static_assert(details::ConstEvalDate::equalsCi(std::string_view("rUi"), std::string_view("Rui")) == true); // Make sure it's case insensitive
	static_assert(details::ConstEvalDate::equalsCi(std::string_view("aa"), std::string_view("aaa")) == false); // make sure it deals with different sizes
	static_assert(details::ConstEvalDate::equalsCi(std::string_view("aa"), std::string_view("ab")) == false);



	// Test day parsing
	static_assert(CZ_DATE_TO_NUMBER("Dec  2 2024") == 20241202000000);
	static_assert(CZ_DATE_TO_NUMBER("Dec 02 2024") == 20241202000000);
	static_assert(CZ_DATE_TO_NUMBER("Dec 12 2024") == 20241212000000);
	static_assert(CZ_DATE_TO_NUMBER("Dec 23 2024") == 20241223000000);

	// Test month parsing
	static_assert(CZ_DATE_TO_NUMBER("Jan 23 2024") == 20240123000000);
	static_assert(CZ_DATE_TO_NUMBER("Feb 23 2024") == 20240223000000);
	static_assert(CZ_DATE_TO_NUMBER("Mar 23 2024") == 20240323000000);
	static_assert(CZ_DATE_TO_NUMBER("Apr 23 2024") == 20240423000000);
	static_assert(CZ_DATE_TO_NUMBER("May 23 2024") == 20240523000000);
	static_assert(CZ_DATE_TO_NUMBER("Jun 23 2024") == 20240623000000);
	static_assert(CZ_DATE_TO_NUMBER("Jul 23 2024") == 20240723000000);
	static_assert(CZ_DATE_TO_NUMBER("Aug 23 2024") == 20240823000000);
	static_assert(CZ_DATE_TO_NUMBER("Sep 23 2024") == 20240923000000);
	static_assert(CZ_DATE_TO_NUMBER("Oct 23 2024") == 20241023000000);
	static_assert(CZ_DATE_TO_NUMBER("Nov 23 2024") == 20241123000000);
	static_assert(CZ_DATE_TO_NUMBER("Dec 23 2024") == 20241223000000);

	// test year parsing
	static_assert(CZ_DATE_TO_NUMBER("Dec 01 1970") == 19701201000000);
	static_assert(CZ_DATE_TO_NUMBER("Dec 01 2999") == 29991201000000);

	// test time parsing
	static_assert(CZ_DATE_TO_NUMBER("Dec 01 1970 12:34:56") == 19701201123456);

	// Test some comparisons
	static_assert(CZ_DATE_TO_NUMBER("Dec 01 1970 12:34:56") < CZ_DATE_TO_NUMBER("Dec 01 1970 12:34:57"));
#endif

	// These will not trigger any time soon :)
	CZ_TODO_BY("Jan 01 3000");
	CZ_COMPILE_TIMEBOMB("Jan 01 3000", "Custom message");

	// These should not trigger, because the username doesn't match the user compiling the code
	CZ_TODO_BY_USER("_Rui_invalidname", "Dec 16 1999");
	CZ_COMPILE_TIMEBOMB_USER("_Rui_invalidname", "Dec 16 1999", "Custom message");

	printf("Compilation date and time: %s %s\n", __DATE__, __TIME__);

	return 0;
}


