#include "Mail.hpp"


Mail::Mail(const std::string& plainTextData, const std::string& mailId, const std::string& tag) : _plainText(plainTextData), _mailId(mailId), _tag(tag) {
    parsePlainText();
}

void Mail::parsePlainText() {
    _message = std::make_shared<vmime::message>();
    _message->parse(_plainText);
    _messageParser = new vmime::messageParser(_message);
    _isMimeMessageInit = true;
}

Mail::Mail(const Mail& rhs) {
    _plainText = rhs._plainText;
    parsePlainText();
}

Mail::Mail(Mail&& rhs) {
    _plainText = std::move(rhs._plainText);
    _mailId = std::move(rhs._mailId);
    _tag = std::move(rhs._tag);

    _message = std::move(rhs._message);

    _isMimeMessageInit = rhs._isMimeMessageInit;
    rhs._isMimeMessageInit = false;

    parsePlainText();
}

Mail::~Mail() {
    delete _messageParser;
}

vmime::shared_ptr<vmime::headerFieldValue> Mail::getHeaderValue(const std::string& key) const {
    return _message->getHeader()->getField(key)->getValue();
}

std::string Mail::getHeaderField(const std::string& key, bool unicode) const {
    if (!_isMimeMessageInit) {
        throw MailException("Mail not parsed yet");
    }
    if (_message->getHeader()->hasField(key)) {
        // possibly undecoded
        auto data = getHeaderValue(key)->generate();
        if (!unicode) {
            return vmime::text::decodeAndUnfold(data)->getConvertedText(vmime::charset("us-ascii"));
        }
        else {
            return vmime::text::decodeAndUnfold(data)->getConvertedText(vmime::charset("utf-8"));
        }
    }
    else {
        return "";
    }
}

const std::string& Mail::plainText() const {
    return _plainText;
}

const vmime::htmlTextPart& Mail::getHTMLPart() const {
    for (size_t i = 0 ; i < _messageParser->getTextPartCount() ; ++i) {
        const vmime::textPart& part = *_messageParser->getTextPartAt(i);
        if (part.getType().getSubType() == vmime::mediaTypes::TEXT_HTML) {
            const vmime::htmlTextPart& hp = dynamic_cast<const vmime::htmlTextPart&>(part);
            return hp;
        }
    }
    throw MailException("Mail does not have HTML parts");
}

std::string Mail::getHTMLText() const {
    const vmime::htmlTextPart& hp = getHTMLPart();
    std::string htmlText;
    vmime::utility::outputStreamStringAdapter x(htmlText);
    hp.getText()->extract(x);
    return htmlText;
}

std::vector<InlineAttachmentData> Mail::getInlineHTMLAttachments() const {
    std::vector<InlineAttachmentData> result;
    const vmime::htmlTextPart& hp = getHTMLPart();
    for (size_t j = 0 ; j < hp.getObjectCount() ; ++j) {
        const vmime::htmlTextPart::embeddedObject& obj = *hp.getObjectAt(j);
        std::string data;
        vmime::utility::outputStreamStringAdapter x(data);
        obj.getData()->extract(x);
        result.push_back({
            obj.getId(),
            obj.getReferenceId(),
            data
        });
    }
    return result;
}

std::string Mail::getPlainTextPartText() const {
    for (size_t i = 0 ; i < _messageParser->getTextPartCount() ; ++i) {
        const vmime::textPart& part = *_messageParser->getTextPartAt(i);
        if (part.getType().getSubType() == vmime::mediaTypes::TEXT_PLAIN) {
            const vmime::textPart& tp = dynamic_cast<const vmime::textPart&>(part);
            std::string plaintext;
            vmime::utility::outputStreamStringAdapter x(plaintext);
            tp.getText()->extract(x);
            return plaintext;
        }
    }
    throw MailException("Mail does not have plaintext parts");
}

AttachmentMetadata Mail::attachmentMetadataAt(size_t idx) const {
    if (idx >= _messageParser->getAttachmentCount()) {
        throw MailException("Invalid attachment idx: " + std::to_string(idx));
    }
    const vmime::attachment& attachment = *_messageParser->getAttachmentAt(idx);
    return {
        attachment.getName().generate(),
        attachment.getType().generate(),
        attachment.getData()->getLength() * 3.0 / 4
    };
}

std::vector<AttachmentMetadata> Mail::attachmentMetadata() const {
    std::vector<AttachmentMetadata> data;
    for (size_t i = 0; i < _messageParser -> getAttachmentCount(); ++i) {
        data.push_back(attachmentMetadataAt(i));
    }
    return data;
}

std::string Mail::attachmentDataAt(size_t idx) const {
    if (idx >= _messageParser->getAttachmentCount()) {
        throw MailException("Invalid attachment idx: " + std::to_string(idx));
    }
    const vmime::attachment& attachment = *_messageParser->getAttachmentAt(idx);
    std::string data;
    vmime::utility::outputStreamStringAdapter x(data);
    attachment.getData()->extract(x);
    return data;
}

const std::string& Mail::tag() const {
    return _tag;
}

const std::string& Mail::mailId() const {
    return _mailId;
}

std::vector<std::string> Mail::displayableFrom() const {
    try {
        // get from in mime format, return only mail if only mail, return both if both
        auto vmimeVal = getHeaderValue("From");
        auto vmimeMailbox = vmime::dynamic_pointer_cast<vmime::mailbox>(vmimeVal);
        auto name = vmimeMailbox->getName().getConvertedText(_defaultCharset);
        
        std::vector<std::string> result = {};
        if (!name.empty()) {
            result.push_back(name);
        }
        auto email = vmimeMailbox->getEmail().toString();
        if (!email.empty()) {
            result.push_back(email);
        }

        return result;
    }
    catch(...) {
        return {};
    }
}

// --- MailBuilder ---


void MailBuilder::addMIMEAttachment(vmime::messageBuilder& builder, const Attachment& attachment) {
    vmime::shared_ptr<vmime::stringContentHandler> x = vmime::make_shared<vmime::stringContentHandler>(attachment._data);
    vmime::shared_ptr<vmime::contentHandler> y = vmime::dynamic_pointer_cast<vmime::stringContentHandler>(x);
    vmime::shared_ptr <vmime::fileAttachment> a = vmime::make_shared <vmime::fileAttachment>(y, vmime::word(attachment._filename), vmime::mediaType("application/octet-stream"));
    builder.appendAttachment(a);
}



void MailBuilder::addAttachment(const std::string& attachmentName, const std::string& fileData, const std::string& contentType) {
    _attachments.push_back({{
            attachmentName, contentType, static_cast<double>(fileData.length())
        }, fileData
    });
}

void MailBuilder::removeAttachment(size_t idx) {
    if (idx >= _attachments.size()) {
        throw MailException("Invalid MailBuilder attachment idx: " + std::to_string(idx));
    }
    _attachments.erase(_attachments.begin() + idx);
}

std::vector<AttachmentMetadata> MailBuilder::attachments() const {
    std::vector<AttachmentMetadata> result;
    for (const auto& att : _attachments) {
        result.push_back(static_cast<AttachmentMetadata>(att));
    }
    return result;
}

std::string MailBuilder::generateMIMEMessage() {
    vmime::messageBuilder builder;
    for (auto& att : _attachments) {
        addMIMEAttachment(builder, att);
    }
    vmime::addressList list;
    list.appendAddress(vmime::make_shared<vmime::mailbox>(_to));
    builder.setRecipients(list);
    builder.setExpeditor(*_from);
    builder.setSubject(vmime::text(_subject, _defaultCharset));
    builder.getTextPart()->setText(
        vmime::make_shared <vmime::stringContentHandler>(
            _plainText
        )
    );
    builder.getTextPart()->setCharset(_defaultCharset);
    vmime::shared_ptr<vmime::message> msg = builder.construct();
    std::string text;
    vmime::utility::outputStreamStringAdapter x(text);
    msg->generate(x);
    return text;
}

std::string MailBuilder::generateStarterBody() {
    return ""; // no starter here :)
}


// --- ReplyMailBuilder ---

ReplyMailBuilder::ReplyMailBuilder(const Mail& mail, const std::string& fromEmailAddress, const std::string& name) : MailBuilder(fromEmailAddress, name) {
    try {
        _referenceText = mail.getPlainTextPartText();
        
        // todo: get attachments...?

        _referenceId = mail.getHeaderValue("Message-Id");
        _referenceReplyTo = mail.getHeaderValue("In-Reply-To");
        _referenceReferences = mail.getHeaderValue("References");
        _referenceSubject = mail.getHeaderField("Subject");
        _referenceDate = mail.getHeaderField("Date");

        _referenceFrom = mail.getHeaderField("From");
        _referenceTo = mail.getHeaderField("To");

        auto vmimeTo = vmime::dynamic_pointer_cast<vmime::mailbox>(mail.getHeaderValue("From"));
        _to = vmimeTo->getEmail().toString();

        _subject = "Re: " + _referenceSubject;

        auto attachments = mail.attachmentMetadata();
        for (int i = 0; i < attachments.size(); ++i) {
            auto data = mail.attachmentDataAt(i);
            addAttachment(attachments[i]._filename, data, attachments[i]._contentType);
        }
        try {
            auto inlineAtts = mail.getInlineHTMLAttachments();
            for (const auto& att : inlineAtts) {
                addAttachment("[INLINE] " + att._id, att._data);
            }
        }
        catch(Exception& e) {
            // no point handling. it means there's no attachments
        }
    }
    catch(MailException& e) {
        throw MailException(std::string("Could not construct ReplyMailBuilder (") + e.what() + ")");
    }
}

std::string ReplyMailBuilder::generateStarterBody() {
    std::string starter =
        "\n\n\n"
        "On " + _referenceDate + ", " + _to + " said: \n\n";
    std::istringstream stream(_referenceText);
    std::string line = "";
    while (std::getline(stream, line)) {
        starter += ">" + line + "\n"; // "\n" is the default delimiter for getline
    }
    return starter;
}


std::string ReplyMailBuilder::generateMIMEMessage() {
    vmime::messageBuilder builder;
    for (auto& att : _attachments) {
        addMIMEAttachment(builder, att);
    }
    vmime::addressList list;
    list.appendAddress(vmime::make_shared<vmime::mailbox>(_to));
    builder.setRecipients(list);
    builder.setExpeditor(*_from);
    builder.setSubject(vmime::text(_subject, _defaultCharset));
    builder.getTextPart()->setText(
        vmime::make_shared <vmime::stringContentHandler>(
            _plainText
        )
    );
    builder.getTextPart()->setCharset(_defaultCharset);
    vmime::shared_ptr<vmime::message> msg = builder.construct();

    auto replyTo = vmime::dynamic_pointer_cast<vmime::messageIdSequence>(_referenceReplyTo);    
    auto references = vmime::dynamic_pointer_cast<vmime::messageIdSequence>(_referenceReferences);

    auto msgId = vmime::dynamic_pointer_cast<vmime::messageId>(_referenceId);

    replyTo->appendMessageId(msgId);
    references->appendMessageId(msgId);

    msg->getHeader()->InReplyTo()->setValue(replyTo);
    msg->getHeader()->References()->setValue(references);

    std::string text;
    vmime::utility::outputStreamStringAdapter x(text);
    msg->generate(x);
    return text;
}

ForwardMailBuilder::ForwardMailBuilder(const Mail& mail, const std::string& fromEmailAddress, const std::string& name)  : ReplyMailBuilder(mail, fromEmailAddress, name) {
    try {
        _subject = "Fwd: " + _referenceSubject;
        _to = "";
    }
    catch(MailException& e) {
        throw MailException(std::string("Could not construct ForwardMailBuilder (") + e.what() + ")");
    }
}

std::string ForwardMailBuilder::generateStarterBody() {
    std::string starter =
        "\n\n"
        "--- FORWARDED MESSAGE ---\n"
        "Subject: " + _referenceSubject + "\n"
        "Date: " + _referenceDate + "\n"
        "From: " + _referenceFrom + "\n"
        "To: " + _referenceTo + "\n"
        "\n\n" 
        ;
    starter += _referenceText;
    return starter;
}
