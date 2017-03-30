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
#include "ui/fileblobitem.h"

#include "dbif/universe.h"

namespace veles {
namespace ui {

bool FileBlobItem::operator<(const FileBlobItem &other) {
  uint64_t start, end;
  if (!range(&start, &end)) {
    return false;
  }
  uint64_t start_other, end_other;
  if (!other.range(&start_other, &end_other)) {
    return true;
  }
  return start < start_other;
}

int FileBlobItem::childrenCount() { return children_.size(); }

FileBlobItem::FileBlobItem(QString name, QString value, QString comment,
                           uint64_t start, uint64_t end, QObject *parent)
    : QObject(parent),
      name_(name),
      comment_(comment),
      value_(value),
      start_(start),
      end_(end) {}

void FileBlobItem::insertingChildrenHandle(FileBlobItem *item, bool before,
                                           int count) {
  emit insertingChildren(item, before, count);
}

void FileBlobItem::removingChildrenHandle(FileBlobItem *item, bool before) {
  emit removingChildren(item, before);
}

bool compareItems(FileBlobItem *a, FileBlobItem *b) { return *a < *b; }

bool FileBlobItem::sortChildren() {
  if (std::is_sorted(children_.begin(), children_.end(), compareItems)) {
    return false;
  }
  std::sort(children_.begin(), children_.end(), compareItems);
  return true;
}

void FileBlobItem::dataUpdatedHandle(FileBlobItem *item) {
  emit dataUpdated(item);
  if (sortChildren()) {
    emit removingChildren(this, true);
    auto childrenCopy = children_;
    children_.clear();
    emit removingChildren(this, false);

    emit insertingChildren(this, true, children_.size());
    children_ = childrenCopy;
    emit insertingChildren(this, false, children_.size());
  }
}

void FileBlobItem::addChildren(const QList<FileBlobItem *> &children) {
  if (children.size() == 0) {
    return;
  }

  emit insertingChildren(this, true, children.size());

  for (auto &child : children) {
    children_.append(child);
    connect(child, SIGNAL(insertingChildren(FileBlobItem *, bool, int)), this,
            SLOT(insertingChildrenHandle(FileBlobItem *, bool, int)));
    connect(child, SIGNAL(removingChildren(FileBlobItem *, bool)), this,
            SLOT(removingChildrenHandle(FileBlobItem *, bool)));
    connect(child, SIGNAL(dataUpdated(FileBlobItem *)), this,
            SLOT(dataUpdatedHandle(FileBlobItem *)));
  }

  emit insertingChildren(this, false, children.size());
}

FileBlobItem *FileBlobItem::child(int num) {
  if (num >= children_.size()) {
    return nullptr;
  }
  return children_[num];
}

int FileBlobItem::childIndex(FileBlobItem *child) {
  return children_.indexOf(child);
}

QString FileBlobItem::comment() { return comment_; }

QString FileBlobItem::value() { return value_; }

QString FileBlobItem::name() { return name_; }

bool FileBlobItem::range(uint64_t *start, uint64_t *end) const {
  *start = start_;
  *end = end_;
  return true;
}

void FileBlobItem::setComment(QString comment) { comment_ = comment; }

void FileBlobItem::setFields(QString name, QString comment, uint64_t start,
                             uint64_t end) {
  name_ = name;
  comment_ = comment;
  start_ = start;
  end_ = end;
}

dbif::ObjectHandle FileBlobItem::objectHandle() { return dataObj_; }

bool FileBlobItem::isRemovable() {
  return objectHandle();
}

void FileBlobItem::removeOldChildren() {
  bool hasChilds = children_.size() > 0;
  if (hasChilds) {
    emit removingChildren(this, true);
  }

  qDeleteAll(children_);
  children_.clear();

  if (hasChilds) {
    emit removingChildren(this, false);
  }
}

}  // namespace ui
}  // namespace veles
