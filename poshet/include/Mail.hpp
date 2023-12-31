#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <iostream>
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
    
    std::string getHeaderField(const std::string& key, bool unicode = false) const;

    std::string getHTMLText() const;
    std::vector<InlineAttachmentData> getInlineHTMLAttachments() const;
    std::string getPlainTextPart() const;

    AttachmentMetadata attachmentMetadataAt(size_t index) const;
    std::vector<AttachmentMetadata> attachmentMetadata() const;

    std::string attachmentDataAt(size_t index) const;
    //std::vector<AttachmentData> attachmentData() const;

    // Attachment attachmentAt(size_t index) const;
    // std::vector<Attachment> attachments() const;
};


// mail builders


class MailBodyBuilder {
public:
    const std::string& to() const {
        return _to;
    }
    void setTo(const std::string& to) {
        _to = to;
    }

    const std::string& from() const {
        return _from;
    }
    void setFrom(const std::string& from) {
        _from = from;
    }

    const std::string& plainText() const {
        return _plainText;
    }
    void setPlainText(const std::string plainText) {
        _plainText = plainText;
    }

    void addAttachment(const std::string& attachmentName, const std::string& fileData, const std::string& contentType = "application/octet-stream") {
        _attachments.push_back({{
                attachmentName, contentType, fileData.length() * sizeof(char)
            }, fileData
        });
    }
    void removeAttachment(size_t idx) {
        // find, pop
    }
    // const std::vector<AttachmentMetadata>& attachments() const;
    
    // virtual std::string generateMIMEMessage();

protected:
    std::string _to;
    std::string _from;

    std::string _plainText;
    std::vector<Attachment> _attachments;
};

class ReplyMailBuilder : public MailBodyBuilder {
public:
    ReplyMailBuilder(const Mail& mail) {
        // get plainText from it, get attachments, add them to builder
    }

    void setReferenceId(const std::string& referenceId) {
        _referenceId = referenceId;
    } 
    const std::string& referenceId() const {
        return _referenceId;
    }

protected:
    std::string _referenceId;
    
};

class ForwardMailBuilder : public MailBodyBuilder {
public:
    ForwardMailBuilder(const Mail& mail); // init plaintext, attachments
protected:
    std::string _referenceId;
};

class MailBuilderObserver {
    virtual void handleMailBuilderDataUpdate() = 0;
};


class MailException : public Exception {
public:
    MailException(const std::string& message) : Exception(message) {}
};