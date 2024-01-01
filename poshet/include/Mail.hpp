#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <memory>

#include <vmime/vmime.hpp>

#include "Utils.hpp"
#include "Exceptions.hpp"

struct AttachmentMetadata {
    std::string _filename = "", _contentType = "";
    unsigned long long _size = 0;
};

struct Attachment : public AttachmentMetadata {
    std::string _data = "";
};

struct InlineAttachmentData {
    std::string _id, _referenceId, _data;
};

class Mail {
protected:
    std::string _plainText;
    std::string _mailId;
    std::string _tag;
    vmime::shared_ptr<vmime::message> _message;
    vmime::messageParser* _messageParser;
    
    void parsePlainText();

    const vmime::htmlTextPart& getHTMLPart() const;

public:
    bool _isMimeMessageInit;
    Mail(const std::string& plainTextData, const std::string& mailId, const std::string& tag = "");
    Mail(const Mail& rhs);
    Mail(Mail&& rhs);
    ~Mail();

    const std::string& plainText() const;
    const std::string& mailId() const;
    const std::string& tag() const;

    // only use this if you're not happy with the pre-processing done by getHeaderField and if you know how vmime works
    vmime::shared_ptr<vmime::headerFieldValue> getHeaderValue(const std::string& key) const; 

    std::string getHeaderField(const std::string& key, bool unicode = true) const;

    std::string getHTMLText() const;
    std::vector<InlineAttachmentData> getInlineHTMLAttachments() const;
    std::string getPlainTextPart() const;

    AttachmentMetadata attachmentMetadataAt(size_t idx) const;
    std::vector<AttachmentMetadata> attachmentMetadata() const;

    std::string attachmentDataAt(size_t idx) const;
    //std::vector<AttachmentData> attachmentData() const;

    // Attachment attachmentAt(size_t index) const;
    // std::vector<Attachment> attachments() const;
};



// --- mail body builders ---



class MailBodyBuilder {
protected:
    virtual void setFrom(const std::string& emailAddress, const std::string& name = "") {
        _from = vmime::make_shared<vmime::mailbox>(vmime::text(name), vmime::emailAddress(emailAddress));
    }
public:
    // implicit constructor
    MailBodyBuilder(const std::string& fromEmailAddress, const std::string& name = "") {
        setFrom(fromEmailAddress, name);
        std::cout << "[MailBodyBuilder] ctor\n";
    }

    ~MailBodyBuilder() {
        std::cout << "[MailBodyBuilder] dtor\n";
    }

    virtual const std::string& to() const {
        return _to;
    }
    virtual void setTo(const std::string& to) {
        _to = to;
    }

    virtual const std::string& subject() const {
        return _subject;
    }
    virtual void setSubject(const std::string& subject) {
        _subject = subject;
    }

    virtual const std::string& plainText() const {
        return _plainText;
    }
    virtual void setPlainText(const std::string plainText) {
        _plainText = plainText;
    }

    // attachment utils
    void addAttachment(const std::string& attachmentName, const std::string& fileData, const std::string& contentType = "application/octet-stream");
    void removeAttachment(size_t idx);
    std::vector<AttachmentMetadata> attachments() const;
    
    // virtuals
    virtual std::string generateStarterBody(); // generates starting point for user input... basically what'll be put in the html
    // virtual std::string generateMIMEMessage(); 

protected:
    std::string _to;

    std::string _fromRaw;
    vmime::shared_ptr<vmime::mailbox> _from;
    std::string _subject;
    std::string _plainText;

    std::vector<Attachment> _attachments;
};

class ReplyMailBodyBuilder : public MailBodyBuilder {
public:
    ReplyMailBodyBuilder(const Mail& mail, const std::string& fromEmailAddress, const std::string& name = "");

    // virtual std::string generateMIMEMessage() override; 
    virtual std::string generateStarterBody() override;
protected:
    std::string _referenceText;
    vmime::shared_ptr<vmime::headerFieldValue> _referenceId = nullptr;
    std::string _referenceSubject;
    std::string _referenceDate;
};

// class ForwardMailBodyBuilder : public MailBodyBuilder {
// public:
//     ForwardMailBodyBuilder(const Mail& mail); // init plaintext, attachments
// protected:
//     std::string _referenceId;
// };

class MailBodyBuilderObserver {
    virtual void handleMailBuilderDataUpdate() = 0;
};


class MailException : public Exception {
public:
    MailException(const std::string& message) : Exception(message) {}
};