#include "waveform/widgets/qtvsynctestwidget.h"

#include <QPainter>
#include <QtDebug>

#include "moc_qtvsynctestwidget.cpp"
#include "util/performancetimer.h"
#include "waveform/renderers/qtvsynctestrenderer.h"
#include "waveform/renderers/waveformrenderbackground.h"
#include "waveform/renderers/waveformrenderbeat.h"
#include "waveform/renderers/waveformrendererendoftrack.h"
#include "waveform/renderers/waveformrendererpreroll.h"
#include "waveform/renderers/waveformrendermark.h"
#include "waveform/renderers/waveformrendermarkrange.h"
#include "waveform/renderers/waveformwidgetrenderer.h"

QtVSyncTestWidget::QtVSyncTestWidget(const QString& group, QWidget* parent)
        : QOpenGLWidget(parent),
          WaveformWidgetAbstract(group) {
    addRenderer<QtVSyncTestRenderer>();

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);

    m_initSuccess = init();
}

QtVSyncTestWidget::~QtVSyncTestWidget() {
}

void QtVSyncTestWidget::castToQWidget() {
    m_widget = this;
}

void QtVSyncTestWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
}

mixxx::Duration QtVSyncTestWidget::render() {
    PerformanceTimer timer;
    mixxx::Duration t1;
    //mixxx::Duration t2, t3;
    timer.start();
    // QPainter makes QGLContext::currentContext() == context()
    // this may delayed until previous buffer swap finished
    QPainter painter(this);
    t1 = timer.restart();
    draw(&painter, NULL);
    //t2 = timer.restart();
    //qDebug() << "GLVSyncTestWidget "<< t1 << t2;
    return t1; // return timer for painter setup
}
