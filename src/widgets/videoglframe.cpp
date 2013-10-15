/***************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "videoglframe.h"

#include <QtCore/QDebug>
#include <QtOpenGL>

#include <math.h>

#include <lib/videorenderer.h>
#include <lib/videomodel.h>

#include <GL/glu.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

class ThreadedPainter2: public QObject {
   Q_OBJECT
public:
   friend class VideoGLFrame;
   ThreadedPainter2(VideoGLFrame* frm,QGLWidget* wdg);
   virtual ~ThreadedPainter2(){
//       QThread::currentThread()->quit();
      if (m_Data)
         free(m_Data);
   }

   //GL
   QPointF anchor;
   float scale;
   float rot_x, rot_y, rot_z;
   GLuint tile_list;
   bool isRendering;

   //Render
   VideoRenderer* m_pRenderer;

private:
   QGLWidget* m_pW;
   VideoGLFrame* m_pFrm;
   QMutex mutex;
   char* m_Data;
   QSize m_ActiveSize;

   //Methods
   void saveGLState();
   void restoreGLState();

public Q_SLOTS:
   void draw(QPainter* p);
   void copyFrame();
   void reset();
   void rendererStopped();
   void rendererStarted();
Q_SIGNALS:
   void changed();
};

ThreadedPainter2::ThreadedPainter2(VideoGLFrame* frm,QGLWidget* wdg) : QObject(), m_pRenderer(nullptr),
   m_pW(wdg), rot_x(0.0f),rot_y(0.0f),rot_z(0.0f),scale(0.8f),isRendering(false),m_pFrm(frm),
   m_Data(nullptr),tile_list(0)
{
}

void ThreadedPainter2::reset()
{
   m_pRenderer = nullptr;
}

void ThreadedPainter2::rendererStopped()
{
   QMutexLocker locker(&mutex);
   isRendering = false;
}

void ThreadedPainter2::rendererStarted()
{
   QMutexLocker locker(&mutex);
   m_pW->makeCurrent();
   isRendering = true;
}

void ThreadedPainter2::copyFrame()
{
   if (m_pRenderer) {
      m_pRenderer->mutex()->lock();
      const QByteArray raw = m_pRenderer->currentFrame();
      if (m_Data)
         free(m_Data);
      m_Data = (char*)malloc(raw.size()*sizeof(char));
      memcpy(m_Data,raw.data(),raw.size());
      m_ActiveSize = m_pRenderer->activeResolution();
      m_pRenderer->mutex()->unlock();
      emit changed();
   }
}

void ThreadedPainter2::draw(QPainter* p)
{
   Q_UNUSED(p)
   if (m_pRenderer && isRendering) {
//       QMutexLocker locker(&mutex);
//       m_pRenderer->mutex()->lock();

      // save the GL state set for QPainter
      saveGLState();

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      if (m_ActiveSize.width() && m_ActiveSize.height() && m_Data)
         gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_ActiveSize.width(), m_ActiveSize.height(), GL_BGRA, GL_UNSIGNED_BYTE, m_Data);

      // draw into the GL widget
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glFrustum(-1, 1, -1, 1, 10, 100);
      glTranslatef(0.0f, 0.0f, -15.0f);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glViewport(0, 0, m_pW->width(), m_pW->height());
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_MULTISAMPLE);
      glEnable(GL_CULL_FACE);
      glScalef(-1.0f*scale, -1.0f*scale, 1.0f*scale);
      glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
      glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
      glRotatef(rot_z, 0.0f, 0.0f, 1.0f);

      // draw background
      glPushMatrix();
      glScalef(1.7f, 1.7f, 1.7f);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glCallList(tile_list);
      glPopMatrix();

      // restore the GL state that QPainter expects
      restoreGLState();
      glDeleteTextures(1, &texture);
   }
}


 VideoGLFrame::VideoGLFrame(QGLWidget *parent)
     : QObject(parent),m_pParent(parent),
     m_pPainter(new ThreadedPainter2(this,parent))
 {

//    Should work, does not
//    QThread* t = new QThread(this);
//    connect(t, SIGNAL(started()), m_pPainter, SLOT(rendererStarted()));
//    m_pPainter->moveToThread(t);
//    t->start();
   connect(m_pPainter,SIGNAL(changed()),this,SLOT(slotEmitChanged()));

   m_pPainter->tile_list = glGenLists(1);
   glNewList(m_pPainter->tile_list, GL_COMPILE);
   glBegin(GL_QUADS);
   {
      //Define the video plane
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
   }
   glEnd();
   glEndList();
 }

VideoGLFrame::~VideoGLFrame()
{
   QMutexLocker locker(&m_pPainter->mutex);
   glDeleteLists(m_pPainter->tile_list, 1);
}

void VideoGLFrame::paintEvent(QPainter* p)
{
   m_pPainter->draw(p);
}

void ThreadedPainter2::saveGLState()
{
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
}

void ThreadedPainter2::restoreGLState()
{
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glPopAttrib();
}

///Set widget renderer
void VideoGLFrame::setRenderer(VideoRenderer* renderer)
{
//    if (m_pPainter->m_pRenderer && m_pPainter->m_pRenderer->isRendering())
//       m_pPainter->rendererStopped();
   QMutexLocker locker(&m_pPainter->mutex);
   if (m_pPainter->m_pRenderer) {
      disconnect(m_pPainter->m_pRenderer,SIGNAL(frameUpdated()),m_pPainter,SLOT(copyFrame()));
      disconnect(m_pPainter->m_pRenderer,SIGNAL(started()),m_pPainter,SLOT(rendererStarted()));
      disconnect(m_pPainter->m_pRenderer,SIGNAL(stopped()),m_pPainter,SLOT(rendererStopped()));
   }
   m_pPainter->m_pRenderer = renderer;
   if (m_pPainter->m_pRenderer) {
      connect(m_pPainter->m_pRenderer,SIGNAL(frameUpdated()),m_pPainter,SLOT(copyFrame()));
      connect(m_pPainter->m_pRenderer,SIGNAL(started()),m_pPainter,SLOT(rendererStarted()));
      connect(m_pPainter->m_pRenderer,SIGNAL(stopped()),m_pPainter,SLOT(rendererStopped()));
      connect(m_pPainter->m_pRenderer,SIGNAL(destroyed()),m_pPainter,SLOT(reset()));
//       setSizeIncrement(1,((float)m_pPainter->m_pRenderer->activeResolution().height()/(float)m_pPainter->m_pRenderer->activeResolution().width()));
      if (m_pPainter->thread()->isRunning())
         m_pPainter->isRendering = true;
   }
}

///Force widget aspect ratio
int VideoGLFrame::heightForWidth( int w ) const
{
   if (m_pPainter->m_pRenderer  )
   if (m_pPainter->m_pRenderer)
      return w*((float)m_pPainter->m_pRenderer->activeResolution().height()/(float)m_pPainter->m_pRenderer->activeResolution().width());
   return w*.75f;
}

QSize VideoGLFrame::sizeHint() const
{
   if (m_pPainter->m_pRenderer) {
      return m_pPainter->m_pRenderer->activeResolution();
   }
   return QSize(100,75);
}

void VideoGLFrame::slotEmitChanged()
{
   emit changed();
}

void VideoGLFrame::setRotZ(float rot)
{
   m_pPainter->rot_z = rot;
}

void VideoGLFrame::setRotY(float rot)
{
   m_pPainter->rot_y = rot;
}

void VideoGLFrame::setRotX(float rot)
{
   m_pPainter->rot_x = rot;
}

void VideoGLFrame::setScale(float scale)
{
   m_pPainter->scale = scale;
}

void VideoGLFrame::setAnchor(const QPointF& point)
{
   m_pPainter->anchor = point;
}

QPointF VideoGLFrame::anchor() const
{
   return m_pPainter->anchor;
}

float VideoGLFrame::rotZ() const
{
   return m_pPainter->rot_z;
}

float VideoGLFrame::rotY() const
{
   return m_pPainter->rot_y;
}

float VideoGLFrame::rotX() const
{
   return m_pPainter->rot_x;
}

float VideoGLFrame::scale() const
{
   return m_pPainter->scale;
}

#include "videoglframe.moc"
#include "moc_videoglframe.cpp"