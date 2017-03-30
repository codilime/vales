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
#ifndef ROOTFILEBLOBITEM_H
#define ROOTFILEBLOBITEM_H

#include <QList>
#include <QObject>

#include <data/field.h>

#include "dbif/types.h"
#include "ui/fileblobitem.h"

namespace veles {
namespace ui {

class RootFileBlobItem : public FileBlobItem {
  Q_OBJECT

 public:
  explicit RootFileBlobItem(dbif::ObjectHandle obj, QObject *parent = 0);

 private slots:
  void gotChildrenResponse(veles::dbif::PInfoReply reply);
};

}  // namespace ui
}  // namespace veles

#endif  // ROOTFILEBLOBITEM_H
