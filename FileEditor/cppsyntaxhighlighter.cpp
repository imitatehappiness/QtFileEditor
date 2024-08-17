#include "cppsyntaxhighlighter.h"

CppSyntaxHighlighter::CppSyntaxHighlighter(QTextDocument *parent)
    : BaseSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

void CppSyntaxHighlighter::setupHighlightingRules() {
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
        "\\bchar\\b", "\\bclass\\b", "\\bconst\\b", "\\bdouble\\b",
        "\\benum\\b", "\\bexplicit\\b", "\\bfriend\\b", "\\binline\\b",
        "\\bint\\b", "\\blong\\b", "\\bnamespace\\b", "\\boperator\\b",
        "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b", "\\bshort\\b",
        "\\bsignals\\b", "\\bsigned\\b", "\\bslots\\b", "\\bstatic\\b",
        "\\bstruct\\b", "\\btemplate\\b", "\\btypedef\\b", "\\btypename\\b",
        "\\bunion\\b", "\\bunsigned\\b", "\\bvirtual\\b", "\\bvoid\\b",
        "\\bvolatile\\b", "\\bbool\\b", "\\busing\\b", "\\bnamespace\\b", "\\bauto\\b", "\\bthis\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        addHighlightingRule(pattern, keywordFormat);
    }

    // Classes (turquoise)
    QTextCharFormat classFormat;
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor(78, 201, 176));  // #4EC9B0
    addHighlightingRule("\\bQ[A-Za-z]+\\b", classFormat);

    // Single-line comments (green)
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor(106, 153, 85));  // #6A9955
    addHighlightingRule("//[^\n]*", singleLineCommentFormat);

    // Single-line red comments (!)
    QTextCharFormat singleLineRedCommentFormat;
    singleLineRedCommentFormat.setForeground(QColor(255, 0, 0));  // Red color
    addHighlightingRule("//![^\n]*", singleLineRedCommentFormat);

    // Single-line orange comments (TODO)
    QTextCharFormat singleLineTODOCommentFormat;
    singleLineRedCommentFormat.setForeground(QColor(255, 178, 0));  // orange color
    addHighlightingRule("//TODO[^\n]*", singleLineRedCommentFormat);

    // Strings (orange)
    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(QColor(206, 145, 120));  // #CE9178
    addHighlightingRule("\".*\"", quotationFormat);

    // Functions (light yellow)
    QTextCharFormat functionFormat;
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor(220, 205, 170));  // #DCDCAA
    addHighlightingRule("\\b[A-Za-z0-9_]+(?=\\()", functionFormat);

    // Numbers (light green)
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168));  // #B5CEA8
    addHighlightingRule("\\b\\d+\\b", numberFormat);

    // Preprocessor directives (purple)
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(197, 134, 192));  // #C586C0
    addHighlightingRule("^#.*", preprocessorFormat);
}
