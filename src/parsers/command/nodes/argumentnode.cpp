#include "argumentnode.h"

Command::ArgumentNode::ArgumentNode(QObject *parent,
                                    int pos,
                                    int length,
                                    QString parserId) : Command::ParseNode(
        parent,
        -1,
        0) {
    setParserId(parserId);
}

QString Command::ArgumentNode::toString() const {
    return QString("ArgumentNode<%1>").arg(parserId());
}

QString Command::ArgumentNode::parserId() const {
    return m_parserId;
}

void Command::ArgumentNode::setParserId(const QString &parserId) {
    m_parserId = parserId;
}
