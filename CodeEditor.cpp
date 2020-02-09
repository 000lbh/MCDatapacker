#include "codeeditor.h"
#include "linenumberarea.h"
#include "jsonhighlighter.h"
#include "mainwindow.h"

#include <ui_mainwindow.h>

#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QTextCursor>
#include <QFileInfo>
#include <QStringLiteral>
#include <QApplication>
#include <QDir>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    jsonHighlighter = new JsonHighlighter(this->document());
    mcfunctionHighlighter = new MCfunctionHighlighter(this->document(), this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::onCursorPositionChanged);

    updateLineNumberAreaWidth(0);
    onCursorPositionChanged();
}

void CodeEditor::wheelEvent(QWheelEvent *e) {
    if (e->modifiers().testFlag(Qt::ControlModifier)) {
        int delta = e->delta() / 120 * 2;
        //qDebug() << e->delta() << " " << delta;
        QFont font = this->font();
        font.setPointSize(font.pointSize() + delta);
        this->setFont(font);
    }
    //this->mcfunctionHighlighter->rehighlight();

    QPlainTextEdit::wheelEvent(e);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::mouseMoveEvent(QMouseEvent *e) {
    QPlainTextEdit::mouseMoveEvent(e);

    //if(e->buttons() == Qt::LeftButton) {}
    QTextCursor cursor = cursorForPosition(e->pos());
    //cursor.setPosition(this->document()->documentLayout()->hitTest( QPointF( e->x(), e->y() ), Qt::ExactHit ));
    this->mouseTextCursor = cursor;

    if(cursor.blockNumber() != this->lastMouseTextCursor.blockNumber())
        this->mcfunctionHighlighter->rehighlightBlock(this->document()->findBlockByNumber(this->lastMouseTextCursor.blockNumber()));
    this->mcfunctionHighlighter->rehighlightBlock(this->document()->findBlockByNumber(cursor.blockNumber()));

    this->lastMouseTextCursor = cursorForPosition(e->pos());
}

void CodeEditor::mousePressEvent(QMouseEvent *e) {
    //qDebug() << "Mouse press event";
    if(QGuiApplication::keyboardModifiers() & Qt::ControlModifier
        && e->modifiers() & Qt::ControlModifier) {
        followCurrentNamespacedID();
    }
    QPlainTextEdit::mousePressEvent(e);
}

void CodeEditor::onCursorPositionChanged() {
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(180);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();

        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}

void CodeEditor::setCurFile(QString filepath) {
    this->curFile = filepath;
    QFileInfo info = QFileInfo(filepath);
    qDebug() << info;
    qDebug() << info.completeSuffix();
    const QString jsonExts = "json mcmeta";

    qDebug() << jsonExts;
    qDebug() << jsonExts.contains(info.completeSuffix());

    jsonHighlighter->setEnabled(jsonExts.contains(info.completeSuffix()));
    mcfunctionHighlighter->setEnabled(info.completeSuffix() == "mcfunction");
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

CodeEditor::CurrentNamespacedID CodeEditor::getCurrentNamespacedID() {
    QTextCursor cursor = this->mouseTextCursor;
    //qDebug() << "cursor pos: " << cursor.position();
    //qDebug() << "cursor pos in block: " << cursor.positionInBlock();
    //qDebug() << "cursor block number: " << cursor.blockNumber();
    //qDebug() << "cursor column: " << cursor.columnNumber();

    QTextBlock block = this->document()->findBlockByNumber(cursor.blockNumber());
    QString lineText = block.text();
    //qDebug() << lineText;
    int cursorRelPos = cursor.positionInBlock();

    QChar currChar = lineText[cursor.columnNumber()];
    if(QRegularExpression(QStringLiteral("[^0-9a-z-_.:/]")).match(currChar).hasMatch()) {
        --cursorRelPos;
    }

    bool isStartofString = false;
    while(!isStartofString && cursorRelPos >= 0) {
        currChar = lineText[cursorRelPos];
        //qDebug() << "current character: " << currChar;
        QRegularExpression regex = QRegularExpression(QStringLiteral("[0-9a-z-_.:/]"));
        QRegularExpressionMatch match = regex.match(currChar);
        isStartofString = !match.hasMatch();
        //qDebug() << "is start of string: " << isStartofString;
        if(!isStartofString && cursorRelPos >= 0) --cursorRelPos;
    }
    //qDebug() << "startIndex: " << cursorRelPos;
    int startIndex = (qMax(cursorRelPos + 1, 0));
    QString str = lineText.mid(startIndex);
    QRegularExpression namespacedIDRegex = QRegularExpression(QStringLiteral("^\\b[a-z0-9-_]+:[a-z0-9-_/.]+\\b"));
    //qDebug() << str;
    CurrentNamespacedID currNamespacedID;
    currNamespacedID.startingIndex = startIndex;
    currNamespacedID.blockNumber = cursor.blockNumber();
    currNamespacedID.string = namespacedIDRegex.match(str).captured();

    return currNamespacedID;
}

void CodeEditor::followCurrentNamespacedID() {
    QString str = getCurrentNamespacedID().string;
    //qDebug() << "under cursor string: " << str;
    QString path;

    QString fileExt;
    if(!str.isEmpty()) {
        QString dirname = qobject_cast<MainWindow*>(this->parent()->parent()->parent()->parent())->getCurDir();
        //qDebug() << "dirname: " << dirname;
        if(dirname.isEmpty()) return;

        QStringList dirList = QDir(dirname + "/data/" + str.split(":")[0]).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        for(const auto& dirType: dirList) {
            //qDebug() << "dirType: " << dirType;
            path = dirname + "/data/" + str.split(":")[0] + "/" + dirType + "/" + str.split(":")[1];
            if(dirType == "functions" && QFile::exists(path+".mcfunction")) {
                fileExt = ".mcfunction";
                break;
            } else if (QFile::exists(path+".json")) {
                fileExt = ".json";
                break;
            }
        }

        //qDebug() << "final path: " << path;

        if(!path.isEmpty() && !fileExt.isEmpty()) {
            path += fileExt;
            this->prevCurFile = path;
            /*
            qobject_cast<MainWindow*>(this->parent()->parent()->parent()->parent())->openFile(path);
            qobject_cast<DatapackTreeView*>(
                        qobject_cast<QSplitter*>(this->parent()->parent())->widget(0)
                    )->selectFromPath(path);
                    */
            qobject_cast<DatapackTreeView*>(
                        qobject_cast<QSplitter*>(this->parent()->parent())->widget(0)
                    )->openFromPath(path);
        }
    }
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(210, 210, 210));
    //painter.setPen(QColor(240, 240, 240));
    //painter.drawLine(event->rect().topRight(), event->rect().bottomRight());
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            if(blockNumber == this->textCursor().blockNumber()) {
                painter.setPen(QColor(40, 40, 40));
            } else {
                painter.setPen(Qt::darkGray);
            }
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
