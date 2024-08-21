
#include "terminalsyntaxhighlighter.h"

TerminalSyntaxHighlighter::TerminalSyntaxHighlighter(QTextDocument *parent)
    : BaseSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

void TerminalSyntaxHighlighter::setupHighlightingRules() {
    // Default text (light gray)
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QColor(212, 212, 212));  // #D4D4D4
    defaultFormat.setFontWeight(QFont::Normal);
    addHighlightingRule(".", defaultFormat);

    // Path and command prompt (red)
    QTextCharFormat pathPromptFormat;
    pathPromptFormat.setForeground(QColor("#007cad"));  // blue color
    pathPromptFormat.setFontWeight(QFont::Bold);
    addHighlightingRule("^([A-Za-z]:\\\\[^>]*>)", pathPromptFormat);

    // Unrecognized commands (green)
    QTextCharFormat unrecognizedCommandFormat;
    unrecognizedCommandFormat.setForeground(QColor(181, 206, 168));  // Light green color
    unrecognizedCommandFormat.setFontWeight(QFont::Normal);
    addHighlightingRule("^\".*\"", unrecognizedCommandFormat);
}
