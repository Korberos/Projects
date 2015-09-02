#include "DashboardViewController.h"
#include "ui/UIButton.h"
#include <json/writer.h>
#include <json/stringbuffer.h>
#include "PokerApi.h"
#include "GameController.h"
#include "Dispatch.h"
#include "SimpleAudioEngine.h"
#include "Shared.h"
#include "Downloader.h"
#include "AvatarView.h"
#include "CoverflowView.h"
#include "SizeTo.h"
#include "CasinoViewController.h"
#include "TableViewController.h"
#include "AlertView.h"
#include "LoginViewController.h"
#include "Version.h"
#include "Text.h"
#include "NewsViewController.h"
#include "LeaderboardViewController.h"
#include "SocialViewController.h"
#include "Stage.h"
#include "SettingsViewController.h"
#include "AchievementViewController.h"
#include "AlertImageView.h"
#include "TierImageView.h"
#include "FreeChipsViewController.h"
#include "Shared.h"
#include "InviteFriendsViewController.h"

using namespace cocos2d;
using namespace cocos2d::ui;

const auto viewTransitionAnimationTime = .5;

DashboardViewController* DashboardViewController::create(GameController *game) {
    auto controller = DashboardViewController::create();
    controller->_game = game;
    controller->_api = game->getApi();
    controller->buildView();
    return controller;
}

DashboardViewController::~DashboardViewController() {
    _game->getModel()->removeListenersForTarget(this);
}

void DashboardViewController::transitionFromView(Node* node) {
    // Detach from current parent
    node->retain();
    node->removeFromParentAndCleanup(false);
    addChild(node);
    
    _tableFlow->setEnabled(false);
    
    auto targetLocation = Vec2(_tableFlow->getStopScreenLocation().x, getContentSize().height / 2);
    
    node->runAction(EaseExponentialOut::create(SizeTo::create(viewTransitionAnimationTime, Size(1136 / 3, 640 / 3))));
    node->runAction(EaseExponentialOut::create(MoveTo::create(viewTransitionAnimationTime, targetLocation)));
    node->runAction(Sequence::createWithTwoActions(DelayTime::create(viewTransitionAnimationTime), CallFunc::create([=] {
        node->removeFromParentAndCleanup(false);
        
        // Add live view to coverflow
        _tableFlow->insertChild(0, node);
        
        // Reenable coverflow
        _tableFlow->setEnabled(true);
        
        node->release();
    })));
}

void DashboardViewController::transitionToView(Node* node) {
    Stage::replaceScene(_game->getCasinoScene());
    
    // Disable view passing for now (definition needed)
    return;
    
    auto size = getContentSize();
    
    _tableFlow->setEnabled(false);
    
    _elements->setVisible(false);
//    _elements->runAction(FadeOut::create(viewTransitionAnimationTime));
    
    node->stopAllActions();
    node->runAction(EaseExponentialOut::create(SizeTo::create(viewTransitionAnimationTime, size)));
    node->runAction(EaseExponentialOut::create(MoveTo::create(viewTransitionAnimationTime, Vec2(size.width / 2, 0))));
    node->runAction(Sequence::createWithTwoActions(DelayTime::create(viewTransitionAnimationTime), CallFunc::create([=] {
        // Detach from current parent
        node->retain();
        node->removeFromParentAndCleanup(false);
        
        // Reenable coverflow
        _tableFlow->setEnabled(true);
        
        // Add to casino
        _game->getCasino()->addTable((TableViewController*)node);
        
        Stage::replaceScene(_game->getCasinoScene());
        
        node->release();
    })));
}

void DashboardViewController::buildView() {
    auto size = Director::getInstance()->getWinSize();
    _background = Sprite::create("dashboard-background.png");
    _background->setPosition(size.width / 2, size.height / 2);
    coverFill(_background, size);
    addChild(_background);
    
    _elements = Layer::create();
    addChild(_elements);
    
    auto online = Text::create("", UniSansRegular, 18);
    online->setAnchorPoint(Vec2(0, 1));
    online->setPosition(50, size.height - 50);
    
    _game->getApi()->getPlayersOnline(bind1(this, [=](const DashboardViewController*, const Message* message) {
        if (message->isSuccess()) {
            online->setString((std::to_string(message->getData()["onlineUserCount"].GetInt()) + std::string(" players online")).c_str());
        }
    }));
    _elements->addChild(online);
    
    auto build = Text::create("BUILD " HDPOKER_VERSION, UniSansRegular, 18);
    build->setAnchorPoint(Vec2(0, 1));
    build->setPosition(50, size.height - 80);
    _elements->addChild(build);
    
    auto playnow = Button::create("playnow.png", "playnow.png");
    playnow->setPosition(Vec2(size.width / 2, size.height / 2 + 250));
    playnow->addClickEventListener([this] (Ref*) {
        CCLOG("Play now!");
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/playnow.wav");
        _api->tableSubscribeImmediate(NullCallback);
    });
    _elements->addChild(playnow);
    
    _tableFlow = CoverflowView::create();
    // Children should use a centered anchor point
    _tableFlow->setContentSize(Size(800, 550));
    _tableFlow->setPosition(Vec2(- size.height * .25, size.height / 2));
    addChild(_tableFlow);
    
    // TODO release
    retain();
    int tableCount = 30;
    _game->getApi()->getTables(tableCount, [=](const Message* message) {
        _lobbyModel.clear();
        
        if (message->isSuccess()) {
            auto &tables = message->getData()["result"]["tables"];
			int numEntries = tables.Size();
            for (auto i = 0; i < numEntries; i++) {
                auto &table = tables[i];
                
                LobbyEntry entry;
                entry.name = table["name"].GetString();
                entry.tableId = table["tableID"].GetString();
                entry.numPlayers = table["numPlayers"].GetInt();
                entry.tableSize = table["tableSize"].GetInt();
                _lobbyModel.push_back(entry);
                
                std::string tableName = std::string(table["name"].GetString());
                auto url = std::string("http://play.qa.hdpoker.com/common/library/themes/default/thumbs/theme_") + std::to_string(table["themeID"].GetInt()) + std::string("_lg.png");
				Downloader::getSpriteFromUrl(url, [=](cocos2d::Sprite* sprite, const std::string& path, bool) {
                    if (sprite) {
                        _tableFlow->insertChild(_game->getTableCount() + i, sprite);
                    }
//                    if (--tableCount <= 0) {
//                        release();
//                    }
                });
            }
        }
    });
    
    _title = Text::create("", UniSansRegular, 32);
    _title->setAnchorPoint(Vec2(0, 0));
    _title->setPosition(Vec2(100, 150));
    addChild(_title);
    
    _tableStats = Text::create("", UniSansRegular, 18);
    _tableStats->setAnchorPoint(Vec2(0, 0));
    _tableStats->setPosition(Vec2(100, 120));
    _elements->addChild(_tableStats);
    
    _tableFlow->setFocusCallback([=](int index, Node *node) {
		int numModels = static_cast<int>(_lobbyModel.size());
        if (index >= 0 && index < numModels) {
            auto &model = _lobbyModel[index];
            _tableStats->stopAllActions();
            _title->stopAllActions();
            
            _tableStats->setString(std::string("Players: ") + std::to_string(model.numPlayers) + " / " + std::to_string(model.tableSize));
            _tableStats->runAction(Sequence::createWithTwoActions(DelayTime::create(0.15f), FadeIn::create(0.2f)));
            
            _title->setString(_lobbyModel[index].name.c_str());
            _title->runAction(FadeIn::create(0.2f));
        }
    });
    
    _tableFlow->setDefocusCallback([=](int index, Node *node) {
        _title->runAction(FadeOut::create(0.2f));
        _tableStats->runAction(FadeOut::create(0.2f));
    });
    
    _tableFlow->setClickedCallback([=](int index, Node *node) {
        if (dynamic_cast<TableViewController*>(node) != nullptr) {
            // Live view
            transitionToView(node);
        } else {
            // Tile view
            // TODO transition
			int numModels = _lobbyModel.size();
            if (index >= 0 && index < numModels) {
                _game->getApi()->tableSubscribe(_lobbyModel[index].tableId.c_str(), NullCallback);
            }
        }
    });
    
    auto myAvatar = AvatarView::create();
    myAvatar->loadAvatar(_game, _game->getModel()->getCurrentAvatarId(), 0);
    myAvatar->setPosition(size.width - 150, 240);
    _elements->addChild(myAvatar);
    
    auto chips = Text::create(numberWithCommas(_game->getModel()->getChips()), UniSansRegular, 24);
    chips->setAnchorPoint(Vec2(1, .5));
    chips->setPosition(size.width - 50, size.height - 50);
    chips->setAlignment(TextHAlignment::RIGHT);
    _elements->addChild(chips);
    
    auto chip = Sprite::create("icon-chip.png");
    chip->setPosition(chips->getPositionX() - chips->getContentSize().width - 20, chips->getPositionY());
    _elements->addChild(chip);
    
    auto gems = Text::create(numberWithCommas(_game->getModel()->getGems()), UniSansRegular, 24);
    gems->setAnchorPoint(Vec2(1, .5));
    gems->setPosition(size.width - 50, size.height - 100);
    gems->setAlignment(TextHAlignment::RIGHT);
    _elements->addChild(gems);
    
    auto gemIcon = Sprite::create("icon-gem.png");
    gemIcon->setPosition(gems->getPositionX() - gems->getContentSize().width - 20, gems->getPositionY());
    _elements->addChild(gemIcon);
    
    _game->getModel()->addListener(this, [=](GameModelUpdate::Value change, GameModel *model) {
		if (GameModelUpdate::Chips == change) {
            chips->setString(numberWithCommas(model->getChips()));
            chip->setPosition(chips->getPositionX() - chips->getContentSize().width - 20, chips->getPositionY());
		}
		else if (GameModelUpdate::Gems == change) {
            gems->setString(numberWithCommas(model->getGems()));
            gemIcon->setPosition(gems->getPositionX() - gems->getContentSize().width - 20, gems->getPositionY());
		}
		else if (GameModelUpdate::ActiveAvatar == change) {
            myAvatar->loadAvatar(_game, _game->getModel()->getCurrentAvatarId(), 0);
        }
    });
    
    auto logout = Button::create("button.png", "button-press.png");
    logout->setTitleText("Logout");
    logout->setAnchorPoint(Vec2(1, 0));
    logout->setTitleFontSize(18);
    logout->setPosition(Vec2(size.width - 50, 50));
    logout->addClickEventListener([=] (Ref*) {
        _game->getNetwork()->disconnect();
        LoginViewController::clearAuthenticationInformation();

#if FACEBOOK_AVAILABLE
		_game->getFacebook()->logout();
#endif
        _game->removeAllGames();
		_game->getModel()->reset();
        
        auto scene = Scene::create();
        auto login = LoginViewController::create(_game);
        scene->addChild(login);
        
        Stage::clearLayer();
        Stage::replaceScene(scene);
    });
    addChild(logout);

	auto leaderboard = Button::create("button.png", "button-press.png");
	leaderboard->setTitleText("Leaderboard");
	leaderboard->setAnchorPoint(Vec2(1, 0));
	leaderboard->setTitleFontSize(18);
	leaderboard->setPosition(Vec2(size.width - 50, 50 + 62));
	leaderboard->addClickEventListener([=](Ref*) {
		LeaderboardViewController::create(_game, LeaderboardViewController::TOP_PLAYERS_VIEW);
	});
	addChild(leaderboard);
	
	auto news = Button::create("button.png", "button-press.png");
	news->setTitleText("News");
	news->setAnchorPoint(Vec2(1, 0));
	news->setTitleFontSize(18);
	news->setPosition(Vec2(size.width - 50, 50 + 62 * 2));
	news->addClickEventListener([=](Ref*) {
		NewsViewController::create(_game);
	});
	addChild(news);
    
    auto settings = Button::create("button.png", "button-press.png");
    settings->setTitleText("Settings");
    settings->setAnchorPoint(Vec2(1, 0));
    settings->setTitleFontSize(18);
    settings->setPosition(Vec2(size.width - 50, 50 + 62 * 3));
    settings->addClickEventListener([=](Ref*) {
        Director::getInstance()->getRunningScene()->addChild(SettingsViewController::create(_game, SettingsViewController::GlobalSettings));
    });
    addChild(settings);

	auto achieveTest = Button::create("button.png", "button-press.png");
	achieveTest->setTitleText("Random Achievement");
	achieveTest->setAnchorPoint(Vec2(1, 0));
	achieveTest->setTitleFontSize(18);
	achieveTest->setPosition(Vec2(size.width - 50, 50 + 62 * 4));
	achieveTest->addClickEventListener([=](Ref*) {
		auto achievementsData = _game->getModel()->getAchievements();
		int list[100];
		auto count = 0;
		for (auto &x : achievementsData) {
			list[count] = x.first;
			if (++count == 100) break;
		}
		AchievementViewController::handleAchievement(_game, list[cocos2d::random() % count]);
	});
	addChild(achieveTest);

	auto rankupTest = Button::create("button.png", "button-press.png");
	rankupTest->setTitleText("Rank Up");
	rankupTest->setAnchorPoint(Vec2(1, 0));
	rankupTest->setTitleFontSize(18);
	rankupTest->setPosition(Vec2(size.width - 50, 50 + 62 * 5));
	rankupTest->addClickEventListener([=](Ref*) {
		AlertImageView::show("Congratulations!\nYou have ranked up to the Diamond tier!", TierImageView::getImageFilename(TierImageView::DIAMOND));
	});
	addChild(rankupTest);
    
    auto inviteFacebookFriends = Button::create("button.png", "button-press.png");
    inviteFacebookFriends->setTitleText("Invite Facebook Friends!");
    inviteFacebookFriends->setAnchorPoint(Vec2(1, 0));
    inviteFacebookFriends->setTitleFontSize(18);
    inviteFacebookFriends->setPosition(Vec2(size.width - 50, 50 + 62 * 6));
    inviteFacebookFriends->addClickEventListener([=](Ref*) {
#if FACEBOOK_AVAILABLE
		if (_game->getFacebook()->isLoggedIn()) {
            addChild(InviteFriendsViewController::create(_game));
        } else {
            AlertView::show("Log into Facebook to view this feature");
		}
#else
		playNotImplemented();
#endif
    });
    addChild(inviteFacebookFriends);

	addChild(FreeChipsViewController::create(_game));

	auto gameController = _game;
	_game->getModel()->addListener(this, [=](GameModelUpdate::Value change, GameModel *model) {
		if (change == GameModelUpdate::Tier && model->getTierDelta() != 0) {
			auto scene = gameController->getCasinoScene();
			if (!scene) {
				showRankUp(true);
			}
		}
	});

	if (_game->getModel()->getTierDelta() != 0) {
		showRankUp(false);
	}
}

void DashboardViewController::showRankUp(bool show) {
	auto model = _game->getModel();
	auto tier = model->getTier();
	auto delta = model->getTierDelta();
	auto msg = std::string((delta > 0) ? "Congratulations!\nYou have ranked up to the " : "Attention:\nYou have been demoted to the ") + getTierName(tier) + " tier!"; // <-- until we get a comp for tier demotion
	auto view = AlertImageView::create(msg.c_str(), TierImageView::getImageFilename(tier), [=]{_game->getModel()->setTierDelta(0); });
	if (show) {
		Director::getInstance()->getRunningScene()->addChild(view);
	} else {
		addChild(view);
	}
}