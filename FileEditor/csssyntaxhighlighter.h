#pragma once

#include "basesyntaxhighlighter.h"

class CssSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    CssSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};
