/***************************************************************************
                          kreportchartview.h
                             -------------------
    begin                : Sat May 22 2004
    copyright            : (C) 2004-2005 by Ace Jones
    email                : <ace.j@hotpop.com>
                           Thomas Baumgart <ipwizard@users.sourceforge.net>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KREPORTCHARTVIEW_H
#define KREPORTCHARTVIEW_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

#include <QLabel>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <KDChartWidget>
#include <KDChartChart>
#include <KDChartAbstractDiagram>
#include <KDChartAbstractCoordinatePlane>

// ----------------------------------------------------------------------------
// Project Includes

#include "pivotgrid.h"
#include "mymoneyreport.h"

using namespace KDChart;

namespace reports
{

class KReportChartView: public Chart
{
public:
  KReportChartView(QWidget* parent);
  ~KReportChartView() {}

  /**
    * Returns the labels for the X axis
    * @see m_abscissaNames
    */
  QStringList& abscissaNames() {
    return m_abscissaNames;
  }

  /**
    * Draw the chart for a pivot table report
    */
  void drawPivotChart(const PivotGrid &grid, const MyMoneyReport &config, int numberColumns, const QStringList& columnHeadings, const QList<ERowType>& rowTypeList, const QStringList& columnTypeHeaderList);

  /**
    * Draw a limit chart
    * @param limit is either a maximum credit or minimum balance for an account
    */
  void drawLimitLine(const double limit);

  /**
    * Remove the chart legend
    */
  void removeLegend();

private:

  /**
    * Draw a PivotGridRowSet in a chart
    */
  unsigned drawPivotRowSet(int rowNum, const PivotGridRowSet& rowSet, const ERowType rowType, const QString& legendText, int startColumn = 1, int endColumn = 0);

  /**
    * Set the data value
    */
  void setDataCell(int row, int column, const double data);

  /**
    * Set the tooltip for a data value
    */
  void setCellTip(int row, int column, QString tip);

  /**
    * Make sure the model has the right size
    */
  void justifyModelSize(int rows, int columns);

  /**
    * Adjust line width of all datasets
    */
  void setLineWidth(const int lineWidth);

  /**
    * Set the accountSeries
    * @see m_accountSeries
    */
  void setAccountSeries(bool accountSeries) {
    m_accountSeries = accountSeries;
  }

  /**
    * Returns accountSeries
    * @see m_accountSeries
    */
  bool accountSeries() {
    return m_accountSeries;
  }

  /**
    * Set the seriesTotals
    * @see m_seriesTotals
    */
  void setSeriesTotals(bool seriesTotals) {
    m_seriesTotals = seriesTotals;
  }

  /**
    * Returns accountSeries
    * @see m_seriesTotals
    */
  bool seriesTotals() {
    return m_seriesTotals;
  }

  /**
    * Set the number of columns
    * @see m_numColumns
    */
  void setNumColumns(int numColumns) {
    m_numColumns = numColumns;
  }

  /**
    * Returns number of columns
    * @see m_numColumns
    */
  int numColumns() {
    return m_numColumns;
  }

  /**
    * The labels of the X axis
    */
  QStringList m_abscissaNames;

  /**
    * whether series (rows) are accounts (true) or months (false). This causes a lot
    * of complexity in the charts.  The problem is that circular reports work best with
    * an account in a COLUMN, while line/bar prefer it in a ROW.
    */
  bool m_accountSeries;

  /**
    * whether to limit the chart to use series totals only.  Used for reports which only
    * show one dimension (pie)
    */
  bool m_seriesTotals;

  /**
    * Number of columns on the report
    */
  int m_numColumns;

  /**
    * Model to store chart data
    */
  QStandardItemModel m_model;

  /**
    * whether to skip values if zero
    */
  bool m_skipZero;

  /**
    * The cached background brush obtained from the style.
    */
  QBrush m_backgroundBrush;

  /**
    * The cached foreground brush obtained from the style.
    */
  QBrush m_foregroundBrush;
};

} // end namespace reports

#endif // KREPORTCHARTVIEW_H