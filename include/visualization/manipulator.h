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

#ifndef VELES_VISUALIZATION_MANIPULATOR_H
#define VELES_VISUALIZATION_MANIPULATOR_H

#include <QObject>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QVector3D>

namespace veles {
namespace visualization {

/*****************************************************************************/
/* Manipulator */
/*****************************************************************************/

class Manipulator : public QObject {
  Q_OBJECT

 public:
  Manipulator(QObject* parent = nullptr);
  virtual QMatrix4x4 transform() = 0;
  virtual void initFromMatrix(QMatrix4x4 m) {};
  virtual void update(float dt) {};
  virtual QString manipulatorName() = 0;
  virtual bool processEvent(QObject* watched, QEvent* event);
  virtual bool handlesPause();
};

/*****************************************************************************/
/* TrackballManipulator */
/*****************************************************************************/

class TrackballManipulator : public Manipulator {
  Q_OBJECT

 public:
  TrackballManipulator(QObject* parent = nullptr);
  QMatrix4x4 transform() override;
  void initFromMatrix(QMatrix4x4 m) override;
  void update(float dt) override;
  QString manipulatorName() override;

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;
  virtual bool mouseEvent(QWidget* widget, QMouseEvent* event);
  virtual bool wheelEvent(QWidget* widget, QWheelEvent* event);

  float distance_;
  float min_distance_;
  float max_distance_;
  QVector3D init_position_;
  QQuaternion rotation_;

  bool lmb_drag_;
  QPoint prev_pos_;

  float factor_;
  float time_;
};

/*****************************************************************************/
/* FreeManipulator */
/*****************************************************************************/

class FreeManipulator : public Manipulator {
  Q_OBJECT

 public:
  FreeManipulator(QObject* parent = nullptr);
  QMatrix4x4 transform() override;
  void initFromMatrix(QMatrix4x4 m) override;
  void update(float dt) override;
  QString manipulatorName() override;

  static bool isCtrlButton(int key);

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;
  virtual bool keyboardEvent(QKeyEvent* event);
  virtual bool mouseEvent(QObject* watched, QMouseEvent* event);
  virtual bool wheelEvent(QObject* watched, QWheelEvent* event);

  static const int key_left = Qt::Key_A;
  static const int key_right = Qt::Key_D;
  static const int key_up = Qt::Key_Q;
  static const int key_down = Qt::Key_Z;
  static const int key_forward = Qt::Key_W;
  static const int key_back = Qt::Key_S;

  QVector3D position_;
  QQuaternion eye_rotation_;
  QQuaternion cube_rotation_;

  bool lmb_drag_;
  QPoint prev_pos_;

  bool left_, right_, up_, down_, forward_, back_;
};

/*****************************************************************************/
/* SpinManipulator */
/*****************************************************************************/

class SpinManipulator : public Manipulator {
  Q_OBJECT

 public:
  SpinManipulator(QObject* parent = nullptr);
  QMatrix4x4 transform() override;
  void initFromMatrix(QMatrix4x4 m) override;
  void update(float dt) override;
  QString manipulatorName() override;
  bool handlesPause() override;

 protected:
  float distance_;
  QQuaternion init_rotation_;
  QVector3D init_position_;
  float angle_;
  float factor_;
  float time_;
  QQuaternion base_rotation_;

  const float angular_speed_ = 30.f;  // deg/s
};

}  // namespace visualization
}  // namespace veles

#endif  // VELES_VISUALIZATION_MANIPULATOR_H
