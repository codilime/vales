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
#ifndef VELES_DB_UNIVERSE_H
#define VELES_DB_UNIVERSE_H

#include <QObject>
#include <QStringList>
#include "data/bindata.h"
#include "db/types.h"
#include "dbif/types.h"
#include "parser/parser.h"

namespace veles {
namespace db {

class ParserWorker : public QObject {
  Q_OBJECT

 public slots:
  void parse(
      veles::dbif::ObjectHandle blob, MethodRunner *runner, QString parser_id,
      quint64 start = 0,
      veles::dbif::ObjectHandle parent_chunk = veles::dbif::ObjectHandle());

 public:
  void registerParser(parser::Parser *parser);
  QStringList parserIdsList();
  ~ParserWorker();

 private:
  QList<parser::Parser *> _parsers;

signals:
  void newParser(QString id);
};

class Universe : public QObject {
  Q_OBJECT

  PLocalObject root_;
  ParserWorker *parser_;

 public slots:
  void getInfo(veles::db::PLocalObject obj, InfoGetter *getter, veles::dbif::PInfoRequest req, bool once);
  void runMethod(veles::db::PLocalObject obj, MethodRunner *runner, veles::dbif::PMethodRequest req);

 public:
  Universe(ParserWorker *parser) : parser_(parser) {}
  dbif::ObjectHandle handle(PLocalObject obj);
  void setRoot(PLocalObject root) { root_ = root; }
  ~Universe();
  QThread *parserThread() {
    return parser_->thread();
  }
  ParserWorker* parser() {return parser_;}

 signals:
  void parse(
      veles::dbif::ObjectHandle blob, MethodRunner *runner, QString parser_id,
      quint64 start = 0,
      veles::dbif::ObjectHandle parent_chunk = veles::dbif::ObjectHandle());
};
};
};

#endif
