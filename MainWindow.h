#pragma once
#include "ui_MainWindow.h"
#include "AccountModel.h"
#include <QSettings>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtWebEngineWidgets>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    AccountModel m_AccountModel;
    QWebEngineView m_WebEngineView;

public:
    explicit MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent),
          ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        this->InitAccountView();
        this->readSettings();
    }

    virtual ~MainWindow()
    {
        this->saveSettings();
        delete ui;
    }

public:
    void InitAccountView();
    void saveSettings();
    void readSettings();

private slots:
    void on_actionInsert_triggered();
    void on_actionRemove_triggered();
    void on_actionMyAccount_triggered();
    void on_actionStart_triggered();
};
