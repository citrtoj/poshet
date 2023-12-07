#include <wx/wx.h>
#include "LoginFrame.hpp"

class App: public wxApp {
protected:
    Session* session;
public:
    bool OnInit() {
        LoginFrame* frame = new LoginFrame("Login Example");
        frame->Show(true);
        return true;
    }
};