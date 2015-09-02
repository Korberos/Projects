#pragma once

#include "Callback.h"
#include <boost/shared_ptr.hpp>

namespace cocos2d { class Sprite; }

class _CancelationToken {
public:
    _CancelationToken(const boost::signals2::connection& connection);

    void cancel();
    bool isCancelled() const;
    
private:
    boost::signals2::connection _connection;
};

typedef boost::shared_ptr<_CancelationToken> CancelationToken;

// Also caches
class Downloader {
public:
    typedef Signal<void(cocos2d::Sprite*, const std::string&, bool wasCached)> DownloadSignal;
    typedef Callback<void(cocos2d::Sprite*, const std::string&, bool wasCached)> DownloadCallback;

    static CancelationToken getSpriteFromUrl(const std::string& url, const DownloadCallback& callback);
};