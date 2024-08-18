#pragma once
#include "basesyntaxhighlighter.h"

class PythonSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    PythonSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};
