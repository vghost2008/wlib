#ifndef QTSINGLEAPPLICATION_H
#define QTSINGLEAPPLICATION_H

#include <QApplication>

class QtLocalPeer;
class QtSingleApplication : public QApplication
{
    Q_OBJECT
public:
    QtSingleApplication(int &argc, char **argv, bool GUIenabled = true);
    QtSingleApplication(const QString &id, int &argc, char **argv);
#if defined(Q_WS_X11)
    QtSingleApplication(Display *dpy, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
    QtSingleApplication(Display *dpy, int &argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE cmap = 0);
#endif

    bool isRunning();
    QString id() const;
    void setActivationWindow(QWidget* aw, bool activateOnMessage = true);
    QWidget* activationWindow() const;
    bool event(QEvent *event);
    
public:
    bool sendMessage(const QString &message, int timeout = 5000);
    void activateWindow();
    
//Obsolete methods:
public:
    void initialize(bool = true)
        { isRunning(); }
#if defined(Q_WS_X11)
    QtSingleApplication(Display* dpy, const QString &id, int argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
#endif
// end obsolete methods

Q_SIGNALS:
    void messageReceived(const QString &message);
    void fileOpenRequest(const QString &file);
private:
    void sysInit(const QString &appId = QString());
    QtLocalPeer *peer;
    QWidget *actWin;
};

#endif // QTSINGLEAPPLICATION_H
