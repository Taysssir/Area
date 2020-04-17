#include "sftp.h"

namespace eos
{
namespace sftp
{

QSftp::QSftp()
    : session_(0)
    , sftp_(0)
    ,port_(22)
{

}

QSftp::~QSftp()
{
    disconnect();
    libssh2_exit();
}

void QSftp::setParameters(QString const& host, QString const& user, QString const& pwd, int port)
{
    host_ = host;
    user_ = user;
    pwd_ = pwd;
    port_ = port;
}

QSftp::Status QSftp::connect()
{
    if(session_ || sftp_)
    {
        disconnect();
        //emit error(tr("Already connected !"));
        //return Error;
    }
    unsigned long hostaddr = inet_addr(host_.toStdString().c_str());
    int rc = libssh2_init(0);
    //const char *fingerprint;
    struct sockaddr_in sin;
    if(rc != 0)
    {
        emit error(tr("Failed to initialize ssh !"));
        return Error;
    }

    socket_ = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port_);
    sin.sin_addr.s_addr = hostaddr;
    if (::connect(socket_, (struct sockaddr*)(&sin),
                  sizeof(struct sockaddr_in)) != 0)
    {
        emit error(tr("Failed to connect !"));
        return Error;
    }

    session_ = libssh2_session_init();

    if(!session_)
    {
        emit error(tr("Failed to create the session !"));
        return Error;
    }


    try {
        // rc = libssh2_session_handshake(session_, socket_);

    } catch (std::exception  ex) {
        emit error(tr("lib ssh 2 failure %s!", ex.what()));

    }

    if(rc)
    {
        emit error(tr("Failure etablishing SSH session %d!"));
        return Error;
    }

    //libssh2_hostkey_hash(session_, LIBSSH2_HOSTKEY_HASH_SHA1);


    if (libssh2_userauth_password(session_, user_.toStdString().c_str(), pwd_.toStdString().c_str()))
    {
        emit error(tr("Authentification by password failed !"));
        return Authentification_Failed;
    }

    sftp_ = libssh2_sftp_init(session_);

    if (!sftp_)
    {
        emit error(tr("Unable to init SFTP session !"));
        return Error;
    }
    return OK;
}

bool QSftp::download(QString const& remote_file, QString const& local_file)
{
    if(!session_ || !sftp_)
    {
        emit error(tr("ssh not opened !"));
        return false;
    }

    LIBSSH2_SFTP_ATTRIBUTES attrs;
    libssh2_sftp_stat(sftp_, remote_file.toStdString().c_str(), &attrs);
    if(attrs.filesize == 0)
    {
        emit error(tr("Remote file size is null !"));
        return false;
    }

    LIBSSH2_SFTP_HANDLE * sftp_handle =
            libssh2_sftp_open(sftp_, remote_file.toStdString().c_str(), LIBSSH2_FXF_READ, 0);

    if (!sftp_handle)
    {
        emit error(tr("Unable to open the remote file !"));
        return false;
    }

    FILE * local = fopen(local_file.toStdString().c_str(), "wb");
    if (!local)
    {
        emit error(tr("Unable to open the local file !"));
        return false;
    }

    static char mem[1024];
    int rc = 0;
    unsigned int total = 0;

    /* Since we have set non-blocking, tell libssh2 we are non-blocking */
    libssh2_session_set_blocking(session_, 0);

    emit progressing( 0 );
    do {
        /* loop until we fail */
        while ( (rc = libssh2_sftp_read(sftp_handle, mem, sizeof(mem))) == LIBSSH2_ERROR_EAGAIN )
        {
            waitsocket(socket_, session_);
        }

        if (rc > 0) {
            fwrite(mem, 1, rc, local);
            total += rc;
        } else if (rc < 0){ // read error
            qDebug() << rc;
            qDebug() << "File read error!";
            break;
        } else { // rc == 0 : EOF
            break;
        }
        emit progressing( double(total) / double(attrs.filesize) );
    } while (1);
    fclose(local);
    libssh2_sftp_close(sftp_handle);

    return (total == attrs.filesize);
}

bool QSftp::upload(QString const& local_file, QString const& remote_file)
{
    if(!session_ || !sftp_)
    {
        emit error(tr("ssh not opened !"));
        return false;
    }

    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open( sftp_, remote_file.toStdString().c_str(),
                                                          LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                                                          LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                                                          LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH );

    FILE * local = fopen(local_file.toStdString().c_str(), "rb");
    if (!local)
    {
        emit error(tr("Failed to open the local file !"));
        return false;
    }

    fseek(local, 0, SEEK_END);
    int fileSize = ftell(local);
    rewind(local);

    char mem[1024];
    int rc = 0;
    char * ptr;
    size_t nread;
    int total = 0;

    libssh2_session_set_blocking(session_, 0);

    emit progressing( 0 );
    do {
           nread = fread(mem, 1, sizeof(mem), local);
           if (nread <= 0) {
               /* end of file */
               break;
           }
           ptr = mem;

           total += nread;

           do {
               /* write data in a loop until we block */
               while ( (rc = libssh2_sftp_write(sftp_handle, ptr, nread)) == LIBSSH2_ERROR_EAGAIN )
               {
                   waitsocket(socket_, session_);
               }

               if ( rc == LIBSSH2_ERROR_SOCKET_RECV )
               {
                   libssh2_sftp_close(sftp_handle);
                   disconnect();
                   connect();
                   sftp_handle = libssh2_sftp_open(sftp_, remote_file.toStdString().c_str(),
                                                   LIBSSH2_FXF_WRITE|LIBSSH2_FXF_APPEND,
                                                   LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                                                   LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);

                   qDebug() << "CONNECTION REFRESHED !!!";
                   continue;
               }

               if(rc < 0)
               {
                   qDebug() << rc;
                   qDebug() << "File write error!";
                   break;
               }
               ptr += rc;
               nread -= rc;
               emit progressing( double(total) / double(fileSize) );
           } while (nread);
       } while (rc > 0);

    fclose(local);
    libssh2_sftp_close(sftp_handle);

    return (total == fileSize);
}

int QSftp::waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);


    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

    return rc;
}

bool QSftp::disconnect()
{
    if(!sftp_ || !session_) return false;

    libssh2_sftp_shutdown(sftp_);

    libssh2_session_disconnect(session_, "Normal Shutdown, Thank you for playing");

    libssh2_session_free(session_);


#ifdef WIN32
    closesocket(socket_);
#else
    close(socket_);
#endif
    sftp_ = 0;
    session_ = 0;

    return true;
}

} // namespace sftp
} // namespace eos
