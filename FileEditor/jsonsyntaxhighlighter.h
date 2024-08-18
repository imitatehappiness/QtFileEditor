#pragma once
#include "basesyntaxhighlighter.h"

class JsonSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    JsonSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};
