#include "GUIComponents.hpp"

void AttachmentButton::OnAttachmentButtonClick(wxCommandEvent& event) {
    event.SetInt(_index);
    event.Skip();
}