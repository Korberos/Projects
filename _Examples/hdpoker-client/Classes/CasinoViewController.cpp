#include "CasinoViewController.h"
#include "GameController.h"
#include "DashboardViewController.h"
#include "TableViewController.h"
#include "UI/UIButton.h"
#include "UI/UIPageView.h"
#include "SimpleAudioEngine.h"
#include "CasinoTableContainerView.h"
#include "TableManagerViewController.h"
#include <functional>
#include "Stage.h"
#include "BadgeView.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class PinchGestureRecognizer {
public:
    typedef std::function<void(float zoom)> PinchCallback;
    static EventListenerTouchAllAtOnce* create(const PinchCallback& callback) {
        auto listener = EventListenerTouchAllAtOnce::create();
        
//        auto isRecognizing = false;
//        const auto threshold = 2;
        listener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event *event) {
            if (touches.size() == 2) {
//                isRecognizing = true;
            }
        };
//
        listener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event *event) {
            if (touches.size() < 2) {
                return;
            }
            
            auto initialDistance = touches[0]->getStartLocation().getDistance(touches[1]->getStartLocation());
            auto currentDistance = touches[0]->getLocation().getDistance(touches[1]->getLocation());
            
            callback(initialDistance / currentDistance);
        };
//        
//        listener->onTouchesEnded = listener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event *event) {
//            isRecognizing = false;
//        };
        
        return listener;
    }
};

CasinoViewController* CasinoViewController::create(GameController *game) {
    auto controller = CasinoViewController::create();
    controller->_game = game;
    controller->_viewLayout = OneUp;
	controller->buildView();
    return controller;
}

CasinoViewController::~CasinoViewController() {
	_game->getModel()->removeListenersForTarget(this);
}

cocos2d::Size CasinoViewController::getSizeForViewMode() const {
    if (OneUp == _viewLayout) {
        return getContentSize();
    } else {
        auto quad = getContentSize();
        quad.width /= 2;
        quad.height /= 2;
        return quad;
    }
}

cocos2d::Vec2 CasinoViewController::getPositionForTableAt(int index) const {
    if (OneUp == _viewLayout) {
        return Vec2(getContentSize().width * index, 0);
    } else {
        auto quad = getSizeForViewMode();
        return Vec2((index % 2) * quad.width, static_cast<int>(index / 2) * quad.height);
    }
}

void CasinoViewController::setViewModeThirdPerson(bool isFirstPerson) {
    for (auto &table : _tables) {
        table.second->getTable()->setViewMode(isFirstPerson ? TableViewController::ThirdPerson : TableViewController::FirstPerson);
    }
}

void CasinoViewController::setViewLayout(CasinoViewController::ViewLayout mode) {
    if (_viewLayout != mode) {
        _viewLayout = mode;
    
        int i = 0;
        for (auto tEntry: _tables) {
            tEntry.second->setContentSize(getSizeForViewMode());
            tEntry.second->setPosition(getPositionForTableAt(i));
            i++;
        }
        
        _panGesture->setEnabled(OneUp == _viewLayout && _tables.size() > 1);
        _tablesLayer->setPositionX(0);  // Moving to 4 up, move back to center.  Moving to 1 up from 4, start at beginning.
    }
}

void CasinoViewController::setOnTableRemoved(const TableRemovedCallback &callback) {
    _tableRemovedCallback = callback;
}

void CasinoViewController::leaveTable(const char* identifier)
{
	_game->getApi()->tableUnsubscribe(identifier, NullCallback);

	// Find the iterator of the table we have the identifier for
	auto iter = _tables.end();
	for (iter = _tables.begin(); iter != _tables.end(); ++iter)
		if (strcmp(iter->second->getTable()->getModel()->getId().c_str(), identifier) == 0)
			break;
	if (iter == _tables.end()) return;

	//  Remove the table container view from both it's parent and the tables map
	CasinoTableContainerView* container = iter->second;

    _tables.erase(iter);
    
    //  Call the table remove callback
    if (_tableRemovedCallback) {
        _tableRemovedCallback(container->getTable());
    }
    
    container->removeFromParent();
	
	if (_tables.size() == 0) {
		// Go back to dash
		auto dash = DashboardViewController::create(_game);
		auto scene = Scene::create();
		scene->addChild(dash);
        Stage::replaceScene(scene);
	}
	else {
		if (OneUp == _viewLayout) {
			//for (int i = 0; i < 4; i++) {
				// Found the hole, move the tables up
				//                    if (_tables.find(i) == _tables.end()) {
				//                        
				//                        // Walk other tables up
				//                        for (int k = i + 1; k < 4; k++) {
				//                            auto it = _tables.find(k);
				//                            if (it != _tables.end()) {
				//                                _tables[i] = _tables[k];
				//                            } else {
				//                                // Delete the last entry
				//                                _tables.erase(_tables.find(k));
				//                                break;
				//                            }
				//                            
				//                            table.second->setPositionX(i * getContentSize().width);
				//                            i++;
				//                        }
				//                        break;
				//                    }
			//}

			// TODO Go to next table vs start of list
			setActiveTable(0);

			if (_tables.size() == 1) {
				_panGesture->setEnabled(false);
			}
		}
	}
}

void CasinoViewController::setActiveTable(int index) {
    if (OneUp == _viewLayout) {
        _tablesLayer->setPositionX(-index * getContentSize().width);
    }
}

void CasinoViewController::addTable(TableViewController* table) {
    // Find empty index
    int index;
    for (auto i = 0; i < 4; i++) {
        if (_tables.find(i) == _tables.end()) {
            index = i;
            break;
        }
    }

    auto tableContainer = CasinoTableContainerView::create(_game);
    tableContainer->addTable(table);
    tableContainer->setCloseCallback([=](CasinoTableContainerView* container) {
		leaveTable(container->getTable()->getModel()->getId().c_str());
    });
    
    _tables[index] = tableContainer;
    if (_tables.size() > 1 && OneUp == _viewLayout) {
        _panGesture->setEnabled(true);
    }
    
    setActiveTable(index);
    
    tableContainer->setContentSize(getSizeForViewMode());
    tableContainer->setPosition(getPositionForTableAt(index));
    _tablesLayer->addChild(tableContainer);
}

void CasinoViewController::removeAllTables() {
    _tablesLayer->removeAllChildren();
    _panGesture->setEnabled(false);
}

void CasinoViewController::onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
    
}

//void CasinoViewController::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
//}

void CasinoViewController::buildView() {
    _tablesLayer = Node::create();
    _tablesLayer->setContentSize(getContentSize());
    addChild(_tablesLayer);
    
    // Build the shell around the tables
    auto home = Button::create("button-home.png", "button-home.png");
    home->setPosition(PT(Vec2(9, 4)));
    home->setAnchorPoint(Vec2(0, 0));
    home->addClickEventListener([=] (Ref*) {
        // auto index = 0;
        
        auto dash = DashboardViewController::create(_game);
        
        // Disable view passing for now (definition needed)
        
//        dash->transitionFromView(_tables[index]->getTable());
//        _tables[index]->removeFromParent();
//        _tables.erase(index);
        
        auto scene = Scene::create();
        scene->addChild(dash);
        Stage::replaceScene(scene);
    });
    addChild(home);
    
	auto szHome = home->getContentSize();
	_badge = BadgeView::create(1);
	_badge->setPosition(Vec2(szHome.width - PT(8), szHome.height - PT(8)));
	_badge->setVisible(_game->getModel()->getTierDelta() ? true : false);
	home->addChild(_badge);

	_game->getModel()->addListener(this, [=](GameModelUpdate::Value change, GameModel *model) {
		if (change == GameModelUpdate::Tier) {
			_badge->setVisible(model->getTierDelta() ? true : false);
		}
	});

    // Stash a mode switch button here for now
    auto modeSwitch = CheckBox::create("button-multi.png", "button-single.png");
    modeSwitch->setZoomScale(-0.05f);
	modeSwitch->setAnchorPoint(Vec2::ZERO);
    modeSwitch->setPosition(Vec2(PT(9), getContentSize().height * .3f));
    modeSwitch->addClickEventListener([=] (Ref*) {
        setViewLayout(modeSwitch->isSelected() ? CasinoViewController::OneUp : CasinoViewController::FourUp);
    });
    addChild(modeSwitch);

	// 4up is for tablets only
	auto sizeInInches = Director::getInstance()->getWinSize().width / Device::getDPI();
	modeSwitch->setVisible((sizeInInches >= 7) ? true : false);  // 7" = tablet

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(PinchGestureRecognizer::create([] (float zoom) {
        CCLOG("Zoom %f", zoom);
    }), _tablesLayer);
    
    _panGesture = EventListenerTouchOneByOne::create();
    _panGesture->setEnabled(false);
    _panGesture->setSwallowTouches(false);
    _panGesture->onTouchBegan = [&](const Touch* touch, Event *event) {
        _tablesLayer->stopAllActions();
        return true;
    };
    _panGesture->onTouchMoved = [&](const Touch* touch, Event *event) {
        _tablesLayer->setPositionX(_tablesLayer->getPositionX() + touch->getDelta().x);
        
        const int viewWidth = _tablesLayer->getContentSize().width;
        const int halfView = viewWidth / 2;
        
        // Advance one?
        int index = (_tablesLayer->getPositionX() - halfView) / _tablesLayer->getContentSize().width;
        
        CCLOG("%d %d", (int)_tablesLayer->getPositionX(), index);
    };
    _panGesture->onTouchEnded = [&](const Touch* touch, Event *event) {
        const int viewWidth = _tablesLayer->getContentSize().width;
        const int halfView = viewWidth / 2;
        
        int index = (_tablesLayer->getPositionX() - halfView) / _tablesLayer->getContentSize().width;
        CCLOG("%d", index);
        _tablesLayer->runAction(EaseExponentialOut::create(MoveTo::create(0.3f, Vec2(index * viewWidth, 0))));
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_panGesture, _tablesLayer);

	_tableManageMenu = TableManagerViewController::create(_game, &_tables);
	addChild(_tableManageMenu);
	_tableManageMenu->setCloseCallback([=](CasinoTableContainerView* container) {
		leaveTable(container->getTable()->getModel()->getId().c_str());
	});
	_tableManageMenu->setClickCallback([=](CasinoTableContainerView* container) {
		// Find the index of the specified table
		int index = -1;
		for (auto i = 0; i < 4; i++) {
			if (_tables.find(i) == _tables.end()) continue;
			if (_tables.find(i)->second == container) {
				index = i;
				break;
			}
		}
		if (index == -1) return;

		setActiveTable(index);
	});
}