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
#pragma once

#include <stdlib.h>

#include <QMap>
#include <QVector>

#include "data/bindata.h"

namespace veles {
namespace util {

class EditEngine {
 public:
  explicit EditEngine(int edit_stack_limit = 100) : edit_stack_limit_(edit_stack_limit) {}

  void changeBytes(size_t pos, const QVector<uint64_t>& bytes,
      const QVector<uint64_t>& old_bytes, bool add_to_history = true);

  bool hasUndo() const {return !edit_stack_.isEmpty();}
  /** Undo last changeBytes and returns first byte changed by this operation */
  size_t undo();

  void applyChanges(data::BinData& data, size_t offset = 0, int64_t max_bytes = -1) const;
  QPair<size_t, data::BinData> popFirstChange(uint32_t bindata_width);

  bool isChanged(size_t byte_pos) const;
  uint64_t byteValue(size_t byte_pos) const;
  bool hasChanges() const;

  void clear();

 private:
  int edit_stack_limit_;
  QList<QVector<uint64_t>> edit_stack_data_;
  QList<QPair<size_t, size_t>> edit_stack_;
  QMap<size_t, QVector<uint64_t>> changes_;

  QMap<size_t, QVector<uint64_t>>::const_iterator itFromPos(size_t pos) const;
  QMap<size_t, QVector<uint64_t>> changesFromRange(size_t pos, size_t size) const;
  void removeChanges(size_t pos, size_t size);

};

}  // namespace util
}  // namespace veles
