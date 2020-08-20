// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TScriptInterpreter, LoadScript)
{
    TString script;
    TScriptInterpreter interpreter(TestDataFactory::GetTestScriptDir());

    // Load script with invalid content
    script = "error1.hs";
    ASSERT_EQ(false, interpreter.LoadScript(script));
    ASSERT_STREQ("Invalid number <0x3t> passed as parameter 2", interpreter.GetLastError());
    ASSERT_STREQ("Checksum (XOR) with errors to test if error handling works", interpreter.ScriptDescription());

    // Load script with valid content
    script = "error2.hs";
    ASSERT_EQ(true, interpreter.LoadScript(script)) << interpreter.GetLastError();
    ASSERT_STREQ("", interpreter.GetLastError());
    ASSERT_STREQ("A plugin that has a very very very long title, so the creation of the Viewer menu can be tested to all extends - really all!", interpreter.ScriptDescription());
}

TEST(TScriptInterpreter, ScriptExecuteAt)
{
    TFile file(TestDataFactory::GetFilesDir() + "test.zip", true);
    TString buffer[HE_SCRIPT_MAX_SCREEN_ROWS];
    TScriptInterpreter interpreter(TestDataFactory::GetScriptDir());

    // Invalid parameter tests
    ASSERT_EQ(0, interpreter.ScriptExecuteAt(nullptr, 0, nullptr));
    ASSERT_EQ(0, interpreter.ScriptExecuteAt(&file, 0, nullptr));
    ASSERT_EQ(0, interpreter.ScriptExecuteAt(nullptr, 0, buffer));
}

TEST(TScriptInterpreter, ScriptExecuteAt_zip_hs)
{
    TFile file(TestDataFactory::GetFilesDir() + "test.zip", true);
    TString script;
    TString buffer[HE_SCRIPT_MAX_SCREEN_ROWS];
    TScriptInterpreter interpreter(TestDataFactory::GetScriptDir());

    // Load script for zip files
    script = "zip.hs";
    ASSERT_EQ(true, interpreter.LoadScript(script)) << interpreter.GetLastError();
    ASSERT_STREQ("", interpreter.GetLastError());
    ASSERT_STREQ("ZIP Archive", interpreter.ScriptDescription());

    // Open the test data and execute the script
    ASSERT_EQ(true, file.Open(TFileMode::READ));
    ASSERT_EQ(5, interpreter.ScriptExecuteAt(&file, 0, buffer));
    ASSERT_STREQ(" ERROR: Invalid cursor y position: Should be between 1 and 90, is 91", buffer[1].ToString());
    file.Close();

    // Debug output for displaying the screen buffer
    //for (const auto& val : buffer) if (!val.IsEmpty()) std::cout << val.ToString() << "\n";
}

TEST(TScriptInterpreter, ScriptExecuteAt_numeric_hs)
{
    TFile file(TestDataFactory::GetFilesDir() + "test.zip", true);
    TString script;
    TString buffer[HE_SCRIPT_MAX_SCREEN_ROWS];
    TScriptInterpreter interpreter(TestDataFactory::GetScriptDir());

    // Load script for numeric values
    script = "numeric.hs";
    ASSERT_EQ(true, interpreter.LoadScript(script)) << interpreter.GetLastError();
    ASSERT_STREQ("", interpreter.GetLastError());
    ASSERT_STREQ("Numeric Value", interpreter.ScriptDescription());

    // Open the test data and execute the script
    ASSERT_EQ(true, file.Open(TFileMode::READ));
    ASSERT_EQ(12, interpreter.ScriptExecuteAt(&file, 0, buffer));
    ASSERT_STREQ(" Hex Value......: 504B0304", buffer[1].ToString());
    ASSERT_STREQ(" Signed dword...: 67324752", buffer[6].ToString());
    ASSERT_STREQ(" Double.........: 2.125322e-313", buffer[11].ToString());
    file.Close();
}

TEST(TScriptInterpreter, ScriptExecuteAt_bmp_hs)
{
    TFile file(TestDataFactory::GetFilesDir() + "test.bmp", true);
    TString script;
    TString buffer[HE_SCRIPT_MAX_SCREEN_ROWS];
    TScriptInterpreter interpreter(TestDataFactory::GetScriptDir());

    // Load script for numeric values
    script = "bmp.hs";
    ASSERT_EQ(true, interpreter.LoadScript(script)) << interpreter.GetLastError();
    ASSERT_STREQ("", interpreter.GetLastError());
    ASSERT_STREQ("Bitmap", interpreter.ScriptDescription());

    // Open the test data and execute the script
    ASSERT_EQ(true, file.Open(TFileMode::READ));
    ASSERT_EQ(5, interpreter.ScriptExecuteAt(&file, 0, buffer));
    ASSERT_STREQ(" Width.......: 64 Pixel", buffer[2].ToString());
    ASSERT_STREQ(" Height......: 64 Pixel", buffer[3].ToString());
    ASSERT_STREQ(" Color depth.: 24 Bit", buffer[4].ToString());
    file.Close();
}
