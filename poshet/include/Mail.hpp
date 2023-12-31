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
    AttachmentData _data;
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
    
    MailBuilder(); // no reference, for new mail
    MailBuilder(const Mail& mail, ReferenceType type);
};


class MailException : public Exception {
public:
    MailException(const std::string& message) : Exception(message) {}
};