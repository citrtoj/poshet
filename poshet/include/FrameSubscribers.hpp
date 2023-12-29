#pragma once

class FrameSubscriber {
public:
    virtual void onCloseApp() = 0;
};

class LoginFrameSubscriber : virtual public FrameSubscriber {
public:
    virtual void onLoginSubmit() = 0;
};

class DashboardFrameSubscriber : virtual public FrameSubscriber {
public:
    virtual void onSelectMail() = 0;
    virtual void onNewMail() = 0;
    virtual void onReplyMail() = 0;
    virtual void onForwardMail() = 0;
    virtual void onDeleteMail() = 0;
    virtual void onRefreshMailList() = 0;
    virtual void onAttachmentClick(int index) = 0;
};

class MailCreatorFrameSubscriber : virtual public FrameSubscriber {
public:
    virtual void onMailCreatorSend() = 0;
    virtual void onMailCreatorClose() = 0;
};


