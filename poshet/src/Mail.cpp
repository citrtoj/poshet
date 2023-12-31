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
    //std::cout << "[Mail] Copy ctor\n";
    _plainText = rhs._plainText;
    parsePlainText();
}

Mail::Mail(Mail&& rhs) {
    //std::cout << "[Mail] Move ctor\n";
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

std::string Mail::getHeaderField(const std::string& key, bool unicode) const {
    if (!_isMimeMessageInit) {
        throw MailException("Mail not parsed yet");
    }
    if (_message->getHeader()->hasField(key)) {
        // possibly undecoded
        auto data = _message->getHeader()->getField(key)->getValue()->generate();
        if (!unicode) {
            return vmime::text::decodeAndUnfold(data)->getConvertedText(vmime::charset("ascii"));
        }
        else {
            return vmime::text::decodeAndUnfold(data)->getConvertedText(vmime::charset("utf-8"));
        }
    }
    else {
        return ""; //temporary
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
            // std::string htmlText;
            // vmime::utility::outputStreamStringAdapter x(htmlText);
            // hp.getText()->extract(x);
            // for (size_t j = 0 ; j < hp.getObjectCount() ; ++j) {
                
            //     const vmime::htmlTextPart::embeddedObject& obj = *hp.getObjectAt(j);
                
            //     std::cout << obj.getId() << " " << obj.getReferenceId() << " " <<  obj.getType().generate() << "\n";

            //     // Identifier (content-id or content-location) is in "obj.getId()"
            //     // Object data is in "obj.getData()"
            // }
            //return htmlText;
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
    // for (size_t i = 0 ; i < _messageParser->getTextPartCount() ; ++i) {
    //     const vmime::textPart& part = *_messageParser->getTextPartAt(i);
    //     if (part.getType().getSubType() == vmime::mediaTypes::TEXT_HTML) {
    //         const vmime::htmlTextPart& hp = dynamic_cast<const vmime::htmlTextPart&>(part);
    //         std::string htmlText;
    //         vmime::utility::outputStreamStringAdapter x(htmlText);
    //         hp.getText()->extract(x);
    //         for (size_t j = 0 ; j < hp.getObjectCount() ; ++j) {
                
    //             const vmime::htmlTextPart::embeddedObject& obj = *hp.getObjectAt(j);
                
    //             std::cout << obj.getId() << " " << obj.getReferenceId() << " " <<  obj.getType().generate() << "\n";

    //             // Identifier (content-id or content-location) is in "obj.getId()"
    //             // Object data is in "obj.getData()"
    //         }
    //         return htmlText;
    //     }
    // }
    // throw MailException("Mail does not have HTML parts");
}

std::vector<InlineAttachmentData> Mail::getInlineHTMLAttachments() const {
    std::vector<InlineAttachmentData> result;
    const vmime::htmlTextPart& hp = getHTMLPart();
    for (size_t j = 0 ; j < hp.getObjectCount() ; ++j) {
        const vmime::htmlTextPart::embeddedObject& obj = *hp.getObjectAt(j);
        //std::cout << obj.getId() << " " << obj.getReferenceId() << " " <<  obj.getType().generate() << "\n";
        std::string data;
        vmime::utility::outputStreamStringAdapter x(data);
        obj.getData()->extract(x);
        result.push_back({
            obj.getId(),
            obj.getReferenceId(),
            data
        });
        
        // Identifier (content-id or content-location) is in "obj.getId()"
        // Object data is in "obj.getData()"
    }
    return result;
}

std::string Mail::getPlainTextPart() const {
    for (size_t i = 0 ; i < _messageParser->getTextPartCount() ; ++i) {
        const vmime::textPart& part = *_messageParser->getTextPartAt(i);
        if (part.getType().getSubType() == vmime::mediaTypes::TEXT_PLAIN) {
            const vmime::textPart& tp = dynamic_cast<const vmime::textPart&>(part);
            std::string htmlText;
            vmime::utility::outputStreamStringAdapter x(htmlText);
            tp.getText()->extract(x);
            return htmlText;
        }
    }
    throw MailException("Mail does not have plaintext parts");
}

AttachmentMetadata Mail::attachmentMetadataAt(size_t index) const {
    // todo: index sanity check
    const vmime::attachment& attachment = *_messageParser->getAttachmentAt(index);
    return {
        attachment.getName().generate(),
        attachment.getType().generate(),
        attachment.getData()->getLength()
    };
}

std::vector<AttachmentMetadata> Mail::attachmentMetadata() const {
    std::vector<AttachmentMetadata> data;
    for (size_t i = 0; i < _messageParser -> getAttachmentCount(); ++i) {
        data.push_back(attachmentMetadataAt(i));
    }
    return data;
}

AttachmentData Mail::attachmentDataAt(size_t index) const {
    // todo: index sanity check
    const vmime::attachment& attachment = *_messageParser->getAttachmentAt(index);
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