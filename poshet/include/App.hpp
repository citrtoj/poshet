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
    void OnUnhandledException(){
        try {
            throw;
        }
        catch (Exception& e) {
            std::cout << "Exception: " << e.what() << "\nQuitting...\n";
            exit(EXIT_FAILURE);
        }
    }
};