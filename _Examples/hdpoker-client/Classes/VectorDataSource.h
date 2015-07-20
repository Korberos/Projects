#pragma once

#include "gui/CCScrollView/CCTableView.h"
#include <vector>

template <typename TData, typename TCell>
class VectorDataSource : public cocos2d::extension::TableViewDataSource {
public:
    void setCellSize(const cocos2d::Size& size) {
        _size = size;
    }
    
    void push_back(const TData& item) {
        _items.push_back(item);
    }
    
    void remove(int index) {
        _items.erase(_items.begin() + index);
    }
    
    const int size() {
        return _items.size();
    }
    
    const TData& operator[] (int index) const {
        return _items[index];
    }
    
    TData& operator[] (int index) {
        return _items[index];
    }
    
    virtual cocos2d::Size cellSizeForTable(cocos2d::extension::TableView *table) override {
        return _size;
    }
    
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx) override {
        TCell *cell = static_cast<TCell*>(table->dequeueCell());
        if (!cell) {
            cell = TCell::create(table);
        }
        
        cell->update(_items[idx]);
        return cell;
    }
    
    virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override {
        return _items.size();
    }
    
    void sort(const std::function<int(const TData&, const TData&)>& comparer) {
        std::sort(_items.begin(), _items.end(), comparer);
    }
    
    void clear() {
        _items.clear();
    }
    
private:
    cocos2d::Size _size;
    std::vector<TData> _items;
};