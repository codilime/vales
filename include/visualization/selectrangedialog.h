/*
 * Copyright 2016-2017 CodiLime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef VELES_VISUALIZATION_SELECTRANGEDIALOG_H
#define VELES_VISUALIZATION_SELECTRANGEDIALOG_H

#include <QDialog>
#include <QString>

#include <ui/spinbox.h>

namespace Ui {
class SelectRangeDialog;
}

namespace veles {
namespace visualization {

class SelectRangeDialog : public QDialog {
  Q_OBJECT

 public:
  explicit SelectRangeDialog(QWidget *parent = 0);
  ~SelectRangeDialog();

  void setRange(size_t min_address, size_t max_address);
  void resetNumberFormat();
  size_t getStartAddress();
  size_t getEndAddress();

 private slots:
  void addressChanged(int);
  void numberBaseChanged(veles::ui::SpinBox*, const QString&);

 private:
  void setAddressRanges();

  Ui::SelectRangeDialog *ui;

};

}  // namespace visualization
}  // namespace veles

#endif
