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

TEST(TScriptInterpreter, ExecuteScriptAt)
{
    TFile file(TestDataFactory::GetFilesDir() + "test.zip", true);
    TString script;
    TString buffer[100];
    TScriptInterpreter interpreter(TestDataFactory::GetScriptDir());

    // Invalid parameter tests
    ASSERT_EQ(0, interpreter.ScriptExecuteAt(nullptr, 0, nullptr));
    ASSERT_EQ(0, interpreter.ScriptExecuteAt(&file, 0, nullptr));
    ASSERT_EQ(0, interpreter.ScriptExecuteAt(nullptr, 0, buffer));

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

    // Debug
    //for (const auto& val : buffer) if (!val.IsEmpty()) std::cout << val.ToString() << "\n";
}
