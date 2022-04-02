#include "Account.h"

QJsonValue Account::toJson()
{
    QJsonObject JsonObject;

    for (int i = 0; i < this->metaObject()->propertyCount(); i++)
    {
        const char *key = this->metaObject()->property(i).name();
        QJsonValue value = QJsonValue::fromVariant(this->property(key));
        JsonObject.insert(key, value);
    }

    return JsonObject;
}

void Account::fromJson(QJsonValue JsonValue)
{
    if (JsonValue.isObject())
    {
        QJsonObject JsonObject = JsonValue.toObject();

        for (int i = 0; i < this->metaObject()->propertyCount(); i++)
        {
            const char *key = this->metaObject()->property(i).name();
            QJsonValue value = JsonObject.value(key);
            this->setProperty(key, value);
        }
    }
}

void Account::run()
{
    m_BackendError.clear();

    // ExileClient
    m_ExileClient = new ExileClient;
    // m_ExileGame
    m_ExileGame = new ExileGame;
    m_ExileGame->m_ExileClient = m_ExileClient;

    // signal_BackendError
    connect(m_ExileClient, &ExileClient::signal_BackendError, this, &Account::on_BackendError, Qt::DirectConnection);
    connect(m_ExileGame, &ExileGame::signal_BackendError, this, &Account::on_BackendError, Qt::DirectConnection);

    // errorOccurred
    connect(m_ExileClient, &ExileClient::errorOccurred, this, &Account::quit, Qt::DirectConnection);
    connect(m_ExileGame, &ExileGame::errorOccurred, this, &Account::quit, Qt::DirectConnection);

    // 登录成功
    connect(
        m_ExileClient, &ExileClient::LoginSuccess, this, [=](const QString &AccountName)
        { m_AccountName = AccountName; },
        Qt::DirectConnection);

    // 选择角色成功,进入游戏
    connect(m_ExileClient, &ExileClient::SelectCharacterSuccess,
            m_ExileGame, &ExileGame::connectToGameServer,
            Qt::DirectConnection);

    // 连接登录服务器
    m_ExileClient->connectToLoginServer(Global::hostName, Global::port, m_Email, m_Password);

    // Loop
    this->exec();

    // delete
    delete m_ExileGame;
    delete m_ExileClient;
}