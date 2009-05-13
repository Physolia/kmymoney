/***************************************************************************
                          ksettingscolors.h
                             -------------------
    copyright            : (C) 2005 by Thomas Baumgart
    email                : ipwizard@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSETTINGSCOLORS_H
#define KSETTINGSCOLORS_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "ui_ksettingscolorsdecl.h"

class KSettingsColorsDecl : public QWidget, public Ui::KSettingsColorsDecl
{
public:
  KSettingsColorsDecl( QWidget *parent ) : QWidget( parent ) {
    setupUi( this );
  }
};

class KSettingsColors : public KSettingsColorsDecl
{
  Q_OBJECT

public:
  KSettingsColors(QWidget* parent = 0);
  ~KSettingsColors();
};
#endif

