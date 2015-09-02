#include "TableViewController.h"
#include "ThirdPersonNameplateView.h"
#include "FirstPersonNameplateView.h"
#include "PotView.h"
#include "SimpleAudioEngine.h"
#include "GameController.h"
#include "ActionButtonsView.h"
#include "CardView.h"
#include "BoardCardsView.h"
#include "BetStackView.h"
#include "PlayerWinView.h"
#include "TableGiftView.h"
#include "AvatarView.h"
#include "Downloader.h"
#include "Shared.h"
#include "ChatBubbleView.h"
#include "UI/UIButton.h"
#include "TableModel.h"
#include "BuyInView.h"
#include "OverlayMessageView.h"
#include "SettingsViewController.h"
#include "HoleCardsView.h"
#include "TableChatViewController.h"

using namespace cocos2d;

const float flagSpeed = 0.06f;

const Size designSize = Size(1136, 640);

TableViewController* TableViewController::create(GameController *game, const Message *tableSubscribeMessage) {
	auto controller = TableViewController::create();
	controller->setContentSize(Director::getInstance()->getWinSize());
	controller->setAnchorPoint(Vec2(0.5f, 0.5f));
	controller->_game = game;
	controller->_api = game->getApi();
	controller->_model.populateFromTableSubscription(game->getModel()->getUserId(), tableSubscribeMessage);
	controller->buildView();
	return controller;
}

TableViewController *TableViewController::create(GameController *game, const TableModel& model) {
	auto controller = TableViewController::create();
	controller->setContentSize(Director::getInstance()->getWinSize());
	controller->setAnchorPoint(Vec2(0.5f, 0.5f));
	controller->_game = game;
	controller->_api = game->getApi();
	controller->_model = model;
	controller->buildView();
	return controller;
}

TableViewController::TableViewController() : _backgroundLayer(nullptr), _buyIn(nullptr) {
	setClippingEnabled(true);
	_viewMode = SettingsViewController::isThirdPerson() ? ThirdPerson : FirstPerson;
}

TableViewController::~TableViewController() {
	_backgroundLayer->release();
	_tableView->release();
	_tableLayer->release();
	_armsLayer->release();
	_activeHighlight->release();
	_communityCards->release();
	_cardLayer->release();
	_pot->release();
	_dealerButton->release();
	_tableLabel->release();
	_actionButtons->release();
	_sitBackIn->release();
	_thirdPersonNameplate->release();
	_firstPersonNameplate->release();
	_bestHandDescription->release();
    _showButton->release();

	for (auto i = 0; i < _model.getSeatCount(); i++) {
		_chairs[i]->release();
		_avatars[i]->release();
		_bets[i]->release();
		_nameplates[i]->release();
		_gifts[i]->release();
		_winViews[i]->release();
		_chatBubbles[i]->release();
		_sitButtons[i]->release();
        _holeCards[i]->release();
	}
}

void TableViewController::setContentSize(const Size& contentSize) {
	Node::setContentSize(contentSize);

	// Haven't built the view yet
	if (!_backgroundLayer) {
		return;
	}

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

void TableViewController::setReplayMode(bool replay) {
	_replayMode = replay;
	_actionButtons->setVisible(!replay);
}

void TableViewController::setViewMode(ViewMode viewMode) {
	if (_viewMode != viewMode && (ThirdPerson == viewMode || _model.amISitting())) {
		_viewMode = viewMode;

		if (FirstPerson == viewMode) {
			setupFirstPersonView();
		}
		else {
			setupThirdPersonView();
		}
	}
}

TableModel* TableViewController::getModel() {
	return &_model;
}

/*
This function maps seat indexes from data sources (model or network), to the corresponding array index that maps to the view on the screen.

For 3st person, the seat index is the array index.
For 1st person, the players are "rotated" around the table so the seat you picked maps to you looking out at the table.

Critical!
This function is dependent on the view mode.  If you are going to change the view mode on the current code path, be sure to call this function
AFTER you have done so, otherwise you're likely to get the incorrect index.
*/
int TableViewController::getVisualIndexFromSeatIndex(int seatIndex) {
	return ThirdPerson == _viewMode ? seatIndex : (seatIndex - _model.getMySeat() + _model.getSeatCount()) % _model.getSeatCount();
}

int TableViewController::getRotationForSeat(int seatIndex) {
	if (ThirdPerson == _viewMode) {
		switch (seatIndex) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 4;
		case 4: return 5;
		case 5: return 6;
		case 6: return 7;
		case 7: return 8;
		case 8: return 9;
		}
	}
	else {
		switch (seatIndex) {
		case 0: return 0;
		case 1: return 7;
		case 2: return 8;
		case 3: return 9;
		case 4: return 9;
		case 5: return 1;
		case 6: return 1;
		case 7: return 2;
		case 8: return 3;
		}
	}
	CCASSERT(false, "Invalid seat index");
}

int TableViewController::getRotationForAvatar(int seatIndex) {
	if (ThirdPerson == _viewMode) {
		switch (seatIndex) {
            case 0: case 8: return 1;
            case 1: case 7: return 2;
            case 2: case 6: return 3;
            case 3: case 5: return 4;
            case 4:         return 5;
		}
	}
	else {
		switch (seatIndex) {
            case 0: return 0;
            case 1: return 3;
            case 2: return 2;
            case 3: return 1;
            case 4: return 1;
            case 5: return 1;
            case 6: return 1;
            case 7: return 2;
            case 8: return 3;
		}
	}
	CCASSERT(false, "Invalid seat index");
}

float getCardRotationForSeat(int seatIndex) {
	switch (seatIndex) {
        case 0: return 180.0f;
        case 1: return 210.0f;
        case 2: return 225.0f;
        case 3: return -45.0f;
        case 4: return 0.000f;
        case 5: return 32.00f;
        case 6: return 113.0f;
        case 7: return 156.0f;
        case 8: return 196.0f;
	}
	CCASSERT(false, "Invalid seat index");
}

float TableViewController::getScaleForElement(Element element) {
	if (ThirdPerson == _viewMode) {
		switch (element) {
            case HoleCards:			return 0.16f;
            case CommunityCards:    return 0.70f;
            case Avatar:			return 0.46f;
            case Chair:				return 0.60f;
            case Table:				return 0.60f;
            case BetStack:			return 0.32f;
		}
	}
	else {
		switch (element) {
            case HoleCards:			return 0.20f;
            case CommunityCards:    return 0.80f;
            case Avatar:			return 0.60f;
            case Chair:				return 0.70f;
            case Table:				return 0.85f;
            case BetStack:			return 0.32f;
		}
	}
    return 1.0f;
}

float TableViewController::getScaleForChair(int seatIndex) {
	if (ThirdPerson == _viewMode) {
		switch (seatIndex) {
		case 0: return 0.820f;
		case 1: return 0.875f;
		case 2: return 0.960f;
		case 3: return 1.080f;
		case 4: return 1.080f;
		case 5: return 1.080f;
		case 6: return 0.960f;
		case 7: return 0.875f;
		case 8: return 0.820f;
		}
	}
	else {
		return 1.05f;
	}
}

// Third person, seat 0 is the seat to the right of center
// First person, seat 0 is you, seat 1 is top-left
Vec2 TableViewController::getPositionForSeat(Element element, int seatIndex) {
    if (ViewMode::ThirdPerson == _viewMode) {
        switch (element) {
            case Avatar: switch (seatIndex) {
                case 0: return Vec2(643, 454);
                case 1: return Vec2(785, 424);
                case 2: return Vec2(886, 371);
                case 3: return Vec2(795, 118);
                case 4: return Vec2(568, 85);
                case 5: return Vec2(343, 118);
                case 6: return Vec2(245, 371);
                case 7: return Vec2(353, 424);
                case 8: return Vec2(496, 454);
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
			case ChatBubble:
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
			case Gift:
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
    } else {
        switch (element) {
            case Avatar: {
                const int spacing = 140;
                const int startOffset = 80;
                const int yOffset = 414;
                
                switch (seatIndex) {
                    case 0: return Vec2(-1000, -1000);
                    case 1: return Vec2(startOffset, yOffset - 80);
                    case 2: return Vec2(startOffset + spacing, yOffset - 20);
                    case 3: return Vec2(startOffset + spacing * 2, yOffset);
                    case 4: return Vec2(startOffset + spacing * 3, yOffset);
                    case 5: return Vec2(startOffset + spacing * 4, yOffset);
                    case 6: return Vec2(startOffset + spacing * 5, yOffset);
                    case 7: return Vec2(startOffset + spacing * 6, yOffset - 20);
                    case 8: return Vec2(startOffset + spacing * 7, yOffset - 80);
                }
            } break;
            case Chair: {
                const int spacing = 140;
                const int startOffset = 80;
                const int yOffset = 360;    // 394
                
                switch (seatIndex) {
					case 0: return Vec2(-1000, -1000);
                    case 1: return Vec2(startOffset, yOffset - 80);
                    case 2: return Vec2(startOffset + spacing, yOffset - 20);
                    case 3: return Vec2(startOffset + spacing * 2, yOffset);
                    case 4: return Vec2(startOffset + spacing * 3, yOffset);
                    case 5: return Vec2(startOffset + spacing * 4, yOffset);
                    case 6: return Vec2(startOffset + spacing * 5, yOffset);
                    case 7: return Vec2(startOffset + spacing * 6, yOffset - 20);
                    case 8: return Vec2(startOffset + spacing * 7, yOffset - 80);
                }
            } break;
			case ChatBubble: {
				const int spacing = 140;
				const int startOffset = 80;
				switch (seatIndex) {
					case 0: return Vec2(285, 126);
					case 1: return Vec2(startOffset, 600);
					case 2: return Vec2(startOffset + spacing, 600);
					case 3: return Vec2(startOffset + spacing * 2, 600);
					case 4: return Vec2(startOffset + spacing * 3, 600);
					case 5: return Vec2(startOffset + spacing * 4, 600);
					case 6: return Vec2(startOffset + spacing * 5, 600);
					case 7: return Vec2(startOffset + spacing * 6, 600);
					case 8: return Vec2(startOffset + spacing * 7, 600);
				}
			} break;
			case Nameplate: {
                const int spacing = 140;
                const int startOffset = 80;
                switch (seatIndex) {
                    case 0: return Vec2(100, 0);
                    case 1: return Vec2(startOffset, 600);
                    case 2: return Vec2(startOffset + spacing, 600);
                    case 3: return Vec2(startOffset + spacing * 2, 600);
                    case 4: return Vec2(startOffset + spacing * 3, 600);
                    case 5: return Vec2(startOffset + spacing * 4, 600);
                    case 6: return Vec2(startOffset + spacing * 5, 600);
                    case 7: return Vec2(startOffset + spacing * 6, 600);
                    case 8: return Vec2(startOffset + spacing * 7, 600);
                }
			} break;
			case Gift:
            case DealerButton:
            case BetStack: switch(seatIndex) {
                case 0: return Vec2(375, 140);
                case 1: return Vec2(151, 234);
                case 2: return Vec2(236, 297);
                case 3: return Vec2(357, 329);
                case 4: return Vec2(495, 331);
                case 5: return Vec2(642, 332);
                case 6: return Vec2(784, 326);
                case 7: return Vec2(884, 307);
                case 8: return Vec2(977, 252);
            } break;
            case HoleCards: switch (seatIndex) {
                case 0: return Vec2(568, -50);
                case 1: return Vec2(151, 234);
                case 2: return Vec2(236, 297);
                case 3: return Vec2(357, 329);
                case 4: return Vec2(495, 331);
                case 5: return Vec2(642, 332);
                case 6: return Vec2(784, 326);
                case 7: return Vec2(884, 307);
                case 8: return Vec2(977, 252);
            } break;
        }
    }
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
		if ("handStarted" == eventName && std::string(data["ids"]["tableID"].GetString()) == _model.getId()) {

			// Capture the hand ID
			_model.setActiveHandId(data["ids"]["handID"].GetString());
		}
		else if (data["ids"]["handID"].IsNull() ||		// "handSeatEmptied"
			_model.getActiveHandId() != std::string(data["ids"]["handID"].GetString())) {
			// This hand information does not belong to this table - ignore message
			return false;
		}
	}
	else if ((0 == eventName.compare(0, table.length(), table) || "playerSentGift" == eventName || "playerRemoveGift" == eventName || "chatMessage" == eventName) && std::string(data["ids"]["tableID"].GetString()) != _model.getId()) {
		// Table message is not for us
		return false;
	}
    // else, all other messages that do not start with table or hand flow through

	if ("handStarted" == eventName) {
		_firstDeal = true;
		_firstHoleCard = true;
		_tableLabel->setString(_model.getName() + std::string(" - ") + _model.getActiveHandId());

	}
	else if ("handAllCardsRemoved" == eventName) {
		_model.getBoardCards().clear();
        _model.clearBoardCards();
        _model.clearMyHoleCards();
        
		_actionButtons->clearPreselects();
        _communityCards->clearHighlight();
		_communityCards->removeAll();
        _pot->clear();
        _showButton->setVisible(false);
		_bestHandDescription->setVisible(false);

		for (auto i = 0; i < _model.getSeatCount(); i++) {
			_winViews[i]->setVisible(false);
			_nameplates[i]->undim();
            _nameplates[i]->clearHoleCards();
            _holeCards[i]->clearCards();
		}

	}
	else if ("handSeatChoices" == eventName) {
		// {"event":"handSeatChoices","ids":{"handID":"aa8df376-0c00-4857-9b74-54aff33c0e9d"},"bools":{"canFold":true,"canCheck":false,"canCall":true,"canBet":false,"canRaise":true},"chips":{"minRaiseBet":24000,"maxRaiseBet":800000,"call":8000,"sliderPot":16000,"chipStack":792000}}

		_choicesCallAmount = data["chips"]["call"].GetInt64();

		_actionButtons->hidePreselects();

		// Preselect?
		auto preselectOption = _actionButtons->getPreselectAction();
		switch (preselectOption) {
		case ActionButtonsView::PreselectCallAny:
			_api->handPostAction(_model.getActiveHandId().c_str(), "call", _choicesCallAmount, NullCallback);
			break;
		case ActionButtonsView::PreselectCall:
			// TODO Treat as call any for moment...
			_api->handPostAction(_model.getActiveHandId().c_str(), "call", _choicesCallAmount, NullCallback);
			break;
		case ActionButtonsView::PreselectCheckFold:
			if (data["bools"]["canCheck"].GetBool()) {
				_api->handPostAction(_model.getActiveHandId().c_str(), "check", 0, NullCallback);
			}
			else {
				_api->handPostAction(_model.getActiveHandId().c_str(), "fold", 0, NullCallback);
			}
			break;
		default:
		case ActionButtonsView::PreselectNone:
			_model.setActionsAvailable(true, !data["bools"]["canCall"].GetBool(), true);
			_actionButtons->showActions(data["bools"]["canCall"].GetBool(), data["bools"]["canBet"].GetBool(), data["bools"]["canRaise"].GetBool(),
				data["chips"]["minRaiseBet"].GetInt64(), data["chips"]["maxRaiseBet"].GetInt64(), data["chips"]["sliderPot"].GetInt64());
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/myturn.wav");
			break;
		}

		_actionButtons->clearPreselects();

	}
	else if ("handSeatPosted" == eventName) {
		// Can be posted when placing ante!
		// Sometimes doesn't contain all-in field!

		//        {"event":"handSeatPosted","ids":{"handID":"88c3fece-fd4a-441a-855d-d040038b6d25"},"nums":{"seat":3},"texts":{"action":"check"},"bools":{"allIn":false},"chips":{"chips":0,"chipStack":792000,"betStack":8000}}

		auto seatIndex = data["nums"]["seat"].GetInt();
		auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);

		if (data["chips"].HasMember("chipStack")) {
			_model.setChipsForSeat(seatIndex, data["chips"]["chipStack"].GetInt64());
		}
		else {
			_model.setChipsForSeat(seatIndex, data["chips"]["chips"].GetInt64());
		}

		if (seatIndex == _model.getMySeat()) {
			_model.setActionsAvailable(false, false, false);
			_actionButtons->hideActions();
		}

		_activeHighlight->stopAllActions();
		_activeHighlight->setVisible(false);

		if (data["chips"].HasMember("betStack")) {
			_bets[visualSeatIndex]->setChips(data["chips"]["betStack"].GetInt64());
		}

		auto action = std::string(data["texts"]["action"].GetString());
		_nameplates[visualSeatIndex]->setFlag(action.c_str());
		_nameplates[visualSeatIndex]->setActive(false);
		_nameplates[visualSeatIndex]->cancelTimer();
		_nameplates[visualSeatIndex]->setChips(_model.getSeat(seatIndex).chips);

		if ("fold" == action) {
			if (seatIndex == _model.getMySeat()) {
				_nameplates[visualSeatIndex]->clearHoleCards();
				_model.clearMyHoleCards();
			}

			_nameplates[visualSeatIndex]->dim();
            _holeCards[visualSeatIndex]->fold();
		}

		if (data.HasMember("bools") && data["bools"]["allIn"].GetBool()) {
			_nameplates[visualSeatIndex]->setFlag("ALL IN");
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/allin.wav");
		}
		else if ("check" == action) {
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/check.wav");
		}
		else if ("fold" == action) {
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/fold.wav");
		}
		else if ("raise" == action) {
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/raise.wav");
		}
		else {
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/bet.wav");
		}
	}
    else if ("handPotUpdated" == eventName /*|| "handDisplayPerceivedPot" == eventName*/) {       // PerceivedPot is disabled at the moment
		// {"event":"handDisplayPerceivedPot","ids":{"handID":"c7ef8a3b-bc5f-4d53-a148-a6425706d3ea"},"nums":{"numPots":1},"chips":{"chips":[30000]}}
		_pot->setChips(data["chips"]["chips"][0U].GetInt64());
        
        for (int i = 0; i < _model.getSeatCount(); i++) {
            _bets[i]->animateToPot();
        }
	}
	else if ("handSeatPostedBB" == eventName || "handSeatPostedSB" == eventName) {

		//        {"event":"handSeatPostedBB","ids":{"handID":"b401e24a-5a16-46dd-844d-f5ba9a2a7679"},"nums":{"seat":2},"chips":{"chips":20000,"chipStack":21170000,"betStack":20000}}

		auto visualSeatIndex = getVisualIndexFromSeatIndex(data["nums"]["seat"].GetInt());

		_bets[visualSeatIndex]->setChips(data["chips"]["betStack"].GetInt64());
		_nameplates[visualSeatIndex]->setChips(data["chips"]["chipStack"].GetInt64());

	}
	else if ("handCardDealtToPlayer" == eventName) {
		//        {"event":"handCardDealtToPlayer","ids":{"handID":"4e87338c-e882-4a6c-bd4a-36985c4a6dd9"},"texts":{"card":"Kd"}}

		auto card = data["texts"]["card"].GetString();

		auto visualSeatIndex = getVisualIndexFromSeatIndex(getModel()->getMySeat());
		if (_firstHoleCard) {
			_firstHoleCard = false;

			_nameplates[visualSeatIndex]->showHoleCard(0, card);
			_model.receiveMyHoleCard(card);
		}
		else {
			_nameplates[visualSeatIndex]->showHoleCard(1, card);
			_model.receiveMyHoleCard(card);
		}


	}
	else if ("handCardsDealt" == eventName) {
		// {"event":"handCardsDealt","ids":{"handID":"4e87338c-e882-4a6c-bd4a-36985c4a6dd9"},"nums":{"seats":[2,5,6]}}

		auto &seats = data["nums"]["seats"];
		auto dealerLocation = getPositionForSeat(DealerButton, getVisualIndexFromSeatIndex(_model.getDealerButtonSeatIndex()));
        const float dealSpacing = 0.12f;
		int numSeats = static_cast<int>(seats.Size());
		for (auto i = 0; i < numSeats; i++) {
			auto visualSeatIndex = getVisualIndexFromSeatIndex(seats[i].GetInt());
            _holeCards[visualSeatIndex]->dealCard(_firstDeal, i * dealSpacing + (!_firstDeal ? .5 : 0), getCardRotationForSeat(i));
		}

		_firstDeal = false;
	}
	else if ("tableStood" == eventName) {
		//        {"event":"tableStood","ids":{"tableID":"6ad888aa-3826-4078-9693-da3a86288ebb"},"nums":{"seat":1}}

		auto seatIndex = data["nums"]["seat"].GetInt();
		auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);

		auto formerSeat = _model.getMySeat();
		_model.emptySeat(seatIndex);

		_bets[visualSeatIndex]->clearBet();
		_gifts[visualSeatIndex]->clearGift();
		_nameplates[visualSeatIndex]->setVisible(false);
		_avatars[visualSeatIndex]->stopAndClearAvatar();

		if (seatIndex == formerSeat) {
			if (_buyIn) {
				//  Note: We have to set _buyIn to nullptr but hold on to it for this function until we remove it from it's parent
				auto view = _buyIn;
				_buyIn = nullptr;
				view->removeFromParent();
			}
            
            _sitBackIn->setVisible(false);
			setViewMode(ThirdPerson);
		}

		updateSitButtons();

	}
	else if ("handPotAwarded" == eventName) {
		//  {"event":"handPotAwarded","ids":{"handID":"a25f4227-300f-41d5-8bd6-0a5399bf13b5"},"texts":{"bestHand":[["Ac","5c","4c","2h","3c"]],"bestHandDescription":"Straight"},"nums":{"seats":[1],"pot":0},"chips":{"chips":[50],"chipStacks":[50]},"bools":{"sidePots":false}}

		_actionButtons->hidePreselects();

		for (auto i = 0; i < _model.getSeatCount(); i++) {
			_bets[i]->clearBet();
		}

		auto &seats = data["nums"]["seats"];
		int numSeats = static_cast<int>(seats.Size());
		for (auto i = 0; i < numSeats; i++) {
			auto seatIndex = seats[i].GetInt();
			auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);
			_winViews[visualSeatIndex]->setVisible(true);
			_winViews[visualSeatIndex]->setPlayerName(_model.getSeat(seatIndex).playerName.c_str());
			_winViews[visualSeatIndex]->setChipsWon(data["chips"]["chips"][i].GetInt64());
			_nameplates[visualSeatIndex]->setChips(data["chips"]["chipStacks"][i].GetInt64());
			_pot->sendChipsToParentLocation(getPositionForSeat(Nameplate, visualSeatIndex));
		}
        
        auto &bestHands = data["texts"]["bestHand"];
        std::vector<std::string> allBestHands;
        for (int i = 0; i < static_cast<int>(bestHands.Size()); i++) {
            auto &best = bestHands[i];
			for (int c = 0; c < static_cast<int>(best.Size()); c++) {
                allBestHands.push_back(best[c].GetString());
            }
        }
        
        _communityCards->highlightMatchingCards(allBestHands);
        if (isFirstPerson() && _model.amISittingIn()) {
            _firstPersonNameplate->highlightMatchingCards(allBestHands);
        }

		for (auto i = 0; i < _model.getSeatCount(); i++) {
            _holeCards[i]->highlightMatchingCards(allBestHands);
			_nameplates[i]->clearFlag(0);
		}

		_bestHandDescription->setVisible(true);
		_bestHandDescription->setString(data["texts"]["bestHandDescription"].GetString());

		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/won.wav");

	}
	else if ("handDealerButton" == eventName) {
		auto seatIndex = data["nums"]["seat"].GetInt();
		auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);
		_model.setDealerButtonSeatIndex(seatIndex);
        
        auto dealerButtonLocation = getPositionForSeat(DealerButton, visualSeatIndex);
		_dealerButton->runAction(MoveTo::create(1, dealerButtonLocation));
        for (int i = 0; i < _model.getSeatCount(); i++) {
            _holeCards[i]->setDealerLocation(dealerButtonLocation);
        }
	}
	else if ("tableSitIn" == eventName) {
		auto seatIndex = data["nums"]["seat"].GetInt();
		auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);

		_model.setPlayerStateForSeat(seatIndex, TableModel::Waiting);

		_nameplates[visualSeatIndex]->clearFlag(0);

		if (seatIndex == _model.getMySeat()) {
			OverlayMessageView::show("You will join the game when the next hand starts!");
			_sitBackIn->setVisible(false);
		}

	}
	else if ("tableSitOut" == eventName) {
		//        {"event":"tableSitOut","ids":{"tableID":"eeb6963c-ee8b-4dcf-a6ac-a0f42bd122ba"},"nums":{"seat":6},"bools":{"autoPilot":false}}
		auto seatIndex = data["nums"]["seat"].GetInt();
		auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);

		_nameplates[visualSeatIndex]->setFlag("SITTING OUT", false);
		_model.setPlayerStateForSeat(seatIndex, TableModel::SittingOut);

		if (seatIndex == _model.getMySeat()) {
			_sitBackIn->setVisible(true);
		}
	}
	else if ("tableSeated" == eventName) {
		//        {"event":"tableSeated","ids":{"userID":"fd2f6192-5ce6-42f7-8026-8267d3b3ce95","avatarID":"82823c8f-992a-4bc6-8ece-feed2c4dfeb4","tableID":"b2d37d90-32a0-4f15-8b1b-70533ad13612"},"nums":{"seat":0},"texts":{"name":"nous"}}

		auto seatIndex = data["nums"]["seat"].GetInt();

		auto seat = _model.seatPlayer(seatIndex, _game->getModel()->getUserId(), data["texts"]["name"].GetString(), data["ids"]["userID"].GetString(), data["ids"]["avatarID"].GetString());

		if (_model.getMySeat() == seatIndex) {
			setViewMode(SettingsViewController::isThirdPerson() ? ThirdPerson : FirstPerson);
		}

		auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);

		_nameplates[visualSeatIndex]->setName(seat.playerName.c_str());
		_nameplates[visualSeatIndex]->setPlayerID(seat.playerId);
		_nameplates[visualSeatIndex]->setAvatarID(seat.avatarId);
		_nameplates[visualSeatIndex]->setVisible(true);

		// Load avatar
		std::vector<std::string> avatarIds;
		avatarIds.push_back(seat.avatarId);
		loadAvatars(avatarIds);

		updateSitButtons();

	}
	else if ("tableBuyIn" == eventName) {
		//        {"event":"tableBuyIn","ids":{"userID":"fd2f6192-5ce6-42f7-8026-8267d3b3ce95","tableID":"c032aecb-5429-49fd-96be-ed1e1018a28b"},"nums":{"seat":8},"texts":{"name":"nous"},"chips":{"chips":2000000,"chipStack":2000000}}

		auto visualSeatIndex = getVisualIndexFromSeatIndex(data["nums"]["seat"].GetInt());
		_nameplates[visualSeatIndex]->setChips(data["chips"]["chipStack"].GetInt64());
        _nameplates[visualSeatIndex]->clearFlag(0);

	}
	else if ("handHandDescription" == eventName) {

	}
	else if ("handPreSelect" == eventName) {
		// {{"event":"handPreSelect","ids":{"handID":"59ce22bd-78b7-42b7-a566-11521af03309"},"bools":{"canFold":false,"canCheck":true,"canCall":false,"canCallAny":true}}
		_actionButtons->showPreselects(data["bools"]["canCall"].GetBool(), 0);

	}
	else if ("handPreSelectRemoved" == eventName) {
		// {event: "handPreSelectRemoved"}

		_actionButtons->hidePreselects();

	}
	else if ("handSeatStartedTimeout" == eventName) {
		//    {"event":"handSeatStartedTimeout","ids":{"handID":"f7273311-9f5f-4c7f-998c-15eaac325c44","tableID":"e0b4cb65-3a5d-4516-8597-e3632b058cbb"},"nums":{"seat":0,"time":12600}}
		auto visualSeatIndex = getVisualIndexFromSeatIndex(data["nums"]["seat"].GetInt());
		_nameplates[visualSeatIndex]->startTimer(data["nums"]["time"].GetInt() * .001);

	}
	else if ("handSeatActive" == eventName) {
		//    {"event":"handSeatActive","ids":{"handID":"f7273311-9f5f-4c7f-998c-15eaac325c44"},"nums":{"seat":0}}

		auto seatIndex = data["nums"]["seat"].GetInt();
		auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);

		_model.setActiveSeatIndex(seatIndex);

		// Clear 'active' on all seats
		for (auto i = 0; i < _model.getSeatCount(); i++) {
			_nameplates[i]->setActive(false);
		}

		_nameplates[visualSeatIndex]->setActive(true);

		_activeHighlight->stopAllActions();
		_activeHighlight->setPosition(getPositionForSeat(Avatar, visualSeatIndex));
		_activeHighlight->setVisible(true);
		_activeHighlight->runAction(RepeatForever::create(RotateBy::create(3, 360)));

	}
	else if ("handFlopDealt" == eventName) {
		//        {"event":"handFlopDealt","ids":{"handID":"2ace10b1-7c21-4048-bf26-c00299765448"},"texts":{"cards":["5c","2c","9c"]}}

		_actionButtons->hidePreselects();

		// Clear all flags
		for (auto i = 0; i < _model.getSeatCount(); i++) {
			_nameplates[i]->clearFlag(i * flagSpeed);
		}

		auto &cards = data["texts"]["cards"];
		_communityCards->showFlop(cards[0U].GetString(), cards[1U].GetString(), cards[2U].GetString());
		_model.setBoardCards(cards[0U].GetString(), cards[1U].GetString(), cards[2U].GetString(), "", "");

	}
	else if ("handTurnDealt" == eventName) {
		_actionButtons->hidePreselects();

		// Clear all flags
		for (auto i = 0; i < _model.getSeatCount(); i++) {
			_nameplates[i]->clearFlag(i * flagSpeed);
		}

		_communityCards->showTurn(data["texts"]["card"].GetString());
		_model.setBoardCards("", "", "", data["texts"]["card"].GetString(), "");

	}
	else if ("handRiverDealt" == eventName) {
		_actionButtons->hidePreselects();

		// Clear all flags
		for (auto i = 0; i < _model.getSeatCount(); i++) {
			_nameplates[i]->clearFlag(i * flagSpeed);
		}

		_communityCards->showRiver(data["texts"]["card"].GetString());
		_model.setBoardCards("", "", "", "", data["texts"]["card"].GetString());
    } else if ("handChipsReturned" == eventName) {
//        {"event":"handChipsReturned","ids":{"handID":"0a85e7e8-996d-4557-a27b-ab41b37b158e"},"nums":{"seat":7},"chips":{"chips":10000,"chipStack":1640900,"betStack":10000}}
        
        auto visualSeatIndex = getVisualIndexFromSeatIndex(data["nums"]["seat"].GetInt());
        _nameplates[visualSeatIndex]->setChips(data["chips"]["chipStack"].GetInt64());
        
    } else if ("tableThemeChange" == eventName) {
//        {"event":"tableThemeChange","ids":{"tableID":"c601aa58-9961-45d6-8c17-2f3e1c789c03"},"nums":{"themeID":385}}
        
        _model.setThemeId(data["nums"]["themeID"].GetInt());
        loadTheme(_model.getThemeId());
        
    } else if ("chatMessage" == eventName) {
//        {"event":"chatMessage","ids":{ "userID": "45751b2f-a7a8-4cfa-bc9a-3cd01b0c1521", "tableID": "b4346b70-ab6f-11e2-9e96-0800200c9a66"},"texts":{ "name": "AcesOverEasy", "message": "Hey! glad you could make it" }
        
        auto visualSeatIndex = getVisualIndexFromSeatIndex(_model.getSeatForUserId(data["ids"]["userID"].GetString()));
		if (visualSeatIndex >= 0) {	// server bug:  is allowing chat from non-seated players
			_chatBubbles[visualSeatIndex]->showMessage(data["texts"]["message"].GetString());
		}
        
    } else if ("playerAvatarChange" == eventName) {
        // {"event":"playerAvatarChange","ids":{"userID":"d94d10ee-9817-4dd9-a0af-5f8fbc4b3eec","avatarID":"2f843c67-8ebc-477a-982b-d35061d57528"}}
        auto seatIndex = _model.getSeatForUserId(data["ids"]["userID"].GetString());
        auto visualSeatIndex = getVisualIndexFromSeatIndex(seatIndex);
        
        _avatars[visualSeatIndex]->loadAvatar(_game, data["ids"]["avatarID"].GetString(), getRotationForAvatar(seatIndex));

        if (isFirstPerson() && _model.getMySeat() == seatIndex) {
            _firstPersonNameplate->setAvatar(_game, data["ids"]["avatarID"].GetString());
        }
    } else if ("tableInsufficientBuyIn" == eventName) {
//        {"event":"tableInsufficientBuyIn","ids":{"tableID":"c032aecb-5429-49fd-96be-ed1e1018a28b","userID":"fd2f6192-5ce6-42f7-8026-8267d3b3ce95"},"nums":{"seat":0,"time":null}}
        
        OverlayMessageView::show("You're out of chips!");
        playNotImplemented();
        
    } else if ("tableInsufficientChips" == eventName) {
        // Only sent to me, and i'm only sitting at a given seat
        
        auto time = data["nums"]["time"].GetInt();
        _buyIn = BuyInView::create(time, &_model, _game->getModel());
        
		_buyIn->setBuyInDismissedCallback([=] {
			//  Note: We have to set _buyIn to nullptr but hold on to it for this function until we remove it from it's parent
			auto view = _buyIn;
			_buyIn = nullptr;
			// Stand the player up
			_game->getApi()->tableStandUp(_model.getId().c_str(), NullCallback);
			view->removeFromParent();
        });
		_buyIn->setBuyInClickedCallback([=](int64_t chips, bool autoRebuy) {
			//  Note: We have to set _buyIn to nullptr but hold on to it for this function until we remove it from it's parent
			auto view = _buyIn;
			_buyIn = nullptr;
            _game->getApi()->tableBuyIn(_model.getId().c_str(), chips, autoRebuy, NullCallback);
			view->removeFromParent();
        });
        addChild(_buyIn);
    } else if ("playerLevelChange" == eventName) {
//    {"event":"playerLevelChange","ids":{"userID":"490b7109-36e8-459f-857d-459e813adee4"},"nums":{"level":4,"xpNextLevel":124}}

        auto seatIndex = _model.getSeatForUserId(data["ids"]["userID"].GetString());
        auto location = getPositionForSeat(Chair, getVisualIndexFromSeatIndex(seatIndex));
        
        auto fx = ParticleSystemQuad::create("particles/LevelUp.plist");
        location.x += 80;
        fx->setPosition(location);
        addChild(fx);
        
    } else if ("handCardsShown" == eventName) {
//        {"event":"handCardsShown","ids":{"handID":"3fc1eb5d-3f56-4674-8fa6-d3835da881e0"},"nums":{"seat":3},"texts":{"cards":["Ah","Kh"]}}
        
        auto seatIndex = data["nums"]["seat"].GetInt();
        if (seatIndex == _model.getMySeat() && isFirstPerson()) {
            // In first-person, your cards are shown in your hand
            _firstPersonNameplate->showCards();
        } else {
            // In all other caes, the cards are shown from the table
            auto &cards = data["texts"]["cards"];
            _holeCards[getVisualIndexFromSeatIndex(seatIndex)]->showCards(cards[0u].GetString(), cards[1u].GetString());
            
            if (seatIndex == _model.getMySeat() && !isFirstPerson()) {
                // In third-person, your cards are hidden in the name plate, and shown from the table per above
                _nameplates[getVisualIndexFromSeatIndex(seatIndex)]->clearHoleCards();
            }
        }
    } else if ("handShowOrMuck" == eventName) {
        
        auto seatIndex = data["nums"]["seat"].GetInt();
        if (seatIndex == _model.getMySeat()) {
            _showButton->setVisible(true);
        }
        
    } else if ("playerSentGift" == eventName) {
		auto objectID = data["nums"]["catalogID"].GetInt();
		std::string senderName = data["texts"]["username"].GetString();
		auto &targetNode = data["ids"]["playerID"];
		std::string target = (!targetNode.IsNull() ? targetNode.GetString() : "");
		if (!targetNode.IsNull()) {
			auto seatIndex = _model.getSeatForUserId(targetNode.GetString());
			auto url = std::string("http://play.dev.hdpoker.com/common/library/gifts/default/gift_") + std::to_string(objectID) + std::string(".png");
			Downloader::getSpriteFromUrl(url, [=](cocos2d::Sprite* sprite, const std::string& path, bool) {
				if (sprite) {
					_gifts[seatIndex]->setItem(sprite, "From " + senderName);
				}
			});
		}
		else
		{
			auto &seats = _model.getSeats();
			for (auto i = 0; i < static_cast<int>(seats.size()); ++i)
			{
				if (!seats[i].isSitting()) continue;
				auto url = std::string("http://play.dev.hdpoker.com/common/library/gifts/default/gift_") + std::to_string(objectID) + std::string(".png");
				Downloader::getSpriteFromUrl(url, [=](cocos2d::Sprite* sprite, const std::string& path, bool) {
					if (sprite) {
						_gifts[i]->setItem(sprite, "From " + senderName);
					}
				});
			}
		}
	}
	else if ("playerRemoveGift" == eventName) {
		auto target = data["ids"]["userID"].GetString();
		auto seatIndex = _model.getSeatForUserId(target);
		if (seatIndex != -1) _gifts[seatIndex]->clearGift();
	}
    return true;
}

void TableViewController::updateSitButtons() {
	if (!_model.amISitting()) {
		// Show sit buttons where no-one is sitting
		for (auto &seat : _model.getSeats()) {
            _sitButtons[getVisualIndexFromSeatIndex(seat.seatIndex)]->setVisible(!seat.isSitting());
		}
	}
	else {
		// Hide all sit buttons
		for (auto &button : _sitButtons) {
			button->setVisible(false);
		}
	}
}

void TableViewController::loadTheme(int themeId, bool chairsOnly) {
	retain();
	auto tableUrl = std::string("http://play.qa.hdpoker.com/common/library/themes/default/table_") + std::to_string(themeId) + std::string(".png");
	Downloader::getSpriteFromUrl(tableUrl, [=](Sprite *sprite, const std::string&, bool) {
		if (sprite) {
			_tableLayer->removeAllChildren();
			sprite->setAnchorPoint(Vec2(.5, .5));
			sprite->getTexture()->setAliasTexParameters();
			_tableLayer->addChild(sprite);
		}
		release();
	});

	retain();
	auto url = std::string("http://play.qa.hdpoker.com/common/library/themes/default/bg_") + std::to_string(themeId) + std::string(".jpg");
	Downloader::getSpriteFromUrl(url, [=](Sprite *sprite, const std::string&, bool) {
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

	for (auto i = 0; i < _model.getSeatCount(); i++) {
		retain();
		auto url = std::string("https://play.qa.hdpoker.com/common/library/themes/default/chair_") + std::to_string(themeId) + std::string("_p") + std::to_string(getRotationForSeat(i)) + std::string(".png");
		Downloader::getSpriteFromUrl(url, [=](Sprite *sprite, const std::string&, bool) {
			if (sprite) {
				_chairs[i]->removeAllChildren();
				sprite->setAnchorPoint(Vec2(.5, .5));
				sprite->getTexture()->setAliasTexParameters();
				_chairs[i]->addChild(sprite);
			}
			release();
		});
	}

	_model.setThemeId(themeId);
}

void TableViewController::loadAvatars(const std::vector<std::string> &avatarIds) {
    _game->getAvatarCache()->loadAvatars(avatarIds, [=](const std::string& avatarId, const AvatarCache::AvatarItems& items) {
        // Spin through the model and find the match..
        int x = 0;
        for (auto &seat : _model.getSeats()) {
            if (seat.avatarId == avatarId) {
                auto visualSeatIndex = getVisualIndexFromSeatIndex(x);
                _avatars[visualSeatIndex]->loadAvatar(items, getRotationForAvatar(visualSeatIndex));
                break;
            }
            x++;
        }
	});
}

void TableViewController::updateNameplates() {

	if (isFirstPerson()) {
		_nameplates[0] = _firstPersonNameplate;
	}
	else {
		_nameplates[0] = _thirdPersonNameplate;
	}

	for (int seatIndex = 0; seatIndex < _model.getSeatCount(); seatIndex++) {
		auto visualIndex = getVisualIndexFromSeatIndex(seatIndex);

		auto &nameplate = _nameplates[visualIndex];
		nameplate->setPosition(getPositionForSeat(Nameplate, visualIndex));

		// Update the nameplate
		auto &seat = _model.getSeat(seatIndex);

		// Useful debug:
		// CCLOG("Player @ %d is %s going to %d", seatIndex, seat.playerName.c_str(), visualIndex);

		if (seat.playerName.length() > 0) {
			nameplate->setVisible(true);
			nameplate->setName(seat.playerName.c_str());
			nameplate->setChips(seat.chips);
			nameplate->setActive(_model.getActiveSeatIndex() == seatIndex);
			if (TableModel::SittingOut == seat.playerState) {
				nameplate->setFlag("SITTING OUT", false);
			}
		}
		else {
			nameplate->setVisible(false);
		}
	}
}

void TableViewController::resetView() {
	_tableView->removeAllChildrenWithCleanup(false);
	_tableView->removeFromParentAndCleanup(false);
	_backgroundLayer->removeFromParentAndCleanup(false);
    _cardLayer->removeAllChildren();
	for (auto &avatar : _avatars) {
		avatar->removeAllChildren();
	}
}

void TableViewController::setupThirdPersonView() {
    resetView();
    
    auto size = getContentSize();
    
    _tableLayer->setPosition(designSize.width / 2, 257);
    _tableLayer->setScale(getScaleForElement(Table));
    _tableLayer->setRotation3D(Vec3::ZERO);
    _dealerButton->setPosition(getPositionForSeat(DealerButton, _model.getDealerButtonSeatIndex()));
    _communityCards->setScale(getScaleForElement(CommunityCards));
    _communityCards->setPosition(designSize.width / 2, designSize.height / 2 - 30);
    
    _pot->setPosition(designSize.width / 2, 200);
    _bestHandDescription->setPosition(designSize.width / 2, 200);
    
    for (auto i = 0; i < _model.getSeatCount(); i++) {
        auto &chair = _chairs[i];
        chair->setPosition(getPositionForSeat(Chair, i));
        chair->setScale(getScaleForChair(i) * getScaleForElement(Chair));
        
        auto &avatar = _avatars[i];
        avatar->setPosition(getPositionForSeat(Avatar, i));
        
        auto avatarScale = getScaleForElement(Avatar);
        avatar->setScale(avatarScale);
        
        if (i >= 6) {
            avatar->setScaleX(-avatarScale);    // flip horizontal
        } else {
            avatar->setScaleX(avatarScale);     // set scale for avatars that were formally in 1rd person locations
        }
        
        auto &betStack = _bets[i];
		betStack->setPosition(getPositionForSeat(BetStack, i));
        betStack->setPotLocation(_pot->getPosition());

		auto &giftView = _gifts[i];
		giftView->setPosition(getPositionForSeat(Gift, i));
        
        auto &winView = _winViews[i];
        winView->setPosition(getPositionForSeat(Nameplate, i));
        
        auto &chatView = _chatBubbles[i];
        chatView->setPosition(getPositionForSeat(ChatBubble, i));
        
        auto &sitButton = _sitButtons[i];
        sitButton->setPosition(getPositionForSeat(Nameplate, i));
        
        auto &holeCard = _holeCards[i];
        holeCard->setPosition(getPositionForSeat(HoleCards, i));
        holeCard->setScale(getScaleForElement(HoleCards));
        holeCard->clearCards();
    }
    
    updateNameplates();
    updateSitButtons();
    
    loadTheme(_model.getThemeId());
    
    // Load avatars
    std::vector<std::string> avatarIds;
    for (auto &seat: _model.getSeats()) {
        if (seat.isSitting()) {
            avatarIds.push_back(seat.avatarId);
        }
    }
    
    loadAvatars(avatarIds);
    
    // Build view tree for third person
    
    _tableView->addChild(_activeHighlight); // Add it to retain/release (unused atm)
    
    addChild(_backgroundLayer);
    addChild(_tableView);
    
        // TODO account for seat count
        // Add chairs/avatars behind the table
        for (auto i = 0; i <= 2; i++) {
            _tableView->addChild(_chairs[i]);
            _tableView->addChild(_avatars[i]);
        }
        
        for (auto i = 6; i < 9; i++) {
            _tableView->addChild(_chairs[i]);
            _tableView->addChild(_avatars[i]);
        }
        
        // Add table
        _tableView->addChild(_tableLayer);
        
        // Add chairs/avatars in front of table
        for (auto i = 3; i < 6; i++) {
            _tableView->addChild(_avatars[i]);
            _tableView->addChild(_chairs[i]);
        }
        
        _tableView->addChild(_communityCards);
        _tableView->addChild(_cardLayer);
        _tableView->addChild(_armsLayer);   // place the arms OVER hole cards (do NOT render arms for players facing away from camera)
        _tableView->addChild(_pot);
        _tableView->addChild(_dealerButton);
        _tableView->addChild(_bestHandDescription);
    
        for (int i = 0; i < _model.getSeatCount(); i++) {
			_tableView->addChild(_bets[i]);
			_tableView->addChild(_gifts[i]);
            _tableView->addChild(_nameplates[i]);
            _tableView->addChild(_winViews[i]);
            _tableView->addChild(_chatBubbles[i]);
            _tableView->addChild(_sitButtons[i]);
            _cardLayer->addChild(_holeCards[i]);
        }
        
    _tableView->addChild(_tableLabel);
    _tableView->addChild(_actionButtons);
    _tableView->addChild(_sitBackIn);
    _tableView->addChild(_showButton);
}

void TableViewController::setupFirstPersonView() {
    resetView();
    
    auto size = getContentSize();
    
    _tableLayer->setScale(getScaleForElement(Table));
    _tableLayer->setPosition(designSize.width / 2, 197);
    _tableLayer->setRotation3D(Vec3(-30, 0, 0));
    _dealerButton->setPosition(getPositionForSeat(DealerButton, _model.getDealerButtonSeatIndex()));
    _communityCards->setScale(getScaleForElement(CommunityCards));
    _communityCards->setPosition(designSize.width / 2, 239);
    
    _pot->setPosition(designSize.width / 2, 130);
    
    _bestHandDescription->setPosition(designSize.width / 2, 130);
    
    for (int i = 0; i < _model.getSeatCount(); i++) {
        auto &chair = _chairs[i];
        chair->setPosition(getPositionForSeat(Chair, i));
        chair->setScale(getScaleForChair(i) * getScaleForElement(Chair));
        
        auto &avatar = _avatars[i];
        avatar->setPosition(getPositionForSeat(Avatar, i));
        
        auto avatarScale = getScaleForElement(Avatar);
        avatar->setScale(avatarScale);
        
        if (i <= 2) {
            avatar->setScaleX(-avatarScale);    // flip horizontal
        } else {
            avatar->setScaleX(avatarScale);     // set scale for avatars that were formally in 3rd person locations
        }
    
        auto &betStack = _bets[i];
        betStack->setPosition(getPositionForSeat(BetStack, i));
        betStack->setPotLocation(_pot->getPosition());
        
        auto &winView = _winViews[i];
        winView->setPosition(getPositionForSeat(Nameplate, i));
        
        auto &chatView = _chatBubbles[i];
        chatView->setPosition(getPositionForSeat(ChatBubble, i));
        
        auto &sitButton = _sitButtons[i];
        sitButton->setPosition(getPositionForSeat(Nameplate, i));
        
        auto &holeCard = _holeCards[i];
        holeCard->setPosition(getPositionForSeat(HoleCards, i));
        holeCard->setScale(getScaleForElement(HoleCards));
        // TODO match hole card state with players in hand
        holeCard->clearCards();

		auto &giftView = _gifts[i];
		giftView->setPosition(getPositionForSeat(Gift, i));
    }
    
    updateNameplates();
    updateSitButtons();
    
    loadTheme(_model.getThemeId());
    
    // Load avatars
    std::vector<std::string> avatarIds;
    for (auto &seat: _model.getSeats()) {
        if (seat.isSitting()) {
            avatarIds.push_back(seat.avatarId);
        }
    }
    
    loadAvatars(avatarIds);
    
    // Build view tree for first person
    
    _tableView->addChild(_activeHighlight); // Add it to retain/release (unused atm)
    
    addChild(_backgroundLayer);
    addChild(_tableView);
    
        for (auto i = 0; i < _model.getSeatCount(); i++) {
            _tableView->addChild(_chairs[i]);
        }
    
        for (auto i = 0; i < _model.getSeatCount(); i++) {
			_tableView->addChild(_avatars[i]);
        }
    
        _tableView->addChild(_tableLayer);
        _tableView->addChild(_communityCards);
        _tableView->addChild(_cardLayer);
        _tableView->addChild(_armsLayer);
        _tableView->addChild(_pot);
        _tableView->addChild(_dealerButton);
        _tableView->addChild(_bestHandDescription);
    
        for (int i = 0; i < _model.getSeatCount(); i++) {
			_tableView->addChild(_bets[i]);
			_tableView->addChild(_gifts[i]);
            if (!(isFirstPerson() && i == 0)) {
                // if first person, skip nameplate at nameplateIndex
                _tableView->addChild(_nameplates[i]);
            }
            _tableView->addChild(_winViews[i]);
            _tableView->addChild(_chatBubbles[i]);
            _tableView->addChild(_sitButtons[i]);
            _cardLayer->addChild(_holeCards[i]);
        }
    
    _tableView->addChild(_tableLabel);
    _tableView->addChild(_actionButtons);
    _tableView->addChild(_sitBackIn);
    _tableView->addChild(_showButton);
    _tableView->addChild(_nameplates[0]);
}

void TableViewController::buildView() {
	// Create all the on-screen elements, and setup everything that will not change between first/3rd person
	// Do not add any children! Simplifies thinking when writing 3rd/1st first view setup functions

	auto viewScale = containScale(designSize, getContentSize());
	auto viewSize = containSize(designSize, getContentSize());
	auto size = getContentSize();

	_backgroundLayer = Node::create();
	_backgroundLayer->retain();
	_backgroundLayer->setPosition(size.width / 2, size.height / 2);

	_tableView = Node::create();
	_tableView->retain();
	_tableView->setContentSize(viewSize);
	_tableView->setScale(viewScale);

	for (auto i = 0; i < _model.getSeatCount(); i++) {
		auto chairNode = Node::create();
		chairNode->retain();
		chairNode->setAnchorPoint(Vec2(.5, .5));
		_chairs.push_back(chairNode);

		auto avatar = AvatarView::create();
		avatar->retain();
		avatar->setAnchorPoint(Vec2(.5, .5));
		_avatars.push_back(avatar);
	}

	// Table
	_tableLayer = Node::create();
	_tableLayer->retain();

	_armsLayer = Node::create();
	_armsLayer->retain();

	_activeHighlight = Sprite::create("highlight.png");
	_activeHighlight->retain();
	_activeHighlight->setVisible(false);

	_communityCards = BoardCardsView::create();
	_communityCards->retain();
	_communityCards->setupInitialState(_model.getBoardCards());

	_cardLayer = Node::create();
	_cardLayer->retain();

	// Pot View
	_pot = PotView::create();
	_pot->retain();

	_dealerButton = Sprite::create("dealerButton.png");
	_dealerButton->retain();
	_dealerButton->setPosition(getPositionForSeat(DealerButton, _model.getDealerButtonSeatIndex()));
	_dealerButton->setScale(.5);

	for (auto i = 0; i < _model.getSeatCount(); i++) {
		auto betStack = BetStackView::create();
		betStack->retain();
		betStack->setPosition(getPositionForSeat(BetStack, i));
		_bets.push_back(betStack);

		auto seatGift = TableGiftView::create();
		seatGift->retain();
		_gifts.push_back(seatGift);
	}

	// Build the swap plates
	_firstPersonNameplate = FirstPersonNameplateView::create();
	_firstPersonNameplate->retain();
	_firstPersonNameplate->setPosition(PT(Vec2(125, 8)));
	_firstPersonNameplate->setChatClicked([=]() {
		TableChatViewController::create(_game, _model.getId());
	});

	_thirdPersonNameplate = ThirdPersonNameplateView::create(_game, this);
	_thirdPersonNameplate->retain();

	for (auto i = 0; i < _model.getSeatCount(); i++) {
		auto nameplate = i > 0 ? ThirdPersonNameplateView::create(_game, this) : _thirdPersonNameplate;
		nameplate->retain();
		_nameplates.push_back(nameplate);
        
        auto holeCard = HoleCardsView::create();
        holeCard->retain();
        _holeCards.push_back(holeCard);

		auto winView = PlayerWinView::create();
		winView->retain();
		winView->setPosition(getPositionForSeat(Nameplate, i));
		winView->setVisible(false);
		_winViews.push_back(winView);

		auto chatView = ChatBubbleView::create();
		chatView->retain();
		chatView->setPosition(getPositionForSeat(Nameplate, i));
		_chatBubbles.push_back(chatView);

		auto sitButton = ui::Button::create("button-sit.png", "");
		sitButton->retain();
		sitButton->setPosition(getPositionForSeat(Nameplate, i));
		sitButton->setZoomScale(-0.05f);
		sitButton->setVisible(_model.amISitting() && !_model.amISittingIn());
		sitButton->addClickEventListener([=](Ref*) {
			_game->getApi()->tableRequestSeat(_model.getId().c_str(), i, NullCallback);
		});
		_sitButtons.push_back(sitButton);
	}

	_tableLabel = Text::create(_model.getName() + std::string(" - ") + _model.getActiveHandId(), UniSansRegular, 14);
	_tableLabel->retain();
	_tableLabel->setPosition(Vec2(0, size.height));
	_tableLabel->setAlignment(TextHAlignment::LEFT);
	_tableLabel->setAnchorPoint(Point(0.0f, 1.0f));

	_bestHandDescription = Text::create("", UniSansBold, 42);
	_bestHandDescription->retain();

	_actionButtons = ActionButtonsView::create();
	_actionButtons->retain();
	_actionButtons->setActionCallback([=](ActionButtonsView::Action action, int64_t betAmount) {
		if (action == ActionButtonsView::ActionCall) {
			_api->handPostAction(_model.getActiveHandId().c_str(), "call", _choicesCallAmount, NullCallback);
		}
		else if (action == ActionButtonsView::ActionFold) {
			_api->handPostAction(_model.getActiveHandId().c_str(), "fold", 0, NullCallback);
		}
		else if (action == ActionButtonsView::ActionBet) {
			_api->handPostAction(_model.getActiveHandId().c_str(), "bet", betAmount, NullCallback);
		}
	});
	_actionButtons->setPosition(designSize.width, 0);

	_sitBackIn = Button::create("button-sitin.png");
	_sitBackIn->retain();
	_sitBackIn->setPosition(Vec2(designSize.width, 0));
	_sitBackIn->setAnchorPoint(Vec2(1, 0));
	_sitBackIn->setZoomScale(-0.05f);
	_sitBackIn->addClickEventListener([=](Ref*) {
		// This button is not visible if not sitting
		_game->getApi()->tableSitIn(_model.getId().c_str(), NullCallback);
		_sitBackIn->setVisible(false);
	});
	_sitBackIn->setVisible(_model.amISitting() && !_model.amISittingIn());
    
    _showButton = Button::create("button-show.png");
    _showButton->retain();
    _showButton->setPosition(Vec2(designSize.width, 0));
    _showButton->setAnchorPoint(Vec2(1, 0));
    _showButton->setZoomScale(-0.05f);
    _showButton->addClickEventListener([=](Ref*) {
        _game->getApi()->handShowCards(_model.getActiveHandId().c_str(), NullCallback);
        _showButton->setVisible(false);
    });
    _showButton->setVisible(false);

	if (!_model.amISitting()) {
		_viewMode = ThirdPerson;
	}

	// Customize loaded content for specific view style
	if (ViewMode::FirstPerson == _viewMode) {
		setupFirstPersonView();
	}
	else {
		setupThirdPersonView();
	}
}
