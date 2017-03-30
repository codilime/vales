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
#ifndef FAKE_SAMPLER_H
#define FAKE_SAMPLER_H

#include "util/sampling/isampler.h"

namespace veles {
namespace util {

class FakeSampler : public ISampler {
 public:
  explicit FakeSampler(const QByteArray &data) : ISampler(data) {}
 protected:
  size_t getRealSampleSize() const override;
 private:
  char getSampleByte(size_t index) const override;
  const char* getData() const override;
  size_t getFileOffsetImpl(size_t index) const override;
  size_t getSampleOffsetImpl(size_t address) const override;
  ResampleData* prepareResample(SamplerConfig *sc) override;
  void applyResample(ResampleData *rd) override;
  void cleanupResample(ResampleData *rd) override;
  FakeSampler* cloneImpl() const override;
};

}  // namespace util
}  // namespace veles

#endif
