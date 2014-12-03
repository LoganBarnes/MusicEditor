#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "view.h"
//#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QGLFormat qglFormat;
    qglFormat.setVersion(3, 3);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    qglFormat.setSampleBuffers(true);

    ui->setupUi(this);

    QGridLayout *gridLayout = new QGridLayout(ui->view);
    View *view = new View(qglFormat, this);
    gridLayout->addWidget(view, 0, 1);



//    QSettings qtSettings("CS123");
//    restoreGeometry(qtSettings.value("geometry").toByteArray());
//    restoreState(qtSettings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_view;
}

