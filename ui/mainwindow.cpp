#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvas.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QGLFormat qglFormat;
    qglFormat.setVersion(3, 2);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    qglFormat.setSampleBuffers(true);
    ui->setupUi(this);

    QGridLayout *gridLayout = new QGridLayout(ui->canvas);
    m_canvas = new Canvas(qglFormat, this);
    gridLayout->addWidget(m_canvas, 0, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

