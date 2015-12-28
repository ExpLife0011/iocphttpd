#include "SocketIocpController.h"


SocketIocpController::SocketIocpController()
{
	for (int i = 0; i < DATASIZMIN; i++)
	{
		m_ActiveFlag[i] = 0;
		LPSOCKET_IO_DATA rv = &m_DataBuffer[i];
		ZeroMemory(&(m_DataBuffer[i].operationData.Overlapped), sizeof(OVERLAPPED));
		m_DataBuffer[i].operationData.sequence = i;
		m_DataBuffer[i].sequence = i;
		m_DataBuffer[i].handleData.Socket = 0;
	}

	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);

	m_DataBufferSize = DATASIZMIN;
}


SocketIocpController::~SocketIocpController()
{
	::CloseHandle(ghMutex);
}


SocketIocpController::LPSOCKET_IO_DATA SocketIocpController::Allocate()
{
	::WaitForSingleObject(ghMutex, INFINITE);
	for (int i = 0; i < m_DataBufferSize; i++)
	{
		if (m_ActiveFlag[i] == 0)
		{
			LPSOCKET_IO_DATA rv = &m_DataBuffer[i];
			ZeroMemory(&(rv->operationData.Overlapped), sizeof(OVERLAPPED));
			memset(rv->operationData.Buffer, 0, BUFSIZMIN);
			rv->operationData.BytesRECV = 0;
			rv->operationData.BytesSEND = 0;
			rv->operationData.LPBuffer = NULL;
			rv->operationData.DataBuf.buf = NULL;
			rv->operationData.DataBuf.len = 0;
			rv->handleData.Socket = 0;
			rv->sequence = i;
			rv->operationData.sequence = i;
			rv->operationData.mallocFlag = 0;
			m_ActiveFlag[i] = 1;
			std::wstring szName(std::to_wstring(i));
			rv->operationData.Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, szName.c_str());
			::ReleaseMutex(ghMutex);
			return rv;
		}
	}
	::ReleaseMutex(ghMutex);
	return NULL;
}

void SocketIocpController::Free(LPSOCKET_IO_DATA data)
{
	if (m_ActiveFlag[data->sequence])
	{
		FreeByIndex(data->sequence);
	}
}


void SocketIocpController::FreeByIndex(int index)
{
	::WaitForSingleObject(ghMutex, INFINITE);
	LPSOCKET_IO_DATA rv = NULL;

	if (index < m_DataBufferSize && m_ActiveFlag[index])
	{
		rv = &m_DataBuffer[index];
		rv->operationData.BytesRECV = 0;
		rv->operationData.BytesSEND = 0;
		rv->operationData.LPBuffer = NULL;
		if (rv->operationData.DataBuf.buf != NULL && rv->operationData.mallocFlag == 1)
		{
			free(rv->operationData.DataBuf.buf);
			rv->operationData.mallocFlag = 0;
		}
		rv->operationData.DataBuf.buf = NULL;
		rv->operationData.DataBuf.len = 0;
		rv->handleData.Socket = 0;
		m_ActiveFlag[rv->sequence] = 0;
		CloseHandle(rv->operationData.Overlapped.hEvent);

		ZeroMemory(&(rv->operationData.Overlapped), sizeof(OVERLAPPED));
		memset(rv->operationData.Buffer, 0, BUFSIZMIN);
	}
	::ReleaseMutex(ghMutex);
}

void SocketIocpController::FreeBySocket(SOCKET index)
{
	LPSOCKET_IO_DATA rv = NULL;
	for (int i = 0; i < m_DataBufferSize; i++)
	{
		if (m_ActiveFlag[i] == 1)
		{
			rv = &m_DataBuffer[i];
			if (rv->handleData.Socket != NULL)
			{
				if (rv->handleData.Socket == index)
				{
					FreeByIndex(i);
				}
			}
		}
	}
}