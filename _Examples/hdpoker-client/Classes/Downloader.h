#pragma once

#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

namespace cocos2d { class Sprite; }

class _CancellationToken {
public:
    _CancellationToken() : _cancel(false) {}
    void cancel() { _cancel = true; }
    bool isRunning() {
        return !_cancel;
    }
private:
    bool _cancel;
};
//typedef boost::shared_ptr<_CancellationToken> CancellationToken;
typedef boost::signals2::connection CancellationToken;

// Also caches
class Downloader {
public:
    typedef boost::signals2::signal<void(cocos2d::Sprite*)> DownloadCallback;
//    typedef std::function<void(cocos2d::Sprite*)> DownloadCallback;
    
    static CancellationToken getSpriteFromUrl(const std::string& url, const DownloadCallback::slot_type& callback);
};