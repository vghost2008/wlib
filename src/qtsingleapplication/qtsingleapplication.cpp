#include "qtsingleapplication.h"

#include "qtlocalpeer.h"

#include <QtGui/QWidget>

#include <QtGui/QFileOpenEvent>

void QtSingleApplication::sysInit(const QString &appId)

{

	actWin = 0;
	peer = new QtLocalPeer(this, appId);
	connect(peer, &QtLocalPeer::messageReceived, this,&QtSingleApplication::messageReceived);
}

QtSingleApplication::QtSingleApplication(int &argc, char **argv, bool GUIenabled)

    : QApplication(argc, argv, GUIenabled)

{

    sysInit();

}

QtSingleApplication::QtSingleApplication(const QString &appId, int &argc, char **argv)

    : QApplication(argc, argv)

{

    sysInit(appId);

}

#if defined(Q_WS_X11)

QtSingleApplication::QtSingleApplication(Display* dpy, Qt::HANDLE visual, Qt::HANDLE colormap)

    : QApplication(dpy, visual, colormap)

{

    sysInit();

}

QtSingleApplication::QtSingleApplication(Display *dpy, int &argc, char **argv, Qt::HANDLE visual, Qt::HANDLE cmap)

    : QApplication(dpy, argc, argv, visual, cmap)

{

    sysInit();

}

QtSingleApplication::QtSingleApplication(Display* dpy, const QString &appId,

    int argc, char **argv, Qt::HANDLE visual, Qt::HANDLE colormap)

    : QApplication(dpy, argc, argv, visual, colormap)

{

    sysInit(appId);

}

#endif

bool QtSingleApplication::event(QEvent *event)

{

    if (event->type() == QEvent::FileOpen) {

        QFileOpenEvent *foe = static_cast<QFileOpenEvent*>(event);

        emit fileOpenRequest(foe->file());

        return true;
    }
    return QApplication::event(event);
}
	

bool QtSingleApplication::isRunning()
{

    return peer->isClient();

} 	

bool QtSingleApplication::sendMessage(const QString &message, int timeout)
{
    return peer->sendMessage(message, timeout);
}

QString QtSingleApplication::id() const
{

    return peer->applicationId();
 	

}

void QtSingleApplication::setActivationWindow(QWidget *aw, bool activateOnMessage)
{

    actWin = aw;

    if (activateOnMessage)
        connect(peer, &QtLocalPeer::messageReceived, this, &QtSingleApplication::activateWindow);
    else
        disconnect(peer, &QtLocalPeer::messageReceived, this, &QtSingleApplication::activateWindow);


}	

QWidget* QtSingleApplication::activationWindow() const
{
    return actWin;
}

void QtSingleApplication::activateWindow()
{
    if (actWin) {
        actWin->setWindowState(actWin->windowState() & ~Qt::WindowMinimized);
        actWin->raise();
        actWin->activateWindow();
		actWin->showMaximized();
    }
}	
