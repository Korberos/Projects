#pragma once

#include "TableDataSource.h"

template <typename TData, typename TCell>
class VectorDataSource : public TableDataSource<TData> {
public:
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
    
protected:
    using TableDataSource<TData>::_items;
};