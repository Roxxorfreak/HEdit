// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TUndoEngine, BackupAndRestore)
{
    TUndoStep step;
    TUndoEngine undo(2);
    TUndoEngine undo1(0);

    // Try to backup with buffer size 0
    ASSERT_EQ(false, undo1.Backup(123, 12, 0, '0'));

    // Try to restore without backup
    ASSERT_EQ(false, undo.Restore(&step));

    // Backup 3 steps
    ASSERT_EQ(true, undo.Backup(123, 12, 0, '0'));
    ASSERT_EQ(true, undo.Backup(456, 34, 1, '1'));
    ASSERT_EQ(true, undo.Backup(789, 56, 0, '2'));

    // Restore first step
    ASSERT_EQ(true, undo.Restore(&step));
    ASSERT_EQ(789, step.file_pos);
    ASSERT_EQ(56, step.cursor_pos);
    ASSERT_EQ(0, step.nibble_pos);
    ASSERT_EQ('2', step.value);

    // Restore second step
    ASSERT_EQ(true, undo.Restore(&step));
    ASSERT_EQ(456, step.file_pos);
    ASSERT_EQ(34, step.cursor_pos);
    ASSERT_EQ(1, step.nibble_pos);
    ASSERT_EQ('1', step.value);

    // Try to restore second step
    ASSERT_EQ(false, undo.Restore(&step));
}
