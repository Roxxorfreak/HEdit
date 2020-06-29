// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TString, DefaultConstructor)
{
    TString* string_ptr = nullptr;

    // create string
    ASSERT_NO_THROW(string_ptr = new TString());
    // delete string
    ASSERT_NO_THROW(delete string_ptr);
}

TEST(TString, ParameterizedConstructor)
{
    TString* string_ptr = nullptr;

    // create string (with parameters)
    ASSERT_NO_THROW(string_ptr = new TString(10, 'x'));
    // string must contain 10 x chars
    ASSERT_STREQ(string_ptr->operator const char* (), "xxxxxxxxxx");
    // delete string
    ASSERT_NO_THROW(delete string_ptr);

    // constructor with invalid size (too big)
    string_ptr = nullptr;
    // create string
    ASSERT_THROW(string_ptr = new TString(65535, 'x'), std::invalid_argument);

    // constructor with invalid size (too small)
    string_ptr = nullptr;
    // Create string
    ASSERT_THROW(string_ptr = new TString(static_cast<std::size_t>(-42), 'x'), std::invalid_argument);

    // constructor out of memory
    New.ShouldFail();
    ASSERT_THROW(new TString(10, 'x'), std::bad_alloc);
    New.ShouldNotFail();
}

TEST(TString, CopyConstructor1)
{
    TString* string_ptr = nullptr;

    // create string
    ASSERT_NO_THROW(string_ptr = new TString("Test"));
    // string must contain "Test"
    ASSERT_STREQ(string_ptr->operator const char* (), "Test");
    // delete string
    ASSERT_NO_THROW(delete string_ptr);

    // create string
    ASSERT_NO_THROW(string_ptr = new TString(nullptr));
    // string must be nullptr
    ASSERT_STREQ(string_ptr->operator const char* (), nullptr);
    // delete string
    ASSERT_NO_THROW(delete string_ptr);

    // create string (out of memory)
    New.ShouldFail();
    ASSERT_THROW(new TString("Test"), std::bad_alloc);
    New.ShouldNotFail();
}

TEST(TString, CopyConstructor2)
{
    TString string1("Test");
    TString* string_ptr = nullptr;

    // create string
    ASSERT_NO_THROW(string_ptr = new TString(string1));
    // string must contain "Test"
    ASSERT_STREQ(string_ptr->operator const char* (), "Test");
    // delete string
    ASSERT_NO_THROW(delete string_ptr);

    // create string (out of memory)
    New.ShouldFail();
    ASSERT_THROW(new TString(string1), std::bad_alloc);
    New.ShouldNotFail();
}

TEST(TString, AssignmentOperator)
{
    TString string1;
    TString string2;

    // assign nullptr
    ASSERT_NO_THROW(string1 = nullptr);
    ASSERT_STREQ(string1.operator const char* (), nullptr);
    // assign empty string
    ASSERT_NO_THROW(string1 = "");
    ASSERT_STREQ(string1.operator const char* (), "");
    // assign text
    ASSERT_NO_THROW(string1 = "Test");
    ASSERT_STREQ(string1.operator const char* (), "Test");
    // assign TString
    ASSERT_NO_THROW(string2 = string1);
    ASSERT_STREQ(string2.operator const char* (), "Test");
    // chained assignment
    ASSERT_NO_THROW(string2 = string1 = "Hallo");
    ASSERT_STREQ(string2.operator const char* (), "Hallo");

    // create string (out of memory)
    New.ShouldFail();
    ASSERT_THROW(string1 = "Test", std::bad_alloc);
    New.ShouldNotFail();
}

TEST(TString, AddOperator)
{
    TString string1;
    TString string2;

    // unitialized string
    ASSERT_NO_THROW(string1 += nullptr);
    ASSERT_STREQ(string1.operator const char* (), nullptr);
    // add nullptr to empty string
    string1 = "";
    ASSERT_NO_THROW(string1 += nullptr);
    ASSERT_STREQ(string1.operator const char* (), "");
    // add char to empty string
    ASSERT_NO_THROW(string1 += 't');
    ASSERT_STREQ(string1.operator const char* (), "t");
    // add string
    ASSERT_NO_THROW(string1 += "Hallo");
    ASSERT_STREQ(string1.operator const char* (), "tHallo");
    // chained assignment
    string2 = "String2";
    ASSERT_NO_THROW(string1 += string2 += "Test");
    ASSERT_STREQ(string1.operator const char* (), "tHalloString2Test");
    // add another char
    ASSERT_NO_THROW(string1 += 'u');
    ASSERT_STREQ(string1.operator const char* (), "tHalloString2Testu");

    // append string (out of memory)
    New.ShouldFail();
    ASSERT_THROW(string1 += "Test", std::bad_alloc);
    New.ShouldNotFail();
}

TEST(TString, PlusOperator)
{
    TString string1;
    TString string2;
    TString string3;

    // unitialized string
    ASSERT_NO_THROW(string2 = string1 + nullptr);
    ASSERT_STREQ(string2.operator const char* (), nullptr);
    // add nullptr to empty string
    string1 = "";
    ASSERT_NO_THROW(string2 = string1 + nullptr);
    ASSERT_STREQ(string2.operator const char* (), "");
    // add char to empty string
    ASSERT_NO_THROW(string2 = string1 + "t");
    ASSERT_STREQ(string2.operator const char* (), "t");
    // add string
    ASSERT_NO_THROW(string1 = string2 + "Hallo");
    ASSERT_STREQ(string1.operator const char* (), "tHallo");
    // add string
    ASSERT_NO_THROW(string3 = string1 + string2);
    ASSERT_STREQ(string3.operator const char* (), "tHallot");

    // append strings (out of memory)
    New.ShouldFail();
    ASSERT_THROW(string2 = string1 + "Test", std::bad_alloc);
    New.ShouldNotFail();
}

TEST(TString, BracketOperator)
{
    char c;
    TString string1;
    TString string2;

    // unitialized string
    ASSERT_THROW(c = string1[0u], std::invalid_argument);
    // out of bounds
    string1.New(5, 0);
    ASSERT_THROW(c = string1[6u], std::out_of_range);
    // last char
    ASSERT_EQ(c = string1[5u], 0);

    // assign data
    // no string yet
    ASSERT_THROW(string2[0u] = 'A', std::invalid_argument);
    // works
    string2.New(5, 0);
    ASSERT_NO_THROW(string2[5u] = 'A');
    ASSERT_EQ(c = string2[5u], 'A');
    // out of bounds
    ASSERT_THROW(string2[6u] = 'B', std::out_of_range);

    // assign from one string to other
    ASSERT_NO_THROW(string1[0u] = string2[5u]);

    // assign from same string
    ASSERT_NO_THROW(string1[1u] = string1[0u]);

    ASSERT_STREQ(string1.operator const char* (), "AA");
}

TEST(TString, SubString)
{
    TString string1;
    TString string2;

    // unitialized string
    ASSERT_STREQ(string1.SubString(2), "");
    // out of bounds
    string1 = "Test";
    ASSERT_STREQ(string1.SubString(5), "");

    // works (middle)
    ASSERT_NO_THROW(string2 = string1.SubString(1, 2));
    ASSERT_STREQ(string2, "es");

    // works (end)
    ASSERT_NO_THROW(string2 = string1.SubString(2));
    ASSERT_STREQ(string2, "st");
}

TEST(TString, ToLower)
{
    TString string1;
    TString string2;

    // unitialized string
    ASSERT_NO_THROW(string1.ToLower());

    // get data
    string1 = "HAllo '%' Welt!";
    // works
    ASSERT_NO_THROW(string2 = string1.ToLower());
    ASSERT_STREQ(string1, "hallo '%' welt!");
    ASSERT_STREQ(string2, "hallo '%' welt!");
}

TEST(TString, ToUpper)
{
    TString string1;
    TString string2;

    // unitialized string
    ASSERT_NO_THROW(string1.ToUpper());

    // get data
    string1 = "HAllo '%' Welt!";
    // works
    ASSERT_NO_THROW(string2 = string1.ToUpper());
    ASSERT_STREQ(string1, "HALLO '%' WELT!");
    ASSERT_STREQ(string2, "HALLO '%' WELT!");
}

TEST(TString, Trim)
{
    TString string1;
    TString string2;

    // unitialized string
    ASSERT_NO_THROW(string1.Trim());
    ASSERT_STREQ(string1, nullptr);

    // use data
    string1 = "\t \tHallo\tWelt\t  ";
    // works
    ASSERT_NO_THROW(string2 = string1.Trim());
    ASSERT_STREQ(string1, "Hallo\tWelt");
    ASSERT_STREQ(string2, "Hallo\tWelt");
}

TEST(TString, New)
{
    TString string1;

    // new strings (without filler)
    ASSERT_STREQ(string1.New(0), "");
    ASSERT_STREQ(string1.New(1), "");
    ASSERT_STREQ(string1.New(2), "");

    // new strings (with filler)
    ASSERT_STREQ(string1.New(4, 't'), "tttt");
    ASSERT_STREQ(string1.New(10, 0x37), "7777777777");
}

TEST(TString, Length)
{
    TString string1;

    // empty strings
    ASSERT_EQ(string1.Length(), static_cast<std::size_t>(0));
    string1 = "";
    ASSERT_EQ(string1.Length(), static_cast<std::size_t>(0));
    // non-empty strings
    string1 = "Test";
    ASSERT_EQ(string1.Length(), static_cast<std::size_t>(4));
    string1 = "42t000dh0xvh049hf3408htg094tht439h";
    ASSERT_EQ(string1.Length(), strlen(string1.ToString()));
}

TEST(TString, IsEmpty)
{
    TString string1;

    // empty strings
    ASSERT_TRUE(string1.IsEmpty());
    string1 = "";
    ASSERT_TRUE(string1.IsEmpty());
    // non-empty strings
    string1 = "Test";
    ASSERT_FALSE(string1.IsEmpty());
    string1 = "\t ";
    string1.Trim();
    ASSERT_TRUE(string1.IsEmpty());
}

TEST(TString, Equals)
{
    TString string1;

    // empty strings
    ASSERT_FALSE(string1.Equals(""));
    ASSERT_FALSE(string1.Equals("", 1));
    ASSERT_FALSE(string1.Equals(nullptr));
    ASSERT_FALSE(string1.Equals(nullptr, 1));
    string1 = "";
    ASSERT_TRUE(string1.Equals(""));
    ASSERT_FALSE(string1.Equals("1"));
    // non-empty strings
    string1 = "Test";
    ASSERT_TRUE(string1.Equals("Test"));
    ASSERT_FALSE(string1.Equals("test"));
    ASSERT_TRUE(string1.Equals("Test", 5));
    ASSERT_TRUE(string1.Equals("Test", 4));
    ASSERT_TRUE(string1.Equals("Tes", 3));
    ASSERT_TRUE(string1.Equals("Te", 2));
    ASSERT_TRUE(string1.Equals("T", 1));
    ASSERT_FALSE(string1.Equals("e", 1));
    ASSERT_FALSE(string1.Equals(""));
    ASSERT_FALSE(string1.Equals(nullptr));
    ASSERT_TRUE(string1.Equals("Test", 42));
    string1 = "a\t b";
    ASSERT_TRUE(string1.Equals("a\t b"));
    ASSERT_TRUE(string1.Equals("a", 1));
    ASSERT_TRUE(string1.Equals("a\t b"));
    ASSERT_FALSE(string1.Equals("a\t B"));
    ASSERT_FALSE(string1.Equals("a\t B", 4));
}

TEST(TString, EqualsCI)
{
    TString string1;

    // empty strings
    ASSERT_FALSE(string1.EqualsCI(""));
    ASSERT_FALSE(string1.EqualsCI("", 1));
    ASSERT_FALSE(string1.EqualsCI(nullptr));
    ASSERT_FALSE(string1.EqualsCI(nullptr, 1));
    string1 = "";
    ASSERT_TRUE(string1.EqualsCI(""));
    ASSERT_FALSE(string1.EqualsCI("1"));
    // non-empty strings
    string1 = "Test";
    ASSERT_TRUE(string1.EqualsCI("Test"));
    ASSERT_TRUE(string1.EqualsCI("test"));
    ASSERT_TRUE(string1.EqualsCI("TEST"));
    ASSERT_TRUE(string1.EqualsCI("TeSt", 5));
    ASSERT_TRUE(string1.EqualsCI("TEst", 4));
    ASSERT_TRUE(string1.EqualsCI("TeS", 3));
    ASSERT_TRUE(string1.EqualsCI("TE", 2));
    ASSERT_TRUE(string1.EqualsCI("t", 1));
    ASSERT_FALSE(string1.EqualsCI("e", 1));
    ASSERT_FALSE(string1.EqualsCI(""));
    ASSERT_FALSE(string1.EqualsCI(nullptr));
    ASSERT_TRUE(string1.EqualsCI("Test", 42));
    string1 = "a\t b";
    ASSERT_TRUE(string1.EqualsCI("a\t b"));
    ASSERT_TRUE(string1.EqualsCI("a", 1));
    ASSERT_TRUE(string1.EqualsCI("a\t b"));
    ASSERT_TRUE(string1.EqualsCI("a\t B"));
    ASSERT_TRUE(string1.EqualsCI("a\t B", 4));
}

TEST(TString, InsertAt)
{
    TString string1;

    // empty string
    ASSERT_NO_THROW(string1.InsertAt(10, 'a'));
    ASSERT_STREQ(string1, "          a");

    // non-empty string
    string1 = "a";
    ASSERT_NO_THROW(string1.InsertAt(1, 'b'));
    ASSERT_STREQ(string1, "ab");
    ASSERT_NO_THROW(string1.InsertAt(1, 'c'));
    ASSERT_STREQ(string1, "ac");
    ASSERT_NO_THROW(string1.InsertAt(0, "def"));
    ASSERT_STREQ(string1, "def");
    ASSERT_NO_THROW(string1.InsertAt(1, 'g'));
    ASSERT_STREQ(string1, "dgf");
    ASSERT_NO_THROW(string1.InsertAt(4, "h!"));
    ASSERT_STREQ(string1, "dgf h!");
}

TEST(TString, EndsWith)
{
    TString string1;
    TString string2;

    // unitialized strings
    ASSERT_FALSE(string1.EndsWith(nullptr));
    ASSERT_FALSE(string1.EndsWith(string2));

    // initialized string (empty)
    string1 = "";
    ASSERT_FALSE(string1.EndsWith(nullptr));
    ASSERT_FALSE(string1.EndsWith(string2));
    ASSERT_FALSE(string1.EndsWith("Homes"));

    // initialized string (data)
    string1 = "Home";
    ASSERT_FALSE(string1.EndsWith(nullptr));
    ASSERT_FALSE(string1.EndsWith(string2));
    ASSERT_FALSE(string1.EndsWith("Homes"));

    // both strings initialized
    string2 = "me";
    ASSERT_TRUE(string1.EndsWith(string2));
    ASSERT_TRUE(string1.EndsWith("ome"));
    ASSERT_FALSE(string1.EndsWith("HOME"));
    ASSERT_TRUE(string1.EndsWith("HOME", false));
    ASSERT_TRUE(string1.EndsWith("OME", false));
    ASSERT_TRUE(string1.EndsWith("ME", false));
    ASSERT_TRUE(string1.EndsWith("E", false));
    ASSERT_FALSE(string1.EndsWith("E", true));
}

TEST(TString, IndexOf)
{
    TString string1;

    // unitialized string
    ASSERT_EQ(string1.IndexOf(0), -1);
    ASSERT_EQ(string1.IndexOf(' '), -1);

    // initialized string
    string1 = "Ho me";
    ASSERT_EQ(string1.IndexOf(0), -1);
    ASSERT_EQ(string1.IndexOf(' '), 2);
    ASSERT_EQ(string1.IndexOf('H'), 0);
    ASSERT_EQ(string1.IndexOf('h'), -1);
}

TEST(TString, StripChar)
{
    TString string1;

    // unitialized string
    ASSERT_NO_THROW(string1.StripChar(0));
    ASSERT_NO_THROW(string1.StripChar(' '));

    // initialized string (empty)
    string1 = "";
    ASSERT_NO_THROW(string1.StripChar(0));
    ASSERT_NO_THROW(string1.StripChar(' '));

    // initialized string (data)
    string1 = "\t\t Test Data ";
    ASSERT_NO_THROW(string1.StripChar(0));
    ASSERT_STREQ(string1, "\t\t Test Data ");
    ASSERT_NO_THROW(string1.StripChar(' '));
    ASSERT_STREQ(string1, "\t\tTestData");
    ASSERT_NO_THROW(string1.StripChar('\t'));
    ASSERT_STREQ(string1, "TestData");
    ASSERT_NO_THROW(string1.StripChar('a'));
    ASSERT_STREQ(string1, "TestDt");

    // initialized string (pattern)
    string1 = "xxxxxxxxxxx";
    ASSERT_NO_THROW(string1.StripChar('x'));
    ASSERT_STREQ(string1, "");
}

TEST(TString, RemoveRight)
{
    TString string1;

    // unitialized string
    ASSERT_NO_THROW(string1.RemoveRight(0));
    ASSERT_NO_THROW(string1.RemoveRight(1));

    // initialized string (empty)
    string1 = "";
    ASSERT_NO_THROW(string1.RemoveRight(0));
    ASSERT_NO_THROW(string1.RemoveRight(1));

    // initialized string (data)
    string1 = "\t\t Test Data ";
    ASSERT_NO_THROW(string1.RemoveRight(0));
    ASSERT_STREQ(string1, "\t\t Test Data ");
    ASSERT_NO_THROW(string1.RemoveRight(1));
    ASSERT_STREQ(string1, "\t\t Test Data");
    ASSERT_NO_THROW(string1.RemoveRight(2));
    ASSERT_STREQ(string1, "\t\t Test Da");
    ASSERT_NO_THROW(string1.RemoveRight(8));
    ASSERT_STREQ(string1, "\t\t");
    ASSERT_NO_THROW(string1.RemoveRight(3));
    ASSERT_STREQ(string1, "\t\t");
    ASSERT_NO_THROW(string1.RemoveRight(2));
    ASSERT_STREQ(string1, "");
}

TEST(TString, ParseDec)
{
    bool success = false;
    TString string1;

    // unitialized string
    ASSERT_EQ(string1.ParseDec(&success), 0);
    ASSERT_FALSE(success);

    // initialized string (empty)
    string1 = "";
    ASSERT_EQ(string1.ParseDec(&success), 0);
    ASSERT_TRUE(success);

    // initialized string (data)
    string1 = "27";
    ASSERT_EQ(string1.ParseDec(&success), 27);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), 27);
    string1 = "42!";
    ASSERT_EQ(string1.ParseDec(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseDec(), 0);
    string1 = "-3g";
    ASSERT_EQ(string1.ParseDec(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseDec(), 0);
    string1 = "-3";
    ASSERT_EQ(string1.ParseDec(&success), -3);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), -3);
    string1 = "+5";
    ASSERT_EQ(string1.ParseDec(&success), 5);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), 5);
    string1 = "7.0";
    ASSERT_EQ(string1.ParseDec(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseDec(), 0);
    string1 = "7,0";
    ASSERT_EQ(string1.ParseDec(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseDec(), 0);

    // big numbers (valid)
    string1 = "9223372036854775807";
    ASSERT_EQ(string1.ParseDec(&success), 9223372036854775807ll);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), 9223372036854775807ll);
    string1 = "-9223372036854775808";
    ASSERT_EQ(string1.ParseDec(&success), -9223372036854775808ll);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), -9223372036854775808ll);

    // big numbers (invalid)
    string1 = "9223372036854775808";
    ASSERT_EQ(string1.ParseDec(&success), INT64_MAX);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), INT64_MAX);
    string1 = "-9223372036854775809";
    ASSERT_EQ(string1.ParseDec(&success), INT64_MIN);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), INT64_MIN);
    string1 = "92233720368547758070";
    ASSERT_EQ(string1.ParseDec(&success), INT64_MAX);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), INT64_MAX);
    string1 = "-92233720368547758080";
    ASSERT_EQ(string1.ParseDec(&success), INT64_MIN);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseDec(), INT64_MIN);
}

TEST(TString, ParseHex)
{
    bool success = false;
    TString string1;

    // unitialized string
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_FALSE(success);

    // initialized string (empty)
    string1 = "";
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_TRUE(success);

    // initialized string (data)
    string1 = "1B";
    ASSERT_EQ(string1.ParseHex(&success), 27);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseHex(), 27);
    string1 = "2A!";
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseHex(), 0);
    string1 = "FFFFFFFFFFFFFFFDg";
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseHex(), 0);
    string1 = "FFFFFFFFFFFFFFFD";
    ASSERT_EQ(string1.ParseHex(&success), -3);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseHex(), -3);
    string1 = "5";
    ASSERT_EQ(string1.ParseHex(&success), 5);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseHex(), 5);
    string1 = "07.0";
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseHex(), 0);
    string1 = "07,0";
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseHex(), 0);
    string1 = "0x02";
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseHex(), 0);
    string1 = "0g";
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseHex(), 0);

    // big numbers (valid)
    string1 = "7FFFFFFFFFFFFFFF";
    ASSERT_EQ(string1.ParseHex(&success), 9223372036854775807ll);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseHex(), 9223372036854775807ll);
    string1 = "FFFFFFFFFFFFFFFF";
    ASSERT_EQ(string1.ParseHex(&success), -1);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseHex(), -1);

    // big numbers (too long)
    string1 = "7FFFFFFFFFFFFFFF0";
    ASSERT_EQ(string1.ParseHex(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseHex(), 0);
}

TEST(TString, ParseBin)
{
    bool success = false;
    TString string1;

    // unitialized string
    ASSERT_EQ(string1.ParseBin(&success), 0);
    ASSERT_FALSE(success);

    // initialized string (empty)
    string1 = "";
    ASSERT_EQ(string1.ParseBin(&success), 0);
    ASSERT_TRUE(success);

    // initialized string (data)
    string1 = "11011";
    ASSERT_EQ(string1.ParseBin(&success), 27);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseBin(), 27);
    string1 = "01!";
    ASSERT_EQ(string1.ParseBin(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseBin(), 0);
    string1 = "011010111000101g";
    ASSERT_EQ(string1.ParseBin(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseBin(), 0);
    string1 = "1111111111111111111111111111111111111111111111111111111111111101";
    ASSERT_EQ(string1.ParseBin(&success), -3);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseBin(), -3);
    string1 = "101";
    ASSERT_EQ(string1.ParseBin(&success), 5);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseBin(), 5);
    string1 = "0b01";
    ASSERT_EQ(string1.ParseBin(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseBin(), 0);
    string1 = "0g";
    ASSERT_EQ(string1.ParseBin(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseBin(), 0);

    // big numbers (valid)
    string1 = "111111111111111111111111111111111111111111111111111111111111111";
    ASSERT_EQ(string1.ParseBin(&success), 9223372036854775807ll);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseBin(), 9223372036854775807ll);
    string1 = "0111111111111111111111111111111111111111111111111111111111111111";
    ASSERT_EQ(string1.ParseBin(&success), 9223372036854775807ll);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseBin(), 9223372036854775807ll);
    string1 = "1111111111111111111111111111111111111111111111111111111111111111";
    ASSERT_EQ(string1.ParseBin(&success), -1);
    ASSERT_TRUE(success);
    ASSERT_EQ(string1.ParseBin(), -1);

    // big numbers (too long)
    string1 = "01111111111111111111111111111111111111111111111111111111111111111";
    ASSERT_EQ(string1.ParseBin(&success), 0);
    ASSERT_FALSE(success);
    ASSERT_EQ(string1.ParseBin(), 0);
}

TEST(TString, BinaryFromInt)
{
    TString string1;
    TString string2;

    // test values
    auto val1 = static_cast<int64_t>(1); // "00000001"
    auto val2 = static_cast<int64_t>(65535); // "1111111111111111"
    auto val3 = static_cast<int64_t>(65536); // "10000000000000000"
    auto val4 = static_cast<int64_t>(-9223372036854775806); // "1000000000000000000000000000000000000000000000000000000000000010"
    auto val5 = static_cast<int64_t>(4984465465468465464); // "0100010100101100011000001110111010011000011100110011000100111000"

    // convert data
    ASSERT_STREQ(string1.BinaryFromInt(val1, 8), "00000001");
    ASSERT_STREQ(string1.BinaryFromInt(val2, 16), "1111111111111111");
    ASSERT_STREQ(string1.BinaryFromInt(val3, 16), "0000000000000000");
    ASSERT_STREQ(string1.BinaryFromInt(val3, 17), "10000000000000000");
    ASSERT_STREQ(string1.BinaryFromInt(val4, 3), "010");
    ASSERT_STREQ(string1.BinaryFromInt(val4, 64), "1000000000000000000000000000000000000000000000000000000000000010");
    ASSERT_STREQ(string1.BinaryFromInt(val5, 64), "0100010100101100011000001110111010011000011100110011000100111000");

    // convert back
    ASSERT_EQ(string1.BinaryFromInt(val1, 32).ParseBin(), val1);
    ASSERT_EQ(string1.BinaryFromInt(val2, 32).ParseBin(), val2);
    ASSERT_EQ(string1.BinaryFromInt(val3, 32).ParseBin(), val3);
    ASSERT_EQ(string1.BinaryFromInt(val4, 64).ParseBin(), val4);
    ASSERT_EQ(string1.BinaryFromInt(val5, 64).ParseBin(), val5);
}
