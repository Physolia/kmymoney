/***************************************************************************
                         kguiutils.cpp  -  description
                            -------------------
   begin                : Fri Jan 27 2006
   copyright            : (C) 2006 Tony Bloomfield
   email                : Tony Bloomfield <tonybloom@users.sourceforge.net>
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// ----------------------------------------------------------------------------
// QT Includes
 // No need for QDateEdit, QSpinBox, etc., since these always return values

#include <QCheckBox>
#include <q3listbox.h>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <qwidget.h>
#include <q3hbox.h>
#include <qspinbox.h>
#include <QApplication>

// ----------------------------------------------------------------------------
// KDE Includes
#include <klistwidget.h>

// ----------------------------------------------------------------------------
// Project Includes

#include "kguiutils.h"
#include "kmymoneyglobalsettings.h"

 /**************************************************************************
  *                                                                        *
  * The MandatoryFieldGroup code is courtesy of                            *
  * Mark Summerfield in Qt Quarterly                                       *
  * http://doc.trolltech.com/qq/qq11-mandatoryfields.html                  *
  *                                                                        *
  * Enhanced by Thomas Baumgart to support the lineedit field of a         *
  * a QComboBox.                                                           *
  *                                                                        *
  **************************************************************************/

void kMandatoryFieldGroup::add(QWidget *widget)
{

  if (!m_widgets.contains(widget)) {
    if (qobject_cast<QCheckBox*>( widget))
      connect(qobject_cast<QCheckBox*>(widget),
               SIGNAL(clicked()),
               this, SLOT(changed()));

    else if (qobject_cast<QComboBox*>(widget)) {
      QComboBox* combo = qobject_cast<QComboBox*>(widget);
      QLineEdit* lineedit = combo->lineEdit();
      if(lineedit) {
        connect(lineedit, SIGNAL(textChanged(const QString&)), this, SLOT(changed()));
      } else {
        connect(combo, SIGNAL(highlighted(int)), this, SLOT(changed()));
      }
    }

    else if (qobject_cast<QLineEdit*>(widget))
      connect(qobject_cast<QLineEdit*>(widget),
               SIGNAL(textChanged(const QString&)),
               this, SLOT(changed()));

    else if (qobject_cast<QSpinBox*>(widget))
      connect(qobject_cast<QSpinBox*>(widget),
               SIGNAL(valueChanged(const QString&)),
                      this, SLOT(changed()));

    else if (qobject_cast<Q3ListBox*>(widget))
      connect(qobject_cast<Q3ListBox*>(widget),
               SIGNAL(selectionChanged()),
                      this, SLOT(changed()));

    else if (qobject_cast<KListWidget*>(widget))
      connect(qobject_cast<KListWidget*>(widget),
               SIGNAL(itemSelectionChanged()),
                      this, SLOT(changed()));

    else {
      qWarning("MandatoryFieldGroup: unsupported class %s", ( widget->metaObject()->className() ));
      return;
    }

    QPalette palette = widget->palette();
    palette.setColor(widget->backgroundRole(), KMyMoneyGlobalSettings::requiredFieldColor());
    widget->setPalette(palette);
    m_widgets.append(widget);
    changed();
  }
}


void kMandatoryFieldGroup::remove(QWidget *widget)
{
  widget->setPalette(QApplication::palette());
  m_widgets.removeOne(widget);
  changed();
}


void kMandatoryFieldGroup::setOkButton(QPushButton *button)
{
  if (m_okButton && m_okButton != button)
    m_okButton->setEnabled(true);
  m_okButton = button;
  changed();
}


void kMandatoryFieldGroup::changed(void)
{
  bool enable = true;
  QList<QWidget *>::ConstIterator i;
  for (i = m_widgets.constBegin(); i != m_widgets.constEnd(); ++i) {
    QWidget *widget = *i;
    // disabled widgets don't count
    if(!(widget->isEnabled())) {
      continue;
    }
    if (qobject_cast<QCheckBox*>(widget)) {
      if ((qobject_cast<QCheckBox*>(widget))->checkState() == Qt::PartiallyChecked) {
        enable = false;
        break;
      } else
        continue;
    }
    if (qobject_cast<QComboBox*>(widget)) {
      if ((qobject_cast<QComboBox*>(widget))->currentText().isEmpty()) {
        enable = false;
        break;
      } else
        continue;
    }
    if (qobject_cast<QLineEdit*>(widget)) {
      if ((qobject_cast<QLineEdit*>(widget))->text().isEmpty()) {
        enable = false;
        break;
      } else
        continue;
    }
    if ((qobject_cast<Q3ListBox*>(widget))) {
      if ((qobject_cast<Q3ListBox*>(widget))->selectedItem() == 0) {
        enable = false;
        break;
      } else
        continue;
    }
    if ((qobject_cast<KListWidget*>(widget))) {
      if ((qobject_cast<KListWidget*>(widget))->selectedItems().count() == 0) {
        enable = false;
        break;
      } else
        continue;
    }
  }

  if (m_okButton)
    m_okButton->setEnabled(enable);
  m_enabled = enable;

  emit stateChanged();
  emit stateChanged(enable);
}


void kMandatoryFieldGroup::clear(void)
{
  QList<QWidget *>::Iterator i;
  for (i = m_widgets.begin(); i != m_widgets.end(); ++i)
    (*i)->setPalette(QApplication::palette());
  m_widgets.clear();
  if (m_okButton) {
    m_okButton->setEnabled(true);
    m_okButton = 0;
    m_enabled = true;
  }
}


#include "kguiutils.moc"

