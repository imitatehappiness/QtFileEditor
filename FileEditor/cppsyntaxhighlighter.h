#ifndef CPPSYNTAXHIGHLIGHTER_H
#define CPPSYNTAXHIGHLIGHTER_H

#include "basesyntaxhighlighter.h"

class CppSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    CppSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};

#endif // CPPSYNTAXHIGHLIGHTER_H
