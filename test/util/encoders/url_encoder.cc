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
#include "gtest/gtest.h"
#include "util/encoders/url_encoder.h"

#include <QString>
#include <QByteArray>

namespace veles {
namespace util {
namespace encoders {

TEST(UrlEncoder, encode) {
  auto encoder = UrlEncoder();
  EXPECT_EQ(encoder.encode(QByteArray::fromHex("")), "");
  EXPECT_EQ(encoder.encode(QByteArray::fromHex("414243444546")),
            "%41%42%43%44%45%46");
}

TEST(UrlEncoder, decode) {
  auto encoder = UrlEncoder();
  EXPECT_EQ(encoder.decode(""), QByteArray::fromHex(""));
  EXPECT_EQ(encoder.decode("A%42%43%44E%46"),
            QByteArray::fromHex("414243444546"));
}

TEST(UrlEncoder, validate) {
  auto encoder = UrlEncoder();
  auto test = [&encoder](const QByteArray& bytes) {
    EXPECT_EQ(encoder.decode(encoder.encode(bytes)), bytes);
  };
  auto test_str = [&encoder, &test](const QString& str) {
    QByteArray bytes = str.toLatin1();
    test(bytes);
  };

  test_str("aaaa");
  test_str("test_string_asdf\n\r\r\n\0asdf");
  test_str("");
  test_str(QString(1024, 'x'));
  QByteArray all_bytes;
  for (int i = 0; i < 256; i++) {
    all_bytes.append(static_cast<char>(i));
  }
  test(all_bytes);
}

}  // namespace encoders
}  // namespace util
}  // namespace veles
