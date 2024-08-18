#pragma once

#include "basesyntaxhighlighter.h"

class XsltSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    XsltSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};
