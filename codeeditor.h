#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QTextEdit>
#include <QString>

#include "jsonhighlighter.h"
#include "mcfunctionhighlighter.h"

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);
    struct CurrentNamespacedID {
        int startingIndex;
        int blockNumber;
        QString string;
        QString link;
    };

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setCurFile(QString filepath);
    CurrentNamespacedID getCurrentNamespacedID();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void dropEvent(QDropEvent *e) override;


private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void onCursorPositionChanged();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    QString curFile;
    QString prevCurFile;
    QStringList keyModifiers;
    JsonHighlighter *jsonHighlighter;
    MCfunctionHighlighter *mcfunctionHighlighter;
    QTextCursor lastTextCursor;
    QTextCursor mouseTextCursor;
    QTextCursor lastMouseTextCursor;

    void highlightCurrLineSelection();
    void followCurrentNamespacedID();
};


#endif // CODEEDITOR_H
