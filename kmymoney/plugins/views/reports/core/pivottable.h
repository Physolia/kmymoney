/*
    SPDX-FileCopyrightText: 2005-2006 Ace Jones <acejones@users.sourceforge.net>
    SPDX-FileCopyrightText: 2005-2018 Thomas Baumgart <tbaumgart@kde.org>
    SPDX-FileCopyrightText: 2007-2014 Alvaro Soliverez <asoliverez@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PIVOTTABLE_H
#define PIVOTTABLE_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QMap>
#include <QSet>
#include <QList>
#include <QDate>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "reporttable.h"
#include "pivotgrid.h"
#include "reportaccount.h"

class MyMoneyReport;

namespace reports {
class KReportChartView;
}

namespace reports
{

/**
  * Calculates a 'pivot table' of information about the transaction database.
  * Based on pivot tables in MS Excel, and implemented as 'Data Pilot' in
  * OpenOffice.Org Calc.
  *
  *              | Month,etc
  * -------------+------------
  * Expense Type | Sum(Value)
  *   Category   |
  *
  * This is a middle-layer class, between the UI and the engine.  The
  * MyMoneyReport class holds only the CONFIGURATION parameters.  This
  * class actually does the work of retrieving the data from the engine
  * and formatting it for the user.
  *
  * @author Ace Jones
  *
  * @short
**/
class PivotTable : public ReportTable
{
    KMM_MYMONEY_UNIT_TESTABLE

public:
    /**
      * Create a Pivot table style report
      *
      * @param _report The configuration parameters for this report
      */
    explicit PivotTable(const MyMoneyReport& _report);

    /**
      * virtual Destructor
      */
    virtual ~PivotTable() {}

    /**
      * Render the report body to an HTML stream.
      *
      * @return QString HTML string representing the report body
      */
    QString renderHTML() const final override;
    /**
      * Render the report to a comma-separated-values stream.
      *
      * @return QString CSV string representing the report
      */
    QString renderCSV() const final override;

    /**
      * Render the report to a graphical chart
      *
      * @param view The KReportChartView into which to draw the chart.
      */
    void drawChart(KReportChartView& view) const final override;

    /**
      * Dump the report's HTML to a file
      *
      * @param file The filename to dump into
      * @param context unused, but provided for interface compatibility
      */
    void dump(const QString& file, const QString& context = QString()) const final override;

    /**
      * Returns the grid generated by the report
      *
      */
    PivotGrid grid() {
        return m_grid;
    }

protected:
    void init();    // used for debugging the constructor

private:

    PivotGrid m_grid;

    QStringList m_columnHeadings;
    int m_numColumns;
    QDate m_beginDate;
    QDate m_endDate;
    bool m_runningSumsCalculated;
    int m_startColumn;
    /**
      * For budget-vs-actual reports only, maps each account to the account which holds
      * the budget for it.  If an account is not contained in this map, it is not included
      * in the budget.
      */
    QMap<QString, QString> m_budgetMap;

    /**
      * This list contains the types of PivotGridRows that are going to be shown in the report
      */
    QList<ERowType> m_rowTypeList;

    /**
     * This list contains the i18n headers for the column types
     */
    QStringList m_columnTypeHeaderList;

    /**
      * This method returns the formatted value of @a amount with
      * a possible @a currencySymbol added and @a prec fractional digits.
      * @a currencySymbol defaults to be empty and @a prec defaults to 2.
      *
      * If @a amount is negative the formatted value is enclosed in an
      * HTML font tag to modify the color to reflect the user settings for
      * negative numbers.
      *
      * Example: 1.23 is returned as '1.23' whereas -1.23 is returned as
      *          @verbatim <font color="rgb($red,$green,$blue)">-1.23</font>@endverbatim
      *          with $red, $green and $blue being the actual value for the
      *          chosen color.
      */
    QString coloredAmount(const MyMoneyMoney& amount, const QString& currencySymbol, int prec) const;

    QSet<QString> m_openingBalanceTransactions;

protected:
    /**
      * Creates a row in the grid if it doesn't already exist
      *
      * Downstream assignment functions will assume that this row already
      * exists, so this function creates a row of the needed length populated
      * with zeros.
      *
      * @param outergroup The outer row group
      * @param row The row itself
      * @param recursive Whether to also recursively create rows for our parent accounts
      */
    void createRow(const QString& outergroup, const ReportAccount& row, bool recursive);

    /**
      * Assigns a value into the grid
      *
      * Adds the given value to the value which already exists at the specified grid position
      *
      * @param outergroup The outer row group
      * @param row The row itself
      * @param column The column
      * @param value The value to be added in
      * @param budget Whether this is a budget value (@p true) or an actual
      *               value (@p false). Defaults to @p false.
      * @param stockSplit Whether this is a stock split (@p true) or an actual
      *                   value (@p false). Defaults to @p false.
      */
    void assignCell(const QString& outergroup, const ReportAccount& row, int column, MyMoneyMoney value, bool budget = false, bool stockSplit = false);

    /**
      * Create a row for each included account. This is used when
      * the config parameter isIncludingUnusedAccount() is true
      */
    void createAccountRows();

    /**
      * Record the opening balances of all qualifying accounts into the grid.
      *
      * For accounts opened before the report period, places the balance into the '0' column.
      * For those opened during the report period, places the balance into the appropriate column
      * for the month when it was opened.
      */
    void calculateOpeningBalances();

    /**
      * Calculate budget mapping
      *
      * For budget-vs-actual reports, this creates a mapping between each account
      * in the user's hierarchy and the account where the budget is held for it.
      * This is needed because the user can budget on a given account for that
      * account and all its descendants.  Also if NO budget is placed on the
      * account or any of its parents, the account is not included in the map.
      */
    void calculateBudgetMapping();

    /**
      * Calculate the running sums.
      *
      * After calling this method, each cell of the report will contain the running sum of all
      * the cells in its row in this and earlier columns.
      *
      * For example, consider a row with these values:
      *   01 02 03 04 05 06 07 08 09 10
      *
      * After calling this function, the row will look like this:
      *   01 03 06 10 15 21 28 36 45 55
      */
    void calculateRunningSums();
    void calculateRunningSums(PivotInnerGroup::iterator& it_row);

    /**
      * This method calculates the difference between a @a budgeted and an @a
      * actual amount. The calculation is based on the type of the
       * @a repAccount. The difference value is calculated as follows:
      *
      * If @a repAccount is of type eMyMoney::Account::Type::Income
      *
      * @code
      *      diff = actual - budgeted
      * @endcode
      *
      * If @a repAccount is of type eMyMoney::Account::Type::Expense
      *
      * @code
      *      diff = budgeted - actual
      * @endcode
      *
      * In all other cases, 0 is returned.
      */
    void calculateBudgetDiff();

    /**
      * This method calculates forecast for a report
      */
    void calculateForecast();

    /**
     * This method inserts units to be used to display prices
     */
    void fillBasePriceUnit(ERowType rowType);

    /**
     * This method collects the first date for which there is a price for every security
     */
    QMap<QString, QDate> securityFirstPrice();

    /**
      * This method calculates moving average for a report
      */
    void calculateMovingAverage();

    /**
      * Calculate the row and column totals
      *
      * This function will set the m_total members of all the TGrid objects.  Be sure the values are
      * all converted to the base currency first!!
      *
      */
    void calculateTotals();

    /**
      * Convert each value in the grid to the base currency
      *
      */
    void convertToBaseCurrency();

    /**
      * Convert each value in the grid to the account/category's deep currency
      *
      * See AccountDescriptor::deepCurrencyPrice() for a description of 'deep' currency
      *
      */
    void convertToDeepCurrency();

    /**
      * Turn month-long columns into larger time periods if needed
      *
      * For example, consider a row with these values:
      *   01 02 03 04 05 06 07 08 09 10
      *
      * If the column pitch is 3 (i.e. quarterly), after calling this function,
      * the row will look like this:
      *   06 15 26 10
      */
    void collapseColumns();

    /**
      * Determine the proper column headings based on the time periods covered by each column
      *
      */
    void calculateColumnHeadings();

    /**
      * Helper methods for collapseColumns
      *
      */
    void accumulateColumn(int destcolumn, int sourcecolumn);
    void clearColumn(int column);

    /**
      * Calculate the column of a given date.  This is the absolute column in a
      * hypothetical report that covers all of known time.  In reality an actual
      * report will be a subset of that.
      *
      * @param _date The date
      */
    int columnValue(const QDate& _date) const;

    /**
      * Calculate the date of the last day covered by a given column.
      *
      * @param column The column
      */
    QDate columnDate(int column) const;

    /**
      * Returns the balance of a given cell. Throws an exception once calculateRunningSums() has been run.
      */
    MyMoneyMoney cellBalance(const QString& outergroup, const ReportAccount& _row, int column, bool budget);

    /**
      * Draws a PivotGridRowSet in a chart for the given ERowType
      */
    unsigned drawChartRowSet(int rowNum, const bool seriesTotals, const bool accountSeries, KReportChartView& chartView, const PivotGridRowSet& rowSet, const ERowType rowType) const;

    /**
      * Loads m_rowTypeList with the list of PivotGridRow types that the reporttable
      * should show
      */
    void loadRowTypeList();

    /**
      * If not in expert mode, include all subaccounts for each selected
      * investment account
      */
    void includeInvestmentSubAccounts();

    /**
      * Returns the column which holds the current date
      * Returns -1 if the current date is not within range
      */
    int currentDateColumn();
};


}
#endif
// PIVOTTABLE_H
