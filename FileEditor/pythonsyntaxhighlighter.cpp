#include "pythonsyntaxhighlighter.h"

PythonSyntaxHighlighter::PythonSyntaxHighlighter(QTextDocument *parent)
    : BaseSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

void PythonSyntaxHighlighter::setupHighlightingRules() {
    // Default text (light gray)
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QColor(212, 212, 212));  // #D4D4D4
    defaultFormat.setFontWeight(QFont::Normal);
    addHighlightingRule(".", defaultFormat);

    // Keywords (blue)
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(86, 156, 214));  // #569CD6
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns = {
        "\\bFalse\\b", "\\bNone\\b", "\\bTrue\\b", "\\band\\b", "\\bas\\b",
        "\\bassert\\b", "\\bbreak\\b", "\\bclass\\b", "\\bcontinue\\b",
        "\\bdef\\b", "\\belif\\b", "\\belse\\b", "\\bexcept\\b", "\\bfinally\\b",
        "\\bfor\\b", "\\bfrom\\b", "\\bglobal\\b", "\\bif\\b", "\\bimport\\b",
        "\\bin\\b", "\\bis\\b", "\\blambda\\b", "\\bnonlocal\\b", "\\bnot\\b",
        "\\bor\\b", "\\bpass\\b", "\\braise\\b", "\\breturn\\b", "\\btry\\b",
        "\\bwhile\\b", "\\bwith\\b", "\\byield\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        addHighlightingRule(pattern, keywordFormat);
    }

    // Classes (turquoise)
    QTextCharFormat classFormat;
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor(78, 201, 176));  // #4EC9B0
    addHighlightingRule("\\b[A-Za-z_][A-Za-z0-9_]*\\b(?=\\()", classFormat);

    // Functions (light yellow)
    QTextCharFormat functionFormat;
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor(220, 205, 170));  // #DCDCAA
    addHighlightingRule("\\b[A-Za-z_][A-Za-z0-9_]*\\b(?=\\()", functionFormat);

    // Single-line comments (green)
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor(106, 153, 85));  // #6A9955
    addHighlightingRule("#[^\n]*", singleLineCommentFormat);

    // Multi-line comments (green)
    QTextCharFormat multiLineCommentFormat;
    multiLineCommentFormat.setForeground(QColor(106, 153, 85));  // #6A9955
    // Adjusted to handle multi-line strings (""" or ''') properly
    addHighlightingRule(R"(\"\"\".*?\"\"\"|\'\'\'.*?\'\'\'|\"\"\".*$|\'\'\'.*$)", multiLineCommentFormat);

    // Strings (light orange)
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(206, 145, 120));  // #CE9178
    addHighlightingRule(R"(".*?"|'.*?')", stringFormat);

    // Numbers (light green)
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168));  // #B5CEA8
    addHighlightingRule("\\b\\d+\\b", numberFormat);

    // Punctuation (light gray)
    QTextCharFormat punctuationFormat;
    punctuationFormat.setForeground(QColor(150, 150, 150));  // #969696
    addHighlightingRule("[(){}\\[\\],.:;]", punctuationFormat);

    // Operators (light gray)
    QTextCharFormat operatorFormat;
    operatorFormat.setForeground(QColor(150, 150, 150));  // #969696
    addHighlightingRule("[+\\-*/%&|^~<>!=]", operatorFormat);
}
