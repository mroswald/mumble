/* Copyright (C) 2005-2009, Thorvald Natvig <thorvald@natvig.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _SERVERHANDLER_H
#define _SERVERHANDLER_H

#define SERVERSEND_EVENT 3501

#include "mumble_pch.hpp"
#include "Timer.h"
#include "Message.h"
#include "PacketDataStream.h"

class Connection;
class Message;

class ServerHandlerMessageEvent : public QEvent {
	public:
		unsigned int uiType;
		QByteArray qbaMsg;
		bool bFlush;
		ServerHandlerMessageEvent(const QByteArray &msg, unsigned int type, bool flush = false);
};

typedef boost::shared_ptr<Connection> ConnectionPtr;

class ServerHandler : public QThread {
	private:
		Q_OBJECT
		Q_DISABLE_COPY(ServerHandler)
	protected:
		QString qsHostName;
		QString qsUserName;
		QString qsPassword;
		QString qsDigest;
		unsigned short usPort;

#ifdef Q_OS_WIN
		HANDLE hQoS;
		DWORD dwFlowUDP;
#endif

		QHostAddress qhaRemote;
		QUdpSocket *qusUdp;
		QMutex qmUdp;

		void handleVoicePacket(unsigned int msgFlags, PacketDataStream &pds);
	public:
		Timer tTimestamp;
		QList<QSslError> qlErrors;
		QList<QSslCertificate> qscCert;
		QSslCipher qscCipher;
		ConnectionPtr cConnection;

		unsigned int uiVersion;
		QString qsRelease;

		boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::mean, boost::accumulators::tag::variance, boost::accumulators::tag::count> > accTCP, accUDP, accClean;

		ServerHandler();
		~ServerHandler();
		void setConnectionInfo(const QString &host, unsigned short port, const QString &username, const QString &pw);
		void getConnectionInfo(QString &host, unsigned short &port, QString &username, QString &pw);
		void customEvent(QEvent *evt);

		void sendProtoMessage(const ::google::protobuf::Message &msg, unsigned int msgType);
		void sendMessage(const char *data, int len);

#define MUMBLE_MH_MSG(x) void sendMessage(const MumbleProto:: x &msg) { sendProtoMessage(msg, MessageHandler:: x); }
		MUMBLE_MH_ALL
#undef MUMBLE_MH_MSG

		void disconnect();
		void run();
	signals:
		void disconnected(QString reason);
		void connected();
	protected slots:
		void message(unsigned int, const QByteArray &);
		void serverConnectionConnected();
		void serverConnectionClosed(const QString &);
		void setSslErrors(const QList<QSslError> &);
		void udpReady();
		void sendPing();
};

#else
class ServerHandler;
#endif
