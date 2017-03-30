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
#ifndef DIGRAM_H
#define DIGRAM_H

#include "visualization/digram.h"

#include <stdint.h>

#include <vector>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_2_Core>

#include "visualization/base.h"

namespace veles {
namespace visualization {

class DigramWidget : public VisualizationWidget {
  Q_OBJECT

 public:
  explicit DigramWidget(QWidget *parent = 0);
  ~DigramWidget();


 protected:
  void refresh(AdditionalResampleDataPtr ad) override;
  bool initializeVisualizationGL() override;

  void resizeGLImpl(int w, int h) override;
  void paintGLImpl() override;

  void initShaders();
  void initTextures();
  void initGeometry();

 private:
  QOpenGLShaderProgram program_;
  QOpenGLTexture *texture_;

  QOpenGLBuffer square_vertex_;
  QOpenGLVertexArrayObject vao_;
};

}  // namespace visualization
}  // namespace veles

#endif
