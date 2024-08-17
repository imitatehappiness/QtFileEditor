#ifndef BASESYNTAXHIGHLIGHTER_H
#define BASESYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class BaseSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    BaseSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    void addHighlightingRule(const QString &pattern, const QTextCharFormat &format);
    virtual void setupHighlightingRules() = 0;
    void highlightBlock(const QString &text) override;
};

#endif // BASESYNTAXHIGHLIGHTER_H
