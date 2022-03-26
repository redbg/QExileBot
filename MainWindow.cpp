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
    m_AccountModel.insertRow(ui->AccountView->currentIndex().row());
}

void MainWindow::on_actionRemove_triggered()
{
    m_AccountModel.removeRow(ui->AccountView->currentIndex().row());
}

void MainWindow::on_actionMyAccount_triggered()
{
    QUrl url("https://www.pathofexile.com/my-account");
    QModelIndex currentIndex = ui->AccountView->currentIndex();
    QByteArray POESESSID = m_AccountModel.data(currentIndex).toByteArray();
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