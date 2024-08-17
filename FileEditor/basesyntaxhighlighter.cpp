#include "basesyntaxhighlighter.h"

BaseSyntaxHighlighter::BaseSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {

}

void BaseSyntaxHighlighter::addHighlightingRule(const QString &pattern, const QTextCharFormat &format) {
    HighlightingRule rule;
    rule.pattern = QRegularExpression(pattern);
    rule.format = format;
    highlightingRules.append(rule);
}

void BaseSyntaxHighlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
