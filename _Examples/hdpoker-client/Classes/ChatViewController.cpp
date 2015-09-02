#include "ChatViewController.h"
#include "GameController.h"
#include "ProfileViewController.h"
#include "CountryView.h"

using namespace cocos2d;
using namespace cocos2d::extension;

ChatRowView* ChatRowView::create(TableView* tableView) {
    auto row = ChatRowView::create();
    row->_tableView = static_cast<ChatTableView*>(tableView);
    return row;
}

bool ChatRowView::init() {
    if (!Node::init()) {
        return false;
    }
    
    _avatar = AvatarView::create();
    _avatar->setScale(0.3f);
    _avatar->setPositionY(10);
    addChild(_avatar);
    
    _container = cocos2d::Node::create();
    addChild(_container);
    
    _background = cocos2d::ui::Scale9Sprite::create("chat-bubble.png");
    _background->setAnchorPoint(Vec2(0, 0));
    _background->setContentSize(Size(375, 78));
    _container->addChild(_background);
    
    _name = Text::create("", UniSansBold, 24);
    _name->setPosition(Vec2(15, 75));
    _name->setAnchorPoint(Vec2(0, 1));
    _name->setColor(cocos2d::Color3B(129, 234, 246));
    _container->addChild(_name);
    
    _message = Text::create("", UniSansRegular, 18, cocos2d::Size(350, 0));
    _message->setPosition(Vec2(15, 50));
    _message->setAnchorPoint(cocos2d::Vec2(0, 1));
    _container->addChild(_message);
    
    _when = Text::create("", UniSansBold, 18);
    _when->setPosition(Vec2(15, 15));
    _when->setAnchorPoint(cocos2d::Vec2(1, 1));
    _container->addChild(_when);
    
    _arrow = Sprite::create("chat-bubble-arrow.png");
    _arrow->setPositionY(40);
    addChild(_arrow);
    
    return true;
}

void ChatRowView::update(const ChatMessage& message) {
    _name->setString(message.isFromMe ? _tableView->getMyName() : _tableView->getFriendName());
    _message->setString(message.message.c_str());
    
    _container->setPositionX(message.isFromMe ? 0 : 50);
    
    _arrow->setPositionX(message.isFromMe ? _background->getContentSize().width + 4 : 46);
    _arrow->setFlippedX(!message.isFromMe);

    _avatar->loadAvatar(_tableView->getGame(), message.isFromMe ? _tableView->getMyAvatarId() : _tableView->getFriendAvatarId(), 0);
    _avatar->setPositionX(message.isFromMe ? _background->getContentSize().width + 20 : 20);
}

ChatTableView* ChatTableView::create(GameController* game, const FriendModel& afriend, TableViewDataSource *dataSource, const cocos2d::Size& viewSize) {
    auto table = ChatTableView::create();
    table->_game = game;
    table->_friend = afriend;
    table->initWithViewSize(viewSize);
    table->setDataSource(dataSource);
    return table;
}

GameController* ChatTableView::getGame() const {
    return _game;
}

std::string ChatTableView::getMyName() const {
    return _game->getModel()->getName();
}

std::string ChatTableView::getFriendName() const {
    return _friend.name;
}

std::string ChatTableView::getMyAvatarId() const {
    return _game->getModel()->getCurrentAvatarId();
}

std::string ChatTableView::getFriendAvatarId() const {
    return _friend.avatarId;
}

ChatViewController* ChatViewController::create(GameController *game, const FriendModel& afriend) {
    auto controller = ChatViewController::create();
    controller->_game = game;
    controller->_friend = afriend;
    controller->buildView();
    return controller;
}

ChatViewController::~ChatViewController() {
    _game->getModel()->removeListenersForTarget(this);
}

void ChatViewController::buildView() {
    auto headerBackground = Sprite::create("background-chat-header.png");
    headerBackground->setPosition(Vec2(0, 470));
    headerBackground->setAnchorPoint(Vec2(0, 0));
    addChild(headerBackground);
    
    auto avatar = AvatarView::create();
    avatar->loadAvatar(_game, _friend.avatarId, 0);
    addChild(avatar);
    
    auto name = Text::create(_friend.name, UniSansBold, 26);
    name->setPosition(Vec2(120, 530));
    addChild(name);
    
    auto status = Text::create("In Lobby", UniSansRegular, 18);
    status->setPosition(Vec2(120, 510));
    addChild(status);
    
    auto country = Text::create("Japan", UniSansRegular, 18, Size::ZERO, cocos2d::TextHAlignment::RIGHT);
    country->setPosition(Vec2(330, 525));
    addChild(country);
    
    auto flag = CountryView::create();
    flag->setFlag("JP");
    flag->setPosition(Vec2(400, 530));
    addChild(flag);
    
    auto info = Button::create("button-icon.png", "");
    info->setZoomScale(-0.05f);
    info->setPosition(Vec2(25, 540));
    info->addClickEventListener([=](Ref*) {
        Director::getInstance()->getRunningScene()->addChild(ProfileViewController::create(_game, _friend.name, _friend.userId, _friend.avatarId));
    });
    addChild(info);
    
    auto message = EditBox::create(Size(350, 50), Scale9Sprite::create("textbox.png"));
    message->setPosition(Vec2(25, 400));
    message->setAnchorPoint(Vec2(0, 0));
    message->setFontName(UniSansRegular);
    message->setFontSize(18);
    message->setFontColor(Color3B::WHITE);
    message->setPlaceholderFontColor(Color3B::GRAY);
    message->setReturnType(EditBox::KeyboardReturnType::DONE);
    message->setInputMode(EditBox::InputMode::SINGLE_LINE);
    message->setMaxLength(100);
    message->setDelegate(this);
    addChild(message);
    
    auto sendButton = Button::create("button-chat.png", "");
    sendButton->setZoomScale(-0.05f);
    sendButton->setPosition(Vec2(420, 424));
    sendButton->addClickEventListener([=](Ref*) {
        editBoxReturn(message);
    });
    addChild(sendButton);
    
    const int baseHeight = 78;
    const int wrapWidth = 350;
    auto refLabel = Text::create("", UniSansRegular, 18, cocos2d::Size(wrapWidth, 0));
    
    auto messages = _game->getModel()->getMessagesForUser(_friend.userId);
    for (auto message : messages) {
        refLabel->setString(message.message.c_str());
        
        ChatMessage msg;
        msg.isFromMe = message.isFromMe;
        msg.message = message.message;
        msg.rowHeight = MAX(baseHeight, 36 + refLabel->getContentSize().height);
        _dataSource.push_back(msg);
    }
    
    _chatMessages = ChatTableView::create(_game, _friend, &_dataSource, Size(470, 400));
    addChild(_chatMessages);
    
    _game->getModel()->addListener(this, [=](GameModelUpdate::Value what, GameModel *model) {
        if (GameModelUpdate::ChatMessage != what) {
            return;
        }
        
        auto messages = _game->getModel()->getMessagesForUser(_friend.userId);
        auto &message = messages[messages.size() - 1];
        
        auto refLabel = Text::create("", UniSansRegular, 18, cocos2d::Size(wrapWidth, 0));
        refLabel->setString(message.message.c_str());
        
        ChatMessage msg;
        msg.isFromMe = message.isFromMe;
        msg.message = message.message;
        msg.rowHeight = MAX(baseHeight, 36 + refLabel->getContentSize().height);
        _dataSource.push_back(msg);
        
        _chatMessages->reloadData();
    });

	_chatMessages->reloadData();
}

void ChatViewController::editBoxReturn(EditBox* editBox) {
    if (strlen(editBox->getText()) > 0) {
        _game->getApi()->chatFriendMessage(_friend.userId.c_str(), editBox->getText(), NullCallback);
        editBox->setText("");
    }
}