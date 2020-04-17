#ifndef SENTINEL_COMMON_SFTP_H
#define SENTINEL_COMMON_SFTP_H

#include <QObject>

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <QDebug>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
#include <winsock2.h>
#include <time.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

typedef struct _LIBSSH2_SESSION           LIBSSH2_SESSION;
typedef struct _LIBSSH2_SFTP              LIBSSH2_SFTP;

namespace eos
{
  namespace sftp
  {

    class QSftp
        : public QObject
    {
      Q_OBJECT
    public:
      typedef enum
      {
        OK,
        Authentification_Failed,
        Error
      } Status;

      QSftp();
      ~QSftp();
      Status connect();
      void setParameters(QString const& host, QString const& user, QString const& pwd, int port);
      bool disconnect();
      bool download(QString const& remote_file, QString const& local_file);
      bool upload(QString const& local_file, QString const& remote_file);
      static int waitsocket(int socket_fd, LIBSSH2_SESSION *session);
      void setPort(int port){ port_ = port; }

    signals:
      void progressing(double p);
      void error(QString const& err);
    private:
      static constexpr int MAX_XFER_BUF_SIZE = 1024;
      LIBSSH2_SESSION * session_;
      LIBSSH2_SFTP  *sftp_;
      QString host_, user_, pwd_;
      int socket_;
      int port_;
    };

  } // namespace sftp
} // namespace eos

#endif // SENTINEL_COMMON_SFTP_H
