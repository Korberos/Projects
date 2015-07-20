#include "TableViewController.h"
#include "NameplateView.h"
#include "PotView.h"
#include "SimpleAudioEngine.h"
#include "GameController.h"
#include "ActionButtonsView.h"
#include "CardView.h"
#include "BoardCardsView.h"
#include "BetStackView.h"
#include "PlayerWinView.h"
#include "AvatarView.h"
#include "Downloader.h"
#include "Shared.h"
#include "ChatBubbleView.h"
#include "UI/UIButton.h"
#include "TableModel.h"
#include "BuyInView.h"

using namespace cocos2d;

const float flagSpeed = .06;
const float dealAnimationTime = .6;
const float dealSpacing = .12;
const int holeRestRotation = 30;
const int halfHoleRestRotation = holeRestRotation / 2;
const float holeAnimatingRotation = 360;

const Size designSize = Size(1136, 640);

TableViewController* TableViewController::create(GameController *game, const Message *tableSubscribeMessage) {
    auto controller = TableViewController::create();
    controller->setContentSize(Director::getInstance()->getWinSize());
	controller->setAnchorPoint(Vec2(0.5f, 0.5f));
    controller->_game = game;
    controller->_api = game->getApi();
    controller->buildView(tableSubscribeMessage);
    return controller;
}

TableViewController::TableViewController() : _backgroundLayer(nullptr), _buyIn(nullptr) {}

void TableViewController::setContentSize(const Size& contentSize) {
    Node::setContentSize(contentSize);
    
    // Haven't built the view yet
    if (!_backgroundLayer)
        return;
    
    setClippingEnabled(true);
    setClippingRegion(Rect(0, 0, contentSize.width, contentSize.height));
    
    if (_backgroundLayer->getChildren().size() > 0) {
        coverFill(_backgroundLayer->getChildren().at(0), contentSize);
        _backgroundLayer->setPosition(contentSize.width / 2, contentSize.height / 2);
    }
    
    auto viewScale = containScale(designSize, getContentSize());
    auto viewSize = containSize(designSize, getContentSize());
    
    _tableView->setContentSize(viewSize);
    _tableView->setScale(viewScale);
 }

void TableViewController::setViewMode(ViewMode viewMode) {
    _viewMode = viewMode;
}

const TableModel* TableViewController::getModel() {
    return &_model;
}

int TableViewController::getRotationForSeat(int seatIndex) {
    switch (seatIndex) {
        case 0: case 8:
            return 1;
        case 1: case 7:
            return 2;
        case 2: case 6:
            return 3;
        case 3: case 5:
            return 4;
        case 4:
            return 5;
    }
}

//float getGiftPositionForSeat(int seat) {
//    switch (seat) {
//        case 0: return Vec2();
//        case 1: return Vec2();
//        case 2: return Vec2();
//        case 3: return Vec2();
//        case 4: return Vec2();
//        case 5: return Vec2();
//        case 6: return Vec2();
//        case 7: return Vec2();
//        case 8: return Vec2();
//    }
//}
//
//float getDealerPositionForSeat(int seat) {
//    switch (seat) {
//        case 0: return Vec2();
//        case 1: return Vec2();
//        case 2: return Vec2();
//        case 3: return Vec2();
//        case 4: return Vec2();
//        case 5: return Vec2();
//        case 6: return Vec2();
//        case 7: return Vec2();
//        case 8: return Vec2();
//    }
//}

//float getBetPositionForSeat(int seat) {
//    switch (seat) {
//        case 0: return Vec2();
//        case 1: return Vec2();
//        case 2: return Vec2();
//        case 3: return Vec2();
//        case 4: return Vec2();
//        case 5: return Vec2();
//        case 6: return Vec2();
//        case 7: return Vec2();
//        case 8: return Vec2();
//    }
//}
//
//float getCardPositionForSeat(int seat) {
//    switch (seat) {
//        case 0: return Vec2();
//        case 1: return Vec2();
//        case 2: return Vec2();
//        case 3: return Vec2();
//        case 4: return Vec2();
//        case 5: return Vec2();
//        case 6: return Vec2();
//        case 7: return Vec2();
//        case 8: return Vec2();
//    }
//}

float getCardRotationForSeat(int seat) {
    switch (seat) {
        case 0: return 180;
        case 1: return 210;
        case 2: return 225;
        case 3: return -45;
        case 4: return 0;
        case 5: return 32;
        case 6: return 113;
        case 7: return 156;
        case 8: return 196;
    }
}

float TableViewController::getScaleForElement(Element element) {
    switch (element) {
        case HoleCards:     return .16;
        case CommunityCards:    return .7;
        case Avatar:       return .46;
        case Chair:         return .6;
        case Table:         return .6;//57;
        case BetStack:      return 0.32;
        default:            return 1.0;
    }
}

float TableViewController::getScaleForChair(int seatIndex) {
    switch (seatIndex) {
        case 0: return .82;
        case 1: return .875;
        case 2: return .96;
        case 3: return 1.08;
        case 4: return 1.08;
        case 5: return 1.08;
        case 6: return .96;
        case 7: return .875;
        case 8: return .82;
    }
}

Vec2 TableViewController::getPositionForSeat(Element element, int seatIndex) {
    if (ViewMode::ThirdPerson == _viewMode) {
        
        switch (element) {
            case Avatar: switch (seatIndex) {
                case 0: return Vec2(643, 424);
                case 1: return Vec2(785, 394);
                case 2: return Vec2(886, 341);
                case 3: return Vec2(795, 88);
                case 4: return Vec2(568, 55);
                case 5: return Vec2(343, 88);
                case 6: return Vec2(245, 341);
                case 7: return Vec2(353, 394);
                case 8: return Vec2(496, 424);
            } break;
            case Chair: switch (seatIndex) {
                case 0: return Vec2(643, 394);
                case 1: return Vec2(785, 374);
                case 2: return Vec2(886, 321);
                case 3: return Vec2(795, 68);
                case 4: return Vec2(568, 35);
                case 5: return Vec2(343, 68);
                case 6: return Vec2(245, 321);
                case 7: return Vec2(353, 374);
                case 8: return Vec2(496, 394);
            } break;
            case Nameplate: switch (seatIndex) {
                case 0: return Vec2(643, 563);
                case 1: return Vec2(790, 552);
                case 2: return Vec2(910, 494);
                case 3: return Vec2(800, 70);
                case 4: return Vec2(570, 62);
                case 5: return Vec2(347, 70);
                case 6: return Vec2(231, 494);
                case 7: return Vec2(347, 552);
                case 8: return Vec2(490, 563);
            } break;
            case DealerButton:
            case BetStack:
            case HoleCards: switch (seatIndex) {
                case 0: return Vec2(645, 374);
                case 1: return Vec2(776, 368);
                case 2: return Vec2(849, 325);
                case 3: return Vec2(780, 167);
                case 4: return Vec2(565, 168);
                case 5: return Vec2(357, 178);
                case 6: return Vec2(287, 325);
                case 7: return Vec2(370, 370);
                case 8: return Vec2(496, 376);
            } break;
        }
    }
    
    auto xOff = 50;
    auto size = Director::getInstance()->getWinSize();
    return Vec2(xOff + seatIndex * (size.width / 9), size.height * .75);
}

/** Be sure to update the model & work from the model, not from the network traffic. This helps ensure the model updates are not skipped. */
bool TableViewController::handleMessage(const Message *message) {

    auto &data = message->getData();
    auto eventName = std::string(data["event"].GetString());
    
    // Is this a hand message?  Does it belong to this table?
    auto hand = std::string("hand");
    auto table = std::string("table");
    if (0 == eventName.compare(0, hand.length(), hand)) {
        
        // New hand started. Does this new hand belong to this table?
        if ("handStarted" == eventName && std::string(data["ids"]["tableID"].GetString()) == _model.tableId) {
            
            // Capture the hand ID
            _model.activeHandId = std::string(data["ids"]["handID"].GetString());
        } else if (_model.activeHandId != std::string(data["ids"]["handID"].GetString())) {

            // This hand information does not belong to this table - ignore message
            return false;
        }
    } else if (0 == eventName.compare(0, table.length(), table) && std::string(data["ids"]["tableID"].GetString()) != _model.tableId) {
        // Table message is not for us
        return false;
    }
    
    if ("handStarted" == eventName) {
        _firstDeal = true;
        _firstHoleCard = true;
        _tableLabel->setString(_model.tableName + std::string(" - ") + _model.activeHandId);
        
    } else if ("handAllCardsRemoved" == eventName) {
        _model.boardCards.clear();
        
        // View update
        _actionButtons->clearPreselects();
        _cardsForSeat.clear();
        _communityCards->removeAll();
        _pot->clear();
        
        for (auto i = 0; i < _model.tableSize; i++) {
            _winViews[i]->setVisible(false);
            _nameplates[i]->undim();
        }
        _cardLayer->removeAllChildren();
        
    } else if ("handSeatChoices" == eventName) {
        // {"event":"handSeatChoices","ids":{"handID":"aa8df376-0c00-4857-9b74-54aff33c0e9d"},"bools":{"canFold":true,"canCheck":false,"canCall":true,"canBet":false,"canRaise":true},"chips":{"minRaiseBet":24000,"maxRaiseBet":800000,"call":8000,"sliderPot":16000,"chipStack":792000}}
        
        _choicesCallAmount = data["chips"]["call"].GetInt64();
        
        _actionButtons->hidePreselects();
        
        // Preselect?
        auto preselectOption = _actionButtons->getPreselectAction();
        switch (preselectOption) {
            case ActionButtonsView::PreselectCallAny:
                _api->handPostAction(_model.activeHandId.c_str(), "call", _choicesCallAmount, NullCallback);
                break;
            case ActionButtonsView::PreselectCall:
                // Treat as call any for moment...
                _api->handPostAction(_model.activeHandId.c_str(), "call", _choicesCallAmount, NullCallback);
                break;
            case ActionButtonsView::PreselectCheckFold:
                if (data["bools"]["canCheck"].GetBool()) {
                    _api->handPostAction(_model.activeHandId.c_str(), "check", 0, NullCallback);
                } else {
                    _api->handPostAction(_model.activeHandId.c_str(), "fold", 0, NullCallback);
                }
                break;
            default:
            case ActionButtonsView::PreselectNone:
                _actionButtons->showActions(data["bools"]["canCall"].GetBool(), data["bools"]["canBet"].GetBool(), data["bools"]["canRaise"].GetBool(),
                                            data["chips"]["minRaiseBet"].GetInt64(), data["chips"]["maxRaiseBet"].GetInt64(), data["chips"]["sliderPot"].GetInt64());
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/myturn.wav");
                break;
        }
        
        _actionButtons->clearPreselects();
        
    } else if ("handSeatPosted" == eventName) {
        // Can be posted when placing ante!
        // Sometimes doesn't contain all-in field!
        
//        {"event":"handSeatPosted","ids":{"handID":"88c3fece-fd4a-441a-855d-d040038b6d25"},"nums":{"seat":3},"texts":{"action":"check"},"bools":{"allIn":false},"chips":{"chips":0,"chipStack":792000,"betStack":8000}}
        
        auto seatIndex = data["nums"]["seat"].GetInt();
        
        if (data["chips"].HasMember("chipStack")) {
            _model.seats[seatIndex].chips = data["chips"]["chipStack"].GetInt64();
        } else {
            _model.seats[seatIndex].chips = data["chips"]["chips"].GetInt64();
        }
        
        // View updates
        _activeHighlight->stopAllActions();
        _activeHighlight->setVisible(false);
        
        if (seatIndex == _model.mySeat) {
            _actionButtons->hideActions();
        }
        
        if (data["chips"].HasMember("betStack")) {
            _bets[seatIndex]->setChips(data["chips"]["betStack"].GetInt64());
        }
        
        _nameplates[seatIndex]->setChips(_model.seats[seatIndex].chips);
        
        auto action = std::string(data["texts"]["action"].GetString());
        _nameplates[seatIndex]->setFlag(action.c_str());
        _nameplates[seatIndex]->setActive(false);
        _nameplates[seatIndex]->cancelTimer();
        
        if ("fold" == action) {
            auto dealerLocation = getPositionForSeat(DealerButton, _model.dealerButtonIndex);
            
            // This condition can be removed after initial table state is correctly setup
            if (_cardsForSeat.find(seatIndex) != _cardsForSeat.end()) {
                _cardsForSeat[seatIndex][0]->runAction(Spawn::createWithTwoActions(FadeOut::create(.5), MoveTo::create(.5, dealerLocation)));
                _cardsForSeat[seatIndex][1]->runAction(Spawn::createWithTwoActions(FadeOut::create(.5), MoveTo::create(.5, dealerLocation)));
            }
            
            if (seatIndex == _model.mySeat) {
				_nameplates[seatIndex]->clearHoleCards();
            }
            
            _nameplates[seatIndex]->dim();
        }
        
        if (data.HasMember("bools") && data["bools"]["allIn"].GetBool()) {
            _nameplates[seatIndex]->setFlag("ALL IN");
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/allin.wav");
        } else if ("check" == action) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/check.wav");
        } else if ("fold" == action) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/fold.wav");
        } else if ("raise" == action) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/raise.wav");
        } else {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/bet.wav");
        }
        
//        {"event":"handSeatPosted","ids":{"handID":"c82559fe-62e5-46fb-af12-555e9cfd536c"},"nums":{"seat":4},"texts":{"action":"bet"},"bools":{"allIn":false},"chips":{"chips":40000,"chipStack":3450687,"betStack":40000}}
    } else if ("handPotUpdated" == eventName || "handDisplayPerceivedPot" == eventName) {
        
        // {"event":"handDisplayPerceivedPot","ids":{"handID":"c7ef8a3b-bc5f-4d53-a148-a6425706d3ea"},"nums":{"numPots":1},"chips":{"chips":[30000]}}
        _pot->setChips(data["chips"]["chips"][0U].GetInt64());
        
    } else if ("handSeatPostedBB" == eventName || "handSeatPostedSB" == eventName) {
        
//        {"event":"handSeatPostedBB","ids":{"handID":"b401e24a-5a16-46dd-844d-f5ba9a2a7679"},"nums":{"seat":2},"chips":{"chips":20000,"chipStack":21170000,"betStack":20000}}
        
        auto seatIndex = data["nums"]["seat"].GetInt();
        _bets[seatIndex]->setChips(data["chips"]["betStack"].GetInt64());
        _nameplates[seatIndex]->setChips(data["chips"]["chipStack"].GetInt64());
        
    } else if ("handCardDealtToPlayer" == eventName) {
//        {"event":"handCardDealtToPlayer","ids":{"handID":"4e87338c-e882-4a6c-bd4a-36985c4a6dd9"},"texts":{"card":"Kd"}}
        
        auto card = data["texts"]["card"].GetString();

		auto seatIndex = getModel()->getMySeat();
        if (_firstHoleCard) {
            _firstHoleCard = false;
            
            _nameplates[seatIndex]->showHoleCard(0, card);
        } else {
            _nameplates[seatIndex]->showHoleCard(1, card);
        }
        
        
    } else if ("handCardsDealt" == eventName) {
        // {"event":"handCardsDealt","ids":{"handID":"4e87338c-e882-4a6c-bd4a-36985c4a6dd9"},"nums":{"seats":[2,5,6]}}
        
        auto &seats = data["nums"]["seats"];
        auto dealerLocation = getPositionForSeat(DealerButton, _model.dealerButtonIndex);
        for (auto i = 0; i < seats.Size(); i++) {
            auto seatIndex = seats[i].GetInt();
            
            // Deal
            auto card = Sprite::create("sprites/card-back.png");
            card->setPosition(dealerLocation);
            card->setOpacity(255);
            card->setRotation(holeAnimatingRotation);
            card->setScale(getScaleForElement(HoleCards));
            
            auto target = getPositionForSeat(HoleCards, seatIndex);
            if (!_firstDeal) {
                target.x += 5;
            }
            
            auto delay = DelayTime::create(i * dealSpacing + (!_firstDeal ? .5 : 0));
            auto move = Spawn::create(CallFunc::create([](){
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((std::string("sounds/deal") + std::to_string((rand() % 4) + 1) + std::string(".wav")).c_str());
            }), EaseQuadraticActionOut::create(MoveTo::create(dealAnimationTime, target)), EaseQuadraticActionOut::create(RotateBy::create(dealAnimationTime, getCardRotationForSeat(i) - halfHoleRestRotation + rand() % holeRestRotation)), nullptr);
            card->runAction(Sequence::create(delay, move, nullptr));
            _cardLayer->addChild(card);
            
            _cardsForSeat[seatIndex].push_back(card);
        }
        
        _firstDeal = false;
        
        
    } else if ("tableStood" == eventName) {
//        {"event":"tableStood","ids":{"tableID":"6ad888aa-3826-4078-9693-da3a86288ebb"},"nums":{"seat":1}}
        
        auto seatIndex = data["nums"]["seat"].GetInt();
        _bets[seatIndex]->clearBet();
        _nameplates[seatIndex]->setVisible(false);
        _avatars[seatIndex]->clearAvatar();
        
        auto &seat = _model.seats[seatIndex];
        seat.playerName = "";
        seat.chips = 0;
        seat.avatarId = "";
        seat.playerState = TableModel::Empty;
        seat.playerId = "";
        
        if (seatIndex == _model.mySeat) {
            _model.mySeat = -1;
            
            if (_buyIn) {
                _buyIn->removeFromParent();
                _buyIn = nullptr;
            }
            updateSitButtons();
        }
        
        
    } else if ("handPotAwarded" == eventName) {
//        {"event":"handPotAwarded","ids":{"handID":"c258d880-adc9-4332-a2a9-5f085e1c885c"},"texts":{"bestHand":[],"bestHandDescription":""},"nums":{"seats":[7],"pot":0},"chips":{"chips":[24000],"chipStacks":[1627719]},"bools":{"sidePots":false}}
        
        // Server...
        _actionButtons->hidePreselects();
        
        for (auto i = 0; i < _model.tableSize; i++) {
            _bets[i]->clearBet();
        }
        
        auto &seats = data["nums"]["seats"];
        for (auto i = 0; i < seats.Size(); i++) {
            auto seatIndex = seats[i].GetInt();
            _winViews[seatIndex]->setVisible(true);
            _winViews[seatIndex]->setPlayerName(_model.seats[seatIndex].playerName.c_str());
            _winViews[seatIndex]->setChipsWon(data["chips"]["chips"][i].GetInt64());
            _nameplates[seatIndex]->setChips(data["chips"]["chipStacks"][i].GetInt64());
            _pot->sendChipsToParentLocation(getPositionForSeat(Nameplate, seatIndex));
        }
        
        for (auto i = 0; i < _model.tableSize; i++) {
            _nameplates[i]->clearFlag(0);
        }
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/won.wav");
        
    } else if ("handDealerButton" == eventName) {
        _model.dealerButtonIndex = data["nums"]["seat"].GetInt();
        _dealerButton->runAction(MoveTo::create(1, getPositionForSeat(DealerButton, _model.dealerButtonIndex)));
    } else if ("tableSitIn" == eventName) {
        auto seatIndex = data["nums"]["seat"].GetInt();
        _nameplates[seatIndex]->clearFlag(0);
        _model.seats[seatIndex].playerState = TableModel::Waiting;
        
    } else if ("tableSitOut" == eventName) {
//        {"event":"tableSitOut","ids":{"tableID":"eeb6963c-ee8b-4dcf-a6ac-a0f42bd122ba"},"nums":{"seat":6},"bools":{"autoPilot":false}}
        auto seatIndex = data["nums"]["seat"].GetInt();
        _nameplates[seatIndex]->setFlag("SITTING OUT", false);
        _model.seats[seatIndex].playerState = TableModel::SittingOut;
        
    } else if ("tableSeated" == eventName) {
        auto seatIndex = data["nums"]["seat"].GetInt();
        
        // Update model
        auto &seat = _model.seats[seatIndex];
        seat.playerName = data["texts"]["name"].GetString();
        seat.playerId = data["ids"]["userID"].GetString();
        seat.avatarId = data["ids"]["avatarID"].GetString();
        seat.playerState = TableModel::Waiting;
        
        _nameplates[seatIndex]->setName(seat.playerName.c_str());
        _nameplates[seatIndex]->setVisible(true);
        
        if (seat.playerId == _game->getModel()->getUserId()) {
            _model.mySeat = seatIndex;
            updateSitButtons();
        }
        
        // Load avatar
        std::vector<std::string> avatarIds;
        avatarIds.push_back(seat.avatarId);
        loadAvatars(avatarIds);
        
//        {"event":"tableSeated","ids":{"userID":"fd2f6192-5ce6-42f7-8026-8267d3b3ce95","avatarID":"82823c8f-992a-4bc6-8ece-feed2c4dfeb4","tableID":"b2d37d90-32a0-4f15-8b1b-70533ad13612"},"nums":{"seat":0},"texts":{"name":"nous"}}
        
    } else if ("tableBuyIn" == eventName) {
//        {"event":"tableBuyIn","ids":{"userID":"fd2f6192-5ce6-42f7-8026-8267d3b3ce95","tableID":"c032aecb-5429-49fd-96be-ed1e1018a28b"},"nums":{"seat":8},"texts":{"name":"nous"},"chips":{"chips":2000000,"chipStack":2000000}}
        
        auto seatIndex = data["nums"]["seat"].GetInt();
        _nameplates[seatIndex]->setChips(data["chips"]["chipStack"].GetInt64());
        
    } else if ("handHandDescription" == eventName) {
        
    } else if ("handPreSelect" == eventName) {
        // {{"event":"handPreSelect","ids":{"handID":"59ce22bd-78b7-42b7-a566-11521af03309"},"bools":{"canFold":false,"canCheck":true,"canCall":false,"canCallAny":true}}
        _actionButtons->showPreselects(data["bools"]["canCall"].GetBool(), 0);
        
    } else if ("handPreSelectRemoved" == eventName) {
        // {event: "handPreSelectRemoved"}
        
        _actionButtons->hidePreselects();
    
    } else if ("handSeatStartedTimeout" == eventName) {
        //    {"event":"handSeatStartedTimeout","ids":{"handID":"f7273311-9f5f-4c7f-998c-15eaac325c44","tableID":"e0b4cb65-3a5d-4516-8597-e3632b058cbb"},"nums":{"seat":0,"time":12600}}
        auto seatIndex = data["nums"]["seat"].GetInt();
        _nameplates[seatIndex]->startTimer(data["nums"]["time"].GetInt() * .001);
        
    } else if ("handSeatActive" == eventName) {
        //    {"event":"handSeatActive","ids":{"handID":"f7273311-9f5f-4c7f-998c-15eaac325c44"},"nums":{"seat":0}}
        
        // Clear 'active' on all seats
        for (auto i = 0; i < _model.tableSize; i++) {
            _nameplates[i]->setActive(false);
        }
        
        _model.activeSeatIndex = data["nums"]["seat"].GetInt();
        _nameplates[_model.activeSeatIndex]->setActive(true);
        
        _activeHighlight->stopAllActions();
        _activeHighlight->setPosition(getPositionForSeat(Avatar, _model.activeSeatIndex));
        _activeHighlight->setVisible(true);
        _activeHighlight->runAction(RepeatForever::create(RotateBy::create(3, 360)));
        
    } else if ("handFlopDealt" == eventName) {
        _actionButtons->hidePreselects();
        
        // Clear all flags
        for (auto i = 0; i < _model.tableSize; i++) {
            _nameplates[i]->clearFlag(i * flagSpeed);
            
            _bets[i]->clearBet();
            //_bets[i]->animateToPot
        }
        
//        {"event":"handFlopDealt","ids":{"handID":"2ace10b1-7c21-4048-bf26-c00299765448"},"texts":{"cards":["5c","2c","9c"]}}.
        auto &cards = data["texts"]["cards"];
        _communityCards->showFlop(cards[0U].GetString(), cards[1U].GetString(), cards[2U].GetString());
        
        
    } else if ("handTurnDealt" == eventName) {
        _actionButtons->hidePreselects();
        
        // Clear all flags
        for (auto i = 0; i < _model.tableSize; i++) {
            _nameplates[i]->clearFlag(i * flagSpeed);
            _bets[i]->clearBet();
        }
        
        _communityCards->showTurn(data["texts"]["card"].GetString());
        
    } else if ("handRiverDealt" == eventName) {
        _actionButtons->hidePreselects();
        
        // Clear all flags
        for (auto i = 0; i < _model.tableSize; i++) {
            _nameplates[i]->clearFlag(i * flagSpeed);
            _bets[i]->clearBet();
        }
        
        _communityCards->showRiver(data["texts"]["card"].GetString());
    } else if ("handChipsReturned" == eventName) {
//        {"event":"handChipsReturned","ids":{"handID":"0a85e7e8-996d-4557-a27b-ab41b37b158e"},"nums":{"seat":7},"chips":{"chips":10000,"chipStack":1640900,"betStack":10000}}
        
        auto seatIndex = data["nums"]["seat"].GetInt();
        _nameplates[seatIndex]->setChips(data["chips"]["chipStack"].GetInt64());
        
    } else if ("tableThemeChange" == eventName) {
//        {"event":"tableThemeChange","ids":{"tableID":"c601aa58-9961-45d6-8c17-2f3e1c789c03"},"nums":{"themeID":385}}
        
        _model.themeId = data["nums"]["themeID"].GetInt();
        loadTheme(_model.themeId);
    } else if ("tableThemeVotingPeriod" == eventName) {
//        {"event":"tableThemeVotingPeriod","ids":{"tableID":"ac4ce94e-1326-4c17-9f0b-ec67f29cf9ef"},"texts":{"period":"Vote"},"nums":{"themeIDs":[398,393,378,397],"timeRemaining":30000}}
    } else if ("chatMessage" == eventName) {
//        {"event":"chatMessage","ids":{ "userID": "45751b2f-a7a8-4cfa-bc9a-3cd01b0c1521", "tableID": "b4346b70-ab6f-11e2-9e96-0800200c9a66"},"texts":{ "name": "AcesOverEasy", "message": "Hey! glad you could make it" }
        
        // Find seat for user
        auto seatIndex = getSeatForUserId(data["ids"]["userID"].GetString());
        if (seatIndex >= 0) {
            _chatBubbles[seatIndex]->showMessage(data["texts"]["message"].GetString());
        }
    } else if ("playerAvatarChange" == eventName) {
        // {"event":"playerAvatarChange","ids":{"userID":"d94d10ee-9817-4dd9-a0af-5f8fbc4b3eec","avatarID":"2f843c67-8ebc-477a-982b-d35061d57528"}}
        auto seatIndex = getSeatForUserId(data["ids"]["userID"].GetString());
        if (seatIndex >= 0) {
            _avatars[seatIndex]->loadAvatar(_game->getApi(), data["ids"]["avatarID"].GetString(), getRotationForSeat(seatIndex));
        }
    } else if ("tableInsufficientChips" == eventName) {
        // Only sent to me, and i'm only sitting at a given seat
        
        auto time = data["nums"]["time"].GetInt();
        _buyIn = BuyInView::create(time, &_model, _game->getModel());
        _buyIn->setAnchorPoint(Vec2(.5, .5));
        auto size = getContentSize();
        _buyIn->setPosition(size.width / 2, size.height / 2);
        _buyIn->setBuyInDismissedCallback([=] {
            // Stand the player up
            _game->getApi()->tableStandUp(_model.getId().c_str(), NullCallback);
            _buyIn = nullptr;
        });
        _buyIn->setBuyInClickedCallback([=](int64_t chips, bool autoRebuy) {
            _game->getApi()->tableBuyIn(_model.getId().c_str(), chips, autoRebuy, NullCallback);
            _buyIn->removeFromParent();
            _buyIn = nullptr;
        });
        addChild(_buyIn);
    } else if ("TODORESUME" == eventName) {
//#error resume
//        https://play.qa.hdpoker.com/common/library/gifts/default/gift_417.png
    }
    
    return true;
}

void TableViewController::updateSitButtons() {
    if (_model.mySeat == -1) {
        // Show sit buttons where no-one is sitting
        for (auto &seat : _model.seats) {
            if (!seat.isSitting()) {
                _sitButtons[seat.seatIndex]->setVisible(true);
            }
        }
    } else {
        // Hide all sit buttons
        for (auto &button: _sitButtons) {
            button->setVisible(false);
        }
    }
}

int TableViewController::getSeatForUserId(const std::string& userId) {
    for (auto &seat : _model.seats) {
        if (seat.playerId == userId) {
            return seat.seatIndex;
        } 
    }
    return -1;
}

void TableViewController::setupThirdPersonView() {
    
}

void TableViewController::setupFirstPersonView() {
    
}

void TableViewController::loadTheme(int themeId) {
    retain();
    auto url = std::string("http://play.qa.hdpoker.com/common/library/themes/default/bg_") + std::to_string(themeId) + std::string(".jpg");
    Downloader::getSpriteFromUrl(url, [=](Sprite *sprite) {
        if (sprite) {
            auto contentSize = getContentSize();
            _backgroundLayer->removeAllChildren();
            coverFill(sprite, contentSize);
            _backgroundLayer->setPosition(contentSize.width / 2, contentSize.height / 2);
            sprite->getTexture()->setAliasTexParameters();
            _backgroundLayer->addChild(sprite);
        }
        release();
    });
    
    for (auto i = 0; i < _model.tableSize; i++) {
        retain();
        auto url = std::string("https://play.qa.hdpoker.com/common/library/themes/default/chair_") + std::to_string(themeId) + std::string("_p") + std::to_string(i + 1) + std::string(".png");
        Downloader::getSpriteFromUrl(url, [=](Sprite *sprite) {
            if (sprite) {
                _chairs[i]->removeAllChildren();
                sprite->setAnchorPoint(Vec2(.5, .5));
                sprite->getTexture()->setAliasTexParameters();
                _chairs[i]->addChild(sprite);
            }
            release();
        });
    }
    
    retain();
    auto tableUrl = std::string("http://play.qa.hdpoker.com/common/library/themes/default/table_") + std::to_string(themeId) + std::string(".png");
    Downloader::getSpriteFromUrl(tableUrl, [=](Sprite *sprite) {
        if (sprite) {
            _tableLayer->removeAllChildren();
            sprite->setAnchorPoint(Vec2(.5, .5));
            sprite->getTexture()->setAliasTexParameters();
            _tableLayer->addChild(sprite);
        }
        release();
    });
}

void TableViewController::loadAvatars(const std::vector<std::string> &avatarIds) {
    _api->playerLoadAvatars(avatarIds, [=] (const Message* message) {
        if (message->isSuccess()) {
            auto &data = message->getData()["result"]["avatarDatas"];
            
            auto count = data.Size();
            for (auto i = 0; i < count; i++) {
                auto &items = data[i]["items"];
                
                std::string avatarId = data[i]["id"].GetString();
                
                std::vector<AvatarView::Item> avatarItems;
                auto faceId = -1;
                
                for (auto i = 0; i < items.Size(); i++) {
                    AvatarView::Item item;
                    item.id = items[i]["id"].GetInt();
                    item.type = items[i]["type"].GetString();
                    
                    if (item.type == "face") {
                        // When you get a face.. also load the body & arms
                        faceId = item.id;
                    }
                    
                    if (items[i].HasMember("color")) {
                        item.color = items[i]["color"].GetUint();
                    } else {
                        item.color = 0xFFFFFFFF;
                    }
                    avatarItems.push_back(item);
                }
                
                if (faceId != -1) {
                    AvatarView::Item body;
                    body.id = faceId;
                    body.type = "body";
                    avatarItems.push_back(body);
                    
                    AvatarView::Item arms;
                    arms.id = faceId;
                    arms.type = "arms";
                    avatarItems.push_back(arms);
                }
                
                // Spin through the model and find the match..
                for (auto x = 0; x < _model.tableSize; x++) {
                    if (_model.seats[x].avatarId == avatarId) {
                        _avatars[x]->loadAvatar(getRotationForSeat(x), avatarItems);
                        break;
                    }
                }
            }
        }
    });
}

void TableViewController::buildView(const Message *tableSubscribeMessage) {
    _model.populateFromTableSubscription(_game->getModel()->getUserId(), tableSubscribeMessage);
    
    // Build view
    _viewMode = ViewMode::ThirdPerson;
    auto viewScale = containScale(designSize, getContentSize());
    auto viewSize = containSize(designSize, getContentSize());
    auto size = getContentSize();
    
    // Load avatars
    std::vector<std::string> avatarIds;
    for (auto &seat: _model.seats) {
        if (seat.isSitting()) {
            avatarIds.push_back(seat.avatarId);
        }
    }

    loadAvatars(avatarIds);
    

    // Background
    _backgroundLayer = Node::create();
    _backgroundLayer->setPosition(size.width / 2, size.height / 2);
    addChild(_backgroundLayer);
    
    _tableView = Node::create();
    _tableView->setContentSize(viewSize);
    _tableView->setScale(viewScale);
    addChild(_tableView);
    
    // Chairs
    for (auto i = 0; i < _model.tableSize; i++) {
        auto chairNode = Node::create();
        chairNode->setAnchorPoint(Vec2(.5, .5));
        chairNode->setPosition(getPositionForSeat(Chair, i));
        chairNode->setScale(getScaleForChair(i) * getScaleForElement(Chair));

        _chairs.push_back(chairNode);
        
        // Avatars are loaded through a call to loadAvatars
        auto avatar = AvatarView::create();
        auto pos = getPositionForSeat(Avatar, i);
        pos.y += 30;
        avatar->setPosition(pos);
        avatar->setAnchorPoint(Vec2(.5, .5));
        avatar->setScale(getScaleForElement(Avatar));
        
        if (i >= 6) {
            avatar->setScaleX(avatar->getScaleX() * -1);  // flip horizontal
        }
        
        _avatars.push_back(avatar);
    }
    
    // Table
    _tableLayer = Node::create();
    _tableLayer->setPosition(size.width / 2, 257);
    _tableLayer->setScale(getScaleForElement(Table));
    
    loadTheme(_model.themeId);
    
    _activeHighlight = Sprite::create("sprites/highlight.png");
    _activeHighlight->setVisible(false);
    _tableView->addChild(_activeHighlight);
    
    if (ViewMode::ThirdPerson == _viewMode) {
        // TODO account for table isze
        for (auto i = 0; i <= 2; i++) {
            _tableView->addChild(_chairs[i]);
            _tableView->addChild(_avatars[i]);
        }

        for (auto i = 6; i < 9; i++) {
            _tableView->addChild(_chairs[i]);
            _tableView->addChild(_avatars[i]);
        }
        
        _tableView->addChild(_tableLayer);
        
        for (auto i = 3; i < 6; i++) {
            _tableView->addChild(_avatars[i]);
            _tableView->addChild(_chairs[i]);
        }
    }
    
    _communityCards = BoardCardsView::create();
    _communityCards->setPosition(size.width / 2, size.height / 2 - 30);
    _communityCards->setScale(getScaleForElement(CommunityCards));
    _communityCards->setupInitialState(_model.boardCards);
    _tableView->addChild(_communityCards);
    
    _cardLayer = Node::create();
    _tableView->addChild(_cardLayer);
    
    // Pot View
    _pot = PotView::create();
    _pot->setPosition(size.width / 2, 220);
    _tableView->addChild(_pot);
    
    _dealerButton = Sprite::create("sprites/dealerButton.png");
    _dealerButton->setPosition(getPositionForSeat(DealerButton, _model.dealerButtonIndex));
    _dealerButton->setScale(.5);
    _tableView->addChild(_dealerButton);
    
    _bets.reserve(_model.tableSize);
    for (auto i = 0; i < _model.tableSize; i++) {
        auto betStack = BetStackView::create();
        betStack->setPosition(getPositionForSeat(BetStack, i));
        _tableView->addChild(betStack);
        _bets.push_back(betStack);
    }
    
    // Nameplates
    for (auto i = 0; i < _model.tableSize; i++) {
        auto nameplate = NameplateView::create();
        Vec2 position = getPositionForSeat(Nameplate, i);
        nameplate->setAnchorPoint(Vec2(.5, .5));
        nameplate->setPosition(position);
    
        if (_model.seats[i].playerName.length() > 0) {
            nameplate->setName(_model.seats[i].playerName.c_str());
            nameplate->setChips(_model.seats[i].chips);
            nameplate->setActive(_model.activeSeatIndex == i);
        } else {
            nameplate->setVisible(false);
        }
    
        _tableView->addChild(nameplate);
        
        _nameplates.push_back(nameplate);
        
        auto winView = PlayerWinView::create();
        winView->setPosition(getPositionForSeat(Nameplate, i));
        winView->setVisible(false);
        _tableView->addChild(winView);
        _winViews.push_back(winView);
        
        auto chatView = ChatBubbleView::create();
        chatView->setPosition(getPositionForSeat(Nameplate, i));
        addChild(chatView);
        _chatBubbles.push_back(chatView);
        
        auto sitButton = ui::Button::create("sprites/button-sit.png", "");
        sitButton->setPosition(getPositionForSeat(Nameplate, i));
        sitButton->setZoomScale(-.05);
        sitButton->setVisible(false);
        sitButton->addClickEventListener([=](Ref*) {
            _game->getApi()->tableRequestSeat(_model.getId().c_str(), i, NullCallback);
        });
        addChild(sitButton);
        _sitButtons.push_back(sitButton);
    }
    
    _tableLabel = Label::createWithTTF(_model.tableName + std::string(" - ") + _model.activeHandId, UniSansRegular, 14);
    _tableLabel->setPosition(Vec2(0, size.height));
    _tableLabel->setAlignment(TextHAlignment::LEFT);
    _tableLabel->setAnchorPoint(Point(0.0f, 1.0f));
    _tableView->addChild(_tableLabel);
    
    _actionButtons = ActionButtonsView::create();
    _actionButtons->setActionCallback([=] (ActionButtonsView::Action action, int64_t betAmount) {
        if (action == ActionButtonsView::ActionCall) {
            _api->handPostAction(_model.activeHandId.c_str(), "call", _choicesCallAmount, NullCallback);
        } else if (action == ActionButtonsView::ActionFold) {
            _api->handPostAction(_model.activeHandId.c_str(), "fold", 0, NullCallback);
        } else if (action == ActionButtonsView::ActionBet) {
            _api->handPostAction(_model.activeHandId.c_str(), "bet", betAmount, NullCallback);
        }
    });
    _actionButtons->setPosition(size.width, 0);
    _tableView->addChild(_actionButtons);
    
    updateSitButtons();
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/thenewclient.mp3");
    
    // Customize loaded content for specific view style
    if (ViewMode::ThirdPerson == _viewMode) {
        setupThirdPersonView();
    } else {
        setupFirstPersonView();
    }
    
    CCLOG("Building view COMPLETE");
}
