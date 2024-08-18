#include "htmlsyntaxhighlighter.h"

HtmlSyntaxHighlighter::HtmlSyntaxHighlighter(QTextDocument *parent)
    : BaseSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

void HtmlSyntaxHighlighter::setupHighlightingRules() {
    // Default text (light gray)
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QColor(212, 212, 212));  // #D4D4D4
    defaultFormat.setFontWeight(QFont::Normal);
    addHighlightingRule(".", defaultFormat);

    // Tags (blue)
    QTextCharFormat tagFormat;
    tagFormat.setForeground(QColor(86, 156, 214));  // #569CD6
    tagFormat.setFontWeight(QFont::Bold);
    addHighlightingRule("</?\\w+", tagFormat);

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

    // Doctype (purple)
    QTextCharFormat doctypeFormat;
    doctypeFormat.setForeground(QColor(197, 134, 192));  // #C586C0
    addHighlightingRule("<!DOCTYPE[^>]*>", doctypeFormat);

    // Strings (light green)
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(181, 206, 168));  // #B5CEA8
    addHighlightingRule("\"[^\"]*\"", stringFormat);

    // Tags and Punctuation (light gray)
    QTextCharFormat tagPunctuationFormat;
    tagPunctuationFormat.setForeground(QColor(150, 150, 150));  // #969696
    addHighlightingRule("[<>/]", tagPunctuationFormat);
}
