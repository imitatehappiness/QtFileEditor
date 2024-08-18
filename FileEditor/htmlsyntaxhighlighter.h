#pragma once
#include "basesyntaxhighlighter.h"

class HtmlSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    HtmlSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};
