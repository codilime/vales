/*
 * Copyright 2018 CodiLime
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

#pragma once

#include <QToolBar>

#include "ui/disasm/widget.h"
#include "ui/dockwidget.h"

namespace veles {
namespace ui {
namespace disasm {

class Tab : public IconAwareView {
  Q_OBJECT

  Widget widget;

 public:
  Tab();

 private:
  void createToolbars();

  QToolBar* tool_bar_;
  QMenu* column_display_menu_;
};

}  // namespace disasm
}  // namespace ui
}  // namespace veles
