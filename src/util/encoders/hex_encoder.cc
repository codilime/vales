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
#include "util/encoders/hex_encoder.h"

namespace veles {
namespace util {
namespace encoders {

QString HexEncoder::encodingDisplayName() {
  return "Hex";
}

QString HexEncoder::decodingDisplayName() {
  return "Hex";
}

QString HexEncoder::encode(const QByteArray& data) {
  QByteArray ba = data.toHex();
  return QString::fromLatin1(ba);
}

QByteArray HexEncoder::decode(const QString& str) {
  return QByteArray::fromHex(str.toLatin1());
}

}  // namespace encoders
}  // namespace util
}  // namespace veles
