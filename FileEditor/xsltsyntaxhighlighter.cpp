#include "xsltsyntaxhighlighter.h"

XsltSyntaxHighlighter::XsltSyntaxHighlighter(QTextDocument *parent)
    : BaseSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

void XsltSyntaxHighlighter::setupHighlightingRules() {
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
        "\\bapply-templates\\b", "\\battribute\\b", "\\bcall-template\\b", "\\bchoose\\b",
        "\\bcopy\\b", "\\bcopy-of\\b", "\\bfor-each\\b", "\\bif\\b", "\\bmessage\\b",
        "\\bnamespace\\b", "\\bnumber\\b", "\\bprocessing-instruction\\b", "\\bsort\\b",
        "\\btemplate\\b", "\\bvalue-of\\b", "\\bvariable\\b", "\\bwhen\\b", "\\bwith-param\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        addHighlightingRule(pattern, keywordFormat);
    }

    // Elements (light turquoise)
    QTextCharFormat elementFormat;
    elementFormat.setFontWeight(QFont::Bold);
    elementFormat.setForeground(QColor(78, 201, 176));  // #4EC9B0
    addHighlightingRule("</?\\w+", elementFormat);

    // Attributes (light yellow)
    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(QColor(220, 205, 170));  // #DCDCAA
    addHighlightingRule("\\w+(?==)", attributeFormat);

    // Attribute values (light orange)
    QTextCharFormat attributeValueFormat;
    attributeValueFormat.setForeground(QColor(206, 145, 120));  // #CE9178
    addHighlightingRule("\"[^\"]*\"", attributeValueFormat);

    // Comments (green)
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(106, 153, 85));  // #6A9955
    addHighlightingRule("<!--[^>]*-->", commentFormat);

    // Processing Instructions (purple)
    QTextCharFormat processingInstructionFormat;
    processingInstructionFormat.setForeground(QColor(197, 134, 192));  // #C586C0
    addHighlightingRule("<\\?[^>]*\\?>", processingInstructionFormat);

    // Numbers (light green)
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168));  // #B5CEA8
    addHighlightingRule("\\b\\d+\\b", numberFormat);

    // Tags and Punctuation (light gray)
    QTextCharFormat tagPunctuationFormat;
    tagPunctuationFormat.setForeground(QColor(150, 150, 150));  // #969696
    addHighlightingRule("[<>/]", tagPunctuationFormat);
}
