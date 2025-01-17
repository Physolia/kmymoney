/*
    SPDX-FileCopyrightText: 2010-2015 Allan Anderson <agander93@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022 Alexander Kuznetsov <alx.kuzza@gmail.com>
*/

#include "csvutil.h"
// #include <QStringList>
// #include <QVector>
#include <QLocale>
#include <QRegularExpression>

Parse::Parse()
    : m_lastLine(0)
    , m_symbolFound(false)
    , m_invalidConversion(false)
{
    m_fieldDelimiters = {QLatin1Char(','), QLatin1Char(';'), QLatin1Char(':'), QLatin1Char('\t')};
    m_textDelimiters = {QLatin1Char('"'), QLatin1Char('\'')};
    m_decimalSymbols = {QLatin1Char('.'), QLatin1Char(',')};
    m_thousandsSeparators = {QLatin1Char(','), QLatin1Char('.')};

    setFieldDelimiter(FieldDelimiter::Comma);
    setTextDelimiter(TextDelimiter::DoubleQuote);
    setDecimalSymbol(DecimalSymbol::Dot);
}

Parse::~Parse()
{
}

QStringList Parse::parseLine(const QString& data)
{
    QStringList listOut;
    QString cell;
    int quoteCount = 0;
    int increment = 1;
    bool quoted = false;
    auto fill_quotes = [&](const QChar& c) {
        if (c == m_textDelimiter) {
            if (quoteCount == 0 && cell.length() == 0) {
                quoted = true;
                increment = 2;
            }
            quoteCount++;
            return false;
        }

        if (quoted) {
            if (c == m_fieldDelimiter && quoteCount % 2 == 0 && quoteCount >= 2)
                quoteCount -= 2;
        }

        while (quoteCount >= increment) {
            cell += m_textDelimiter;
            quoteCount -= increment;
        }

        if (c == m_fieldDelimiter && !quoteCount) {
            listOut.append(cell.trimmed());
            quoteCount = 0;
            quoted = false;
            increment = 1;
            cell.clear();
            return false;
        }

        return true;
    };

    for (const auto c : data) {
        if (!fill_quotes(c))
            continue;
        cell.append(c);
    }
    fill_quotes(m_fieldDelimiter);

    return listOut;
}

QStringList Parse::parseFile(const QString& buf)
{
    int lineCount = 0;
    bool inQuotes = false;
    QString line;
    QStringList lines;

    Q_FOREACH (const auto chr, buf) {
        if (chr == m_textDelimiter) {
            line += chr;
            inQuotes = !inQuotes;
            continue;
        } else if (chr == QLatin1Char('\r') || chr == QLatin1Char('\n')) {
            if (inQuotes) {
                line += QLatin1Char('~');
                continue;
            }
            if (line.isEmpty())
                continue;
            ++lineCount;
            lines += line;
            line.clear();
        } else {
            line += chr;
            continue;
        }
    }
    // in case the buffer does not end with a CR or LF we
    // end up here and add the line nevertheless
    if (!line.isEmpty()) {
        ++lineCount;
        lines.append(line);
    }
    m_lastLine = lineCount;
    return lines;
}

void Parse::setFieldDelimiter(const FieldDelimiter _d)
{
    if (_d == FieldDelimiter::Auto)
        return;
    m_fieldDelimiter = m_fieldDelimiters.at((int)_d);
}

void Parse::setTextDelimiter(const TextDelimiter _d)
{
    m_textDelimiter = m_textDelimiters.at((int)_d);
}

void Parse::setDecimalSymbol(const DecimalSymbol _d)
{
    if (_d == DecimalSymbol::Auto)
        return;
    m_decimalSymbol = m_decimalSymbols.at((int)_d);
    if (_d == DecimalSymbol::Comma)
        m_thousandsSeparator = m_thousandsSeparators.at((int)ThousandSeparator::Dot);
    else
        m_thousandsSeparator = m_thousandsSeparators.at((int)ThousandSeparator::Comma);
}

QChar Parse::decimalSymbol(const DecimalSymbol _d)
{
    if (_d == DecimalSymbol::Auto)
        return QChar();
    return m_decimalSymbols.at((int)_d);
}

int Parse::lastLine()
{
    return m_lastLine;
}

QString Parse::possiblyReplaceSymbol(const QString& str)
{
    // examples given if decimal symbol is '.' and thousand symbol is ','
    m_symbolFound = false;
    m_invalidConversion = true;

    QString txt = str.trimmed();
    if (txt.isEmpty()) // empty strings not allowed
        return txt;

    bool parentheses = false;
    if (txt.contains(QLatin1Char('('))) // (1.23) is in fact -1.23
        parentheses = true;

    int length = txt.length();
    int decimalIndex = txt.indexOf(m_decimalSymbol);
    int thouIndex = txt.lastIndexOf(m_thousandsSeparator);

    txt.remove(QRegularExpression(QStringLiteral("\\D.,-+"))); // remove all non-digits
    txt.remove(m_thousandsSeparator);

    if (txt.isEmpty()) // empty strings not allowed
        return txt;

    if (decimalIndex == -1) { // e.g. 1 ; 1,234 ; 1,234,567; 12,
        if (thouIndex == -1 || thouIndex == length - 4) { // e.g. 1 ; 1,234 ; 1,234,567
            txt.append(QLocale().decimalPoint() + QLatin1String("00")); // e.g. 1.00 ; 1234.00 ; 1234567.00
            m_invalidConversion = false;
        }
        return txt;
    }
    m_symbolFound = true; // decimal symbol found

    if (decimalIndex < thouIndex) // e.g. 1.234,567 ; 1.23,45
        return txt;

    m_invalidConversion = false; // it cannot be true after this point
    txt.replace(m_decimalSymbol, QLocale().decimalPoint()); // so swap it

    if (decimalIndex == length - 1) // e.g. 1. ; 123.
        txt.append(QLatin1String("00"));

    if (parentheses)
        txt.prepend(QLatin1Char('-'));

    return txt;
}

bool Parse::invalidConversion()
{
    return m_invalidConversion;
}

//--------------------------------------------------------------------------------------------------------------------------------
