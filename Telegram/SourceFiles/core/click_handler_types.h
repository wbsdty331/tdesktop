/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2016 John Preston, https://desktop.telegram.org
*/
#pragma once

#include "core/click_handler.h"

class TextClickHandler : public ClickHandler {
public:

	TextClickHandler(bool fullDisplayed = true) : _fullDisplayed(fullDisplayed) {
	}

	void copyToClipboard() const override {
		QString u = url();
		if (!u.isEmpty()) {
			QApplication::clipboard()->setText(u);
		}
	}

	QString tooltip() const override {
		return _fullDisplayed ? QString() : readable();
	}

	void setFullDisplayed(bool full) {
		_fullDisplayed = full;
	}

protected:
	virtual QString url() const = 0;
	virtual QString readable() const {
		return url();
	}

	bool _fullDisplayed;

};

class UrlClickHandler : public TextClickHandler {
public:
	UrlClickHandler(const QString &url, bool fullDisplayed = true) : TextClickHandler(fullDisplayed), _originalUrl(url) {
		if (isEmail()) {
			_readable = _originalUrl;
		} else {
			QUrl u(_originalUrl), good(u.isValid() ? u.toEncoded() : QString());
			_readable = good.isValid() ? good.toDisplayString() : _originalUrl;
		}
	}
	QString copyToClipboardContextItemText() const override;

	QString dragText() const override {
		return url();
	}

	QString getExpandedLinkText(ExpandLinksMode mode, const QStringRef &textPart) const override;
	TextWithEntities getExpandedLinkTextWithEntities(ExpandLinksMode mode, int entityOffset, const QStringRef &textPart) const override;

	static void doOpen(QString url);
	void onClick(Qt::MouseButton button) const override {
		if (button == Qt::LeftButton || button == Qt::MiddleButton) {
			doOpen(url());
		}
	}

protected:
	QString url() const override {
		if (isEmail()) {
			return _originalUrl;
		}

		QUrl u(_originalUrl), good(u.isValid() ? u.toEncoded() : QString());
		QString result(good.isValid() ? QString::fromUtf8(good.toEncoded()) : _originalUrl);

		if (!QRegularExpression(qsl("^[a-zA-Z]+:")).match(result).hasMatch()) { // no protocol
			return qsl("http://") + result;
		}
		return result;
	}
	QString readable() const override {
		return _readable;
	}

private:
	static bool isEmail(const QString &url) {
		int at = url.indexOf('@'), slash = url.indexOf('/');
		return ((at > 0) && (slash < 0 || slash > at));
	}
	bool isEmail() const {
		return isEmail(_originalUrl);
	}

	QString _originalUrl, _readable;

};
typedef QSharedPointer<TextClickHandler> TextClickHandlerPtr;

class HiddenUrlClickHandler : public UrlClickHandler {
public:
	HiddenUrlClickHandler(QString url) : UrlClickHandler(url, false) {
	}
	void onClick(Qt::MouseButton button) const override;

	QString getExpandedLinkText(ExpandLinksMode mode, const QStringRef &textPart) const override;
	TextWithEntities getExpandedLinkTextWithEntities(ExpandLinksMode mode, int entityOffset, const QStringRef &textPart) const override;

};

class MentionClickHandler : public TextClickHandler {
public:
	MentionClickHandler(const QString &tag) : _tag(tag) {
	}

	void onClick(Qt::MouseButton button) const override;

	QString dragText() const override {
		return _tag;
	}

	QString copyToClipboardContextItemText() const override;

	TextWithEntities getExpandedLinkTextWithEntities(ExpandLinksMode mode, int entityOffset, const QStringRef &textPart) const override;

protected:
	QString url() const override {
		return _tag;
	}

private:
	QString _tag;

};

class MentionNameClickHandler : public ClickHandler {
public:
	MentionNameClickHandler(QString text, UserId userId, uint64 accessHash)
		: _text(text)
		, _userId(userId)
		, _accessHash(accessHash) {
	}

	void onClick(Qt::MouseButton button) const override;

	TextWithEntities getExpandedLinkTextWithEntities(ExpandLinksMode mode, int entityOffset, const QStringRef &textPart) const override;

	QString tooltip() const override;

private:
	QString _text;
	UserId _userId;
	uint64 _accessHash;

};

class HashtagClickHandler : public TextClickHandler {
public:
	HashtagClickHandler(const QString &tag) : _tag(tag) {
	}

	void onClick(Qt::MouseButton button) const override;

	QString dragText() const override {
		return _tag;
	}

	QString copyToClipboardContextItemText() const override;

	TextWithEntities getExpandedLinkTextWithEntities(ExpandLinksMode mode, int entityOffset, const QStringRef &textPart) const override;

protected:
	QString url() const override {
		return _tag;
	}

private:
	QString _tag;

};

class PeerData;
class UserData;
class BotCommandClickHandler : public TextClickHandler {
public:
	BotCommandClickHandler(const QString &cmd) : _cmd(cmd) {
	}

	void onClick(Qt::MouseButton button) const override;

	QString dragText() const override {
		return _cmd;
	}

	static void setPeerForCommand(PeerData *peer) {
		_peer = peer;
	}
	static void setBotForCommand(UserData *bot) {
		_bot = bot;
	}

	TextWithEntities getExpandedLinkTextWithEntities(ExpandLinksMode mode, int entityOffset, const QStringRef &textPart) const override;

protected:
	QString url() const override {
		return _cmd;
	}
	static PeerData *peerForCommand() {
		return _peer;
	}
	static UserData *botForCommand() {
		return _bot;
	}

private:
	QString _cmd;

	static PeerData *_peer;
	static UserData *_bot;

};
