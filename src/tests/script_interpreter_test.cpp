// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TScriptInterpreter, LoadScript)
{
    TString script;
    TScriptInterpreter interpreter(TestDataFactory::GetScriptDir());

    // Load script with invalid content
    script = "error1.hs";
    ASSERT_EQ(false, interpreter.LoadScript(script));
    ASSERT_STREQ("Invalid number <0x3t> passed as parameter 2", interpreter.GetLastError());
    ASSERT_STREQ("Checksum (XOR) with errors to test if error handling works", interpreter.ScriptDescription());

    // Load script with valud content
    script = "error2.hs";
    ASSERT_EQ(true, interpreter.LoadScript(script)) << interpreter.GetLastError();
    ASSERT_STREQ("", interpreter.GetLastError());
    ASSERT_STREQ("A plugin that has a very very very long title, so the creation of the Viewer menu can be tested to all extends - really all!", interpreter.ScriptDescription());
}
