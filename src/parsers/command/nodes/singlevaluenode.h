#ifndef SINGLEVALUENODE_H
#define SINGLEVALUENODE_H

#include "argumentnode.h"

#include <QUuid>
#include <QRegularExpression>

namespace Command {
    template <class Base, typename T, ArgumentNode::ParserType PT>
    class SingleValueNode : public Base
    {
public:
        SingleValueNode(const QString &text, const T &value,
                        const bool isValid = false)
            : Base(PT, text), m_value(value) {
            this->m_isValid = isValid;
        };
        template <typename _T = T,
                  typename = typename std::enable_if_t<std::is_same<_T,
                                                                    QString>::value> >
        explicit SingleValueNode(const QString &text,
                                 const bool isValid = false)
            : Base(PT, text), m_value(text) {
            this->m_isValid = isValid;
        };

        inline T value() const {
            return m_value;
        }

        inline void setValue(const T &newValue) {
            m_value = newValue;
        }

protected:
        T m_value{};
    };
}

/*
 * Template classes can't be forward declared easily and can't have virtual overrides,
 * therefore creating subclasses is necessary.
 */
#define DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Name, T)                                    \
        namespace Command {                                                             \
            class Name ## Node : public SingleValueNode<ArgumentNode, T,                \
                                                        ArgumentNode::ParserType::Name> \
            {                                                                           \
public:                                                                                 \
                using SingleValueNode::SingleValueNode;                                 \
                void accept(NodeVisitor * visitor, VisitOrder) final;                   \
            };                                                                          \
            DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Name)                           \
        }                                                                               \

namespace Command {
    class FloatNode : public SingleValueNode<ArgumentNode, float,
                                             ArgumentNode::ParserType::Float>
    {
public:
        using SingleValueNode::SingleValueNode;
        void accept(NodeVisitor *visitor, VisitOrder) final;
        void chopTrailingDot();
    };
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Float)
}

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Bool, bool)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Double, double)
//DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Float, float)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Integer, int)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Long, long long)

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Color, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(EntityAnchor, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Heightmap, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ItemSlot, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Message, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Objective, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ObjectiveCriteria, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Operation, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ScoreboardSlot, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Team, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(TemplateMirror, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(TemplateRotation, QString)

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Uuid, QUuid)

/*
 * InternalGreedyStringNode represent a unquoted string contains characters
 * except whitespaces, quote punctiations and backslashes.
 *
 * Namespaced ID: ___:greedy_string
 */
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(InternalGreedyString, QString)
/*
 * InternalRegexPatternNode represent a regular expression pattern.
 * Agument nodes of this type must be put at the end of a command branch.
 *
 * Namespaced ID: ___:regex_pattern
 */
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(InternalRegexPattern, QRegularExpression)

#undef DECLARE_SINGLE_VALUE_ARGUMENT_CLASS

#endif // SINGLEVALUENODE_H
