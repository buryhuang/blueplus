/** \addtogroup bluetooth
 *  @{
 */

#include "BlueToothSocket.h"

CBlueToothSocket::CBlueToothSocket(SOCKET s):
	m_bStarted(true),
	m_bConnected(true),
	m_bCreated(true),
	m_pHandler(NULL),
	m_bAuth(false)
{
	if(s!=INVALID_SOCKET){
		m_socket=s;
		m_iStatus=CONNECTED;
	}else{
		m_socket=INVALID_SOCKET;
		m_iStatus=NOT_CREATED;
	}
}

CBlueToothSocket::CBlueToothSocket(void):
	m_bStarted(false),
	m_bConnected(false),
	m_bCreated(false),
	m_pHandler(NULL),
	m_iStatus(NOT_CREATED),
	m_socket(INVALID_SOCKET),
	m_bAuth(false)
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		WSAGetLastError();
		m_bStarted = false;
	} else {
		m_bStarted = true;
    }
}

CBlueToothSocket::~CBlueToothSocket(void)
{
	if (m_bStarted) {
		WSACleanup();
	}
}

BOOL CBlueToothSocket::Create(BOOL authenticate, BOOL encrypt) {
//    debug(("socket"));
	// create socket

#ifndef LOOPBACK_TEST
	SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
#else
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif

	if (s == INVALID_SOCKET) {
		//throwIOExceptionWinGetLastError(env, "Failed to create socket");
		Utils::ShowError(TEXT("socket"));
		return false;
	}

	// set socket options
#ifndef LOOPBACK_TEST
	if (authenticate) {
		ULONG ul = true;
		m_bAuth = true;

		if (setsockopt(s, SOL_RFCOMM, SO_BTH_AUTHENTICATE, (char *)&ul, sizeof(ULONG))) {
			closesocket(s);
			//throwIOExceptionWinGetLastError(env, "Failed to set authentication option");
			Utils::ShowError(TEXT("socket"));
			return false;
		}
	}

	if (encrypt) {

		ULONG ul = true;

		if (setsockopt(s, SOL_RFCOMM, SO_BTH_ENCRYPT, (char *)&ul, sizeof(ul))) {
			closesocket(s);
			//throwIOExceptionWinGetLastError(env, "Failed to set encryption option");
			Utils::ShowError(TEXT("socket"));
			return false;
		}
	}
#endif
	//debug(("socket[%u] opened", (int)s));
	m_socket = s;
	m_bCreated = true;
	m_iStatus = CREATED;
	return true;
}




BOOL CBlueToothSocket::Connect(BTH_ADDR address, int channel, int retryUnreachable) {
    //debug(("socket[%u] connect", (int)socket));

	m_iStatus = CONNECTING;

	if(m_bAuth){
		BLUETOOTH_DEVICE_INFO btdi;
		CBlueTooth::getBluetoothDeviceInfo(address,&btdi,false);
		if(m_passkey.size()>0){
			switch(BluetoothAuthenticateDevice(NULL, NULL, &btdi, &m_passkey[0], m_passkey.size()))
			{
				case ERROR_SUCCESS:
				case ERROR_NO_MORE_ITEMS:
					printf("successful");
					break;
				case ERROR_CANCELLED:
					printf("cancelled");
					break;
				case ERROR_INVALID_PARAMETER:
					printf("Invalid param");
					break;
			};//Will auth on all radios
		}else{
			switch(BluetoothAuthenticateDevice(NULL, NULL, &btdi, NULL, 0))
			{
				case ERROR_SUCCESS:
				case ERROR_NO_MORE_ITEMS:
					printf("successful");
					break;
				case ERROR_CANCELLED:
					printf("cancelled");
					break;
				case ERROR_INVALID_PARAMETER:
					printf("Invalid param");
					break;
			};//Will auth on all radios
		}

	}


#ifndef LOOPBACK_TEST
	SOCKADDR_BTH addr;
	memset(&addr, 0, sizeof(SOCKADDR_BTH));

	addr.addressFamily = AF_BTH;
	addr.btAddr = address;
	addr.port = channel;

#else
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(sockaddr_in));

    addr.sin_family = AF_INET; // address family Internet
    addr.sin_port = htons (20248); //Port to connect on
    addr.sin_addr.s_addr = inet_addr ("127.0.0.1");

#endif

	int retyCount = 0;
connectRety:
	if (connect((SOCKET)m_socket, (sockaddr *)&addr, sizeof(SOCKADDR_BTH))) {
		retyCount ++;
		int last_error = WSAGetLastError();
		//10051 - A socket operation was attempted to an unreachable network. / Error other than time-out at L2CAP or Bluetooth radio level.
		if (last_error == WSAENETUNREACH) {
			if ((retyCount < retryUnreachable) && (retryUnreachable > 0)) {
//			    if (isCurrentThreadInterrupted(env, peer, "connect")) {
 //                   return;
//                }
				//debug(("connectRety %i", retyCount));
				goto connectRety;
			}
		}
		if (last_error == WSAEACCES) {
			//throwBluetoothConnectionException(env, BT_CONNECTION_ERROR_SECURITY_BLOCK, "Connecting application requested authentication, but authentication failed [10013] .");
			Utils::ShowError(TEXT("Connect"));
			m_iStatus = CONNECTION_AUTH_FAILED;
			return false;
		} else if (last_error == WSAETIMEDOUT) {
			//throwBluetoothConnectionException(env, BT_CONNECTION_ERROR_TIMEOUT, "Connection timeout; [%lu] %S", last_error, getWinErrorMessage(last_error));
			Utils::ShowError(TEXT("Connect"));
			m_iStatus = CONNECTION_TIMEOUT;
			return false;
		} else {
			//throwBluetoothConnectionException(env, BT_CONNECTION_ERROR_FAILED_NOINFO, "Failed to connect; [%lu] %S", last_error, getWinErrorMessage(last_error));
			Utils::ShowError(TEXT("Connect"));
			m_iStatus = CONNECTION_FAILED;
			return false;
		}
	}

	m_iStatus = CONNECTED;

	return true;
}

BOOL CBlueToothSocket::Bind() {
	// bind socket
//	debug(("socket[%u] bind", (int)socket));

#ifndef LOOPBACK_TEST
	SOCKADDR_BTH addr;
	memset(&addr, 0, sizeof(addr));

	addr.addressFamily = AF_BTH;
	addr.port = BT_PORT_ANY;
#else
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	
	addr.sin_family=AF_INET; //Address family
    addr.sin_addr.s_addr=INADDR_ANY; //Wild card IP address
    addr.sin_port=htons((u_short)20248); //port to use
#endif

	if (bind((SOCKET)m_socket, (SOCKADDR *)&addr, sizeof(addr))) {
		closesocket((SOCKET)m_socket);
		//throwIOExceptionWSAGetLastError(env, "Failed to bind socket");
		Utils::ShowError(TEXT("Bind"));
		return false;
	}
	return true;
}

BOOL CBlueToothSocket::Listen() {
    //debug(("socket[%u] listen", (int)socket));
	m_iStatus = LISTENING;

	if (listen((SOCKET)m_socket, SOMAXCONN)) {
		//throwIOExceptionWSAGetLastError(env, "Failed to listen socket");
		Utils::ShowError(TEXT("Listen"));
		return false;
	}
	return true;
}

SOCKET CBlueToothSocket::Accept() {
	//debug(("socket[%u] accept", (int)socket));
	SOCKADDR_BTH addr;

	int size = sizeof(SOCKADDR_BTH);

	SOCKET s = accept((SOCKET)m_socket, (sockaddr *)&addr, &size);

	if (s == INVALID_SOCKET) {
		//throwIOException(env, "Failed to listen socket");
		Utils::ShowError(TEXT("Accept"));
		return INVALID_SOCKET;
	}

	m_iStatus = ACCEPTED;

	//debug(("connection accepted"));
	m_listSocket.push_back(s);
	if(m_pHandler!=NULL){
		m_pHandler->OnAccept(s);
	}
	return s;
}

int CBlueToothSocket::RecveiveAvailable() {
	unsigned long arg = 0;
	if (ioctlsocket((SOCKET)m_socket, FIONREAD, &arg) != 0) {
		//throwIOExceptionWSAGetLastError(env, "Failed to read available");
		Utils::ShowError(TEXT("recvAvailable"));
		return 0;
	}
	//cout<<"Available bytes: "<<arg<<endl;
	return (int)arg;
}

void CBlueToothSocket::Close() {
	//debug(("socket[%u] close", (int)socket));
	if (shutdown((SOCKET)m_socket, SD_BOTH) != 0) {
	    int last_error = WSAGetLastError();
	    if (last_error != WSAENOTCONN) {
	        //debug(("shutdown error [%i] %S", last_error, getWinErrorMessage(last_error)));
	    }
		Utils::ShowError(TEXT("close"));

	}
	if (closesocket((SOCKET)m_socket)) {
		//throwIOExceptionWSAGetLastError(env, "Failed to close socket");
		Utils::ShowError(TEXT("close"));
	}
	m_iStatus = CLOSED;
}

int CBlueToothSocket::RecveiveChar() {
	if(m_iStatus != CONNECTED){
		return -1;
	}

	//debug(("socket[%u] recv()", (int)socket));
	// Use non blocking functions to see if we have one byte
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500 * 1000; //microseconds
    while (true) {
        fd_set readfds;
        fd_set exceptfds;
        FD_ZERO(&readfds);
        FD_SET((SOCKET)m_socket, &readfds);
        FD_ZERO(&exceptfds);
        FD_SET((SOCKET)m_socket, &exceptfds);
        int ready_count = select(FD_SETSIZE, &readfds, NULL, &exceptfds, &timeout);
        if (ready_count == SOCKET_ERROR) {
	        //throwIOExceptionWSAGetLastError(env, "Failed to read(int)/select");
			Utils::ShowError(TEXT("Receive"));
	        return -1;
	    } else if (ready_count > 0) {
            break;
        }
    }
    // Read the data when available
	unsigned char c;
	int rc = recv((SOCKET)m_socket, (char *)&c, 1, 0);
	if (rc == SOCKET_ERROR) {
		//throwIOExceptionWSAGetLastError(env, "Failed to read(int)");
		Utils::ShowError(TEXT("Receive"));
		return 0;
	} else if (rc == 0) {
		//debug(("Connection closed"));
		// See InputStream.read();
		return -1;
	}
	return (int)c;
}

size_t CBlueToothSocket::Recveive() {
	//debug(("socket[%u] recv (byte[],int,int=%i)", (int)socket, len));
	if(m_iStatus != CONNECTED){
		return -1;
	}

	BYTEBUFFER tmpbuff;
	BYTEBUFFER buff;
	int BUFFSIZE = 5000;

	// Use non blocking functions to see if we have one byte
    struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500 * 1000; //microseconds
    while (true) {
        fd_set readfds;
        fd_set exceptfds;
        FD_ZERO(&readfds);
        FD_SET((SOCKET)m_socket, &readfds);
        FD_ZERO(&exceptfds);
        FD_SET((SOCKET)m_socket, &exceptfds);
        int ready_count = select(FD_SETSIZE, &readfds, NULL, &exceptfds, &timeout);
        if (ready_count == SOCKET_ERROR) {
	        //throwIOExceptionWSAGetLastError(env, "Failed to read(byte[])/select");
			Utils::ShowError(TEXT("Receive"));
			m_iStatus = RECV_FAILED;

	        return -1;
	    } else if (ready_count > 0) {
            break;
        }
    }

    // Read the data when available
	size_t len = RecveiveAvailable();

	if(len<=0){
		m_iStatus = RECV_FAILED;
		return -1;
	}
	buff.clear();
	buff.reserve(len);
	
	size_t done = 0;
	while(done < len) {
		tmpbuff.clear();
		tmpbuff.resize(len-done);
		size_t count = recv((SOCKET)m_socket, &tmpbuff[0], static_cast<int>(tmpbuff.size()), 0);
		//cout<<"received bytes: "<<count<<"content: "<<tmpbuff<<endl;

		if (count == SOCKET_ERROR) {
			//throwIOExceptionWSAGetLastError(env, "Failed to read(byte[])");
			Utils::ShowError(TEXT("Receive"));
			m_iStatus = RECV_FAILED;
			done = -1;
			break;
		} else if (count == 0) {
			//debug(("Connection closed"));
			if (done == 0) {
				m_iStatus = RECV_FAILED;
				// See InputStream.read();
				done = -1;
			    break;
			} else {
				break;
			}
		}
		//ASSERT(count>=0);
		if(count<0){
			Utils::ShowError(TEXT("Receive"));
			m_iStatus = RECV_FAILED;
			done = -1;
			break;
		}
		tmpbuff.resize(count);
		buff.insert(buff.end(),tmpbuff.begin(),tmpbuff.end());		

		done += count;
		//cout<<"Done: "<<done<<", len: "<<len<<endl;
		if (done != 0) {
	        if (RecveiveAvailable()==0) {
	            break;
	        }
		}
	}

	//cout<<"Should not be NULL: "<<hex<<(unsigned long)m_pHandler<<endl;
	if(m_pHandler!=NULL){
		//cout<<"About to pass data: "<<buff<<endl;
		m_pHandler->OnReceive(m_socket,buff);
	}
	return done;
}

size_t CBlueToothSocket::Send(BYTEBUFFER buff)
{
	//debug(("socket[%u] send(byte[],int,int=%i)", (int)socket, len));

	//jbyte *bytes = env->GetByteArrayElements(b, 0);
	size_t done = 0;
	size_t len = buff.size();
	while (done < buff.size()) {
		size_t count = send((SOCKET)m_socket, &buff[0], static_cast<int>(len - done), 0);
		if (count <= 0) {
			//throwIOExceptionWSAGetLastError(env, "Failed to write");
			Utils::ShowError(TEXT("Send"));
			break;
		}
		done += count;
	}
	return done;

}

bool CBlueToothSocket::RegisterHandler(CSocketHandler* pHandler)
{
	if(pHandler!=NULL){
		m_pHandler = pHandler;
		return true;
	}
	return false;
}

wstring CBlueToothSocket::GetStatusString()
{
	switch(m_iStatus){
		case NOT_CREATED:
			return L"Not Created";
			break;
		case CREATED:
			return L"Created";
			break;
		case LISTENING:
			return L"Listening";
			break;
		case ACCEPTED:
			return L"Accepted";
			break;
		case CONNECTING:
			return L"Connecting";
			break;
		case CONNECTION_AUTH_FAILED:
			return L"Connection authorization failed";
			break;
		case CONNECTION_TIMEOUT:
			return L"Connection timeout";
			break;
		case CONNECTION_FAILED:
			return L"Connection failed";
			break;
		case CONNECTED:
			return L"Connected";
			break;
		case RECV_FAILED:
			return L"Receive failed";
			break;
		case CLOSED:
			return L"Closed";
			break;
		default:
			break;
	}
	return L"Exception";
}

void CBlueToothSocket::SetPasskey(wstring passkey)
{
	m_passkey = passkey;
}


/** @}*/
