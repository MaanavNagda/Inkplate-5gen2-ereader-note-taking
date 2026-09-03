#ifndef NOTES_TEXTBOOK_APP_H
#define NOTES_TEXTBOOK_APP_H

#include "TextbookApp.h"

class NotesTextbookApp : public TextbookApp {
public:
    AppId id() const override { return AppId::NOTES_TEXTBOOK; }
    const char* name() const override { return "Notes + Textbook"; }
};

#endif
