/*
 * Document Reader for Flipper Zero
 *
 * A small, dependency-free text viewer. It lets you pick any file from the
 * SD card with the standard file browser and reads it on-device with the
 * built-in scrolling text box (Up/Down to scroll, Back to choose another
 * file, Back again from the browser to exit).
 *
 * Copyright (c) 2026 imperator-clawdius
 * SPDX-License-Identifier: MIT
 */

#include <furi.h>
#include <string.h>
#include <gui/gui.h>
#include <gui/view_holder.h>
#include <gui/modules/text_box.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>

#define TAG "DocReader"

/* Maximum number of bytes loaded from a file. Files larger than this are
 * shown truncated with a note appended. Kept modest so the formatted copy
 * the text box builds stays well within the app heap. */
#define DOC_READER_MAX_FILE_SIZE (8 * 1024)
#define DOC_READER_START_PATH    "/ext"

static const char DOC_READER_TRUNC_NOTE[] =
    "\n\n--- [truncated: file larger than 8 KB] ---\n";

typedef struct {
    FuriSemaphore* exit_sem;
} DocReader;

static void doc_reader_back_callback(void* context) {
    furi_assert(context);
    DocReader* app = context;
    furi_semaphore_release(app->exit_sem);
}

/*
 * Read up to DOC_READER_MAX_FILE_SIZE bytes from `path` into a freshly
 * allocated, null-terminated buffer. If the file is larger than the cap, a
 * short truncation note is appended and *out_truncated is set to true.
 * Returns NULL on failure; the caller owns the returned buffer.
 */
static char* doc_reader_read_file(Storage* storage, const char* path, bool* out_truncated) {
    File* file = storage_file_alloc(storage);
    char* buffer = NULL;
    bool truncated = false;

    do {
        if(!storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E(TAG, "Failed to open %s", path);
            break;
        }

        uint64_t file_size = storage_file_size(file);
        size_t to_read = DOC_READER_MAX_FILE_SIZE;
        if(file_size < (uint64_t)DOC_READER_MAX_FILE_SIZE) {
            to_read = (size_t)file_size;
        } else if(file_size > (uint64_t)DOC_READER_MAX_FILE_SIZE) {
            truncated = true;
        }

        size_t extra = truncated ? strlen(DOC_READER_TRUNC_NOTE) : 0;
        buffer = malloc(to_read + extra + 1);

        size_t total = 0;
        while(total < to_read) {
            size_t chunk = storage_file_read(file, buffer + total, to_read - total);
            if(chunk == 0) break; /* EOF or read error */
            total += chunk;
        }

        if(truncated) {
            memcpy(buffer + total, DOC_READER_TRUNC_NOTE, extra);
            total += extra;
        }
        buffer[total] = '\0';

        if(out_truncated) *out_truncated = truncated;
    } while(false);

    storage_file_close(file);
    storage_file_free(file);
    return buffer;
}

int32_t doc_reader_app(void* p) {
    UNUSED(p);

    DocReader* app = malloc(sizeof(DocReader));
    app->exit_sem = furi_semaphore_alloc(1, 0);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    Gui* gui = furi_record_open(RECORD_GUI);

    FuriString* path = furi_string_alloc_set(DOC_READER_START_PATH);

    DialogsFileBrowserOptions browser_options;
    /* "*" shows every file so any text document can be opened. */
    dialog_file_browser_set_basic_options(&browser_options, "*", NULL);
    browser_options.base_path = DOC_READER_START_PATH;

    while(true) {
        /* Pick a file; returns false when the user backs out of the browser. */
        if(!dialog_file_browser_show(dialogs, path, path, &browser_options)) {
            break;
        }

        bool truncated = false;
        char* text = doc_reader_read_file(storage, furi_string_get_cstr(path), &truncated);
        if(text == NULL) {
            continue; /* unreadable file: return to the browser */
        }

        TextBox* text_box = text_box_alloc();
        text_box_set_font(text_box, TextBoxFontText);
        text_box_set_focus(text_box, TextBoxFocusStart);
        text_box_set_text(text_box, text);

        ViewHolder* view_holder = view_holder_alloc();
        view_holder_attach_to_gui(view_holder, gui);
        view_holder_set_back_callback(view_holder, doc_reader_back_callback, app);
        view_holder_set_view(view_holder, text_box_get_view(text_box));

        /* Block until Back is pressed. */
        furi_semaphore_acquire(app->exit_sem, FuriWaitForever);

        view_holder_set_view(view_holder, NULL);
        view_holder_free(view_holder);
        text_box_free(text_box);
        free(text); /* safe now: the text box no longer references it */
    }

    furi_string_free(path);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_STORAGE);
    furi_semaphore_free(app->exit_sem);
    free(app);

    return 0;
}
