#include "csssyntaxhighlighter.h"

CssSyntaxHighlighter::CssSyntaxHighlighter(QTextDocument *parent)
    : BaseSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

void CssSyntaxHighlighter::setupHighlightingRules() {
    // Default text (light gray)
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QColor(212, 212, 212));  // #D4D4D4
    defaultFormat.setFontWeight(QFont::Normal);
    addHighlightingRule(".", defaultFormat);

    // Selectors (blue)
    QTextCharFormat selectorFormat;
    selectorFormat.setForeground(QColor(86, 156, 214));  // #569CD6
    selectorFormat.setFontWeight(QFont::Bold);
    addHighlightingRule("\\b[a-zA-Z_][a-zA-Z0-9_-]*\\b(?=\\s*\\{)", selectorFormat);

    // Properties (light yellow)
    QTextCharFormat propertyFormat;
    propertyFormat.setForeground(QColor(220, 205, 170));  // #DCDCAA
    addHighlightingRule("\\b[a-zA-Z-]+(?=\\s*:)", propertyFormat);

    // Values (light green)
    QTextCharFormat valueFormat;
    valueFormat.setForeground(QColor(181, 206, 168));  // #B5CEA8
    addHighlightingRule("(?<=:\\s*)[^;]+(?=;)", valueFormat);

    // Comments (green)
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(106, 153, 85));  // #6A9955
    addHighlightingRule("/\\*[^*]*\\*/", commentFormat);

    // Strings (light orange)
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(206, 145, 120));  // #CE9178
    addHighlightingRule("\"[^\"]*\"|'[^']*'", stringFormat);

    // Punctuation (light gray)
    QTextCharFormat punctuationFormat;
    punctuationFormat.setForeground(QColor(150, 150, 150));  // #969696
    addHighlightingRule("[{};:]", punctuationFormat);
}
