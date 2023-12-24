#include "Mail.hpp"

// code snippet from Mimetic
void printMimeStructure(MimeEntity* pMe, int tabcount = 0)
{
	Header& h = pMe->header();
	for(int c = tabcount; c > 0; --c) {
		std::cout << "    ";
    }
	std::cout << h.contentType() << "\n";
    if (h.contentType().type() == "image")  {
        std::cout << pMe->body();
    }
	MimeEntityList& parts = pMe->body().parts();
	MimeEntityList::iterator mbit = parts.begin(), meit = parts.end();
	for(; mbit != meit; ++mbit) {
		printMimeStructure(*mbit, 1 + tabcount);
    }
}

Mail::Mail(const std::string& plainTextData) : _plainText(plainTextData) {
    _type = Type::FROM_PLAINTEXT;

    std::istringstream i(plainTextData);
    _mimeEntity = new MimeEntity(i);
}

std::string Mail::getHeaderField(const std::string& key) const {
    if (!_mimeEntity->hasField(key)) {
        throw MailException("Requested mail lacks requested header");
    }
    return _mimeEntity->header().field(key).value();
}

std::string Mail::plainText() const {
    return _mimeEntity->body();
}