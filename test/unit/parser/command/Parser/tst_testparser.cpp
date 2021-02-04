#include <QtTest>
#include <QCoreApplication>

#include "../../../../../src/parsers/command/parser.h"

using namespace Command;

class TestParser : public QObject
{
    Q_OBJECT

public:
    TestParser();
    ~TestParser();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void parseBool();
    void parseDouble();
    void parseFloat();
    void parseInteger();
    void parseString();
    void parse();
    void parserIdToMethodName();
};

TestParser::TestParser() {
    qDebug() << "TestParser::TestParser";
}

TestParser::~TestParser() {
}

void TestParser::initTestCase() {
}

void TestParser::cleanupTestCase() {
}

void TestParser::test_case1() {
    Parser parser(this, "text");

    QCOMPARE(parser.text(), "text");
}

void TestParser::parseBool() {
    Parser                   parser(this, "true");
    QSharedPointer<BoolNode> result(parser.brigadier_bool(&parser));

    QCOMPARE(result->isVaild(), true);
    QCOMPARE(result->value(), true);

    parser.setText("false");
    result = QSharedPointer<BoolNode>(parser.brigadier_bool(&parser));
    QCOMPARE(result->isVaild(), true);
    QCOMPARE(result->value(), false);

    parser.setText("false");
    result = QSharedPointer<BoolNode>(parser.brigadier_bool(&parser));
    QCOMPARE(result->isVaild(), true);
    QCOMPARE(result->value(), false);


    parser.setText("simp");
    result = QSharedPointer<BoolNode>(parser.brigadier_bool(&parser));
    QCOMPARE(result->isVaild(), false);
}

void TestParser::parseDouble() {
    Parser                     parser(this, "3.1415926535897932");
    QScopedPointer<DoubleNode> result(parser.brigadier_double(&parser));

    QCOMPARE(result->isVaild(), true);
    QCOMPARE(result->value(), 3.1415926535897932);
}

void TestParser::parseFloat() {
    Parser                    parser(this, "99.9");
    QScopedPointer<FloatNode> result(parser.brigadier_float(&parser, { { "max",
                                                                100 } }));

    QCOMPARE(result->isVaild(), true);
    QVERIFY(qFuzzyCompare(result->value(), (float)99.9));
}

void TestParser::parseInteger() {
    Parser                      parser(this, "66771508");
    QScopedPointer<IntegerNode> result(
        parser.brigadier_integer(&parser, { { "min", 1000000 } }));

    QCOMPARE(result->isVaild(), true);
    QCOMPARE(result->value(), 66771508);
}

void TestParser::parse() {
    QElapsedTimer timer;

    timer.start();

    Parser::setSchema(":/minecraft/info/commands.json");
    Parser parser(this, "gamemode creative");

    auto *result = parser.parse();
    QCOMPARE(result->toString(),
             "RootNode[2](LiteralNode(gamemode), LiteralNode(creative))");

    parser.setText("schedule clear test");
    result = parser.parse();
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(schedule), LiteralNode(clear), StringNode(\"test\"))");

    parser.setText("gamerule keepInventory true");
    result = parser.parse();
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(gamerule), LiteralNode(keepInventory), BoolNode(true))");

    parser.setText("gamerule doWeatherCycle false");
    result = parser.parse();
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(gamerule), LiteralNode(doWeatherCycle), BoolNode(false))");

    parser.setText("gamerule doDaylightCycle");
    result = parser.parse();
    QCOMPARE(result->toString(),
             "RootNode[2](LiteralNode(gamerule), LiteralNode(doDaylightCycle))");

    qDebug() << "Elapsed time in ms:" << timer.elapsed();
}

void TestParser::parserIdToMethodName() {
    QCOMPARE(Parser::parserIdToMethodName("brigadier:float"),
             "brigadier_float");
    QCOMPARE(Parser::parserIdToMethodName("minecraft:block_pos"),
             "minecraft_blockPos");
    QCOMPARE(Parser::parserIdToMethodName("minecraft:nbt_compound_tag"),
             "minecraft_nbtCompoundTag");
}

void TestParser::parseString() {
    Parser                     parser(this, "firstWord secondWord");
    QSharedPointer<StringNode> result(
        parser.brigadier_string(&parser, { { "type", "word" } }));

    QCOMPARE(result->isVaild(), true);
    QCOMPARE(result->value(), "firstWord");

    parser.setText("cho xin it da cuoi");
    result = QSharedPointer<StringNode>(
        parser.brigadier_string(&parser, { { "type", "greedy" } }));
    QCOMPARE(result->isVaild(), true);
    QCOMPARE(result->value(), "cho xin it da cuoi");

    parser.setText("\"Speed Upgrade for Blocks\"");
    result = QSharedPointer<StringNode>(
        parser.brigadier_string(&parser, { { "type", "phrase" } }));
    QCOMPARE(result->isVaild(), true);
    QCOMPARE(result->value(), "Speed Upgrade for Blocks");
}

QTEST_GUILESS_MAIN(TestParser)

#include "tst_testparser.moc"
