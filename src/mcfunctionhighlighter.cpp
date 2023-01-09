#include "mcfunctionhighlighter.h"
#include "codeeditor.h"

#include "parsers/command/visitors/nodeformatter.h"
#include "parsers/command/visitors/sourceprinter.h"

#include <QDebug>
#include <QElapsedTimer>

McfunctionHighlighter::McfunctionHighlighter(QTextDocument *parent)
    : Highlighter(parent) {
    setupRules();
}

void McfunctionHighlighter::setupRules() {
    auto fmt = QTextCharFormat();

    fmt.setForeground(QColor(62, 195, 0));
    singleCommentHighlightRules.insert('#', fmt);

    fmt = QTextCharFormat();

    HighlightingRule rule;

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
/*    keywordFormat.setToolTip("minecraft command"); */
    static const QString keywordPatterns[] = {
        QStringLiteral("\\badvancement\\b"),
        QStringLiteral("\\abttribute\\b"),
        QStringLiteral("\\bban\\b"),
        QStringLiteral("\\bban-ip\\b"),
        QStringLiteral("\\bbanlist\\b"),
        QStringLiteral("\\bbossbar\\b"),
        QStringLiteral("\\bclear\\b"),
        QStringLiteral("\\bclone\\b"),
        QStringLiteral("\\bdata\\b"),
        QStringLiteral("\\bdatapack\\b"),
        QStringLiteral("\\bdebug\\b"),
        QStringLiteral("\\bdefaultgamemode\\b"),
        QStringLiteral("\\bdeop\\b"),
        QStringLiteral("\\bdifficulty\\b"),
        QStringLiteral("\\beffect\\b"),
        QStringLiteral("\\bechant\\b"),
        QStringLiteral("\\bexecute\\b"),
        QStringLiteral("\\bexperience\\b"),
        QStringLiteral("\\bfill\\b"),
        QStringLiteral("\\bforceload\\b"),
        QStringLiteral("\\bfunction\\b"),
        QStringLiteral("\\bgamemode\\b"),
        QStringLiteral("\\bgamerule\\b"),
        QStringLiteral("\\bgive\\b"),
        QStringLiteral("\\bhelp\\b"),
        QStringLiteral("\\bitem\\b"),
        QStringLiteral("\\bjfr\\b"),
        QStringLiteral("\\bkick\\b"),
        QStringLiteral("\\bkill\\b"),
        QStringLiteral("\\blist\\b"),
        QStringLiteral("\\blocate\\b"),
        QStringLiteral("\\blocatebiome\\b"),
        QStringLiteral("\\bloot\\b"),
        QStringLiteral("\\bme\\b"),
        QStringLiteral("\\bmsg\\b"),
        QStringLiteral("\\bop\\b"),
        QStringLiteral("\\bpardon\\b"),
        QStringLiteral("\\bpardon-ip\\b"),
        QStringLiteral("\\bparticle\\b"),
        QStringLiteral("\\bperf\\b"),
        QStringLiteral("\\bplaysound\\b"),
        QStringLiteral("\\bpublish\\b"),
        QStringLiteral("\\brecipe\\b"),
        QStringLiteral("\\breload\\b"),
        QStringLiteral("\\breplaceitem\\b"),
        QStringLiteral("\\bsave-all\\b"),
        QStringLiteral("\\bsave-off\\b"),
        QStringLiteral("\\bsave-on\\b"),
        QStringLiteral("\\bsay\\b"),
        QStringLiteral("\\bschedule\\b"),
        QStringLiteral("\\bscoreboard\\b"),
        QStringLiteral("\\bseed\\b"),
        QStringLiteral("\\bsetblock\\b"),
        QStringLiteral("\\bsetidletimeout\\b"),
        QStringLiteral("\\bsetworldspawn\\b"),
        QStringLiteral("\\bspawnpoint\\b"),
        QStringLiteral("\\bspectate\\b"),
        QStringLiteral("\\bspreadplayer\\b"),
        QStringLiteral("\\bstop\\b"),
        QStringLiteral("\\bstopsound\\b"),
        QStringLiteral("\\bsummon\\b"),
        QStringLiteral("\\btag\\b"),
        QStringLiteral("\\bteam\\b"),
        QStringLiteral("\\bteleport\\b"),
        QStringLiteral("\\bteammsg\\b"),
        QStringLiteral("\\btell\\b"),
        QStringLiteral("\\btellraw\\b"),
        QStringLiteral("\\btime\\b"),
        QStringLiteral("\\btitle\\b"),
        QStringLiteral("\\btm\\b"),
        QStringLiteral("\\btp\\b"),
        QStringLiteral("\\btrigger\\b"),
        QStringLiteral("\\bw\\b"),
        QStringLiteral("\\bweather\\b"),
        QStringLiteral("\\bwhitelist\\b"),
        QStringLiteral("\\bworldborder\\b"),
        QStringLiteral("\\bxp\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    numberFormat.setForeground(QColor(47, 151, 193));
    rule.pattern =
        QRegularExpression(QStringLiteral(
                               R"((?<!\w)-?\d+(?:\.\d+)?[bBsSlLfFdD]?(?!\w))"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    posFormat.setForeground(QColor(171, 129, 205));
    rule.pattern = QRegularExpression(QStringLiteral("[~^]?-?\\d+(?:\\s|$)"));
    rule.format  = posFormat;
    highlightingRules.append(rule);

    namespacedIDFormat.setForeground(QColor(69, 80, 59));
    rule.pattern =
        QRegularExpression(QStringLiteral(R"(\b[a-z0-9-_]+:[a-z0-9-_/.]+\b)"));
    rule.format = namespacedIDFormat;
    highlightingRules.append(rule);

    auto quoteFmt = QTextCharFormat();
    quoteFmt.setForeground(QColor(163, 22, 33));

    quoteHighlightRules.insert('\'', quoteFmt);

    commentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral(R"(^\s*#.*)"));
    rule.format  = commentFormat;
    highlightingRules.append(rule);
}

void McfunctionHighlighter::highlightBlock(const QString &text) {
    Highlighter::highlightBlock(text);
    if (this->document()) {
        auto *data = static_cast<TextBlockData *>(currentBlockUserData());

        if (!data)
            return;

        const auto &infos = data->namespacedIds();

        for (const auto info: infos) {
            auto fmt = namespacedIDFormat;
            fmt.setFontUnderline(true);
            fmt.setToolTip(info->link);
            setFormat(info->start, info->length, fmt);
        }

        if (m_formats.isEmpty()) {
            for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
                QRegularExpressionMatchIterator &&matchIterator =
                    rule.pattern.globalMatch(text);
                while (matchIterator.hasNext()) {
                    QRegularExpressionMatch &&match = matchIterator.next();
                    setFormat(match.capturedStart(), match.capturedLength(),
                              rule.format);
                }
            }
        } else {
            for (const auto &range: qAsConst(m_formats)) {
                mergeFormat(range.start, range.length, range.format);
            }
        }
    }

    setCurrentBlockState(0);
}

void McfunctionHighlighter::rehighlightBlock(const QTextBlock &block,
                                             const QVector<QTextLayout::FormatRange> &formats)
{
    m_formats = formats;
    Highlighter::rehighlightBlock(block);
    m_formats.clear();
}

//void McfunctionHighlighter::checkProblems(bool checkAll) {
//    QSharedPointer<Command::ParseNode> result = nullptr;

//    QElapsedTimer timer;

//    timer.start();
//    const auto &&changedBlks = changedBlocks();
//    const auto  *doc         = document();
//    Q_ASSERT(doc != nullptr);

//    auto block =
//        (!checkAll) ? ((!changedBlks.isEmpty()) ? changedBlks.constFirst() : doc
//                       ->
//                       findBlock(-1)) : doc->firstBlock();
//    const auto &end =
//        (!checkAll) ? ((!changedBlks.isEmpty()) ? changedBlks.constLast() : doc
//                       ->
//                       findBlock(-1)) : doc->lastBlock();

//    auto *editor = dynamic_cast<CodeEditor *>(document()->parent()->parent());
//    Q_ASSERT(editor != nullptr);

//    while (block.isValid() && block.blockNumber() <= end.blockNumber()) {
//        if (TextBlockData *data =
//                dynamic_cast<TextBlockData *>(block.userData())) {
//            QString lineText = block.text();
//            parser.setText(lineText);
//            //QElapsedTimer timer;
//            //timer.start();
//            result = parser.parse();
//            if (result->isValid()) {
//                data->clearProblems();

//                Command::SourcePrinter printer;
//                printer.startVisiting(result.get());
//                if (printer.source() != lineText)
//                    qDebug() << printer.source();

//                Command::NodeFormatter formatter;
//                formatter.startVisiting(result.get());
//                /*qDebug() << "rehighlight manually" << block.firstLineNumber(); */
//                document()->blockSignals(true);
//                rehighlightBlock(block, formatter.formatRanges());
//                /*
//                   QTextCursor tc = editor->textCursor();
//                   tc.setPosition(block.position());
//                   tc.select(QTextCursor::LineUnderCursor);
//                   tc.insertText(printer.source());
//                   editor->setTextCursor(tc);
//                 */
//                document()->blockSignals(false);

///*
//                  qDebug() << "Size:" << parser.cache().size() << '/' <<
//                      parser.cache().capacity()
//                           << "Total access:" <<
//                      parser.cache().stats().total_accesses()
//                           << "Total hit:" << parser.cache().stats().total_hits()
//                           << "Total miss:" << parser.cache().stats().total_hits()
//                           << "Hit rate:" << parser.cache().stats().hit_rate()
//                           << "Time elapsed:" << timer.elapsed();
// */
//            } else {
//                const auto  parseErr = parser.errors().last();
//                ProblemInfo error{
//                    ProblemInfo::Type::Error, block.blockNumber(),
//                    parseErr.pos, parseErr.length,
//                    parseErr.toLocalizedMessage(),
//                };
//                data->setProblems({ error });
//            }
//        }
//        block = block.next();
//    }
//    qDebug() << "Size:" << parser.cache().size() << '/' <<
//        parser.cache().capacity()
//             << "Total access:" << parser.cache().stats().total_accesses()
//             << "Total hit:" << parser.cache().stats().total_hits()
//             << "Total miss:" << parser.cache().stats().total_misses()
//             << "Hit rate:" << parser.cache().stats().hit_rate()
//             << "Time elapsed:" << timer.elapsed();
//}

void McfunctionHighlighter::checkProblems(bool checkAll) {
    parser.setText(document()->toPlainText());
    parser.parse();
    const auto &&result       = parser.syntaxTree();
    const auto &&resultLines  = result->lines();
    const auto  &errorsByLine = parser.errorsByLine();

    auto resultIter = resultLines.cbegin();
    for (auto block = getParentDoc()->begin();
         block != getParentDoc()->end(); block = block.next()) {
        if (TextBlockData *data =
                dynamic_cast<TextBlockData *>(block.userData())) {
            const QString &&lineText = block.text();
            if (result->isValid() ||
                !errorsByLine.contains(block.blockNumber())) {
                data->clearProblems();

                auto *lineResult = resultIter->get();
                if (lineResult->kind() ==
                    Command::ParseNode::Kind::Root) {
                    Command::SourcePrinter printer;
                    printer.startVisiting(lineResult);
                    if (printer.source() != lineText)
                        qDebug() << printer.source();

                    Command::NodeFormatter formatter;
                    formatter.startVisiting(lineResult);
                    /*qDebug() << "rehighlight manually" << block.firstLineNumber(); */
                    document()->blockSignals(true);
                    rehighlightBlock(block, formatter.formatRanges());
                    document()->blockSignals(false);
                }
            } else {
                QTextCursor          tc(getParentDoc());
                QVector<ProblemInfo> problems;
                for (const auto &err: errorsByLine[block.blockNumber()]) {
                    tc.setPosition(err.pos);
                    ProblemInfo error{ ProblemInfo::Type::Error,
                                       tc.blockNumber(),
                                       tc.positionInBlock(), err.length,
                                       err.toLocalizedMessage() };
                    problems.append(std::move(error));
                }
                data->setProblems(problems);
            }
        }

        ++resultIter;
        /*emit document()->documentLayout()->updateBlock(block); */
    }
}
