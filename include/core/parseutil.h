#pragma once
#ifndef PARSEUTIL_H
#define PARSEUTIL_H

#include "heallocation.h"
#include "log.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QRegularExpression>

enum TokenClass {
    Number,
    Operator,
    Error,
};

class Token {
public:
    Token(QString value = "", QString type = "") {
        this->value = value;
        this->type = TokenClass::Operator;
        if (type == "decimal" || type == "hex") {
            this->type = TokenClass::Number;
            this->operatorPrecedence = -1;
        } else if (type == "operator") {
            this->operatorPrecedence = precedenceMap[value];
        } else if (type == "error") {
            this->type = TokenClass::Error;
        }
    }
    static QMap<QString, int> precedenceMap;
    QString value;
    TokenClass type;
    int operatorPrecedence; // only relevant for operator tokens
};

class ParseUtil
{
public:
    ParseUtil();
    void set_root(QString);
    static QString readTextFile(QString);
    void strip_comment(QString*);
    QList<QStringList>* parseAsm(QString);
    int evaluateDefine(QString, QMap<QString, int>*);
    QStringList readCArray(QString text, QString label);
    QMap<QString, QString> readNamedIndexCArray(QString text, QString label);
    QString readCIncbin(QString text, QString label);
    QMap<QString, int> readCDefines(QString filename, QStringList prefixes, QMap<QString, int> = QMap<QString, int>());
    void readCDefinesSorted(QString, QStringList, QStringList*, QMap<QString, int> = QMap<QString, int>());
    QList<QStringList>* getLabelMacros(QList<QStringList>*, QString);
    QStringList* getLabelValues(QList<QStringList>*, QString);
    bool tryParseJsonFile(QJsonDocument *out, QString filepath);
    bool ensureFieldsExist(QJsonObject obj, QList<QString> fields);

    // Returns the 1-indexed line number for the definition of scriptLabel in the scripts file at filePath.
    // Returns 0 if a definition for scriptLabel cannot be found.
    static int getScriptLineNumber(const QString &filePath, const QString &scriptLabel);
    static int getRawScriptLineNumber(QString text, const QString &scriptLabel);
    static int getPoryScriptLineNumber(QString text, const QString &scriptLabel);
    static QString &removeStringLiterals(QString &text);
    static QString &removeLineComments(QString &text, const QString &commentSymbol);
    static QString &removeLineComments(QString &text, const QStringList &commentSymbols);

    static QStringList splitShellCommand(QStringView command);

private:
    QString root;
    QString text;
    QString file;
    QList<Token> tokenizeExpression(QString expression, QMap<QString, int>* knownIdentifiers);
    QList<Token> generatePostfix(QList<Token> tokens);
    int evaluatePostfix(QList<Token> postfix);
    void error(QString message, QString expression);
};

#endif // PARSEUTIL_H
