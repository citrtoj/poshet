#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <utility>
#include <fstream>

#include <vmime/vmime.hpp>

#include "Utils.hpp"
#include "Exceptions.hpp"

struct AttachmentMetadata {
    std::string _filename, _contentType;
    unsigned long long _size;
};

typedef std::string AttachmentData;

struct Attachment {
    AttachmentMetadata _metadata;
    // temp???
    AttachmentData _data;
};

class Mail {
protected:
    std::string _plainText;

    vmime::shared_ptr<vmime::message> _message;
    vmime::messageParser* _messageParser;
    
    void parsePlainText();

public:
    bool _isMimeMessageInit;
    Mail(const std::string& plainTextData);
    Mail(const Mail& rhs);
    Mail(Mail&& rhs);
    ~Mail();

    const std::string& plainText() const;
    
    std::string getHeaderField(const std::string& key, bool unicode = false) const;

    std::string getHTMLPart() const;
    std::string getPlainTextPart() const;

    AttachmentMetadata attachmentMetadataAt(size_t index) const;
    std::vector<AttachmentMetadata> attachmentMetadata() const;

    AttachmentData attachmentDataAt(size_t index) const;
    //std::vector<AttachmentData> attachmentData() const;

    // Attachment attachmentAt(size_t index) const;
    // std::vector<Attachment> attachments() const;
};

class MailBuilder {
public:
    enum ReferenceType {
        NONE,
        REPLY,
        FORWARD
    };
    
    MailBuilder(); // no reference
    MailBuilder(const Mail& mail, ReferenceType type);
};


class MailException : public Exception {
public:
    MailException(const std::string& message) : Exception(message) {}
};