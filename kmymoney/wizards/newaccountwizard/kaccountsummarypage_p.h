/***************************************************************************
                             kaccountsummarypage.cpp
                             -------------------
    begin                : Tue Sep 25 2006
    copyright            : (C) 2007 Thomas Baumgart
    email                : Thomas Baumgart <ipwizard@users.sourceforge.net>
                           (C) 2017 by Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KACCOUNTSUMMARYPAGE_P_H
#define KACCOUNTSUMMARYPAGE_P_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "ui_kaccountsummarypage.h"

#include "wizardpage_p.h"

namespace NewAccountWizard
{
  class Wizard;

  class AccountSummaryPagePrivate : public WizardPagePrivate<Wizard>
  {
    Q_DISABLE_COPY(AccountSummaryPagePrivate)

  public:
    AccountSummaryPagePrivate(QObject* parent) :
      WizardPagePrivate<Wizard>(parent),
      ui(new Ui::KAccountSummaryPage)
    {
    }

    ~AccountSummaryPagePrivate()
    {
      delete ui;
    }

    Ui::KAccountSummaryPage *ui;
  };  
}

#endif
