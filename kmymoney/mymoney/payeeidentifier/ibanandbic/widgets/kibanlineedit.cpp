/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Christian David <c.david@christian-david.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kibanlineedit.h"

#include "ibanvalidator.h"

KIbanLineEdit::KIbanLineEdit(QWidget* parent)
  : KLineEdit(parent)
{
  ibanValidator *const validatorPtr = new ibanValidator;
  setValidator( validatorPtr );
  connect(validatorPtr, SIGNAL(feedback(KMyMoneyValidationFeedback::MessageType,QString)), this, SIGNAL(validatorFeedback(KMyMoneyValidationFeedback::MessageType,QString)));
}

const ibanValidator* KIbanLineEdit::validator() const
{
  return qobject_cast<const ibanValidator*>(KLineEdit::validator());
}
