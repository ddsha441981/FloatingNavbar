#ifndef PROMPTLOADER_H
#define PROMPTLOADER_H

#include <QString>

class PromptLoader {
public:
    static QString getPromptFilePath();
    static QString readPromptText();
};

#endif // PROMPTLOADER_H
