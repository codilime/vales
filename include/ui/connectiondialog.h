/*
 * Copyright 2017 CodiLime
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
#ifndef VELES_UI_CONNECTIONDIALOG_H
#define VELES_UI_CONNECTIONDIALOG_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class ConnectionDialog;
}

namespace veles {
namespace ui {

class ConnectionDialog : public QDialog {
  Q_OBJECT

 public:
  ConnectionDialog(QWidget *parent = 0);
  virtual ~ConnectionDialog();

  bool runANewServer();
  QString serverHost();
  int serverPort();
  QString clientInterface();
  QString authenticationKey();
  QString clientName();
  QString databaseFile();

 public slots:
  void serverLocalhost();
  void clientLocalhost();
  void randomKey();
  void userAsClientName();
  void newServerToggled(bool toggled);
  void databaseFileSelected(const QString& file_name);

 protected:
  void showEvent(QShowEvent* event) override;

 private:
  Ui::ConnectionDialog* ui_;
  QFileDialog* file_dialog_;
};

}  // namespace ui
}  // namespace veles

#endif  // VELES_UI_CONNECTIONDIALOG_H
