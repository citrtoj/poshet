#pragma once

class FrameSubscriber {
public:
    virtual void onCloseAnyWindow() = 0;
};

class LoginFrameSubscriber : virtual public FrameSubscriber {
public:
    virtual void onLoginSubmit() = 0;
};

class DashboardFrameSubscriber : virtual public FrameSubscriber {
public:
    virtual void onNewMail() = 0;
    virtual void onReplyMail(/* un fel de a identifica mail-ul la care ma refer... vedem mai incolo*/) = 0;
    virtual void onRefreshMailList() = 0;
};