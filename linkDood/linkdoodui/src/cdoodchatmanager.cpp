#include "cdoodchatmanager.h"
#include "cdoodchatitem.h"

#include <QDebug>

void CDoodChatManager::initConnect()
{
    qDebug() << Q_FUNC_INFO;
    connect(m_pClient,SIGNAL(chatAvatarChanged(int64,QString)),this,
            SLOT(onChatAvatarChanged(int64,QString)));
    connect(m_pClient,SIGNAL(offlineMsgNotice(IMOfflineMsgList)),this,
            SLOT(onChatOfflineMsgNotice(IMOfflineMsgList)));
   connect(m_pClient,SIGNAL(chatMessageNotice(Msg)),this,
            SLOT(onChatMessageNotice(Msg)));
    connect(m_pClient,SIGNAL(sendMessageResult(bool,QString,QString)),this,
            SLOT(onChatSendMessageResult(bool,QString,QString)));
    connect(m_pClient,SIGNAL(getMessagesResult(bool,QString,MsgList)),this,
            SLOT(onChatGetMessagesResult(bool,QString,MsgList)));
    connect(m_pClient,SIGNAL(removeChatResult(bool)),this,
            SLOT(onChatRemoveChatResult(bool)));
    connect(m_pClient,SIGNAL(deleteMessagesResult(int)),this,
            SLOT(onChatDeleteMessagesResult(int)));
    connect(m_pClient,SIGNAL(uploadAvatarResult),this,
        SLOT(onChatAvatarChanged(long long,QString)));
    connect(m_pClient,SIGNAL(uploadFileResult),this,
        SLOT(onChatUploadFile(long long,QString,int)));
    connect(m_pClient,SIGNAL(fileProgressResult),this,
        SLOT(onChatFileProgress(int,int,QString)));
    connect(m_pClient,SIGNAL(downloadFileResult),this,
        SLOT(onChatDownloadFile(int,QString,long long)));
    connect(m_pClient,SIGNAL(uploadImageResult),this,
        SLOT(onChatupLoadImage(int64,QString,QString,int)));
    connect(m_pClient,SIGNAL(downloadImageResult),this,
        SLOT(onChatDownloadImage(int,QString,int64)));
    connect(m_pClient,SIGNAL(getFileListResult),this,
        SLOT(onChatGetFileList(int,std::vector<MsgFileInfo>)));

    /*    connect(m_pClient,SIGNAL(),this,
            SLOT())*/;
}

void CDoodChatManager::analyticalMessage(MsgList list)
{
    qDebug() << Q_FUNC_INFO << "size = " << list.size();

    if(list.size() <= 0) {
        return;
    }

    QList <QObject* > msglist;
    for(int i = 0; i < list.size(); ++i) {
        if(!m_oChatMap.contains(list.at(i).msgid)) {
            CDoodChatItem *pChatItem = new CDoodChatItem(this);
            pChatItem->setMsgType(list.at(i).msgtype);
            pChatItem->setActiveType(list.at(i).activeType);
            pChatItem->setMsgId(list.at(i).msgid);
            pChatItem->setTargetId(list.at(i).targetid);
            pChatItem->setFromId(list.at(i).fromid);
            pChatItem->setToId(list.at(i).toid);
            pChatItem->setTime(QDateTime::fromString(list.at(i).time, "yyyy-MM-dd hh:mm:ss"));
            pChatItem->setBody(list.at(i).body);
            if(list.at(i).fromid != m_pClient->UserId()){
                pChatItem->setName(name());
            }else{
                pChatItem->setName(m_pClient->userName());
            }

//            if(i == 0) {
//                pChatItem->setShowTime(true);
//            } else {
//                if(i - 1 >= 0) {
//                    pChatItem->setShowTime(checkTimeIsShow(p->lastMessageTime,pChatItem->sendTime()));
//                }
//            }

            msglist.append(pChatItem);
            m_oChatMap[list.at(i).msgid] = pChatItem;
            m_oLastMessageTime = QDateTime::fromString(list.at(i).time, "yyyy-MM-dd hh:mm:ss");
            qDebug() << Q_FUNC_INFO << "pChatItem->body() = " << pChatItem->body();
            qDebug() << Q_FUNC_INFO << "pChatItem->time() = " << pChatItem->time();
        }
    }

    if (msglist.size() <= 0) {
        return;
    }

    int nBeginIndexAndSize = 0;
    int nEndIndex = nBeginIndexAndSize + msglist.size() - 1;

    qDebug() << "Dood === nBeginIndex = " << nBeginIndexAndSize;
    qDebug() << "Dood === nEndIndex = " << nEndIndex;

#if 0
    beginInsertRows(QModelIndex(), nBeginIndexAndSize, nEndIndex);

    for(int i = msglist.size()-1; i>=0 ; --i) {
        _list->insert(nBeginIndexAndSize, msglist.at(i));
    }

    endInsertRows();
#else
    for(int i = msglist.size()-1; i>=0 ; --i) {
        addItemBegin(msglist.at(i));
    }
#endif
    emit itemCountChanged();
    // 消息id与数据库的消息id不一致时查询结果可能会有问题
    m_sBeginMsgId = list.at(0).msgid;
}

void CDoodChatManager::addItemToListViewModel(Msg msg)
{
    qDebug() << Q_FUNC_INFO;
    if(!m_oChatMap.contains(msg.msgid)) {

        CDoodChatItem *pChatItem = new CDoodChatItem(this);
        pChatItem->setMsgType(msg.msgtype);
        pChatItem->setActiveType(msg.activeType);
        pChatItem->setMsgId(msg.msgid);
        pChatItem->setTargetId(msg.targetid);
        pChatItem->setFromId(msg.fromid);
        pChatItem->setToId(msg.toid);
        pChatItem->setName(msg.name);
        pChatItem->setTime(QDateTime::fromString(msg.time, "yyyy-MM-dd hh:mm:ss"));
        pChatItem->setBody(msg.body);
        pChatItem->setName(msg.name);
        m_oLastMessageTime = QDateTime::fromString(msg.time, "yyyy-MM-dd hh:mm:ss");
        addItem(pChatItem);
        m_oChatMap[msg.msgid] = pChatItem;

        qDebug() << Q_FUNC_INFO << "pChatItem->body() = " << pChatItem->body();
        qDebug() << Q_FUNC_INFO << "pChatItem->time() = " << pChatItem->time();
    }
}

CDoodChatManager::CDoodChatManager(LinkDoodClient *client, QObject *parent):
    CDoodListModel(parent), m_pClient(client)
{
    qDebug() << Q_FUNC_INFO;
    qRegisterMetaType<CDoodChatManager*>();
    initConnect();
}

CDoodChatManager::~CDoodChatManager()
{
    exitChat();
}

void CDoodChatManager::sendText(QString text)
{
    qDebug() << Q_FUNC_INFO << "send Text:" << text<<"dd:"<<m_sTargetid;
    Msg msgText;
    msgText.body = text;
    msgText.msgtype = "2";
    msgText.targetid = m_sTargetid;
    msgText.fromid = m_pClient->UserId();
    msgText.name = m_pClient->userName();
    qDebug() << Q_FUNC_INFO << "name:" <<  msgText.name <<"id:" << m_pClient->UserId();
    QDateTime time;
    msgText.time = time.currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    // todo
    msgText.msgid = m_pClient->createMsgId();

    qDebug() << Q_FUNC_INFO << "msg.msgid = " << msgText.msgid;
    qDebug() << Q_FUNC_INFO << "msg.body = " << msgText.body;
    qDebug() << Q_FUNC_INFO << "msg.msgtype = " << msgText.msgtype;
    qDebug() << Q_FUNC_INFO << "msg.targetid = " << msgText.targetid;
    qDebug() << Q_FUNC_INFO << "msg.fromid = " << msgText.fromid;
    qDebug() << Q_FUNC_INFO << "msg.time = " << msgText.time;

    sendMessage(msgText);
}

void CDoodChatManager::sendMessage(Msg msg)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->sendMessage(msg);
    msg.fromid = m_pClient->UserId();
    msg.name = m_pClient->userName();
    qDebug() <<Q_FUNC_INFO<<"sfsdgsdfgfdshfsdhsrhsfhfh:"<<msg.fromid << ":" << msg.targetid;
    addItemToListViewModel(msg);
}

void CDoodChatManager::getMessages(QString targetid, int count)
{
     qDebug() << Q_FUNC_INFO;
     m_pClient->getMessages(targetid,m_sBeginMsgId,count,0);
}

void CDoodChatManager::removeChat(QString targetid)
{
     qDebug() << Q_FUNC_INFO;
     m_pClient->removeChat(targetid);
}

void CDoodChatManager::setMessageRead(QString targetid)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->setMessageRead(targetid,"0");
}

void CDoodChatManager::getUnReadMessages()
{
     qDebug() << Q_FUNC_INFO;
     m_pClient->getUnReadMessages();
}

void CDoodChatManager::deleteMessage(QString targetid, QStringList msgs)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->deleteMessage(targetid,msgs);
}

QString CDoodChatManager::id() const
{
    qDebug() << Q_FUNC_INFO;
    return mId;
}

void CDoodChatManager::setId(const QString &id)
{
    qDebug() << Q_FUNC_INFO;
    mId = id;
    emit idChanged();
}

QString CDoodChatManager::name() const
{
    qDebug() << Q_FUNC_INFO;
    return mName;
}

void CDoodChatManager::setName(const QString &name)
{
    qDebug() << Q_FUNC_INFO;
    mName = name;
    emit nameChanged();
}

void CDoodChatManager::entryChat(const QString &targetid)
{
    qDebug() << Q_FUNC_INFO << targetid;
    m_sTargetid = targetid;
    m_pClient->entryChat(targetid);
}

void CDoodChatManager::exitChat()
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->exitChat(m_sTargetid);
}

void CDoodChatManager::deleteMessageListItem()
{
    qDebug() << Q_FUNC_INFO;
    initChatState();
    if(m_oChatMap.size() <= 0)
        return;

    QMap<QString, CDoodChatItem*>::const_iterator i = m_oChatMap.constBegin();
    while (i != m_oChatMap.constEnd()) {
        removeItem(m_oChatMap[i.key()]);
        delete m_oChatMap[i.key()];
        m_oChatMap[i.key()] = 0;
        ++i;
    }
    m_oChatMap.clear();
}

void CDoodChatManager::showChatPage(QString chatName,
                                    QString targetid,
                                    QString chatType,
                                    QString icon)
{
    qDebug() << Q_FUNC_INFO;
    emit sendShowChatPage(chatName, targetid, chatType, icon);
}

void CDoodChatManager::initChatState()
{
    qDebug() << Q_FUNC_INFO;
    m_sBeginMsgId = "";
    m_sTargetid = "";
}

void CDoodChatManager::uploadAvatar(QString path)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->uploadAvatar(path);
}

void CDoodChatManager::uploadFile(QString path, QString property)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->uploadFile(path, property);
}

void CDoodChatManager::downloadFile(QString path, QString url, QString property)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->downloadFile(path, url, property);
}

void CDoodChatManager::uploadImage(QString thumbimg, QString srcimg, QString property)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->uploadImage(thumbimg, srcimg, property);
}

void CDoodChatManager::downloadImage(QString url, QString property)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->downloadImage(url, property);
}

bool CDoodChatManager::decryptFile(QString encryptkey, QString srcpath, QString destpath)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->decryptFile(encryptkey, srcpath, destpath);
}

void CDoodChatManager::getFileList(int64 targetid, int64 fileid, int count, int flag)
{
    qDebug() << Q_FUNC_INFO;
    m_pClient->getFileList(targetid, fileid, count, flag);
}


void CDoodChatManager::onChatAvatarChanged(int64 id, QString avatar)
{
    qDebug() << Q_FUNC_INFO;
   // emit chatAvatarChanged(id,avatar);
}

void CDoodChatManager::onChatOfflineMsgNotice(IMOfflineMsgList msgList)
{
    qDebug() << Q_FUNC_INFO << msgList.size();
    //emit offlineMsgNotice(msgList);
}

void CDoodChatManager::onChatMessageNotice(Msg msg)
{
    qDebug() << Q_FUNC_INFO << msg.body;
    qDebug() << Q_FUNC_INFO << msg.msgid;
    qDebug() << Q_FUNC_INFO << msg.msgtype;
    qDebug() << Q_FUNC_INFO <<"ssssssssssss"<< msg.name;
    if(msg.fromid == m_pClient->UserId()){
        msg.name = m_pClient->userName();
    }else{
        msg.name = name();
    }

    addItemToListViewModel(msg);
}

void CDoodChatManager::onChatSendMessageResult(bool code, QString sendTime, QString msgId)
{
    qDebug() << Q_FUNC_INFO;
//    emit sendMessageResult(code,sendTime,msgId);
}

void CDoodChatManager::onChatGetMessagesResult(bool code, QString sessionId, MsgList msgList)
{
    qDebug() << Q_FUNC_INFO << code << sessionId << msgList.size();
    if (msgList.size() <= 0) {
        return;
    }
    qDebug() << Q_FUNC_INFO <<"DDD:" << name();
    analyticalMessage(msgList);
}

void CDoodChatManager::onChatRemoveChatResult(bool code)
{
    qDebug() << Q_FUNC_INFO;
//    emit removeChatResult(code);
}

void CDoodChatManager::onChatDeleteMessagesResult(int code)
{

}

void CDoodChatManager::onChatUploadAvatar(QString orgijson, QString thumbjson, int code)
{
    qDebug() << Q_FUNC_INFO;
}

void CDoodChatManager::onChatUploadFile(int64 tagetid, QString jasoninfo, int code)
{
    qDebug() << Q_FUNC_INFO;
}

void CDoodChatManager::onChatFileProgress(int extra_req, int process, QString info)
{
    qDebug() << Q_FUNC_INFO;
}

void CDoodChatManager::onChatDownloadFile(int code, QString localpath, int64 tagetid)
{
    qDebug() << Q_FUNC_INFO;
}

void CDoodChatManager::onChatupLoadImage(int64 tagetid, QString orgijson, QString thumbjson, int code)
{
    qDebug() << Q_FUNC_INFO;
}

void CDoodChatManager::onChatDownloadImage(int code, QString localpath, int64 tagetid)
{
    qDebug() << Q_FUNC_INFO;
}

void CDoodChatManager::onChatGetFileList(int code, std::vector<MsgFileInfo> files)
{
    qDebug() << Q_FUNC_INFO;
}
