#include "glrgbwaveformwidget.h"

#include "waveform/renderers/waveformwidgetrenderer.h"
#include "waveform/renderers/waveformrenderbackground.h"
#include "waveform/renderers/glwaveformrendererrgb.h"
#include "waveform/renderers/waveformrendererpreroll.h"
#include "waveform/renderers/waveformrendermark.h"
#include "waveform/renderers/waveformrendermarkrange.h"
#include "waveform/renderers/waveformrendererendoftrack.h"
#include "waveform/renderers/waveformrenderbeat.h"

#include "util/performancetimer.h"

#include <QOpenGLContext>

GLRGBWaveformWidget::GLRGBWaveformWidget(const char* group, QWidget* parent)
        : BaseQOpenGLWidget(group, parent) {
    addRenderer<WaveformRenderBackground>();
    addRenderer<WaveformRendererEndOfTrack>();
    addRenderer<WaveformRendererPreroll>();
    addRenderer<WaveformRenderMarkRange>();
    addRenderer<GLWaveformRendererRGB>();
    addRenderer<WaveformRenderBeat>();
    addRenderer<WaveformRenderMark>();

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);

    if (QOpenGLContext::currentContext() != context()) {
        makeCurrent();
    }
    m_initSuccess = init();
}

GLRGBWaveformWidget::~GLRGBWaveformWidget() {

}

void GLRGBWaveformWidget::castToQWidget() {
    m_widget = static_cast<QWidget*>(static_cast<QOpenGLWidget*>(this));
}

void GLRGBWaveformWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
}

mixxx::Duration GLRGBWaveformWidget::render() {
    PerformanceTimer timer;
    mixxx::Duration t1;
    //mixxx::Duration t2, t3;
    timer.start();
    // QPainter makes QOpenGLContext::currentContext() == context()
    // this may delayed until previous buffer swap finished
    QPainter painter(this);
    t1 = timer.restart();
    draw(&painter, NULL);
    //t2 = timer.restart();
    // glFinish();
    //t3 = timer.restart();
    //qDebug() << "GLVSyncTestWidget "<< t1 << t2 << t3;
    return t1; // return timer for painter setup
}
