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
#ifndef VELES_UI_DOCKWIDGET_H
#define VELES_UI_DOCKWIDGET_H

#include <set>
#include <map>

#include <QObject>
#include <QDropEvent>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QString>
#include <QStringList>
#include <QRubberBand>
#include <QPointer>
#include <QIcon>
#include <QProxyStyle>
#include <QDateTime>

namespace veles {
namespace ui {

class MainWindowWithDetachableDockWidgets;

/*****************************************************************************/
/* QProxyStyleForDockWidgetWithIconOnTitleBar */
/*****************************************************************************/

class QProxyStyleForDockWidgetWithIconOnTitleBar : public QProxyStyle {
  Q_OBJECT

public:
  QProxyStyleForDockWidgetWithIconOnTitleBar(QStyle* default_style);

  void drawControl(QStyle::ControlElement element, const QStyleOption *option,
      QPainter *painter, const QWidget *widget = nullptr) const override;
};

/*****************************************************************************/
/* DockWidget */
/*****************************************************************************/

class DockWidget : public QDockWidget {
  Q_OBJECT

 public:
  DockWidget();
  virtual ~DockWidget();
  const QAction* maximizeHereAction();
  static DockWidget* getParentDockWidget(QObject* obj);

 public slots:
  void displayContextMenu(const QPoint& pos);
  void moveToDesktop();
  void moveToWindow();
  void detachToNewTopLevelWindow();
  void detachToNewTopLevelWindowAndMaximize();
  void topLevelChangedNotify(bool top_level);
  void switchTitleBar(bool is_default);
  void centerTitleBarOnPosition(QPoint pos);
  void splitHorizontally();
  void splitVertically();

 protected:
  void moveEvent(QMoveEvent *event) Q_DECL_OVERRIDE;
  void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;
  QMenu* createMoveToDesktopMenu();
  QMenu* createMoveToWindowMenu();
  QAction* createMoveToNewWindowAction();
  QAction* createMoveToNewWindowAndMaximizeAction();
  void createSplitActions();

 protected:
  static constexpr int max_ticks_ = 4;
  static constexpr int step_msec_ = 100;

  int timer_id_;
  int ticks_;
  QMenu* context_menu_;
  QAction* detach_action_;
  QAction* maximize_here_action_;
  QAction* split_horizontally_action_;
  QAction* split_vertically_action_;
  QWidget* empty_title_bar_;
};

/*****************************************************************************/
/* DockWidgetVisibilityGuard
 *
 * Sometimes it's useful to have a QDockWidget within a QMainWindow
 * within a QDockWidget within a QMainWindow. It seems that Qt (5.5/5.7)
 * doesn't handle that situation correctly: when an outer QDockWidget is
 * docked/undocked or its parent is changed, all internal QDockWidgets
 * are closed.
 *
 * Purpose of this class is to keep an internal QDockWidget visible (actually
 * to reopen it) when such a situation happens.
 */
/*****************************************************************************/

class DockWidgetVisibilityGuard : public QObject {
  Q_OBJECT

 public:
  DockWidgetVisibilityGuard(QDockWidget* dock_widget);
  void setEnabled(bool enabled);
  bool isEnabled();
  bool eventFilter(QObject* watched, QEvent* event) override;

 public slots:
  void innerDockWidgetVisibilityChanged(bool visible);

 private:
  QDockWidget* findOuterQDockWidget(QDockWidget* dock_widget);
  bool enabled_;
  QDockWidget* inner_dock_widget_;

  /*
   * Unfortunately it seems that we don't have a better method to detect that
   * undesired visibility change has occurred than to detect that two events
   * happened in a sequence:
   * 1. Visibility of inner QDockWidget is set to false.
   * 2. One of the following conditions is met:
   *   - Parent of an outer QDockWidget is changed.
   *   - Native window id of an outer QDockWidget is changed.
   *
   * We assume that both events happened in a sequence when they both
   * happened closely in time.
   */
  bool inner_dock_widget_has_been_hidden_;
  QTime time_stamp_;
  static constexpr int treshold_msec_ = 100;
};

/*****************************************************************************/
/* TabBarEventFilter */
/*****************************************************************************/

class TabBarEventFilter : public QObject {
  Q_OBJECT

 public:
  TabBarEventFilter(QObject* parent = nullptr);

 public slots:
  void tabMoved(int from, int to);

 protected:
  bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
  virtual bool mouseMove(QTabBar* tab_bar, QMouseEvent* event);
  virtual bool mouseButtonPress(QTabBar* tab_bar, QMouseEvent* event);
  virtual bool mouseButtonRelease(QTabBar* tab_bar, QMouseEvent* event);
  virtual bool mouseButtonDblClick(QTabBar* tab_bar, QMouseEvent* event);

  QTabBar* dragged_tab_bar_;
  int dragged_tab_index_;
  QPoint drag_init_pos_;
  static const int k_drag_treshold_ = 5;
};

/*****************************************************************************/
/* View */
/*****************************************************************************/

class View : public QMainWindow {
  Q_OBJECT

 public:
  View(QString category, QString path);
  ~View();
  virtual void reapplySettings() {};

 signals:
  void maximize();

 protected:
  void getOrCreateIcon(QString category, QString icon_path);
  static void deleteIcons();
  static std::map<QString, QIcon*> icons_;
};

/*****************************************************************************/
/* MainWindowWithDetachableDockWidgets */
/*****************************************************************************/

class MainWindowWithDetachableDockWidgets: public QMainWindow {
  Q_OBJECT

 public:
  MainWindowWithDetachableDockWidgets(QWidget* parent = nullptr);
  virtual ~MainWindowWithDetachableDockWidgets();
  DockWidget* addTab(QWidget *widget, const QString &title,
      DockWidget* sibling = nullptr);
  void bringDockWidgetToFront(QDockWidget* dock_widget);
  void moveDockWidgetToWindow(DockWidget* dock_widget);
  void findTwoNonTabifiedDocks(DockWidget*& sibling1, DockWidget*& sibling2);
  DockWidget* findDockNotTabifiedWith(DockWidget* dock_widget);
  DockWidget* findDockNotTabifiedWith(QWidget* widget);
  QDockWidget* findSibling(QDockWidget* dock_widget);
  void setDockWidgetsWithNoTitleBars(bool no_title_bars);
  bool dockWidgetsWithNoTitleBars();
  QDockWidget* tabToDockWidget(QTabBar* tab_bar, int index);
  QTabBar* dockWidgetToTab(QDockWidget* dock_widget);
  void splitDockWidget2(QDockWidget* first, QDockWidget* second,
      Qt::Orientation orientation);
  void showRubberBand(bool show);

  static void splitDockWidget2(QMainWindow* main_window, QDockWidget* first,
      QDockWidget* second, Qt::Orientation orientation);
  static MainWindowWithDetachableDockWidgets* getParentMainWindow(
      QObject* obj);
  static bool intersectsWithAnyMainWindow(DockWidget* dock_widget);
  static MainWindowWithDetachableDockWidgets* getParentCandidateForDockWidget(
      DockWidget* dock_widget);
  static const std::set<MainWindowWithDetachableDockWidgets*>& getMainWindows();
  static MainWindowWithDetachableDockWidgets* getFirstMainWindow();
  static MainWindowWithDetachableDockWidgets* getOrCreateWindowForScreen(
      int screen);
  static MainWindowWithDetachableDockWidgets* getOwnerOfDockWidget(
        DockWidget* dock_widget);
  static void hideAllRubberBands();

 public slots:
  void dockLocationChanged(Qt::DockWidgetArea area);
  void tabCloseRequested(int index);
  void childAddedNotify(QObject* child);
  void updateDockWidgetTitleBars();
  void updateCloseButtonsAndIconsOnTabBars();
  void updateDocksAndTabs();

 signals:
  void childAdded(QObject* child);
  void childRemoved();

 protected:
  bool event(QEvent* event) Q_DECL_OVERRIDE;
  static bool splitDockWidgetImpl(QMainWindow* main_window,
      QDockWidget* first, QDockWidget* second, Qt::Orientation orientation);

 private:
  static std::set<MainWindowWithDetachableDockWidgets*> main_windows_;
  static MainWindowWithDetachableDockWidgets* first_main_window_;
  static int last_created_window_id_;

  TabBarEventFilter* tab_bar_event_filter_;
  QRubberBand* rubber_band_;

  bool dock_widgets_with_no_title_bars_;
  bool icons_on_tabs_;
};

}  // namespace ui
}  // namespace veles

#endif // VELES_UI_DOCKWIDGET_H
