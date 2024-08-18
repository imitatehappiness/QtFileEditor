#include "xmlsyntaxhighlighter.h"

XmlSyntaxHighlighter::XmlSyntaxHighlighter(QTextDocument *parent)
    : BaseSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

void XmlSyntaxHighlighter::setupHighlightingRules() {
    // Default text (light gray)
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QColor(212, 212, 212));  // #D4D4D4
    defaultFormat.setFontWeight(QFont::Normal);
    addHighlightingRule(".", defaultFormat);

    // Tags (blue)
    QTextCharFormat tagFormat;
    tagFormat.setForeground(QColor(86, 156, 214));  // #569CD6
    tagFormat.setFontWeight(QFont::Bold);
    addHighlightingRule("</?\\w+.*?>", tagFormat);

    // Attribute names (light green)
    QTextCharFormat attributeNameFormat;
    attributeNameFormat.setForeground(QColor(106, 153, 85));  // #6A9955
    addHighlightingRule("\\b\\w+(?=\\=)", attributeNameFormat);

    // Attribute values (orange)
    QTextCharFormat attributeValueFormat;
    attributeValueFormat.setForeground(QColor(206, 145, 120));  // #CE9178
    addHighlightingRule("\"[^\"]*\"", attributeValueFormat);

    // Comments (light gray)
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(150, 150, 150));  // #969696
    addHighlightingRule("<!--.*?-->", commentFormat);

    // Processing Instructions (purple)
    QTextCharFormat processingInstructionFormat;
    processingInstructionFormat.setForeground(QColor(197, 134, 192));  // #C586C0
    addHighlightingRule("<\\?.*?\\?>", processingInstructionFormat);

    // CDATA sections (light yellow)
    QTextCharFormat cdataFormat;
    cdataFormat.setForeground(QColor(220, 205, 170));  // #DCDCAA
    addHighlightingRule("<!\\[CDATA\\[.*?\\]\\]>", cdataFormat);
}
