#include "jsonsyntaxhighlighter.h"

JsonSyntaxHighlighter::JsonSyntaxHighlighter(QTextDocument *parent)
    : BaseSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

void JsonSyntaxHighlighter::setupHighlightingRules() {
    // Default text (light gray)
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QColor(212, 212, 212));  // #D4D4D4
    defaultFormat.setFontWeight(QFont::Normal);
    addHighlightingRule(".", defaultFormat);

    // Keys (light blue)
    QTextCharFormat keyFormat;
    keyFormat.setForeground(QColor(86, 156, 214));  // #569CD6
    keyFormat.setFontWeight(QFont::Bold);
    addHighlightingRule("\"[^\"]*\":", keyFormat);

    // Strings (orange)
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(206, 145, 120));  // #CE9178
    addHighlightingRule("\"[^\"]*\"", stringFormat);

    // Numbers (light green)
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168));  // #B5CEA8
    addHighlightingRule("\\b\\d+\\.?\\d*\\b", numberFormat);

    // Boolean and null literals (light purple)
    QTextCharFormat literalFormat;
    literalFormat.setForeground(QColor(197, 134, 192));  // #C586C0
    addHighlightingRule("\\b(true|false|null)\\b", literalFormat);

    // Braces and brackets (light gray)
    QTextCharFormat braceBracketFormat;
    braceBracketFormat.setForeground(QColor(150, 150, 150));  // #969696
    addHighlightingRule("[{}\\[\\]]", braceBracketFormat);

    // Commas and colons (light gray)
    QTextCharFormat punctuationFormat;
    punctuationFormat.setForeground(QColor(150, 150, 150));  // #969696
    addHighlightingRule("[,:]", punctuationFormat);

    // Comments (not applicable to JSON but placeholder for other types)
    // Note:
}
