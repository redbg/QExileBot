#include "MainWindow.h"

void MainWindow::InitAccountView()
{
    // setModel
    ui->AccountView->setModel(&m_AccountModel);
    // 设置右键菜单
    ui->AccountView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->AccountView->addActions(ui->menuAccount->actions());
    // 交替行的颜色
    ui->AccountView->setAlternatingRowColors(true);
    // 自动调整宽度
    ui->AccountView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

// 保存设置
void MainWindow::saveSettings()
{
    // json
    QJsonObject JsonObject;
    JsonObject.insert("AccountList", m_AccountModel.toJson());
    QJsonDocument JsonDocument(JsonObject);
    QByteArray Json = JsonDocument.toJson();

    // save
    QSettings settings;
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("windowState", this->saveState());
    settings.setValue("Json", Json);
}

// 读取设置
void MainWindow::readSettings()
{
    // read
    QSettings settings;
    this->restoreGeometry(settings.value("geometry").toByteArray());
    this->restoreState(settings.value("windowState").toByteArray());
    QByteArray Json = settings.value("Json").toByteArray();

    // json
    QJsonDocument JsonDocument = QJsonDocument::fromJson(Json);
    QJsonObject JsonObject = JsonDocument.object();
    m_AccountModel.fromJson(JsonObject.value("AccountList"));
}

void MainWindow::on_actionInsert_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        m_AccountModel.insertRow(currentIndex.row());
    }
    else
    {
        m_AccountModel.insertRow(0);
    }
}

void MainWindow::on_actionRemove_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        m_AccountModel.removeRow(currentIndex.row());
    }
}

void MainWindow::on_actionMyAccount_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        QUrl url("https://www.pathofexile.com/my-account");
        QByteArray POESESSID = m_AccountModel.at(currentIndex.row())->m_POESESSID.toLatin1();
        QNetworkCookie cookie("POESESSID", POESESSID);
        QWebEnginePage *page = m_WebEngineView.page();
        QWebEngineProfile *profile = page->profile();
        QWebEngineCookieStore *cookieStore = profile->cookieStore();

        cookieStore->deleteAllCookies();
        cookieStore->setCookie(cookie, url);

        m_WebEngineView.setWindowTitle(POESESSID);
        m_WebEngineView.load(url);
        m_WebEngineView.show();
    }
}

void MainWindow::on_actionStart_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        m_AccountModel.start(currentIndex);
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        m_AccountModel.quit(currentIndex);
    }
}

void MainWindow::on_actionCharacterList_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        Account *account = m_AccountModel.at(currentIndex.row());

        if (account->isRunning())
        {
            m_CharacterView.setModel(&account->m_ExileClient->m_CharacterModel);
            m_CharacterView.show();
        }
    }
}
