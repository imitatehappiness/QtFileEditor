#pragma once

#include <basesyntaxhighlighter.h>

class TerminalSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    TerminalSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};
