#pragma once

#include "basesyntaxhighlighter.h"

class CppSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    CppSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};
