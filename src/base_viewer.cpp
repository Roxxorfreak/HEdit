// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * The constructor for the base class for all viewer objects.
 * @param console The console object used for all output.
 * @param file The file object that is used to access the data to be is displayed.
 * @param file_pos The current absolute position within the file.
 * @param settings The program settings.
 * @param editor The information about the editor that owns the viewer.
 * @param marker The marker object (used to determine if a byte is selected).
 * @param comparator The comparator engine used to compare data between editors.
 * @param undo_engine The undo engine used to track and undo changes.
 */
TBaseViewer::TBaseViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator, TUndoEngine* undo_engine) noexcept
{
    this->console_      = console;
    this->file_         = file;
    this->file_pos_     = file_pos;
    this->settings_     = settings;
    this->editor_       = editor;
    this->marker_       = marker;
    this->comparator_   = comparator;
    this->undo_engine_  = undo_engine;
    this->redraw_       = true;
}

/**
 * Virtual destructor
 */
TBaseViewer::~TBaseViewer()
{
}

/**
 * Marks the viewer as changed, causing redraw the next time the viewer is painted.
 */
void TBaseViewer::SetChanged() noexcept
{
    this->redraw_ = true;
}

/**
 * Marks the viewer as unchanged, causing no redraw the next time the viewer is painted.
 */
void TBaseViewer::ClearChanged() noexcept
{
    this->redraw_ = false;
}

/**
 * Returns true, if the viewer was changed.
 * @return true, if the viewer was changed.
 */
bool TBaseViewer::IsChanged() const noexcept
{
    return this->redraw_;
}

/**
 * A shortcut function for displaying a message box.
 * @param title The title of the message box.
 * @param text1 The first line of text.
 * @param text2 The second line of text (can be an empty string).
 */
void TBaseViewer::MessageBox(const char* title, const char* text1, const char* text2)
{
    // Create the message box
    std::unique_ptr<TMessageBox> message_box(new TMessageBox(this->console_, title, this->settings_->dialog_color_, this->settings_->dialog_back_color_));

    // Display the message box
    message_box->Display(TMessageBoxType::INFO, TString(text1), TString(text2));
}

/**
 * Displays information about the character at the specified offset in the status bar of the editor.
 * @param offset The offset (relative to the file pointer).
 */
void TBaseViewer::UpdateCurrentCharStatus(int32_t offset)
{
    char text[64] = {};
    uint8_t byte_value;
    TString binary_string;

    // Read the character at the current position
    if (this->file_->ReadAt(&byte_value, sizeof(uint8_t), (*this->file_pos_) + offset) == sizeof(uint8_t))
    {
        binary_string.BinaryFromInt(byte_value, 8);
        snprintf(text, sizeof(text), "[ H: %02" PRIX8 "  D: %03" PRIu8 "  B: %s ]", byte_value, byte_value, binary_string.ToString());
    }
    else
    {
        snprintf(text, sizeof(text), "[No character info beyond EOF]");
    }

    // Print the status
    this->console_->SetCursor(this->editor_->status_column_, this->editor_->status_line_);
    this->console_->Print(text);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_MIDDLE);
}
