#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <WinSock2.h>

#include "astring.h"
#include "tcpsocket.h"
#include "types.h"

class TcpServerEventListener;

/**
 * This class provides a TcpServer with some useful functions.
 *
 * @author Daniel Hentzschel on 13.05.2019.
 */
class TcpServer : public TcpSocket {
	friend class TcpServerEventListener;

public:
	/**
	 * Sets default values.
	 */
	explicit TcpServer();

	/**
	 * Closes the server socket.
	 */
	~TcpServer();

	/**
	 * Tries to listen to the specified hostaddress and port and returns success.
	 *
	 * @param hostaddress the hostaddress to connect to
	 * @param port the port to connect to
	 * @return whether to the hostaddress and port could be listened
	 */
	virtual bool listen(const AString& hostaddress, ushort port);

	/**
	 * Terminates the serverEventListener thread.
	 */
	void terminate() const;

protected:
	TcpSocket* currentSocket_;

	AVector<TcpSocket*> socketList_;

	uint addressLength_;

	/**
	 * Will be called when a client connects to the server.
	 *
	 * @param socket the socket which connected
	 */
	virtual void incomingConnection(const TcpSocket& socket) = 0;

	/**
	 * Will be called when a client disconnects from the server.
	 *
	 * @param socket the socket which disconnected
	 */
	virtual void disconnected(const TcpSocket& socket) = 0;

	/**
	 * Will be called when the server receives a packet from a client.
	 *
	 * @param packet the packet received from client
	 * @param length the length of the packet received from the client
	 */
	virtual void receive(const char* packet, uint length) override = 0;

	std::shared_ptr<TcpServerEventListener> serverEventListener_;

private:
	/**
	 * Initializes the WSA instance and socket.
	 */
	void initialize();

	/**
	 * Returns the pointer to the TcpSocket equals to the specified socket.
	 * Returns nullptr when the socket does not exist in the client list.
	 *
	 * @param socket the socket to find
	 * @return the pointer to the TcpSocket
	 */
	TcpSocket* getSocket(const SOCKET& socket);
};

#endif   // TCPSERVER_H
