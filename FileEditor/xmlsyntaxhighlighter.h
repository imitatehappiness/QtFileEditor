#pragma once

#include "basesyntaxhighlighter.h"

class XmlSyntaxHighlighter : public BaseSyntaxHighlighter {
    Q_OBJECT

public:
    XmlSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void setupHighlightingRules() override;
};
